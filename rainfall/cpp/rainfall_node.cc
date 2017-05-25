#include "rainfall.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <node.h>
#include <uv.h>
#include <v8.h>
#include <thread>
#include <chrono>

using namespace v8;

sample unpack_sample(Isolate *isolate, const Handle<Object> sample_obj) {
  sample s;

  Handle<Value> date_Value = sample_obj->Get(String::NewFromUtf8(isolate, "date"));
  Handle<Value> rainfall_Value = sample_obj->Get(String::NewFromUtf8(isolate, "rainfall"));

  v8::String::Utf8Value utfValue(date_Value);
  s.date = std::string(*utfValue);

  // Unpack numeric rainfall amount from V8 value
  s.rainfall = rainfall_Value->NumberValue();
  return s;
}

location unpack_location(Isolate *isolate, const Handle<Object> location_obj) {
  location loc;

  //Handle<Object> location_obj = Handle<Object>::Cast(args[0]);
  Handle<Value> lat_Value = location_obj->Get(String::NewFromUtf8(isolate, "latitude"));
  Handle<Value> lon_Value = location_obj->Get(String::NewFromUtf8(isolate, "longitude"));

  loc.latitude = lat_Value->NumberValue();
  loc.longitude = lon_Value->NumberValue();

  Handle<Array> array = Handle<Array>::Cast(location_obj->Get(String::NewFromUtf8(isolate, "samples")));

  int sample_count = array->Length();
  for (int i = 0; i < sample_count; i++) {
    sample s = unpack_sample(isolate, Handle<Object>::Cast(array->Get(i)));
    loc.samples.push_back(s);
  }

  std::cout << "💩\n";

  return loc;
}

void AvgRainfall(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();

  location loc = unpack_location(isolate, Local<Object>::Cast(args[0]));
  double avg = avg_rainfall(loc);

  Local<Number> value = Number::New(isolate, avg);
  args.GetReturnValue().Set(value);
}

void RainfallData(const v8::FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();

  location loc = unpack_location(isolate, Local<Object>::Cast(args[0]));
  rain_result result = calc_rain_stats(loc);

  Local<Object> obj = Object::New(isolate);

  obj->Set(String::NewFromUtf8(isolate, "mean"),
           Number::New(isolate, result.mean));
  obj->Set(String::NewFromUtf8(isolate, "median"),
           Number::New(isolate, result.median));
  obj->Set(String::NewFromUtf8(isolate, "standard_deviation"),
           Number::New(isolate, result.standard_deviation));
  obj->Set(String::NewFromUtf8(isolate, "n"), Integer::New(isolate, result.n));

  args.GetReturnValue().Set(obj);
}


void RainfallDataArray(const v8::FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();

  Local<Array> result_array = Array::New(isolate);

  Local<Array> locs = Local<Array>::Cast(args[0]);
  int loc_count = locs->Length();

  for (int i = 0; i < loc_count; i++) {
    location loc = unpack_location(isolate, Local<Object>::Cast(locs->Get(i)));
    rain_result result = calc_rain_stats(loc);

    Local<Object> obj = Object::New(isolate);

    obj->Set(String::NewFromUtf8(isolate, "mean"),
            Number::New(isolate, result.mean));
    obj->Set(String::NewFromUtf8(isolate, "median"),
            Number::New(isolate, result.median));
    obj->Set(String::NewFromUtf8(isolate, "standard_deviation"),
            Number::New(isolate, result.standard_deviation));
    obj->Set(String::NewFromUtf8(isolate, "n"), Integer::New(isolate, result.n));

    result_array->Set(i, obj);
  }

  Local<Function> cb = Local<Function>::Cast(args[1]);
  Local<Value> argv[2] = {Null(isolate), result_array};
  cb->Call(Null(isolate), 2, argv);

  args.GetReturnValue().Set(result_array);
}

struct Work {
  uv_work_t request;
  Persistent<Function> callback;

  std::vector<location> locations;
  std::vector<rain_result> results;
};

static void WorkAsync(uv_work_t *req) {
  Work *work = static_cast<Work *>(req->data);

  work->results.resize(work->locations.size());

  std::transform(work->locations.begin(), work->locations.end(),
                 work->results.begin(), calc_rain_stats);

  //std::this_thread::sleep_for(chrono::seconds(3));
}

static void WorkAsyncComplete(uv_work_t *req, int status) {
  Isolate *isolate = Isolate::GetCurrent();

  HandleScope handleScope(isolate);

  Work *work = static_cast<Work *>(req->data);

  Local<Array> result_list = Array::New(isolate);

  for (auto i = 0; i < work->results.size(); i++) {
    Local<Object> obj = Object::New(isolate);
    rain_result result = work->results[i];

    obj->Set(String::NewFromUtf8(isolate, "mean"),
             Number::New(isolate, result.mean));
    obj->Set(String::NewFromUtf8(isolate, "median"),
             Number::New(isolate, result.median));
    obj->Set(String::NewFromUtf8(isolate, "standard_deviation"),
             Number::New(isolate, result.standard_deviation));
    obj->Set(String::NewFromUtf8(isolate, "n"),
             Integer::New(isolate, result.n));

    result_list->Set(i, obj);
  }

  Handle<Value> argv[] = {Null(isolate), result_list};
  Local<Function>::New(isolate, work->callback)
      ->Call(isolate->GetCurrentContext()->Global(), 2, argv);

  work->callback.Reset();

  delete work;
}

void CalcResultsAsync(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();

  args.GetReturnValue().Set(Undefined(isolate));

  Work *work = new Work();
  work->request.data = work;

  Local<Array> input = Local<Array>::Cast(args[0]);
  auto num_locations = input->Length();

  for (auto i = 0; i < num_locations; i++) {
    work->locations.push_back(
        unpack_location(isolate, Local<Object>::Cast(input->Get(i))));
  }

  Local<Function> callback = Local<Function>::Cast(args[1]);
  work->callback.Reset(isolate, callback);

  uv_queue_work(uv_default_loop(), &work->request, WorkAsync,
                WorkAsyncComplete);

}

void init(Handle<Object> exports, Handle<Object> module) {
  NODE_SET_METHOD(exports, "avg_rainfall", AvgRainfall);
  NODE_SET_METHOD(exports, "rainfall_data", RainfallData);
  NODE_SET_METHOD(exports, "rainfall_data_array", RainfallDataArray);
  NODE_SET_METHOD(exports, "calculate_results_async", CalcResultsAsync);
}

NODE_MODULE(rainfall, init)
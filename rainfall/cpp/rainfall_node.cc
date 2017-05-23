#include <node.h>
#include <v8.h>
#include "rainfall.h"
#include <iostream>
#include <cmath>
#include <algorithm>

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

location unpack_location(Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args) {
  location loc;

  Handle<Object> location_obj = Handle<Object>::Cast(args[0]);
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

  location loc = unpack_location(isolate, args);
  double avg = avg_rainfall(loc);

  Local<Number> value = Number::New(isolate, avg);
  args.GetReturnValue().Set(value);
}

void RainfallData(const v8::FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();

  location loc = unpack_location(isolate, args);
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

void init(Handle<Object> exports, Handle<Object> module) {
  NODE_SET_METHOD(exports, "avg_rainfall", AvgRainfall);
  NODE_SET_METHOD(exports, "rainfall_data", RainfallData);
}

NODE_MODULE(rainfall, init)
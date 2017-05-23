#include <node.h>
#include <string>
using namespace v8;


void PassNumber(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  
  if (!args[0]->IsNumber()) {
    isolate->ThrowException(
        Exception::TypeError(String::NewFromUtf8(isolate, "wrong args")));
    return;
  }

  if (args.Length() == 1) {
    double n = args[0]->NumberValue();

    n += 20;

    Local<Number> num = Number::New(isolate, n);

    args.GetReturnValue().Set(num);
  }
}

void PassInt(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();

  if (!args[0]->IsNumber()) {
    isolate->ThrowException(
        Exception::TypeError(String::NewFromUtf8(isolate, "wrong args")));
    return;
  }

  if (args.Length() == 1) {
    int n = args[0]->Int32Value();

    n += 20;

    Local<Number> num = Int32::New(isolate, n);

    args.GetReturnValue().Set(num);
  }
}

void PassBool(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();

  bool b = args[0]->BooleanValue();

  Local<Boolean> boo = Boolean::New(isolate, !b);

  args.GetReturnValue().Set(boo);
}

void PassString(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();

  v8::String::Utf8Value s(args[0]);

  std::string str(*s);
  std::reverse(str.begin(), str.end());

  Local<String> retval = String::NewFromUtf8(isolate, str.c_str());

  args.GetReturnValue().Set(retval);
}

void PassObject(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> obj = args[0]->ToObject();
  obj->Set(String::NewFromUtf8(isolate, "prop"), Number::New(isolate, 6));
  args.GetReturnValue().Set(obj);
}

void IncrementArray(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Array> array = Local<Array>::Cast(args[0]);

  for (unsigned int i = 0; i < array->Length(); i++) {
    double n = array->Get(i)->NumberValue();
    array->Set(i, Number::New(isolate, n + 1));
  }
}

void Init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "number", PassNumber);
  NODE_SET_METHOD(exports, "int", PassInt);
  NODE_SET_METHOD(exports, "bool", PassBool);
  NODE_SET_METHOD(exports, "string", PassString);
  NODE_SET_METHOD(exports, "object", PassObject);
  NODE_SET_METHOD(exports, "array", IncrementArray);
}

NODE_MODULE(pass_stuff, Init)
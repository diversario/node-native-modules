#include <node.h>
using namespace v8;


void Mutate(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();

  Local<Object> obj = args[0]->ToObject();

  obj->Set(String::NewFromUtf8(isolate, "x"), Number::New(isolate, 22));
}

void Init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "mutate", Mutate);
}

NODE_MODULE(mutate, Init)
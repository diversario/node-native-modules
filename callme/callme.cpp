#include <node.h>
using namespace v8;

// void CallThis(const FunctionCallbackInfo<Value> &args) {
//   Isolate *isolate = args.GetIsolate();
//   Local<Function> cb = Local<Function>::Cast(args[0]);
//   cb->Call(Null(isolate), 0, nullptr);
//   return;
// }

// void CallThisWithThis(const FunctionCallbackInfo<Value> &args) {
//   Isolate *isolate = args.GetIsolate();
//   Local<Function> cb = Local<Function>::Cast(args[0]);

//   Local<Value> argv[1] = {args[1]};

//   cb->Call(Null(isolate), 1, argv);
//   return;
// }

void CallStuff(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();

  Local<Function> cb = Local<Function>::Cast(args[0]);

  if (args.Length() == 1) {    
    cb->Call(Null(isolate), 0, nullptr);
  } else if (args.Length() == 2)  {
    Local<Value> argv[1] = {args[1]};
    cb->Call(Null(isolate), 1, argv);
  } else {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Must provide 1 or 2 arguments.")));
  }
}

void Init(Local<Object> exports) {
  // NODE_SET_METHOD(exports, "callme", CallThis);
  // NODE_SET_METHOD(exports, "callme_withthis", CallThisWithThis);
  NODE_SET_METHOD(exports, "call_cb", CallStuff);
}

NODE_MODULE(callme, Init)
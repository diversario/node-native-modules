#include <node.h>
#include <node_object_wrap.h>
#include <cmath>
#include <iostream>

using namespace v8;
using namespace std;

class WrappedPoly : public node::ObjectWrap {
  public:
    static void Init(Local<Object> exports) {
      Isolate *isolate = exports->GetIsolate();

      Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
      
      tpl->SetClassName(String::NewFromUtf8(isolate, "Polynomial"));
      tpl->InstanceTemplate()->SetInternalFieldCount(1);

      NODE_SET_PROTOTYPE_METHOD(tpl, "at", At);
      NODE_SET_PROTOTYPE_METHOD(tpl, "roots", Roots);

      // tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "a"),
                                          //  GetCoeff, SetCoeff);

      constructor.Reset(isolate, tpl->GetFunction());
      exports->Set(String::NewFromUtf8(isolate, "Polynomial"),
                   tpl->GetFunction());
    }

  private:
    explicit WrappedPoly(double a = 0, double b = 0, double c = 0)
        : a_(a), b_(b), c_(c) {}

    ~WrappedPoly() {}

    static void New(const FunctionCallbackInfo<Value> &args) {
      Isolate *isolate = args.GetIsolate();

      if (args.IsConstructCall()) {
        double a = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
        double b = args[1]->IsUndefined() ? 0 : args[1]->NumberValue();
        double c = args[2]->IsUndefined() ? 0 : args[2]->NumberValue();

        WrappedPoly *obj = new WrappedPoly(a, b, c);
        obj->Wrap(args.This());
        args.GetReturnValue().Set(args.This());
      } else {
        const auto argc = 3;
        Local<Value> argv[argc] = {args[0], args[1], args[2]};
        Local<Function> constr = Local<Function>::New(isolate, constructor);
        args.GetReturnValue().Set(constr->NewInstance(argc, argv));
      }
    }

    static void At(const FunctionCallbackInfo<Value> &args);
    static void Roots(const FunctionCallbackInfo<Value> &args);
    // static void GetCoeff(Local<String> property, const PropertyCallbackInfo<Value> &info);
    // static void SetCoeff(Local<String> property, Local<Value> value, const PropertyCallbackInfo<Value> &info);

    static Persistent<Function> constructor;

    double a_;
    double b_;
    double c_;
};

Persistent<Function> WrappedPoly::constructor;

void WrappedPoly::At(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  
  double x = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();

  WrappedPoly *poly = ObjectWrap::Unwrap<WrappedPoly>(args.Holder());
  auto results = x * x * poly->a_ + x * poly->b_ + poly->c_;

  args.GetReturnValue().Set(Number::New(isolate, results));
}

vois WrappedPoly::Roots(const FunctionCallbackInfo<Value> &args) {
  
}

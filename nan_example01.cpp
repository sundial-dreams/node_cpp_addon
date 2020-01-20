#include <iostream>
#include <node.h>
#include <nan.h>

using namespace v8;

NAN_METHOD(Hello) {
  std::cout << "c++ say: hello world!" << std::endl;
  info.GetReturnValue().Set(Nan::New("hello world!").ToLocalChecked());
}

NAN_METHOD(Filter) {
  Local<Array> array = info[0].As<Array>();
  Local<Function> fn = info[1].As<Function>();
  Local<Context> context = Nan::GetCurrentContext();
  Local<Array> result = Nan::New<Array>();

  Local<Value> argv[3] = {Nan::New<Object>(), Nan::Null(), array};
  for (uint32_t i = 0, j = 0; i < array->Length(); i++) {
    argv[0] = array->Get(context, i).ToLocalChecked();
    argv[1] = Nan::New<Number>(i);
    Local<Value> v = Nan::Call(fn, Nan::New<Object>(), 3, argv).ToLocalChecked();
    if (v->IsTrue()) {
      result->Set(context, j++, argv[0]).FromJust();
    }
  }
  info.GetReturnValue().Set(result);
}

void CreateObject(const Nan::FunctionCallbackInfo<Value>& info) {
  Local<Object> object = Nan::New<Object>();
  Nan::Set(object, Nan::New("name").ToLocalChecked(), info[0].As<String>());
  Nan::Set(object, Nan::New("age").ToLocalChecked(), info[1].As<Number>());
  Nan::SetMethod(object, "getAll", [](const Nan::FunctionCallbackInfo<Value>& args) -> void {
    Local<Object> self = args.Holder();
    Local<String> name = Nan::Get(self, Nan::New("name").ToLocalChecked()).ToLocalChecked().As<String>();
    Local<Number> age = Nan::Get(self, Nan::New("age").ToLocalChecked()).ToLocalChecked().As<Number>();
    Nan::Utf8String n(name);
    int a = int(age->Value());
    char result[1024];
    sprintf(result, "{ name: %s, age: %d }", *n, a);
    args.GetReturnValue().Set(Nan::New(result).ToLocalChecked());
  });
  info.GetReturnValue().Set(object);
}

NAN_MODULE_INIT(Init) {
  Nan::Export(target, "hello", Hello);
  Nan::Export(target, "filter", Filter);
  Nan::Export(target, "createObject", CreateObject);
}

NODE_MODULE(nan_addon01, Init)

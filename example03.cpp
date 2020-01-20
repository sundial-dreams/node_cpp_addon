#include <node.h>
#include <iostream>
using namespace v8;

void Eval(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ObjectTemplate> global = ObjectTemplate::New(isolate);
  Local<String> key = String::NewFromUtf8(isolate, "CONST", NewStringType::kNormal).ToLocalChecked();
  Local<String> value = String::NewFromUtf8(isolate, "I am global value", NewStringType::kNormal).ToLocalChecked();
  global->Set(key, value);
  Local<String> printStr = String::NewFromUtf8(isolate, "print", NewStringType::kNormal).ToLocalChecked();
  global->Set(printStr, FunctionTemplate::New(isolate, [](const FunctionCallbackInfo<Value>& args) -> void {
    Isolate* isolate = args.GetIsolate();
    for (size_t i = 0; i < args.Length(); i++) {
      Local<String> str = args[i].As<String>();
      String::Utf8Value s(isolate, str);
      std::cout<<*s<<" ";
    }
    std::cout<<std::endl;
  }));
  Local<Context> myContext = Context::New(isolate, nullptr, global);
  Local<String> code = args[0].As<String>();
  Local<Script> script = Script::Compile(myContext, code).ToLocalChecked();
  args.GetReturnValue().Set(script->Run(context).ToLocalChecked());
}

Local<Number> getValue() {
  Isolate* isolate = Isolate::GetCurrent();
  EscapableHandleScope scope(isolate);
  Local<Number> result = Number::New(isolate, 12);
  return scope.Escape(result);
}

void Add(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Number> a = args[0].As<Number>();
  Local<Number> b = args[1].As<Number>();
  double r = a->Value() + b->Value();
  Local<Number> result = Number::New(isolate, r);
  args.GetReturnValue().Set(result);
}

Local<String> ToLocalString(const char* str) {
  Isolate* isolate = Isolate::GetCurrent();
  EscapableHandleScope scope(isolate);
  Local<String> result = String::NewFromUtf8(isolate, str, NewStringType::kNormal).ToLocalChecked();
  return scope.Escape(result);
}

char* ToCString(Local<String> from) {
  Isolate* isolate = Isolate::GetCurrent();
  String::Utf8Value v(isolate, from);
  return *v;
}

void Filter(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  if (!args[0]->IsArray() && !args[1]->IsArray()) {
    isolate->ThrowException(Exception::TypeError(ToLocalString("Type error")));
    return;
  }
  Local<Array> array = args[0].As<Array>();
  Local<Function> fn = args[1].As<Function>();
  Local<Array> result = Array::New(isolate);
  Local<Value> fnArgs[3] = { Local<Value>(), Number::New(isolate, 0), array };
  for (uint32_t i = 0, j = 0; i < array->Length(); i++) {
    fnArgs[0] = array->Get(context, i).ToLocalChecked();
    fnArgs[1] = Number::New(isolate, i);
    Local<Value> v = fn->Call(context, Null(isolate), 3, fnArgs).ToLocalChecked();
    if (v->IsTrue()) {
      result->Set(context, j++, fnArgs[0]).FromJust();
    }
  }
  args.GetReturnValue().Set(result);
}

void CreateObject(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> object = Object::New(isolate);
  Local<String> nameKey = ToLocalString("name");
  Local<String> ageKey = ToLocalString("age");
  Local<String> nameValue = args[0].As<String>();
  Local<Number> ageValue = args[1].As<Number>();
  object->Set(context, nameKey, nameValue).FromJust();
  object->Set(context, ageKey, ageValue).FromJust();
  args.GetReturnValue().Set(object);
}

void Init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "eval", Eval);
  NODE_SET_METHOD(exports, "add", Add);
  NODE_SET_METHOD(exports, "filter", Filter);
  NODE_SET_METHOD(exports, "createObject", CreateObject);
}

NODE_MODULE(addon03, Init)
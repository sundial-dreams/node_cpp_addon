#include <node.h>
#include <iostream>

using namespace v8;

Local<String> ToLocalString(const char* str) {
  Isolate* isolate = Isolate::GetCurrent();
  EscapableHandleScope scope(isolate);
  Local<String> result = String::NewFromUtf8(isolate, str, NewStringType::kNormal).ToLocalChecked();
  return scope.Escape(result);
}

// Person
void Person(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> self = args.This();
  Local<String> nameKey = ToLocalString("name");
  Local<String> ageKey = ToLocalString("age");

  Local<String> nameValue = args[0].As<String>();
  Local<Number> ageValue = args[1].As<Number>();
  self->Set(context, nameKey, nameValue).FromJust();
  self->Set(context, ageKey, ageValue).FromJust();
  args.GetReturnValue().Set(self);
}

void GetName(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  args.GetReturnValue().Set(args.This()->Get(context, ToLocalString("name")).ToLocalChecked());
}

void GetAge(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  args.GetReturnValue().Set(args.This()->Get(context, ToLocalString("age")).ToLocalChecked());
}
// 使用对象模板
void CreateObject(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ObjectTemplate> obj = ObjectTemplate::New(isolate); // 利用对象模板创建空对象
  obj->Set(ToLocalString("name"), args[0].As<String>());
  obj->Set(ToLocalString("age"), args[1].As<Number>());
  args.GetReturnValue().Set(obj->NewInstance(context).ToLocalChecked()); // 实例化这个对象
}

// internal field
struct person {
  person(const char* name, int age): name(name), age(age) {}
  std::string name;
  int age;
};

// with internal field
void GetAll(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Object> self = args.Holder();
  Local<External> wrapper = Local<External>::Cast(self->GetInternalField(0));
  auto p = static_cast<person*>(wrapper->Value());
  char result[1024];
  sprintf(result, "{ name: %s, age: %d }", p->name.c_str(), p->age);
  args.GetReturnValue().Set(ToLocalString(result));
}

// with internal field
void CreateObject2(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<ObjectTemplate> objectTemplate = ObjectTemplate::New(isolate);
  objectTemplate->SetInternalFieldCount(1);
  Local<Object> object = objectTemplate->NewInstance(context).ToLocalChecked();
  Local<String> name = args[0].As<String>();
  Local<Number> age = args[0].As<Number>();
  String::Utf8Value nameValue(isolate, name);

  auto p = new person(*nameValue, int(age->Value()));
  object->SetInternalField(0, External::New(isolate, p));
  Local<Function> getAll = FunctionTemplate::New(isolate, GetAll)->GetFunction(context).ToLocalChecked();
  object->Set(context, ToLocalString("getAll"), getAll).FromJust();
  args.GetReturnValue().Set(object);
}

void Init(Local<Object> exports) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope handleScope(isolate);
  Local<Context> context = isolate->GetCurrentContext();
  Local<FunctionTemplate> person = FunctionTemplate::New(isolate, Person);
  person->SetClassName(ToLocalString("Person"));
  Local<ObjectTemplate> prototype = person->PrototypeTemplate();
  prototype->Set(ToLocalString("getName"), FunctionTemplate::New(isolate, GetName));
  prototype->Set(ToLocalString("getAge"), FunctionTemplate::New(isolate, GetAge));
  exports->Set(context, ToLocalString("Person"), person->GetFunction(context).ToLocalChecked()).FromJust();

  NODE_SET_METHOD(exports, "createObject", CreateObject);
  NODE_SET_METHOD(exports, "createObject2", CreateObject2);
}

NODE_MODULE(addon04, Init)
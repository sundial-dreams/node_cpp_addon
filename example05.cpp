#include <iostream>
#include <string>
#include <node.h>
#include <node_object_wrap.h>

using namespace v8;

Local<String> ToLocalString(const char* str) {
  Isolate* isolate = Isolate::GetCurrent();
  EscapableHandleScope scope(isolate);
  Local<String> result = String::NewFromUtf8(isolate, str, NewStringType::kNormal).ToLocalChecked();
  return scope.Escape(result);
}

// Person Class
class Person : public node::ObjectWrap {
  public:
    static void Init(Local<Object>);

  private:
    explicit Person(const char* name, int age) : name(name), age(age) {}

    ~Person() override = default;

    static void New(const FunctionCallbackInfo<Value>&);

    static void GetName(const FunctionCallbackInfo<Value>&);

    static void GetAge(const FunctionCallbackInfo<Value>&);

    std::string name;
    int age;
};

void Person::Init(Local<Object> exports) {
  Isolate* isolate = exports->GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<ObjectTemplate> dataTemplate = ObjectTemplate::New(isolate);
  dataTemplate->SetInternalFieldCount(1);
  Local<Object> data = dataTemplate->NewInstance(context).ToLocalChecked();
  Local<FunctionTemplate> fnTemplate = FunctionTemplate::New(isolate, New, data);
  fnTemplate->SetClassName(ToLocalString("Person"));
  fnTemplate->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(fnTemplate, "getName", GetName);
  NODE_SET_PROTOTYPE_METHOD(fnTemplate, "getAge", GetAge);

  Local<Function> constructor = fnTemplate->GetFunction(context).ToLocalChecked();
  data->SetInternalField(0, constructor);
  exports->Set(context, ToLocalString("Person"), constructor).FromJust();
}

void Person::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Local<Context> constext = isolate->GetCurrentContext();
  if (args.IsConstructCall()) {
    Local<String> t = args[0].As<String>();
    String::Utf8Value name(isolate, t);
    int age = int(args[1].As<Number>()->Value());
    auto person = new Person(*name, age);
    person->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
    return;
  }
  const int argc = 2;
  Local<Value> argv[argc] = {args[0], args[1]};
  Local<Function> constructor = args.Data().As<Object>()->GetInternalField(0).As<Function>();
  Local<Object> result = constructor->NewInstance(constext, argc, argv).ToLocalChecked();
  args.GetReturnValue().Set(result);
}

void Person::GetAge(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  auto person = node::ObjectWrap::Unwrap<Person>(args.Holder());
  args.GetReturnValue().Set(ToLocalString(person->name.c_str()));
}

void Person::GetName(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HandleScope handleScope(isolate);
  auto person = node::ObjectWrap::Unwrap<Person>(args.Holder());
  args.GetReturnValue().Set(Number::New(isolate, person->age));
}

void Init(Local<Object> exports) {
  Person::Init(exports);
}

NODE_MODULE(addon05, Init)

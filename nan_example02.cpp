#include <iostream>
#include <string>
#include <node.h>
#include <nan.h>

using namespace v8;
// Person Class, extends Nan::ObjectWrap
class Person : public Nan::ObjectWrap {
  public:
    static NAN_MODULE_INIT(Init) { // void Init(Local<Object> exports)
      Local<FunctionTemplate> fnTemplate = Nan::New<FunctionTemplate>(New);
      fnTemplate->SetClassName(Nan::New("Person").ToLocalChecked());
      fnTemplate->InstanceTemplate()->SetInternalFieldCount(1);
      Nan::SetPrototypeMethod(fnTemplate, "getName", GetName);
      Nan::SetPrototypeMethod(fnTemplate, "getAge", GetAge);
      constructor().Reset(Nan::GetFunction(fnTemplate).ToLocalChecked()); // save a constructor function in global
      Nan::Set(target, Nan::New("Person").ToLocalChecked(), Nan::GetFunction(fnTemplate).ToLocalChecked());
    }

  private:
    explicit Person(const char* name, int age) : name(name), age(age) {}

    static NAN_METHOD(New) { // void New(const FunctionCallbackInfo<Value>& args)
      if (info.IsConstructCall()) {
        Local<String> name = info[0].As<String>();
        Local<Number> age = info[1].As<Number>();
        Nan::Utf8String n(name);
        int a = int(age->Value());
        auto person = new Person(*n, a);
        person->Wrap(info.This()); // like node::ObjectWrap
        info.GetReturnValue().Set(info.This());
        return;
      }
      const int argc = 2;
      Local<Value> argv[argc] = {info[0], info[1]};
      Local<Function> c = Nan::New(constructor());
      info.GetReturnValue().Set(Nan::NewInstance(c, argc, argv).ToLocalChecked());
    }

    static NAN_METHOD(GetName) {
      auto person = static_cast<Person*>(Nan::ObjectWrap::Unwrap<Person>(info.Holder()));
      info.GetReturnValue().Set(Nan::New(person->name.c_str()).ToLocalChecked());
    }

    static NAN_METHOD(GetAge) {
      auto person = static_cast<Person*>(Nan::ObjectWrap::Unwrap<Person>(info.Holder()));
      info.GetReturnValue().Set(Nan::New(person->age));
    }
    // define a global handle of constructor
    static inline Nan::Global<Function>& constructor() {
      static Nan::Global<Function> _constructor; // global value
      return _constructor;
    }

    std::string name;
    int age;
};

NODE_MODULE(nan_addon02, Person::Init)

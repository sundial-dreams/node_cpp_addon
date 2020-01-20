#include <iostream>
#include <napi.h>
using namespace Napi;

Array Filter(const CallbackInfo& info) {
  Env env = info.Env();
  Array result = Array::New(env);
  Array arr = info[0].As<Array>();
  Function fn = info[1].As<Function>();
  for (uint32_t i = 0, j = 0; i < arr.Length(); i++) {
    Boolean ret = fn.Call(arr, {arr.Get(i), Number::New(env, i)}).As<Boolean>();
    if (ret.Value()) {
      result.Set(j++, arr.Get(i));
    }
  }
  return result;
}

Object CreateObject(const CallbackInfo& info) {
  Env env = info.Env();
  Object object = Object::New(env);
  PropertyDescriptor nameProp = PropertyDescriptor::Value("name", info[0], napi_enumerable);
  PropertyDescriptor ageProp = PropertyDescriptor::Value("age", info[1], napi_enumerable);
  PropertyDescriptor getAllFn = PropertyDescriptor::Function("getAll", [](const CallbackInfo& args) -> void {
    Object self = args.This().ToObject();
    std::string name = self.Get("name").As<String>(); //隐式类型转换
    int age = self.Get("age").As<Number>();
    std::cout<<name<<" "<<age<<std::endl;
  });
  object.DefineProperties({ nameProp, ageProp, getAllFn });
  return object;
}

Object Init(Env env, Object exports) {
  exports.Set("filter", Function::New(env, Filter));
  exports.Set("createObject", Function::New(env, CreateObject));
  return exports;
}

NODE_API_MODULE(addon, Init)

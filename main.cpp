#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <node.h>
#include <node_object_wrap.h>
#include "lib/utils.h"
#include "lib/bigNumber.h"
const int N = 10000;
using namespace v8;
// 对findSubStr(const char*, const char*)的包装
void FindSubStr(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  if (!args[0]->IsString() || !args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(MakeString("type error")));
  }
  // 将Local<String> 转化到 char*类型，下文会介绍
  String::Utf8Value str(isolate, args[0].As<String>());
  String::Utf8Value subStr(isolate, args[1].As<String>());
  int i = findSubStr(*str, *subStr);
  args.GetReturnValue().Set(Number::New(isolate, i));
}

// 对 subStrCount(const char*, const char*)的包装
void SubStrCount(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  if (!args[0]->IsString() || !args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(MakeString("type error")));
  }
  // 将Local<String> 转化到 char*类型，下文会介绍
  String::Utf8Value str(isolate, args[0].As<String>());
  String::Utf8Value subStr(isolate, args[1].As<String>());
  int i = subStrCount(*str, *subStr); // 调用c++侧的方法
  args.GetReturnValue().Set(Number::New(isolate, i));
}

void Init(Local<Object> exports) {
  // 暴露出两个函数
  NODE_SET_METHOD(exports, "findSubStr", FindSubStr);
  NODE_SET_METHOD(exports, "subStrCount", SubStrCount);
  // 利用BigNumber的Init静态方法来暴露BigNumber类
  BigNumber::Init(exports);
}

NODE_MODULE(addon, Init)
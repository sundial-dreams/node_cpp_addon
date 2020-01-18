#ifndef NODE_CPP_ADDON_BIGNUMBER_H
#define NODE_CPP_ADDON_BIGNUMBER_H

#include <iostream>
#include <stdio.h>
#include <node.h>
#include <node_object_wrap.h>
using namespace v8;
class BigNumber: node::ObjectWrap {
  public:
    static void Init(Local<Object>); // Init函数
  private:
    explicit BigNumber(const char* value): value(value) {} // 构造函数
    ~BigNumber() override = default;
    static void New(const FunctionCallbackInfo<Value>&); // New
    static void Val(const FunctionCallbackInfo<Value>&); // 返回值
    static void Add(const FunctionCallbackInfo<Value>&); // 相加
    static void Multiply(const FunctionCallbackInfo<Value>&); // 相乘
    std::string value; // 用一个std::string 来存
};

Local<String> MakeString(const char* str);

#endif //NODE_CPP_ADDON_BIGNUMBER_H

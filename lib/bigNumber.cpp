#include <iostream>
#include <stack>
#include <string>
#include <algorithm>
#include <node.h>
#include <node_object_wrap.h>
#include "bigNumber.h"

Local<String> ToLocalString(const char* str) {
  Isolate* isolate = Isolate::GetCurrent();
  EscapableHandleScope scope(isolate);
  Local<String> s = String::NewFromUtf8(isolate, str, NewStringType::kNormal).ToLocalChecked();
  return scope.Escape(s);
}

char* ToCString(Local<String> from) {
  Isolate* isolate = Isolate::GetCurrent();
  String::Utf8Value v(isolate, from);
  return *v;
}

void FillZero(std::string& from, int length) {
  while (length--) {
    from.insert(from.begin(), '0');
  }
}
void RightFillZero(std::string& from, int length) {
  while(length--) {
    from.push_back('0');
  }
}

void NativeAdd(std::string& a, std::string& b) {
  FillZero(a, std::max(a.size(), b.size()) - a.size());
  FillZero(b, std::max(a.size(), b.size()) - b.size());
  std::string result;
  int t = 0;
  for (int i = a.size() - 1; i >= 0; i--) {
    int v1 = a[i] - '0';
    int v2 = b[i] - '0';
    int v = v1 + v2 + t;
    if (v > 10) {
      t = v / 10;
      v = v % 10;
    } else {
      t = 0;
    }
    result.insert(result.begin(), v + '0');
  }
  if (t) result.insert(result.begin(), t + '0');
  a = result;
}


void NativeMultiply(std::string& a, std::string& b) {
  FillZero(a, std::max(a.size(), b.size()) - a.size());
  FillZero(b, std::max(a.size(), b.size()) - b.size());
  std::string result;
  for (int i = a.length() - 1; i >= 0; i-- ) {
    std::string t;
    int sum = 0;
    int v1 = a[i] - '0';
    for (int j = b.length() - 1; j >= 0; j--) {
      int v2 = b[j] - '0';
      int v = v1 * v2 + sum;
      if (v > 10) {
        sum = v / 10;
        v = v % 10;
      } else {
        sum = 0;
      }
      t.insert(t.begin(), v + '0');
    }
    if (sum) t.insert(t.begin(), sum + '0');
    RightFillZero(t, a.size() - i - 1);
    NativeAdd(result, t);
  }
  a = result;
}

void BigNumber::Init(Local<Object> exports) {
  Isolate* isolate = exports->GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<ObjectTemplate> dataTemplate = ObjectTemplate::New(isolate);
  dataTemplate->SetInternalFieldCount(1);
  Local<Object> data = dataTemplate->NewInstance(context).ToLocalChecked();
  Local<FunctionTemplate> fnTemplate = FunctionTemplate::New(isolate, New, data);
  fnTemplate->SetClassName(ToLocalString("BigNumber"));
  fnTemplate->InstanceTemplate()->SetInternalFieldCount(1);
  NODE_SET_PROTOTYPE_METHOD(fnTemplate, "add", Add);
  NODE_SET_PROTOTYPE_METHOD(fnTemplate, "multiply", Multiply);
  NODE_SET_PROTOTYPE_METHOD(fnTemplate, "val", Val);
  Local<Function> constructor = fnTemplate->GetFunction(context).ToLocalChecked();
  data->SetInternalField(0, constructor);
  exports->Set(context, ToLocalString("BigNumber"), constructor).FromJust();
}

void BigNumber::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  if (args.IsConstructCall()) {
    Local<String> val = args[0].As<String>();
    auto bigNumber = new BigNumber(ToCString(val));
    bigNumber->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
    return;
  }
  const int argc = 1;
  Local<Value> argv[] = {args[0]};
  Local<Function> constructor = args.Data().As<Object>()->GetInternalField(0).As<Function>();
  Local<Object> result = constructor->NewInstance(context, argc, argv).ToLocalChecked();
  args.GetReturnValue().Set(result);
}

void BigNumber::Add(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  auto bigNumber = node::ObjectWrap::Unwrap<BigNumber>(args.Holder());
  if (args[0]->IsString()) {
    Local<String> val = args[0].As<String>();
    std::string other(ToCString(val));
    NativeAdd(bigNumber->value, other);
    args.GetReturnValue().Set(ToLocalString(bigNumber->value.c_str()));
    return;
  }
  if (args[0]->IsNumber()) {
    int number = int(args[0].As<Number>()->NumberValue(context).FromJust());
    std::string other = std::to_string(number);
    NativeAdd(bigNumber->value, other);
    args.GetReturnValue().Set(ToLocalString(bigNumber->value.c_str()));
    return;
  }
  if (args[0]->IsObject()) {
    Local<Object> object = args[0].As<Object>();
    Local<Function> getVal = object->Get(context, ToLocalString("val")).ToLocalChecked().As<Function>();
    Local<Value> e;
    Local<String> val = getVal->Call(context, args.Holder(), 0, &e).ToLocalChecked().As<String>();
    std::string other(ToCString(val));
    NativeAdd(bigNumber->value, other);
    args.GetReturnValue().Set(ToLocalString(bigNumber->value.c_str()));
    return;
  }

  isolate->ThrowException(Exception::TypeError(ToLocalString("Type error")));
}

void BigNumber::Multiply(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  auto bigNumber = node::ObjectWrap::Unwrap<BigNumber>(args.Holder());
  if (args[0]->IsString()) {
    Local<String> str = args[0].As<String>();
    std::string other(ToCString(str));
    NativeMultiply(bigNumber->value, other);
    args.GetReturnValue().Set(ToLocalString(bigNumber->value.c_str()));
    return;
  }
  if (args[0]->IsNumber()) {
    int number = int (args[0].As<Number>()->NumberValue(context).FromJust());
    std::string other = std::to_string(number);
    NativeMultiply(bigNumber->value, other);
    args.GetReturnValue().Set(ToLocalString(bigNumber->value.c_str()));
    return;
  }
  if (args[0]->IsObject()) {
    Local<Object> obj = args[0].As<Object>();
    Local<Function> getVal = obj->Get(context, ToLocalString("val")).ToLocalChecked().As<Function>();
    Local<Value> e;
    Local<String> str = getVal->Call(context, args.Holder(), 0, &e).ToLocalChecked().As<String>();
    std::string other(ToCString(str));
    NativeMultiply(bigNumber->value, other);
    args.GetReturnValue().Set(ToLocalString(bigNumber->value.c_str()));
    return;
  }
  isolate->ThrowException(Exception::TypeError(ToLocalString("Type error")));
}

void BigNumber::Val(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  auto bigNumber = node::ObjectWrap::Unwrap<BigNumber>(args.Holder());
  args.GetReturnValue().Set(ToLocalString(bigNumber->value.c_str()));
}
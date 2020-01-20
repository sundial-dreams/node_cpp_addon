#include <iostream>
#include <stdio.h>
#include <node_api.h>
#define NAPI_CALL(env, call)                                    \
      do {                                                      \
        napi_status status = (call);                            \
        if (status != napi_ok) {                                \
          const napi_extended_error_info* error_info = nullptr; \
          napi_get_last_error_info((env), &error_info);         \
          bool is_pending;                                      \
          napi_is_exception_pending((env), &is_pending);        \
          if (!is_pending) {                                    \
            const char* message = error_info->error_message;    \
            napi_throw_error((env), "ERROR", message);          \
            return nullptr;                                     \
          }                                                     \
        }                                                       \
      } while(0)


napi_value filter(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value argv[2];
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
  napi_value arr = argv[0], fn = argv[1];
  uint32_t length = 0;
  NAPI_CALL(env, napi_get_array_length(env, arr, &length));
  napi_value result = nullptr;
  NAPI_CALL(env, napi_create_array(env, &result));
  napi_value fn_argv[3] = {nullptr, nullptr, arr};
  for (uint32_t i = 0, j = 0; i < length; i++) {
    napi_value fn_ret = nullptr;
    uint32_t fn_argc = 3;
    napi_value index = nullptr, arr_val = nullptr;
    NAPI_CALL(env, napi_get_element(env, argv[0], i, &arr_val));
    NAPI_CALL(env, napi_create_int32(env, (int32_t) i, &index));
    fn_argv[0] = arr_val;
    fn_argv[1] = index;
    NAPI_CALL(env, napi_call_function(env, arr, fn, fn_argc, fn_argv, &fn_ret));
    bool ret;
    NAPI_CALL(env, napi_get_value_bool(env, fn_ret, &ret));
    if (ret) {
      NAPI_CALL(env, napi_set_element(env, result, j++, arr_val));
    }
  }
  return result;
}

napi_value hello(napi_env env, napi_callback_info info) {
  napi_value str = nullptr;
  NAPI_CALL(env, napi_create_string_utf8(env, "hello world", NAPI_AUTO_LENGTH, &str));
  std::cout<<"c++ say: hello world!"<<std::endl;
  return str;
}

napi_value make_string(napi_env env, const char* name) {
  napi_escapable_handle_scope scope;
  NAPI_CALL(env, napi_open_escapable_handle_scope(env, &scope));
  napi_value str = nullptr;
  NAPI_CALL(env, napi_create_string_utf8(env, name, NAPI_AUTO_LENGTH, &str));
  napi_value ret = nullptr;
  NAPI_CALL(env, napi_escape_handle(env, scope, str, &ret));
  NAPI_CALL(env, napi_close_escapable_handle_scope(env, scope));
  return ret;
}

napi_value create_object(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value object = nullptr, args[2];
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));

  NAPI_CALL(env, napi_create_object(env, &object));
  napi_property_descriptor descriptors[] = {
      {"name", 0 ,0 ,0 ,0, args[0], napi_enumerable, 0},
      {"age", 0, 0, 0, 0, args[1], napi_enumerable, 0}
  };
  NAPI_CALL(env, napi_define_properties(env, object, 2, descriptors));
  return object;
}

void set_function(napi_env env, napi_value* target, const char* name, napi_callback cb) {
  napi_value fn = nullptr;
  napi_create_function(env, name, NAPI_AUTO_LENGTH, cb, nullptr, &fn);
  napi_set_named_property(env, *target, name, fn);
}

napi_value person(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value args[2], self = nullptr, target = nullptr;
  NAPI_CALL(env, napi_get_new_target(env, info, &target));
  if (target == nullptr) {
    NAPI_CALL(env, napi_throw_error(env, "ERROR", "need new"));
  }
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &self, nullptr));
  NAPI_CALL(env, napi_set_named_property(env, self, "name", args[0]));
  NAPI_CALL(env, napi_set_named_property(env, self, "age", args[1]));
  return self;
}

napi_value create_person(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value args[2], instance = nullptr, constructor = nullptr;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
  NAPI_CALL(env, napi_create_function(env, "Person", NAPI_AUTO_LENGTH, person, nullptr, &constructor));
  NAPI_CALL(env, napi_new_instance(env, constructor, argc, args, &instance));
  return instance;
}

napi_value init(napi_env env, napi_value exports) {
  set_function(env, &exports, "filter", filter);
  set_function(env, &exports, "hello", hello);
  set_function(env, &exports, "createObject", create_object);
  set_function(env, &exports, "Person", person);
  set_function(env, &exports, "createPerson", create_person);
  return exports;
}

NAPI_MODULE(napi_addon01, init)
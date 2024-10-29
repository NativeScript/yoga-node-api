#pragma once

#include "js_native_api.h"

#define NAPI_FUNCTION(name)                                                    \
  napi_value name(napi_env env, napi_callback_info cbinfo)

#define NAPI_METHOD(clsname, method)                                           \
  {#method, 0, clsname##_##method, 0, 0, 0, napi_writable, 0}

#define NAPI_STATIC_METHOD(clsname, method)                                    \
  {#method, 0, clsname##_##method, 0, 0, 0, napi_static, 0}

#define NAPI_VALUE(name) {#name, 0, 0, 0, 0, name, napi_enumerable, 0}

#define NAPI_FINALIZER(name) void name(napi_env env, void *data, void *hint)

#define NAPI_METHOD_HEADER(objtype, objname, argcount)                         \
  napi_value jsThis;                                                           \
  size_t argc = argcount;                                                      \
  napi_value argv[argcount];                                                   \
  napi_get_cb_info(env, cbinfo, &argc, argv, &jsThis, NULL);                   \
  objtype objname = (objtype)unwrap(env, jsThis)

#define NAPI_METHOD_HEADER_NO_ARGS(objtype, objname)                           \
  napi_value jsThis;                                                           \
  napi_get_cb_info(env, cbinfo, NULL, NULL, &jsThis, NULL);                    \
  objtype objname = (objtype)unwrap(env, jsThis)

#define NAPI_ARG_INT32(name, index)                                            \
  int32_t name = 0;                                                            \
  napi_get_value_int32(env, argv[index], &name)

#define NAPI_ARG_BOOL(name, index)                                             \
  bool name = false;                                                           \
  napi_get_value_bool(env, argv[index], &name)

#define NAPI_ARG_DOUBLE(name, index)                                           \
  double name = 0;                                                             \
  napi_get_value_double(env, argv[index], &name)

inline void *unwrap(napi_env env, napi_value jsobj) {
  void *data;
  napi_unwrap(env, jsobj, &data);
  return data;
}

inline napi_value js_bool(napi_env env, bool value) {
  napi_value result;
  napi_get_boolean(env, value, &result);
  return result;
}

inline napi_value js_int32(napi_env env, int32_t value) {
  napi_value result;
  napi_create_int32(env, value, &result);
  return result;
}

inline napi_value js_double(napi_env env, double value) {
  napi_value result;
  napi_create_double(env, value, &result);
  return result;
}

inline napi_value defineClass(napi_env env, const char *name,
                              napi_callback constructor,
                              napi_property_descriptor *props, size_t propc) {
  napi_value result;
  napi_define_class(env, name, NAPI_AUTO_LENGTH, constructor, NULL, propc,
                    props, &result);
  return result;
}

#define DEFINE_CLASS(name, propc)                                              \
  napi_value name =                                                            \
      defineClass(env, #name, name##_constructor, name##_props, propc)

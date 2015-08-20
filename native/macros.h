#ifndef LIBNICE_MACROS_H
#define LIBNICE_MACROS_H

#define PROTO_RDONLY(class, propName, fieldName) \
    Nan::SetAccessor(                            \
      tpl->PrototypeTemplate()                   \
    , Nan::New(propName).ToLocalChecked()        \
    , class::Get##fieldName)                     \

#define PROTO_GETSET(class, propName, fieldName) \
    Nan::SetAccessor(                            \
      tpl->PrototypeTemplate()                   \
    , Nan::New(propName).ToLocalChecked()        \
    , class::Get##fieldName                      \
    , class::Set##fieldName)                     \

#define PROTO_METHOD(class, propName, fieldName) \
    Nan::SetPrototypeMethod(                     \
      tpl                                        \
    , propName                                   \
    , class::fieldName)                          \

#define MODULE_EXPORT(class)           \
    exports->Set(                      \
      Nan::New(class).ToLocalChecked() \
    , tpl->GetFunction())              \

#define IMPL_GETTER_VAL(class, gobj, prop, field, type)  \
  NAN_GETTER(class::Get##field) {                        \
    type val;                                            \
    class* obj = ObjectWrap::Unwrap<class>(info.This()); \
    const gchar* property_name = prop;                   \
    g_object_get(                                        \
      G_OBJECT(obj->gobj)                                \
    , property_name, &val                                \
    , NULL);                                             \
    info.GetReturnValue().Set(Nan::New(val));            \
  }                                                      \

#define IMPL_SETTER_VAL(class, gobj, prop, field, conv)  \
  NAN_SETTER(class::Set##field) {                        \
    class* obj = ObjectWrap::Unwrap<class>(info.This()); \
    const gchar* property_name = prop;                   \
    g_object_set(                                        \
      G_OBJECT(obj->gobj)                                \
    , property_name, value->conv()                       \
    , NULL);                                             \
  }                                                      \

#define IMPL_GETSET_BOOL(class, gobj, prop, field)        \
  IMPL_GETTER_VAL(class, gobj, prop, field, bool)         \
  IMPL_SETTER_VAL(class, gobj, prop, field, BooleanValue) \

#define IMPL_GETSET_UINT32(class, gobj, prop, field)     \
  IMPL_GETTER_VAL(class, gobj, prop, field, uint32_t)    \
  IMPL_SETTER_VAL(class, gobj, prop, field, Uint32Value) \

#define IMPL_GETTER_STR(class, gobj, prop, field)        \
  NAN_GETTER(class::Get##field) {                        \
    gchar* value;                                        \
    class* obj = ObjectWrap::Unwrap<class>(info.This()); \
                                                         \
    const gchar* property_name = prop;                   \
                                                         \
    g_object_get(                                        \
      G_OBJECT(obj->gobj)                                \
    , property_name, &value                              \
    , NULL);                                             \
                                                         \
    if (value != NULL) {                                 \
      info.GetReturnValue().Set(                         \
        Nan::New(value)                                  \
          .ToLocalChecked());                            \
      g_free(value);                                     \
    }                                                    \
  }                                                      \

#define IMPL_SETTER_STR(class, gobj, prop, field)        \
  NAN_SETTER(class::Set##field) {                        \
    class* obj = ObjectWrap::Unwrap<class>(info.This()); \
                                                         \
    const gchar* property_name = prop;                   \
    v8::String::Utf8Value param(value->ToString());      \
    std::string from = std::string(*param);              \
                                                         \
    g_object_set(                                        \
      G_OBJECT(obj->gobj)                                \
    , property_name, from.c_str()                        \
    , NULL);                                             \
  }                                                      \

#define IMPL_GETSET_STR(class, gobj, prop, field) \
  IMPL_GETTER_STR(class, gobj, prop, field)       \
  IMPL_SETTER_STR(class, gobj, prop, field)       \

#define RUN_GLIB(ctx, func, data)\
  GSource* source = g_idle_source_new();\
  g_source_set_callback(\
    source\
  , (GSourceFunc) func\
  , data\
  , NULL);\
  g_source_attach(source, ctx);\

#define GLIB_CALLBACK(name)\
  gboolean name(gpointer user_data)

#define GLIB_CALLBACK_RETURN\
  return G_SOURCE_REMOVE;

#endif

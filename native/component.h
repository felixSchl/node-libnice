#ifndef LIBNICE_COMPONENT_H
#define LIBNICE_COMPONENT_H

#include <v8.h>
#include <nan.h>
#include <glib.h>
#include <nice/nice.h>
#include "macros.h"

namespace libnice {

  class Component : public Nan::ObjectWrap {

    public:
      static void Init(v8::Handle<v8::Object> exports);
      static Nan::Persistent<v8::Function> constructor;

    private:
      explicit Component(v8::Local<v8::Object> stream);
      ~Component();

      static NAN_METHOD(New);
  };
}

#endif


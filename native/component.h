#ifndef LIBNICE_COMPONENT_H
#define LIBNICE_COMPONENT_H

#include <v8.h>
#include <nan.h>
#include <glib.h>
#include <nice/nice.h>
#include "macros.h"

namespace libnice {

  class Component : public Nan::ObjectWrap {

    friend class Agent;
    friend class Stream;

    public:
      static void Init(v8::Handle<v8::Object> exports);
      static Nan::Persistent<v8::Function> constructor;

    private:
      explicit Component(v8::Local<v8::Object> stream, int id);
      ~Component();

      int id;

      static NAN_METHOD(New);
      static NAN_METHOD(Send);
      static NAN_GETTER(GetId);

      void onStateChanged(guint state);
      void onReliableTransportWritable();
      void onData(const char* buf, size_t size);

      Nan::Persistent<v8::Object> stream;
  };
}

#endif


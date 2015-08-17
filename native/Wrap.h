#ifndef LIBNICE_WRAP_H
#define LIBNICE_WRAP_H

#ifdef WIN32
#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif
#endif

#include <nan.h>
#include <node_object_wrap.h>

namespace libnice {
  class NiceWrap : public Nan::ObjectWrap {
   public:
    static void Init(v8::Handle<v8::Object> exports);
};

#endif

#ifndef LIBNICE_AGENT_H
#define LIBNICE_AGENT_H

#include <v8.h>
#include <nan.h>

namespace libnice {
  class Agent : Nan::ObjectWrap {

    public:
      static void Init(v8::Handle<v8::Object> exports);

    private:
      static Nan::Persistent<v8::Function> constructor;
      static NAN_METHOD(New);
      static NAN_METHOD(GatherCandidates);
  };
}

#endif

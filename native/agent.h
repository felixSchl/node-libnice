#ifndef LIBNICE_AGENT_H
#define LIBNICE_AGENT_H

#include <v8.h>
#include <nan.h>
#include <glib.h>
#include <nice/nice.h>

namespace libnice {
  class Agent : public Nan::ObjectWrap {

    public:
      static void Init(v8::Handle<v8::Object> exports);

    private:
      explicit Agent();

      static Nan::Persistent<v8::Function> constructor;

      static NAN_METHOD(New);
      static NAN_METHOD(GatherCandidates);

      static NAN_GETTER(GetControllingMode);
      static NAN_SETTER(SetControllingMode);

      GMainLoop* main_loop;
      GMainContext* main_context;
      NiceAgent* agent;
  };
}

#endif

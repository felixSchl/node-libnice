#ifndef LIBNICE_STREAM_H
#define LIBNICE_STREAM_H

#include <v8.h>
#include <nan.h>
#include <glib.h>
#include <nice/nice.h>
#include "macros.h"

namespace libnice {

  class Agent;

  class Stream : public Nan::ObjectWrap {

    friend class Agent;
    friend class Component;

    public:
      static void Init(v8::Handle<v8::Object> exports);
      static Nan::Persistent<v8::Function> constructor;

    private:
      explicit Stream(
        v8::Local<v8::Object> self
      , v8::Local<v8::Object> agent
      , int stream_id
      , int num_components);
      ~Stream();

      struct GatherCandidatesArgs {
        friend class Agent;
        friend class Stream;
        Agent* agent;
        Stream* stream;
        Nan::Callback* callback;
      };

      /**
       * Glib idle functions
       */

      static GLIB_CALLBACK(RunGatherCandidates);

      static NAN_METHOD(New);
      static NAN_METHOD(GatherCandidates);
      static NAN_METHOD(Send);
      static NAN_GETTER(GetComponents);
      static NAN_GETTER(GetId);
      static NAN_GETTER(GetName);
      static NAN_SETTER(SetName);

      void onData(int component, const char* buf, size_t size);
      void onGatheringDone();
      void onStateChanged(guint state, guint component_id);

      Nan::Persistent<v8::Object> agent;
      Nan::Persistent<v8::Object> components;

      int stream_id;
      int num_components;
  };
}

#endif

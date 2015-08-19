#ifndef LIBNICE_STREAM_H
#define LIBNICE_STREAM_H

#include <v8.h>
#include <nan.h>
#include <glib.h>
#include <nice/nice.h>

namespace libnice {
  class Stream : public Nan::ObjectWrap {

    friend class Agent;

    public:
      static void Init(v8::Handle<v8::Object> exports);
      static Nan::Persistent<v8::Function> constructor;

    private:
      explicit Stream(
        v8::Local<v8::Object> nice_agent
      , int stream_id
      , int num_components);
      ~Stream();

      static NAN_METHOD(New);
      static NAN_METHOD(GatherCandidates);

      void onGatheringDone();
      Nan::Persistent<v8::Object> agent;
      int stream_id;
      int num_components;
  };
}

#endif

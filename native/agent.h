#ifndef LIBNICE_AGENT_H
#define LIBNICE_AGENT_H

#include <uv.h>
#include <v8.h>
#include <nan.h>
#include <deque>
#include <mutex>
#include <thread>
#include <glib.h>
#include <nice/nice.h>
#include "stream.h"

namespace libnice {

  class Agent : public Nan::ObjectWrap {

    friend class Stream;

    public:
      static void Init(v8::Handle<v8::Object> exports);

    private:
      explicit Agent();
      ~Agent();

      struct ParseRemoteSdpArgs {
        friend class Agent;
        Agent* agent;
        std::string* sdp;
        Nan::Callback* callback;
      };

      static Nan::Persistent<v8::Function> constructor;

      static NAN_METHOD(New);
      static NAN_METHOD(AddStream);
      static NAN_METHOD(GenerateLocalSdp);
      static NAN_METHOD(ParseRemoteSdp);

      static NAN_GETTER(GetStreams);
      static NAN_GETTER(GetControllingMode);
      static NAN_SETTER(SetControllingMode);
      static NAN_GETTER(GetIceTcp);
      static NAN_SETTER(SetIceTcp);
      static NAN_GETTER(GetIceUdp);
      static NAN_SETTER(SetIceUdp);
      static NAN_GETTER(GetKeepAliveConnCheck);
      static NAN_SETTER(SetKeepAliveConnCheck);
      static NAN_GETTER(GetMaxConnectivityChecks);
      static NAN_SETTER(SetMaxConnectivityChecks);
      static NAN_GETTER(GetProxyIp);
      static NAN_SETTER(SetProxyIp);
      static NAN_GETTER(GetProxyPassword);
      static NAN_SETTER(SetProxyPassword);
      static NAN_GETTER(GetProxyPort);
      static NAN_SETTER(SetProxyPort);
      static NAN_GETTER(GetProxyType);
      static NAN_SETTER(SetProxyType);
      static NAN_GETTER(GetProxyUsername);
      static NAN_SETTER(SetProxyUsername);
      static NAN_GETTER(GetStunPacingTimer);
      static NAN_SETTER(SetStunPacingTimer);
      static NAN_GETTER(GetStunServer);
      static NAN_SETTER(SetStunServer);
      static NAN_GETTER(GetStunServerPort);
      static NAN_SETTER(SetStunServerPort);
      static NAN_GETTER(GetUpnp);
      static NAN_SETTER(SetUpnp);
      static NAN_GETTER(GetUpnpTimeout);
      static NAN_SETTER(SetUpnpTimeout);

      GMainLoop* main_loop;
      GMainContext* main_context;
      NiceAgent* nice_agent;
      Nan::Persistent<v8::Object> streams;

      uv_async_t *async;
      uv_mutex_t async_lock;

      std::thread thread;
      std::mutex work_mutex;
      std::deque<std::function<void(void)>> work_queue;

      void run(const std::function<void(void)>& fun);
      static NAUV_WORK_CB(work);

      /**
       * Glib idle functions
       */

      static GLIB_CALLBACK(RunParseRemoteSdp);

      /**
       * Signal callbacks
       */

      static void onGatheringDone(
        NiceAgent *nice_agent
      , guint stream_id
      , gpointer user_data);

      static void onStateChanged(
        NiceAgent *nice_agent
      , guint stream_id
      , guint component_id
      , guint state
      , gpointer user_data);

      static void onNewCandidateFull(
        NiceAgent* nice_agent
      , NiceCandidate* state
      , gpointer user_data);

      static void receive(
        NiceAgent* agent
      , guint stream_id
      , guint component_id
      , guint len
      , gchar* buf
      , gpointer user_data);
  };

}

#endif

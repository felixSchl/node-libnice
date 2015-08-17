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
      static NAN_GETTER(GetStunServer);
      static NAN_SETTER(SetStunServer);
      static NAN_GETTER(GetStunServerPort);
      static NAN_SETTER(SetStunServerPort);

      GMainLoop* main_loop;
      GMainContext* main_context;
      NiceAgent* agent;
  };
}

#endif

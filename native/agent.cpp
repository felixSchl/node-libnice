#include <nan.h>
#include <iostream>
#include "macros.h"
#include "agent.h"

namespace libnice {

  Agent::Agent() {
    this->main_loop = g_main_loop_new(NULL, FALSE);
    this->main_context = g_main_loop_get_context(main_loop);
    this->agent = nice_agent_new(
        this->main_context
      , NICE_COMPATIBILITY_RFC5245
    );
  }


  Nan::Persistent<v8::Function> Agent::constructor;

  void Agent::Init(v8::Handle<v8::Object> exports) {
    Nan::HandleScope scope;

    /**
     * Prepare constructor template
     */
    auto tpl = Nan::New<v8::FunctionTemplate>(Agent::New);
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    tpl->SetClassName(Nan::New("Agent").ToLocalChecked());

    /**
     * Prototype
     */

    PROTO_METHOD(Agent, "gatherCandidates", GatherCandidates);
    PROTO_GETSET(Agent, "controllingMode", ControllingMode);
    PROTO_GETSET(Agent, "iceTcp", IceTcp);
    PROTO_GETSET(Agent, "iceUdp", IceTcp);
    PROTO_GETSET(Agent, "keepaliveConncheck", KeepAliveConnCheck);
    PROTO_GETSET(Agent, "maxConnectivityChecks", MaxConnectivityChecks);
    PROTO_GETSET(Agent, "proxyIp", ProxyIp);
    PROTO_GETSET(Agent, "proxyPassword", ProxyPassword);
    PROTO_GETSET(Agent, "proxyPort", ProxyPort);
    PROTO_GETSET(Agent, "proxyType", ProxyType);
    PROTO_GETSET(Agent, "proxyUsername", ProxyUsername);
    PROTO_GETSET(Agent, "stunServer", StunServer);
    PROTO_GETSET(Agent, "stunServerPort", StunServerPort);

    /**
     * Export Agent
     */

    MODULE_EXPORT("Agent");

    /**
     * Tie the knot
     */

    Agent::constructor.Reset(tpl->GetFunction());
  }

  NAN_METHOD(Agent::New) {
    Agent* agent = new Agent();
    agent->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  }

  void Agent::GatherCandidates(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.Holder());
  }

  IMPL_GETSET_BOOL(Agent, agent, "controlling-mode", ControllingMode)
  IMPL_GETSET_BOOL(Agent, agent, "ice-tcp", IceTcp)
  IMPL_GETSET_BOOL(Agent, agent, "ice-udp", IceUdp)
  IMPL_GETSET_BOOL(Agent, agent, "keepalive-conncheck", KeepAliveConnCheck)
  IMPL_GETSET_UINT32(Agent, agent, "max-connectivity-checks", MaxConnectivityChecks)
  IMPL_GETSET_STR(Agent, agent, "proxy-ip", ProxyIp)
  IMPL_GETSET_STR(Agent, agent, "proxy-password", ProxyPassword)
  IMPL_GETSET_UINT32(Agent, agent, "proxy-port", ProxyPort)
  IMPL_GETSET_UINT32(Agent, agent, "proxy-type", ProxyType)
  IMPL_GETSET_STR(Agent, agent, "proxy-username", ProxyUsername)
  IMPL_GETSET_STR(Agent, agent, "stun-server", StunServer)
  IMPL_GETSET_UINT32(Agent, agent, "stun-server-port", StunServerPort)
}

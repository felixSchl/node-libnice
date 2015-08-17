#include <nan.h>
#include <iostream>
#include "Agent.h"

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
    auto proto = tpl->PrototypeTemplate();

    /**
     * Prototype
     */
    Nan::SetPrototypeMethod(
      tpl
    , "gatherCandidates"
    , Agent::GatherCandidates);

    /**
     * Properties
     */

    Nan::SetAccessor(
      proto
    , Nan::New("controllingMode").ToLocalChecked()
    , Agent::GetControllingMode
    , Agent::SetControllingMode);

    Nan::SetAccessor(
      proto
    , Nan::New("iceTcp").ToLocalChecked()
    , Agent::GetIceTcp
    , Agent::SetIceTcp);

    Nan::SetAccessor(
      proto
    , Nan::New("iceUdp").ToLocalChecked()
    , Agent::GetIceUdp
    , Agent::SetIceUdp);

    Nan::SetAccessor(
      proto
    , Nan::New("keepaliveConncheck").ToLocalChecked()
    , Agent::GetKeepAliveConnCheck
    , Agent::SetKeepAliveConnCheck);

    Nan::SetAccessor(
      proto
    , Nan::New("maxConnectivityChecks").ToLocalChecked()
    , Agent::GetMaxConnectivityChecks
    , Agent::SetMaxConnectivityChecks);

    Nan::SetAccessor(
      proto
    , Nan::New("proxyIp").ToLocalChecked()
    , Agent::GetProxyIp
    , Agent::SetProxyIp);

    Nan::SetAccessor(
      proto
    , Nan::New("proxyPassword").ToLocalChecked()
    , Agent::GetProxyPassword
    , Agent::SetProxyPassword);

    Nan::SetAccessor(
      proto
    , Nan::New("proxyPort").ToLocalChecked()
    , Agent::GetProxyPort
    , Agent::SetProxyPort);

    Nan::SetAccessor(
      proto
    , Nan::New("proxyType").ToLocalChecked()
    , Agent::GetProxyType
    , Agent::SetProxyType);

    Nan::SetAccessor(
      proto
    , Nan::New("proxyUsername").ToLocalChecked()
    , Agent::GetProxyUsername
    , Agent::SetProxyUsername);

    Nan::SetAccessor(
      proto
    , Nan::New("stunServer").ToLocalChecked()
    , Agent::GetStunServer
    , Agent::SetStunServer);

    Nan::SetAccessor(
      proto
    , Nan::New("stunServerPort").ToLocalChecked()
    , Agent::GetStunServerPort
    , Agent::SetStunServerPort);

    /**
     * Export Agent
     */

    exports->Set(
      Nan::New("Agent").ToLocalChecked()
    , tpl->GetFunction());

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

  /**
   * Properties
   */

  /**
   * Get / Set controlling-mode
   */

  NAN_GETTER(Agent::GetControllingMode) {
    bool value;
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "controlling-mode";
    g_object_get(
      G_OBJECT(agent->agent)
    , property_name, &value
    , NULL);

    info.GetReturnValue().Set(Nan::New(value));
  }

  NAN_SETTER(Agent::SetControllingMode) {
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "controlling-mode";
    g_object_set(
      G_OBJECT(agent->agent)
    , property_name, value->BooleanValue()
    , NULL);
  }

  /**
   * Get / Set ice-tcp
   */

  NAN_GETTER(Agent::GetIceTcp) {
    bool value;
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "ice-tcp";
    g_object_get(
      G_OBJECT(agent->agent)
    , property_name, &value
    , NULL);

    info.GetReturnValue().Set(Nan::New(value));
  }

  NAN_SETTER(Agent::SetIceTcp) {
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "ice-tcp";
    g_object_set(
      G_OBJECT(agent->agent)
    , property_name, value->BooleanValue()
    , NULL);
  }

  /**
   * Get / Set ice-udp
   */

  NAN_GETTER(Agent::GetIceUdp) {
    bool value;
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "ice-udp";
    g_object_get(
      G_OBJECT(agent->agent)
    , property_name, &value
    , NULL);

    info.GetReturnValue().Set(Nan::New(value));
  }

  NAN_SETTER(Agent::SetIceUdp) {
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "ice-udp";
    g_object_set(
      G_OBJECT(agent->agent)
    , property_name, value->BooleanValue()
    , NULL);
  }

  /**
   * Get / Set keepalive-conncheck
   */

  NAN_GETTER(Agent::GetKeepAliveConnCheck) {
    bool value;
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "keepalive-conncheck";
    g_object_get(
      G_OBJECT(agent->agent)
    , property_name, &value
    , NULL);

    info.GetReturnValue().Set(Nan::New(value));
  }

  NAN_SETTER(Agent::SetKeepAliveConnCheck) {
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "keepalive-conncheck";
    g_object_set(
      G_OBJECT(agent->agent)
    , property_name, value->BooleanValue()
    , NULL);
  }

  /**
   * Get / Set max-connectivity-checks
   */

  NAN_GETTER(Agent::GetMaxConnectivityChecks) {
    uint32_t value;
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "max-connectivity-checks";
    g_object_get(
      G_OBJECT(agent->agent)
    , property_name, &value
    , NULL);

    info.GetReturnValue().Set(Nan::New(value));
  }

  NAN_SETTER(Agent::SetMaxConnectivityChecks) {
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "max-connectivity-checks";
    g_object_set(
      G_OBJECT(agent->agent)
    , property_name, value->Uint32Value()
    , NULL);
  }

  /**
   * Get / Set proxy-ip
   */

  NAN_GETTER(Agent::GetProxyIp) {
    gchar* value;
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "proxy-ip";

    g_object_get(
      G_OBJECT(agent->agent)
    , property_name, &value
    , NULL);

    if (value != NULL) {
      info.GetReturnValue().Set(
        Nan::New(value)
          .ToLocalChecked());
      g_free(value);
    }
  }

  NAN_SETTER(Agent::SetProxyIp) {
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "proxy-ip";
    v8::String::Utf8Value param(value->ToString());
    std::string from = std::string(*param);

    g_object_set(
      G_OBJECT(agent->agent)
    , property_name, from.c_str()
    , NULL);
  }

  /**
   * Get / Set proxy-password
   */

  NAN_GETTER(Agent::GetProxyPassword) {
    gchar* value;
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "proxy-password";

    g_object_get(
      G_OBJECT(agent->agent)
    , property_name, &value
    , NULL);

    if (value != NULL) {
      info.GetReturnValue().Set(
        Nan::New(value)
          .ToLocalChecked());
      g_free(value);
    }
  }

  NAN_SETTER(Agent::SetProxyPassword) {
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "proxy-password";
    v8::String::Utf8Value param(value->ToString());
    std::string from = std::string(*param);

    g_object_set(
      G_OBJECT(agent->agent)
    , property_name, from.c_str()
    , NULL);
  }

  /**
   * Get / Set proxy-port
   */

  NAN_GETTER(Agent::GetProxyPort) {
    uint32_t value;
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "proxy-port";
    g_object_get(
      G_OBJECT(agent->agent)
    , property_name, &value
    , NULL);

    info.GetReturnValue().Set(Nan::New(value));
  }

  NAN_SETTER(Agent::SetProxyPort) {
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "proxy-port";
    g_object_set(
      G_OBJECT(agent->agent)
    , property_name, value->Uint32Value()
    , NULL);
  }

  /**
   * Get / Set proxy-type
   */

  NAN_GETTER(Agent::GetProxyType) {
    uint32_t value;
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "proxy-type";
    g_object_get(
      G_OBJECT(agent->agent)
    , property_name, &value
    , NULL);

    info.GetReturnValue().Set(Nan::New(value));
  }

  NAN_SETTER(Agent::SetProxyType) {
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "proxy-type";
    g_object_set(
      G_OBJECT(agent->agent)
    , property_name, value->Uint32Value()
    , NULL);
  }

  /**
   * Get / Set proxy-username
   */

  NAN_GETTER(Agent::GetProxyUsername) {
    gchar* value;
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "proxy-username";

    g_object_get(
      G_OBJECT(agent->agent)
    , property_name, &value
    , NULL);

    if (value != NULL) {
      info.GetReturnValue().Set(
        Nan::New(value)
          .ToLocalChecked());
      g_free(value);
    }
  }

  NAN_SETTER(Agent::SetProxyUsername) {
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "proxy-username";
    v8::String::Utf8Value param(value->ToString());
    std::string from = std::string(*param);

    g_object_set(
      G_OBJECT(agent->agent)
    , property_name, from.c_str()
    , NULL);
  }

  /**
   * Get / Set stun-server
   */

  NAN_GETTER(Agent::GetStunServer) {
    gchar* value;
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "stun-server";

    g_object_get(
      G_OBJECT(agent->agent)
    , property_name, &value
    , NULL);

    if (value != NULL) {
      info.GetReturnValue().Set(
        Nan::New(value)
          .ToLocalChecked());
      g_free(value);
    }
  }

  NAN_SETTER(Agent::SetStunServer) {
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "stun-server";
    v8::String::Utf8Value param(value->ToString());
    std::string from = std::string(*param);

    g_object_set(
      G_OBJECT(agent->agent)
    , property_name, from.c_str()
    , NULL);
  }

  /**
   * Get / Set stun-server-port
   */

  NAN_GETTER(Agent::GetStunServerPort) {
    uint32_t value;
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "stun-server-port";
    g_object_get(
      G_OBJECT(agent->agent)
    , property_name, &value
    , NULL);

    info.GetReturnValue().Set(Nan::New(value));
  }

  NAN_SETTER(Agent::SetStunServerPort) {
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "stun-server-port";
    g_object_set(
      G_OBJECT(agent->agent)
    , property_name, value->Uint32Value()
    , NULL);
  }


}

#include <uv.h>
#include <nan.h>
#include <iostream>
#include "macros.h"
#include "agent.h"
#include <thread>
#include <mutex>

namespace libnice {

  Agent::Agent() {

    this->main_loop = g_main_loop_new(NULL, FALSE);
    this->main_context = g_main_loop_get_context(main_loop);

    /**
     * Initialize async worker
     */

    this->async = (uv_async_t *) malloc(sizeof(*this->async));
    uv_async_init(uv_default_loop(), this->async, &Agent::work);
    this->async->data = this;

    /**
     * Create a NiceAgent
     */

    this->agent = nice_agent_new(
      this->main_context
    , NICE_COMPATIBILITY_RFC5245
    );

    /**
     * Connect signals
     */

    g_signal_connect(
      G_OBJECT(this->agent)
    , "candidate-gathering-done"
    , G_CALLBACK(Agent::onGatheringDone)
    , this
    );

    /**
     * This creates a new thread, secure your v8 calls!
     */

    this->thread = std::thread([=]() {
      std::cout << "running";
      g_main_loop_run(this->main_loop);
      g_main_loop_unref(this->main_loop);
      g_main_context_unref(this->main_context);
      this->main_loop = NULL;
      this->main_context = NULL;
    });
  }

  Agent::~Agent() {
    g_main_loop_quit(this->main_loop);
    this->thread.join();

    g_object_unref(this->agent);
    this->agent = NULL;

    uv_close((uv_handle_t*) this->async, (uv_close_cb) free);
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
    PROTO_GETSET(Agent, "upnp", Upnp);
    PROTO_GETSET(Agent, "upnpTimeout", Upnp);

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
  IMPL_GETSET_BOOL(Agent, agent, "upnp", Upnp);
  IMPL_GETSET_UINT32(Agent, agent, "upnp-timeout", UpnpTimeout)

  /*****************************************************************************
   * Async work
   ****************************************************************************/

  void Agent::runOnNodeThread(const std::function<void(void)>& fun) {
    std::lock_guard<std::mutex> guard(this->work_mutex);
    this->work_queue.push_back(fun);
    uv_async_send(this->async);
  }

  void Agent::work(uv_async_t *async) {

    Agent *agent = (Agent*) async->data;

    std::lock_guard<std::mutex> guard(agent->work_mutex);

    while(agent->work_queue.size()) {
      agent->work_queue.front()();
      agent->work_queue.pop_front();
    }
  }

  /*****************************************************************************
   * Signal callbacks
   ****************************************************************************/

  void Agent::onGatheringDone(
                NiceAgent *nice_agent
              , guint stream_id
              , gpointer user_data) {
    Agent *agent = reinterpret_cast<Agent*>(user_data);
    std::cout << "cool";
  }
>>>>>>> 4f2a299... Bring back async code
}

#include <uv.h>
#include <nan.h>
#include <iostream>
#include "macros.h"
#include "agent.h"
#include <thread>
#include <mutex>
#include "stream.h"

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

    this->nice_agent = nice_agent_new(
      this->main_context
    , NICE_COMPATIBILITY_RFC5245
    );

    /**
     * Connect signals
     */

    g_signal_connect(
      G_OBJECT(this->nice_agent)
    , "candidate-gathering-done"
    , G_CALLBACK(Agent::onGatheringDone)
    , this);

    g_signal_connect(
      G_OBJECT(this->nice_agent)
    , "component-state-changed"
    , G_CALLBACK(Agent::onStateChanged)
    , this);

    /**
     * This creates a new thread, secure your v8 calls!
     */

    std::cout << "starting thread" << std::endl;
    this->thread = std::thread([=]() {
      std::cout << "Agent's main loop is running" << std:: endl;
      g_main_loop_run(this->main_loop);
      g_main_loop_unref(this->main_loop);
      g_main_context_unref(this->main_context);
      std::cout << "Agent's main loop has finished" << std:: endl;
      this->main_loop = NULL;
      this->main_context = NULL;
    });
  }

  Agent::~Agent() {
    g_main_loop_quit(this->main_loop);
    this->thread.join();

    g_object_unref(this->nice_agent);
    this->nice_agent = NULL;

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
     * Prototype methods
     */
    PROTO_METHOD(Agent, "gatherCandidates", GatherCandidates);
    PROTO_METHOD(Agent, "addStream", AddStream);

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

  NAN_METHOD(Agent::AddStream) {
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.Holder());
    NiceAgent* nice_agent = agent->nice_agent;

    /**
     * Create a new Stream
     */

    int num_components = info[0]->IsUndefined()
      ? 1
      : info[0]->IntegerValue();
    int stream_id = nice_agent_add_stream(nice_agent, num_components);

    /**
     * Wrap Stream
     */

    const int argc = 3;
    v8::Local<v8::Value> argv[argc] = {
      v8::Local<v8::Value>(info.This())
    , Nan::New(stream_id)
    , Nan::New(num_components)
    };

    auto cons = Nan::New<v8::Function>(Stream::constructor);
    auto stream = cons->NewInstance(argc, argv);

    /**
     * Attach receive callback
     */

    for(int index = 1; index <= num_components; ++index) {
      nice_agent_attach_recv(
        nice_agent
      , stream_id
      , index
      , agent->main_context
      , Agent::receive
      , agent);
    }

    /**
     * Save stream for callback handling
     */

    agent->streams[stream_id] = ObjectWrap::Unwrap<Stream>(stream);

    /**
     * Return the stream
     */

    info.GetReturnValue().Set(stream);
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

  void Agent::run(const std::function<void(void)>& fun) {
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
   * Callbacks
   ****************************************************************************/

  void Agent::receive(
    NiceAgent* nice_agent
  , guint stream_id
  , guint component_id
  , guint len
  , gchar* buf
  , gpointer user_data) {
      Agent *agent = reinterpret_cast<Agent*>(user_data);

      // // TODO: this might not be the best solution ...
      // auto tmp_buf = std::make_shared<std::vector<char>>(len);
      // memcpy(tmp_buf->data(), buf, len);
      //
      // agent->addWork([=]() {
      //     auto it = agent->_streams.find(stream_id);
      //     if(it != agent->_streams.end()) {
      //         it->second->receive(component_id, tmp_buf->data(), len);
      //     } else {
      //         DEBUG("receiving on unknown stream");
      //     }
      // });
  }

  /*****************************************************************************
   * Signal callbacks
   ****************************************************************************/

  void Agent::onGatheringDone(
    NiceAgent *nice_agent
  , guint stream_id
  , gpointer user_data) {
    std::cout << "gathering done" << std::endl;
    Agent *agent = reinterpret_cast<Agent*>(user_data);
    agent->run([=]() {
        std::cout << "on v8 thread" << std::endl;
        // auto it = agent->_streams.find(stream_id);
        // if(it != agent->_streams.end()) {
        //     it->second->gatheringDone();
        // } else {
        //     DEBUG("gathering done on unknown stream");
        // }
    });
  }

  void Agent::onStateChanged(
    NiceAgent *nice_agent
  , guint stream_id
  , guint component_id
  , guint state
  , gpointer user_data) {
    std::cout << "component state change" << std::endl;
    Agent *agent = reinterpret_cast<Agent*>(user_data);
  }
>>>>>>> 4f2a299... Bring back async code
}

#include <uv.h>
#include <nan.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <assert.h>
#include "helper.h"
#include "macros.h"
#include "agent.h"
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

    g_signal_connect(
      G_OBJECT(this->nice_agent)
    , "new-candidate-full"
    , G_CALLBACK(Agent::onNewCandidateFull)
    , this);

    /**
     * This creates a new thread, secure your v8 calls!
     */

    this->thread = std::thread([=]() {
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
    PROTO_METHOD(Agent, "addStream",             AddStream);
    PROTO_METHOD(Agent, "generateLocalSdp",      GenerateLocalSdp);
    PROTO_METHOD(Agent, "parseRemoteSdp",        ParseRemoteSdp);
    PROTO_GETSET(Agent, "controllingMode",       ControllingMode);
    PROTO_GETSET(Agent, "iceTcp",                IceTcp);
    PROTO_GETSET(Agent, "iceUdp",                IceUdp);
    PROTO_GETSET(Agent, "keepaliveConncheck",    KeepAliveConnCheck);
    PROTO_GETSET(Agent, "maxConnectivityChecks", MaxConnectivityChecks);
    PROTO_GETSET(Agent, "proxyIp",               ProxyIp);
    PROTO_GETSET(Agent, "proxyPassword",         ProxyPassword);
    PROTO_GETSET(Agent, "proxyPort",             ProxyPort);
    PROTO_GETSET(Agent, "proxyType",             ProxyType);
    PROTO_GETSET(Agent, "proxyUsername",         ProxyUsername);
    PROTO_GETSET(Agent, "stunServer",            StunServer);
    PROTO_GETSET(Agent, "stunServerPort",        StunServerPort);
    PROTO_GETSET(Agent, "upnp",                  Upnp);
    PROTO_GETSET(Agent, "upnpTimeout",           Upnp);

    /**
     * Export Agent
     */

    MODULE_EXPORT("Agent");

    /**
     * Tie the knot
     */

    Agent::constructor.Reset(tpl->GetFunction());
  }

  /*****************************************************************************
   * Methods
   ****************************************************************************/

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

  NAN_METHOD(Agent::GenerateLocalSdp) {
    Agent* agent = Nan::ObjectWrap::Unwrap<Agent>(info.Holder());
    const gchar* sdp = nice_agent_generate_local_sdp(agent->nice_agent);
    if (sdp == NULL) {
      info.GetReturnValue().Set(Nan::Undefined());
    } else {
      info.GetReturnValue().Set(Nan::New(sdp).ToLocalChecked());
    }
  }

  gboolean Agent::RunParseRemoteSdp(gpointer user_data) {
    auto args = reinterpret_cast<Agent::ParseRemoteSdpArgs*>(user_data);

    int res = nice_agent_parse_remote_sdp(
      args->agent->nice_agent
    , args->sdp->c_str());

    delete args->sdp;

    if (args->callback) {
      args->agent->run([=]() {
        v8::Local<v8::Value> num_candidates_added = Nan::New(res);
        args->callback->Call(1, &num_candidates_added);
        delete args;
      });
    } else {
      delete args;
    }

    return G_SOURCE_REMOVE;
  }

  NAN_METHOD(Agent::ParseRemoteSdp) {
    Agent* agent = Nan::ObjectWrap::Unwrap<Agent>(info.Holder());

    auto sdp = std::string(*v8::String::Utf8Value(info[0]->ToString()));

    Nan::Callback* callback = NULL;
    if (!info[1]->IsUndefined()) {
      callback = new Nan::Callback(info[1].As<v8::Function>());
    }

    /**
     * Push the data onto the glib thread.
     */
    auto args = new struct Agent::ParseRemoteSdpArgs;
    args->agent = agent;
    args->sdp = new std::string(sdp);
    args->callback = callback;
    g_idle_add((GSourceFunc) RunParseRemoteSdp, args);

    info.GetReturnValue().Set(Nan::Undefined());
  }

  /*****************************************************************************
   * Implement getters/setters
   ****************************************************************************/

#define GETSET_BOOL(a, b)   IMPL_GETSET_BOOL(Agent, nice_agent, a, b)
#define GETSET_UINT32(a, b) IMPL_GETSET_UINT32(Agent, nice_agent, a, b)
#define GETSET_STR(a, b)    IMPL_GETSET_STR(Agent, nice_agent, a, b)

  GETSET_BOOL   ("controlling-mode",        ControllingMode)
  GETSET_BOOL   ("ice-tcp",                 IceTcp)
  GETSET_BOOL   ("ice-udp",                 IceUdp)
  GETSET_BOOL   ("keepalive-conncheck",     KeepAliveConnCheck)
  GETSET_UINT32 ("max-connectivity-checks", MaxConnectivityChecks)
  GETSET_STR    ("proxy-ip",                ProxyIp)
  GETSET_STR    ("proxy-password",          ProxyPassword)
  GETSET_UINT32 ("proxy-port",              ProxyPort)
  GETSET_UINT32 ("proxy-type",              ProxyType)
  GETSET_STR    ("proxy-username",          ProxyUsername)
  GETSET_STR    ("stun-server",             StunServer)
  GETSET_UINT32 ("stun-server-port",        StunServerPort)
  GETSET_BOOL   ("upnp",                    Upnp);
  GETSET_UINT32 ("upnp-timeout",            UpnpTimeout)

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
  , gpointer user_data
  ) {
    Agent* agent = reinterpret_cast<Agent*>(user_data);

    /**
     * Invoke callback on matching stream
     */

    agent->run([=]() {
      auto it = agent->streams.find(stream_id);
      assert(it != agent->streams.end());
      auto stream = it->second;
      stream->onGatheringDone();
    });
  }

  void Agent::onStateChanged(
    NiceAgent *nice_agent
  , guint stream_id
  , guint component_id
  , guint state
  , gpointer user_data
  ) {
    Agent* agent = reinterpret_cast<Agent*>(user_data);

    /**
     * Invoke callback on matching stream
     */

    agent->run([=]() {
      auto it = agent->streams.find(stream_id);
      assert(it != agent->streams.end());
      auto stream = it->second;
      stream->onStateChanged(state, component_id);
    });
  }

  void Agent::onNewCandidateFull(
    NiceAgent* nice_agent
  , NiceCandidate* candidate
  , gpointer user_data
  ) {
    Agent* agent = reinterpret_cast<Agent*>(user_data);

    /**
     * Invoke callback
     */

    agent->run([=]() {
      const int argc = 1;
      v8::Local<v8::Value> argv[argc] = {
        Nan::New("new-candidate-full").ToLocalChecked()
      , // XXX: Serialize and send candidate with callback
      };
      Nan::MakeCallback(
        Nan::New(agent->persistent()), "emit", argc, argv);
    });
  }
}

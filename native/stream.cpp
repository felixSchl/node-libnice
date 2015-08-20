#include <uv.h>
#include <nan.h>
#include <iostream>
#include <thread>
#include <mutex>
#include "macros.h"
#include "stream.h"
#include "agent.h"

namespace libnice {

  Stream::Stream(
    v8::Local<v8::Object> agent
  , int stream_id
  , int num_components)
  {
    std::cout << "BIRTH OF STREAM " << this << std::endl;

    /**
     * Store agent as persistent
     */

    this->agent.Reset(agent);
    this->stream_id = stream_id;
    this->num_components = num_components;
  }


  Stream::~Stream() {
    std::cout << "DEATH OF STREAM " << this << std::endl;
  }

  Nan::Persistent<v8::Function> Stream::constructor;

  void Stream::Init(v8::Handle<v8::Object> exports) {
    Nan::HandleScope scope;

    auto tpl = Nan::New<v8::FunctionTemplate>(Stream::New);
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    tpl->SetClassName(Nan::New("Stream").ToLocalChecked());

    /**
     * Prototype
     */

    PROTO_METHOD  (Stream, "gatherCandidates", GatherCandidates);
    PROTO_READONLY(Stream, "id",               Id);
    PROTO_GETSET  (Stream, "name",             Name);

    /**
     * Export Stream
     */

    MODULE_EXPORT("Stream");

    /**
     * Tie the knot
     */

    Stream::constructor.Reset(tpl->GetFunction());
  }

  /*****************************************************************************
   * Implement getters/setters
   ****************************************************************************/

  NAN_GETTER(Stream::GetId) {
    Stream* stream = Nan::ObjectWrap::Unwrap<Stream>(info.This());
    info.GetReturnValue().Set(Nan::New(stream->stream_id));
  }

  NAN_GETTER(Stream::GetName) {
    Stream* stream = Nan::ObjectWrap::Unwrap<Stream>(info.This());
    Agent* agent = Nan::ObjectWrap::Unwrap<Agent>(Nan::New(stream->agent));

    const gchar* name = nice_agent_get_stream_name(
      agent->nice_agent
    , stream->stream_id);

    if (name == NULL) {
      info.GetReturnValue().Set(Nan::Undefined());
    } else {
      info.GetReturnValue().Set(Nan::New(name).ToLocalChecked());
    }
  }

  NAN_SETTER(Stream::SetName) {
    Stream* stream = Nan::ObjectWrap::Unwrap<Stream>(info.This());
    Agent* agent = Nan::ObjectWrap::Unwrap<Agent>(Nan::New(stream->agent));
    v8::String::Utf8Value param(value->ToString());
    std::string from = std::string(*param);
    bool res = nice_agent_set_stream_name(
      agent->nice_agent
    , stream->stream_id
    , from.c_str());
    // XXX: Throw if `res` is false
    info.GetReturnValue().Set(Nan::Undefined());
  }

  /*****************************************************************************
   * Methods
   ****************************************************************************/

  NAN_METHOD(Stream::New) {
    Stream* stream = new Stream(
      info[0]->ToObject()
    , info[1]->Uint32Value()
    , info[2]->Uint32Value());
    stream->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  }

  GLIB_CALLBACK(Stream::RunGatherCandidates) {
    auto args = reinterpret_cast<Stream::GatherCandidatesArgs*>(user_data);
    bool res = nice_agent_gather_candidates(
      args->agent->nice_agent
    , args->stream->stream_id);
    GLIB_CALLBACK_RETURN;
  }

  NAN_METHOD(Stream::GatherCandidates) {
    Stream* stream = Nan::ObjectWrap::Unwrap<Stream>(info.This());
    Agent* agent = Nan::ObjectWrap::Unwrap<Agent>(Nan::New(stream->agent));

    auto args = new struct Stream::GatherCandidatesArgs;
    args->agent = agent;
    args->stream = stream;
    RUN_GLIB(agent->main_context, Stream::RunGatherCandidates, args);

    info.GetReturnValue().Set(Nan::Undefined());
  }

  /*****************************************************************************
   * Callbacks
   ****************************************************************************/

  void Stream::onGatheringDone() {
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {
      Nan::New("gathering-done").ToLocalChecked(),
    };
    Nan::MakeCallback(Nan::New(this->persistent()), "emit", argc, argv);
  }

  void Stream::onStateChanged(guint state, guint component_id) {
    const int argc = 3;
    v8::Local<v8::Value> argv[argc] = {
      Nan::New("state-changed").ToLocalChecked()
    , Nan::New(state)
    , Nan::New(component_id)
    };
    Nan::MakeCallback(Nan::New(this->persistent()), "emit", argc, argv);
  }
}

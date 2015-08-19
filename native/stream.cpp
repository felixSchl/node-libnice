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

    /**
     * Store agent as persistent
     */

    this->agent.Reset(agent);
    this->stream_id = stream_id;
    this->num_components = num_components;
  }


  Stream::~Stream() {
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

    PROTO_METHOD(Stream, "gatherCandidates", GatherCandidates);
    PROTO_GETSET(Stream, "name",             Name);

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

  NAN_METHOD(Stream::GatherCandidates) {
    Stream* stream = Nan::ObjectWrap::Unwrap<Stream>(info.This());
    Agent* agent = Nan::ObjectWrap::Unwrap<Agent>(Nan::New(stream->agent));
    bool res = nice_agent_gather_candidates(
      agent->nice_agent
    , stream->stream_id);
    info.GetReturnValue().Set(Nan::New(res));
  }

  /*****************************************************************************
   * Callbacks
   ****************************************************************************/

  void Stream::onGatheringDone() {

    const int argc = 1;
      v8::Local<v8::Value> argv[argc] = {
        Nan::New("gathering-done").ToLocalChecked(),
    };

    Nan::MakeCallback(
        Nan::New(this->persistent()), "emit", argc, argv);
  }
}

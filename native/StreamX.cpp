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
    if (!this->agent.IsEmpty()) {
      this->agent.Reset();
    }
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
    // Stream* stream = ObjectWrap::Unwrap<Stream>(info.Holder());
    // Agent* agent = ObjectWrap::Unwrap<Agent>(stream->agent);
    // bool res = nice_agent_gather_candidates(
    //   agent->nice_agent
    // , stream_id);
    // info.GetReturnValue().Set(Nan::New(res));
  }
}

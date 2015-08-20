#include <uv.h>
#include <nan.h>
#include <iostream>
#include <thread>
#include <mutex>
#include "macros.h"
#include "stream.h"
#include "agent.h"
#include "component.h"

namespace libnice {

#define GET_COMPONENT(id)                            \
  ObjectWrap::Unwrap<Component>(                     \
    Nan::To<v8::Object>(                             \
      Nan::New(this->components)->Get(Nan::New(id))) \
        .ToLocalChecked())

  Stream::Stream(
    v8::Local<v8::Object> self
  , v8::Local<v8::Object> agent
  , int stream_id
  , int num_components)
  {
    Nan::HandleScope scope;

    /**
     * Store agent as persistent
     */

    this->agent.Reset(agent);
    this->stream_id = stream_id;
    this->num_components = num_components;

    /**
     * Prepare the `components` field
     */

    v8::Local<v8::Object> obj = Nan::New<v8::Object>();
    this->components.Reset(obj);

    /**
     * Create `num_components` Components
     */

    for (int i = 1; i < (num_components + 1); i++) {
      const int argc = 2;
      v8::Local<v8::Value> argv[argc] = {
        self
      , Nan::New(i)
      };
      auto cons = Nan::New<v8::Function>(Component::constructor);
      auto component = cons->NewInstance(argc, argv);
      obj->Set(Nan::New(i), component);
    }

  }

  Stream::~Stream() {
    std::cout << "DEATH OF STREAM (" << this << ")" << std::endl;
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
    PROTO_METHOD(Stream, "send",             Send);
    PROTO_RDONLY(Stream, "id",               Id);
    PROTO_RDONLY(Stream, "components",       Components);
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

  NAN_GETTER(Stream::GetId) {
    Stream* stream = Nan::ObjectWrap::Unwrap<Stream>(info.This());
    info.GetReturnValue().Set(Nan::New(stream->stream_id));
  }

  NAN_GETTER(Stream::GetComponents) {
    Stream* stream = Nan::ObjectWrap::Unwrap<Stream>(info.This());
    info.GetReturnValue().Set(Nan::New(stream->components));
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
    // XXX: Add input validation
    Stream* stream = new Stream(
      info.This()
    , info[0]->ToObject()
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

  NAN_METHOD(Stream::Send) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    Stream* stream = Nan::ObjectWrap::Unwrap<Stream>(info.This());
    Agent* agent = Nan::ObjectWrap::Unwrap<Agent>(Nan::New(stream->agent));

    int component = info[0]->Uint32Value();

    if(!node::Buffer::HasInstance(info[1])) {
      isolate->ThrowException(
        v8::Exception::TypeError(
          Nan::New("Expected buffer").ToLocalChecked()));
      return;
    }

    v8::Local<v8::Object> buffer = info[1]->ToObject();

    size_t size = node::Buffer::Length(buffer);
    char* buf = node::Buffer::Data(buffer);

    int ret = nice_agent_send(
      agent->nice_agent
    , stream->stream_id
    , component
    , size
    , buf);

    info.GetReturnValue().Set(Nan::New(ret));
}

  /*****************************************************************************
   * Callbacks
   ****************************************************************************/

  void Stream::onGatheringDone() {
    Nan::HandleScope scope;
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {
      Nan::New("gathering-done").ToLocalChecked(),
    };
    Nan::MakeCallback(Nan::New(this->persistent()), "emit", argc, argv);
  }

  void Stream::onStateChanged(guint state, guint component_id) {
    Nan::HandleScope scope;

    Component* component = GET_COMPONENT(component_id);
    component->onStateChanged(state);

    const int argc = 3;
    v8::Local<v8::Value> argv[argc] = {
      Nan::New("state-changed").ToLocalChecked()
    , Nan::New(component_id)
    , Nan::New(state)
    };
    Nan::MakeCallback(Nan::New(this->persistent()), "emit", argc, argv);
  }

  void Stream::onData(int component_id, const char* buf, size_t size) {
    Nan::HandleScope scope;

    Component* component = GET_COMPONENT(component_id);
    component->onData(buf, size);

    const int argc = 3;
    v8::Local<v8::Value> argv[argc] = {
      Nan::New("data").ToLocalChecked()
    , Nan::CopyBuffer(buf, size).ToLocalChecked()
    , Nan::New(component)
    };
    Nan::MakeCallback(Nan::New(this->persistent()), "emit", argc, argv);
  }

}

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

  Component::Component(
    v8::Local<v8::Object> stream
  , int id
  ) {
    Nan::HandleScope scope;

    this->id = id;

    /**
     * Store stream as persistent
     */

    this->stream.Reset(stream);
  }

  Component::~Component() {
    std::cout << "DEATH OF COMPONENT (" << this << ")" << std::endl;
  }

  Nan::Persistent<v8::Function> Component::constructor;

  void Component::Init(v8::Handle<v8::Object> exports) {
    Nan::HandleScope scope;

    auto tpl = Nan::New<v8::FunctionTemplate>(Component::New);
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    tpl->SetClassName(Nan::New("Component").ToLocalChecked());

    /**
     * Prototype
     */

    PROTO_RDONLY(Component, "id",   Id);
    PROTO_METHOD(Component, "send", Send);

    /**
     * Export Component
     */

    MODULE_EXPORT("Component");

    /**
     * Tie the knot
     */

    Component::constructor.Reset(tpl->GetFunction());
  }

  /*****************************************************************************
   * Methods
   ****************************************************************************/

  NAN_METHOD(Component::New) {
    Component* component = new Component(
      info[0]->ToObject()
    , info[1]->Uint32Value());
    component->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  }

  NAN_METHOD(Component::Send) {
    Component* component = Nan::ObjectWrap::Unwrap<Component>(info.This());

    const int argc = 3;
    v8::Local<v8::Value> argv[argc] = {
      Nan::New(component->id)
    , info[0]
    , info[1]
    };

    /**
     * Proxy the call to `stream.send`
     */

    info.GetReturnValue().Set(
      Nan::New(component->stream)->Get(
        Nan::New("send").ToLocalChecked())
          .As<v8::Function>()->Call(Nan::New(component->stream), argc, argv));
}

  /*****************************************************************************
   * Implement getters/setters
   ****************************************************************************/

  NAN_GETTER(Component::GetId) {
    Component* component = Nan::ObjectWrap::Unwrap<Component>(info.This());
    info.GetReturnValue().Set(Nan::New(component->id));
  }

  /*****************************************************************************
   * Callbacks
   ****************************************************************************/

  void Component::onStateChanged(guint state) {
    Nan::HandleScope scope;
    const int argc = 2;
    v8::Local<v8::Value> argv[argc] = {
      Nan::New("state-changed").ToLocalChecked()
    , Nan::New(state)
    };
    Nan::MakeCallback(Nan::New(this->persistent()), "emit", argc, argv);
  }

  void Component::onReliableTransportWritable() {
    Nan::HandleScope scope;
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {
      Nan::New("reliable-transport-writable").ToLocalChecked()
    };
    Nan::MakeCallback(Nan::New(this->persistent()), "emit", argc, argv);
  }

  void Component::onData(const char* buf, size_t size) {
    Nan::HandleScope scope;

    const int argc = 2;
    v8::Local<v8::Value> argv[argc] = {
      Nan::New("data").ToLocalChecked()
    , Nan::CopyBuffer(buf, size).ToLocalChecked()
    };
    Nan::MakeCallback(Nan::New(this->persistent()), "emit", argc, argv);
  }
}

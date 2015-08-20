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
  ) {
    // Nan::HandleScope scope;

    /**
     * Store stream as persistent
     */

    // this->stream.Reset(stream);
  }

  Component::~Component() {
  }

  Nan::Persistent<v8::Function> Component::constructor;

  void Component::Init(v8::Handle<v8::Object> exports) {
    Nan::HandleScope scope;

    auto tpl = Nan::New<v8::FunctionTemplate>(Component::New);
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    tpl->SetClassName(Nan::New("Component").ToLocalChecked());

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
      info[0]->ToObject());
    component->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  }
}

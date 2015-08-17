#include <nan.h>
#include "Agent.h"

namespace libnice {

  Nan::Persistent<v8::Function> Agent::constructor;

  void Agent::Init(v8::Handle<v8::Object> exports) {
    Nan::HandleScope scope;

    /**
     * Prepare constructor template
     */
    auto tpl = Nan::New<v8::FunctionTemplate>(Agent::New);
    tpl->SetClassName(Nan::New("Agent").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Agent::constructor.Reset(tpl->GetFunction());

    /**
     * Prototype
     */
    Nan::SetPrototypeMethod(tpl, "gatherCandidates", Agent::GatherCandidates);

    /**
     * Export Agent
     */
    exports->Set(
      Nan::New("Agent").ToLocalChecked()
    , tpl->GetFunction());
  }

  NAN_METHOD(Agent::New) {
    Agent* agent = new Agent();
    agent->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  }

  void Agent::GatherCandidates(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.Holder());
  }
}

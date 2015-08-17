#include <nan.h>
#include "Agent.h"

namespace libnice {

  Agent::Agent() {
    this->main_loop = g_main_loop_new(NULL, FALSE);
    this->main_context = g_main_loop_get_context(main_loop);
    this->agent = nice_agent_new(
        this->main_context
      , NICE_COMPATIBILITY_RFC5245
    );
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
    auto proto = tpl->PrototypeTemplate();

    /**
     * Prototype
     */
    Nan::SetPrototypeMethod(
      tpl
    , "gatherCandidates"
    , Agent::GatherCandidates);

    /**
     * Properties
     */
    Nan::SetAccessor(
      proto
    , Nan::New("controllingMode").ToLocalChecked()
    , Agent::GetControllingMode
    , Agent::SetControllingMode);

    /**
     * Export Agent
     */
    exports->Set(
      Nan::New("Agent").ToLocalChecked()
    , tpl->GetFunction());

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

  NAN_GETTER(Agent::GetControllingMode) {
    bool value;
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "controlling-mode";
    g_object_get(
      G_OBJECT(agent->agent)
    , property_name, &value
    , NULL);

    info.GetReturnValue().Set(Nan::New(value));
  }

  NAN_SETTER(Agent::SetControllingMode) {
    Agent* agent = ObjectWrap::Unwrap<Agent>(info.This());

    const gchar* property_name = "controlling-mode";
    g_object_set(
      G_OBJECT(agent->agent)
    , property_name, value->BooleanValue()
    , NULL
    );
  }
}

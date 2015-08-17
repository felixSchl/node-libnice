#include <node.h>
#include <v8.h>

#include "Agent.h"

void initAll(v8::Handle<v8::Object> exports) {
  libnice::Agent::Init(exports);
}

NODE_MODULE(native_libnice, initAll)

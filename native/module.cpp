#include <node.h>
#include <v8.h>

#include "agent.h"
#include "Stream.h"

extern "C"
void initAll(v8::Handle<v8::Object> exports) {
  libnice::Agent::Init(exports);
  libnice::Stream::Init(exports);
}

NODE_MODULE(native_libnice, initAll)

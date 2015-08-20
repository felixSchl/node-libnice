#include <node.h>
#include <v8.h>
#include <glib.h>
#include <nice/nice.h>

#include "agent.h"
#include "stream.h"
#include "component.h"

extern "C"
void initAll(v8::Handle<v8::Object> exports) {
  libnice::Agent::Init(exports);
  libnice::Stream::Init(exports);
  libnice::Component::Init(exports);
  nice_debug_enable(TRUE);
}

NODE_MODULE(native_libnice, initAll)

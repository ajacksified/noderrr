#include <nan.h>
#include "r3.h"

void InitAll(v8::Local<v8::Object> exports) {
  R3::Init(exports);
}

NODE_MODULE(addon, InitAll)

#include "atol.h"

void InitAll(v8::Local<v8::Object> exports) {
  Atol::Init(exports);
}

NODE_MODULE(addon, InitAll)

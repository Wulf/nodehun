#include <nan.h>
#include "NodehunObject.hpp"

void InitAll(v8::Local<v8::Object> exports) {
  MyObject::Init(exports);
}

NODE_MODULE(Nodehun, InitAll)

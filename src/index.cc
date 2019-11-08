#include <napi.h>
#include "Nodehun.cc"

Napi::Object CreateObject(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  return Nodehun::NewInstance(info);
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  Napi::Object new_exports = Napi::Function::New(env, CreateObject, "Nodehun");
  return Nodehun::Init(env, new_exports);
}

NODE_API_MODULE(addon, InitAll)

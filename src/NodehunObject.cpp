#include "NodehunObject.hpp"

Nan::Persistent<v8::Function> MyObject::constructor;

MyObject::MyObject(const char* affbuf, const char* dicbuf) {
   hunspellInstance = new Hunspell(affbuf, dictbuf, NULL, true);
}

MyObject::~MyObject() {
  if (hunspellInstance) {
    delete hunspellInstance;
    hunspellInstance = NULL;
  }
}

void MyObject::Init(v8::Local<v8::Object> exports, v8::Local<v8::Object> module) {
  Nan::HandleScope scope;

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("MyObject").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  // Nan::SetPrototypeMethod(tpl, "value", GetValue);
  // Nan::SetPrototypeMethod(tpl, "plusOne", PlusOne);
  // Nan::SetPrototypeMethod(tpl, "multiply", Multiply);

  constructor.Reset(tpl->GetFunction());
  module->Set(Nan::New("exports").ToLocalChecked(), tpl->GetFunction());
}

void MyObject::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.IsConstructCall()) {
    // Invoked as constructor: `new MyObject(...)`
    double value = info[0]->IsUndefined() ? 0 : info[0]->NumberValue();
    MyObject* obj = new MyObject();
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    info.GetReturnValue().GetIsolate()->ThrowException(Nan::Error("Use the new operator to create an instance of this object."));
  }
}

NODE_MODULE(Nodehun, MyObject::Init)

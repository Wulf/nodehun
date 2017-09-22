#include "NodehunObject.hpp"

Nan::Persistent<v8::Function> MyObject::constructor;

MyObject::MyObject(const char* affixBuffer, const char* dictionaryBuffer) {
   hunspellInstance = new Hunspell(affixBuffer, dictionaryBuffer, NULL, true);
}

MyObject::~MyObject() {
  if (hunspellInstance) {
    delete hunspellInstance;
    hunspellInstance = NULL;
  }
}

NAN_METHOD(MyObject::New) {
  if (info.IsConstructCall()) {
    // Invoked as constructor: `new MyObject(...)`
    if(info[0]->IsUndefined() || !info[0]->IsObject()) {
      // first parameter isn't the affix buffer!
      // TODO: is there a way to check if the argument is a buffer?
      info.GetReturnValue().GetIsolate()->ThrowException(Nan::Error("The first argument must be a Buffer!"));
      return;
    }

    if(info[1]->IsUndefined() || !info[1]->IsObject()) {
      // second parameter isn't the dictionary buffer!
      // TODO: is there a way to check if the argument is a buffer?
      info.GetReturnValue().GetIsolate()->ThrowException(Nan::Error("The second argument must be a Buffer!"));
      return;
    }

    v8::Local<v8::Object> affixBufferObject = info[0]->ToObject();
    v8::Local<v8::Object> dictionaryBufferObject = info[1]->ToObject();

    char* affixBuffer = node::Buffer::Data(affixBufferObject);
    char* dictionaryBuffer = node::Buffer::Data(dictionaryBufferObject);

    MyObject* obj = new MyObject(affixBuffer, dictionaryBuffer);
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    info.GetReturnValue().GetIsolate()->ThrowException(Nan::Error("Use the new operator to create an instance of this object."));
    return;
  }
}

//============================================================================//
// Spelling API
//============================================================================//
NAN_METHOD(MyObject::suggest) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetReturnValue().GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(MyObject::suggestSync) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetReturnValue().GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(MyObject::suggestOne) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetReturnValue().GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(MyObject::suggestOneSync) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetReturnValue().GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(MyObject::corret) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetReturnValue().GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(MyObject::correctSync) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetReturnValue().GetIsolate(), "Not yet implemented."));
}

//============================================================================//
// Analysis API
//============================================================================//

NAN_METHOD(MyObject::stem) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetReturnValue().GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(MyObject::stemSync) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetReturnValue().GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(MyObject::generate) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetReturnValue().GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(MyObject::generateSync) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetReturnValue().GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(MyObject::analyze) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetReturnValue().GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(MyObject::analyzeSync) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetReturnValue().GetIsolate(), "Not yet implemented."));
}

//============================================================================//
// Dictionary Management API
//============================================================================//

NAN_METHOD(MyObject::addWord) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetReturnValue().GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(MyObject::addWordSync) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetReturnValue().GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(MyObject::removeWord) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetReturnValue().GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(MyObject::removeWordSync) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetReturnValue().GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(MyObject::addDictionary) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetReturnValue().GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(MyObject::addDictionarySync) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetReturnValue().GetIsolate(), "Not yet implemented."));
}

void MyObject::Init(v8::Local<v8::Object> exports, v8::Local<v8::Object> module) {
  Nan::HandleScope scope;

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Nodehun").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "suggest", MyObject::suggest);
  Nan::SetPrototypeMethod(tpl, "suggestSync", MyObject::suggestSync);
  Nan::SetPrototypeMethod(tpl, "suggestOne", MyObject::suggestOne);
  Nan::SetPrototypeMethod(tpl, "suggestOneSync", MyObject::suggestOneSync);
  Nan::SetPrototypeMethod(tpl, "correct", MyObject::corret);
  Nan::SetPrototypeMethod(tpl, "correctSync", MyObject::correctSync);
  Nan::SetPrototypeMethod(tpl, "stem", MyObject::stem);
  Nan::SetPrototypeMethod(tpl, "stemSync", MyObject::stemSync);
  Nan::SetPrototypeMethod(tpl, "generate", MyObject::generate);
  Nan::SetPrototypeMethod(tpl, "generateSync", MyObject::generateSync);
  Nan::SetPrototypeMethod(tpl, "analyze", MyObject::analyze);
  Nan::SetPrototypeMethod(tpl, "analyzeSync", MyObject::analyzeSync);
  Nan::SetPrototypeMethod(tpl, "addWord", MyObject::addWord);
  Nan::SetPrototypeMethod(tpl, "addWordSync", MyObject::addWordSync);
  Nan::SetPrototypeMethod(tpl, "removeWord", MyObject::removeWord);
  Nan::SetPrototypeMethod(tpl, "removeWordSync", MyObject::removeWordSync);
  Nan::SetPrototypeMethod(tpl, "addDictionary", MyObject::addDictionary);
  Nan::SetPrototypeMethod(tpl, "addDictionarySync", MyObject::addDictionarySync);

  constructor.Reset(tpl->GetFunction());
  module->Set(Nan::New("exports").ToLocalChecked(), tpl->GetFunction());
}

NODE_MODULE(Nodehun, MyObject::Init)

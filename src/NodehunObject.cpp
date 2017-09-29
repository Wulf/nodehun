#include "NodehunObject.hpp"

Nan::Persistent<v8::Function> Nodehun::constructor;

Nodehun::Nodehun(const char* affixBuffer, const char* dictionaryBuffer) {
   hunspellInstance = new Hunspell(affixBuffer, dictionaryBuffer, NULL, true);
}

Nodehun::~Nodehun() {
  if (hunspellInstance) {
    delete hunspellInstance;
    hunspellInstance = NULL;
  }
}

NAN_METHOD(Nodehun::New) {
  if (info.IsConstructCall()) {
    // Invoked as constructor: `new Nodehun(...)`
    if(!node::Buffer::HasInstance(info[0])) {
      info.GetIsolate()->ThrowException(Nan::Error("The first argument must be a Buffer!"));
      return;
    }

    if(!node::Buffer::HasInstance(info[0])) {
      info.GetIsolate()->ThrowException(Nan::Error("The second argument must be a Buffer!"));
      return;
    }

    v8::Local<v8::Object> affixBufferObject = info[0]->ToObject();
    v8::Local<v8::Object> dictionaryBufferObject = info[1]->ToObject();

    char* affixBuffer = node::Buffer::Data(affixBufferObject);
    char* dictionaryBuffer = node::Buffer::Data(dictionaryBufferObject);

    Nodehun* obj = new Nodehun(affixBuffer, dictionaryBuffer);
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    info.GetIsolate()->ThrowException(Nan::Error("Use the new operator to create an instance of this object."));
    return;
  }
}

//============================================================================//
// Spelling API
//============================================================================//
NAN_METHOD(Nodehun::suggest) {
  Nan::HandleScope scope;

  if(info.Length() > 1 && info[0]->IsString() && info[1]->IsFunction()) {
    std::string *word = new std::string(*v8::String::Utf8Value(info[0]->ToString()));
    Hunspell *hunspellInstance = Nan::ObjectWrap::Unwrap<Nodehun>(info.Holder())->hunspellInstance;
    Nan::Callback *callback = new Nan::Callback(info[1].As<v8::Function>());

    info.GetReturnValue().SetUndefined();
    Nan::AsyncQueueWorker(new Nodehun::SuggestWorker(callback, hunspellInstance, word));
  } else {
    info.GetIsolate()->ThrowException(Nan::TypeError("First argument must be a string, second argument must be a function."));
  }
}

NAN_METHOD(Nodehun::suggestSync) {
  Nan::HandleScope scope;

  if(info.Length() > 0 && info[0]->IsString()) {
    Hunspell *hunspellInstance = Nan::ObjectWrap::Unwrap<Nodehun>(info.Holder())->hunspellInstance;
    v8::String::Utf8Value word(info[0]->ToString());

    bool correct = hunspellInstance->spell(*word);
    if(correct) {
      info.GetReturnValue().Set(Nan::New<v8::Array>(0));
    } else {
      char** suggestionsFound = NULL;
      int size = hunspellInstance->suggest(&suggestionsFound, *word);

      v8::Local<v8::Array> suggestions = Nan::New<v8::Array>(size);
      for(int i = 0; i < size; i++) {
        Nan::MaybeLocal<v8::String> maybeSuggestion = Nan::New(suggestionsFound[i]);

        if(maybeSuggestion.IsEmpty()) {
          Nan::Set(suggestions, i, Nan::Undefined());
        } else {
          Nan::Set(suggestions, i, maybeSuggestion.ToLocalChecked());
        }
      }
      info.GetReturnValue().Set(suggestions);

      hunspellInstance->free_list(&suggestionsFound, size);
    }
  } else {
    info.GetReturnValue().SetUndefined();
    info.GetIsolate()->ThrowException(Nan::TypeError("First argument must be a string."));
  }
}

NAN_METHOD(Nodehun::correct) {
  Nan::HandleScope scope;

  if(info.Length() > 1 && info[0]->IsString() && info[1]->IsFunction()) {
    std::string *word = new std::string(*v8::String::Utf8Value(info[0]->ToString()));
    Hunspell *hunspellInstance = Nan::ObjectWrap::Unwrap<Nodehun>(info.Holder())->hunspellInstance;
    Nan::Callback *callback = new Nan::Callback(info[1].As<v8::Function>());

    info.GetReturnValue().SetUndefined();
    Nan::AsyncQueueWorker(new Nodehun::CorrectWorker(callback, hunspellInstance, word));
  } else {
    info.GetIsolate()->ThrowException(Nan::TypeError("First argument must be a string, second argument must be a function."));
  }
}

NAN_METHOD(Nodehun::correctSync) {
    Nan::HandleScope scope;

    if(info.Length() > 0 && info[0]->IsString()) {
      Hunspell *hunspellInstance = Nan::ObjectWrap::Unwrap<Nodehun>(info.Holder())->hunspellInstance;
      v8::String::Utf8Value word(info[0]->ToString());
      bool correct = hunspellInstance->spell(*word);
      info.GetReturnValue().Set(correct);
    } else {
      info.GetReturnValue().SetUndefined();
      info.GetIsolate()->ThrowException(Nan::TypeError("First argument must be a string."));
    }

  // info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), "Not yet implemented."));
}

//============================================================================//
// Analysis API
//============================================================================//

NAN_METHOD(Nodehun::stem) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(Nodehun::stemSync) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(Nodehun::generate) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(Nodehun::generateSync) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(Nodehun::analyze) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(Nodehun::analyzeSync) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), "Not yet implemented."));
}

//============================================================================//
// Dictionary Management API
//============================================================================//

NAN_METHOD(Nodehun::addWord) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(Nodehun::addWordSync) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(Nodehun::removeWord) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(Nodehun::removeWordSync) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(Nodehun::addDictionary) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), "Not yet implemented."));
}

NAN_METHOD(Nodehun::addDictionarySync) {
  info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), "Not yet implemented."));
}

void Nodehun::Init(v8::Local<v8::Object> exports, v8::Local<v8::Object> module) {
  Nan::HandleScope scope;

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Nodehun").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "suggest", Nodehun::suggest);
  Nan::SetPrototypeMethod(tpl, "suggestSync", Nodehun::suggestSync);
  Nan::SetPrototypeMethod(tpl, "correct", Nodehun::correct);
  Nan::SetPrototypeMethod(tpl, "correctSync", Nodehun::correctSync);
  Nan::SetPrototypeMethod(tpl, "stem", Nodehun::stem);
  Nan::SetPrototypeMethod(tpl, "stemSync", Nodehun::stemSync);
  Nan::SetPrototypeMethod(tpl, "generate", Nodehun::generate);
  Nan::SetPrototypeMethod(tpl, "generateSync", Nodehun::generateSync);
  Nan::SetPrototypeMethod(tpl, "analyze", Nodehun::analyze);
  Nan::SetPrototypeMethod(tpl, "analyzeSync", Nodehun::analyzeSync);
  Nan::SetPrototypeMethod(tpl, "addWord", Nodehun::addWord);
  Nan::SetPrototypeMethod(tpl, "addWordSync", Nodehun::addWordSync);
  Nan::SetPrototypeMethod(tpl, "removeWord", Nodehun::removeWord);
  Nan::SetPrototypeMethod(tpl, "removeWordSync", Nodehun::removeWordSync);
  Nan::SetPrototypeMethod(tpl, "addDictionary", Nodehun::addDictionary);
  Nan::SetPrototypeMethod(tpl, "addDictionarySync", Nodehun::addDictionarySync);

  constructor.Reset(tpl->GetFunction());
  module->Set(Nan::New("exports").ToLocalChecked(), tpl->GetFunction());
}

NODE_MODULE(Nodehun, Nodehun::Init)

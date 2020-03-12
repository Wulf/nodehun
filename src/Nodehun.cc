#include "Nodehun.h"
#include <napi.h>
#include <hunspell.hxx>
#include "Async/AddDictionaryWorker.cc"
#include "Async/SpellWorker.cc"
#include "Async/SuggestWorker.cc"
#include "Async/AnalyzeWorker.cc"
#include "Async/StemWorker.cc"
#include "Async/GenerateWorker.cc"
#include "Async/AddWorker.cc"
#include "Async/AddWithAffixWorker.cc"
#include "Async/RemoveWorker.cc"

const std::string INVALID_NUMBER_OF_ARGUMENTS = "Invalid number of arguments.";
const std::string INVALID_FIRST_ARGUMENT = "First argument is invalid (incorrect type).";
const std::string INVALID_SECOND_ARGUMENT = "Second argument is invalid (incorrect type).";
const std::string INVALID_CONSTRUCTOR_CALL = "Use the new operator to create an instance of this object.";

// LOGGING
// #include <iostream>
// #include <fstream>
// std::ofstream logFile("log.txt");

Napi::FunctionReference Nodehun::constructor;

Napi::Object Nodehun::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "Nodehun", {
    InstanceMethod("addDictionary", &Nodehun::addDictionary),
    InstanceMethod("addDictionarySync", &Nodehun::addDictionarySync),
    InstanceMethod("spell", &Nodehun::spell),
    InstanceMethod("spellSync", &Nodehun::spellSync),
    InstanceMethod("suggest", &Nodehun::suggest),
    InstanceMethod("suggestSync", &Nodehun::suggestSync),
    InstanceMethod("analyze", &Nodehun::analyze),
    InstanceMethod("analyzeSync", &Nodehun::analyzeSync),
    InstanceMethod("stem", &Nodehun::stem),
    InstanceMethod("stemSync", &Nodehun::stemSync),
    InstanceMethod("generate", &Nodehun::generate),
    InstanceMethod("generateSync", &Nodehun::generateSync),
    InstanceMethod("add", &Nodehun::add),
    InstanceMethod("addSync", &Nodehun::addSync),
    InstanceMethod("addWithAffix", &Nodehun::addWithAffix),
    InstanceMethod("addWithAffixSync", &Nodehun::addWithAffixSync),
    InstanceMethod("remove", &Nodehun::remove),
    InstanceMethod("removeSync", &Nodehun::removeSync),
    InstanceMethod("getDictionaryEncoding", &Nodehun::getDictionaryEncoding),
    InstanceMethod("getWordCharacters", &Nodehun::getWordCharacters),
    InstanceMethod("getWordCharactersUTF16", &Nodehun::getWordCharactersUTF16),
    InstanceMethod("getVersion", &Nodehun::getVersion)
  });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("Nodehun", func);
  return exports;
}

Nodehun::Nodehun(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Nodehun>(info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  Napi::Buffer<char> affixBuffer = info[0].As<Napi::Buffer<char>>();
  Napi::Buffer<char> dictionaryBuffer = info[1].As<Napi::Buffer<char>>();

  context = new HunspellContext(new Hunspell(affixBuffer.Data(), dictionaryBuffer.Data(), NULL, true));
};

Nodehun::~Nodehun() {
  if (context) {
    delete context;
    context = NULL;
  }
}

Napi::Object Nodehun::NewInstance(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::EscapableHandleScope scope(env);

  if (info.Length() != 2) {
    Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS).ThrowAsJavaScriptException();
  } else if (!info[0].IsBuffer()) {
    Napi::Error::New(env, INVALID_FIRST_ARGUMENT).ThrowAsJavaScriptException();
  } else if (!info[1].IsBuffer()) {
    Napi::Error::New(env, INVALID_SECOND_ARGUMENT).ThrowAsJavaScriptException();
  } else if (!info.IsConstructCall()) {
    Napi::Error::New(env, INVALID_CONSTRUCTOR_CALL).ThrowAsJavaScriptException();
  }
  
  if (env.IsExceptionPending()) {
    return Napi::Object::New(env);
  } else {
    Napi::Object obj = constructor.New({info[0], info[1]});

    return scope.Escape(napi_value(obj)).ToObject();
  }
}

Napi::Value Nodehun::addDictionarySync(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    error.ThrowAsJavaScriptException();
    return error.Value();
  } else if (!info[0].IsBuffer()) {
    Napi::Error error = Napi::Error::New(env, INVALID_FIRST_ARGUMENT);
    error.ThrowAsJavaScriptException();
    return error.Value();
  } else {
    Napi::Buffer<char> dictionaryBuffer = info[0].As<Napi::Buffer<char>>();

    context->instance->add_dic(dictionaryBuffer.Data());
    
    return env.Undefined();
  }
}

Napi::Value Nodehun::addDictionary(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(info.Env());

  if (info.Length() != 1) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    deferred.Reject(error.Value());
  } else if (!info[0].IsBuffer()) {
    Napi::Error error = Napi::Error::New(env, INVALID_FIRST_ARGUMENT);
    deferred.Reject(error.Value());
  } else {
    Napi::Buffer<char> dictionaryBuffer = info[0].As<Napi::Buffer<char>>();

    AddDictionaryWorker* worker = new AddDictionaryWorker(
      context,
      deferred,
      dictionaryBuffer.Data()
    );

    worker->Queue();
  }

  return deferred.Promise();
}

Napi::Value Nodehun::spell(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

  if (info.Length() != 1) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    deferred.Reject(error.Value());
  } else if (!info[0].IsString()) {
    Napi::Error error = Napi::Error::New(env, INVALID_FIRST_ARGUMENT);
    deferred.Reject(error.Value());
  } else {
    std::string word = info[0].ToString().Utf8Value();

    SpellWorker* worker = new SpellWorker(
      context,
      deferred,
      word
    );

    worker->Queue();
  }

  return deferred.Promise();
}

Napi::Value Nodehun::spellSync(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() != 1) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    error.ThrowAsJavaScriptException();
    return error.Value();
  } else if (!info[0].IsString()) {
    Napi::Error error = Napi::Error::New(env, INVALID_FIRST_ARGUMENT);
    error.ThrowAsJavaScriptException();
    return error.Value();
  } else {
    std::string word = info[0].ToString().Utf8Value();

    context->lockRead();
    bool correct = context->instance->spell(word.c_str());
    context->unlockRead();
    
    return Napi::Boolean::New(env, correct);
  }
}

Napi::Value Nodehun::suggest(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

  if (info.Length() != 1) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    deferred.Reject(error.Value());
  } else if (!info[0].IsString()) {
    Napi::Error error = Napi::Error::New(env, INVALID_FIRST_ARGUMENT);
    deferred.Reject(error.Value());
  } else {
    std::string word = info[0].ToString().Utf8Value();

    SuggestWorker* worker = new SuggestWorker(
      context,
      deferred,
      word
    );

    worker->Queue();
  }

  return deferred.Promise();
}

Napi::Value Nodehun::suggestSync(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    error.ThrowAsJavaScriptException();
    return error.Value();
  } else if (!info[0].IsString()) {
    Napi::Error error = Napi::Error::New(env, INVALID_FIRST_ARGUMENT);
    error.ThrowAsJavaScriptException();
    return error.Value();
  } else {
    std::string word = info[0].ToString().Utf8Value();

    context->lockRead();
    bool isCorrect = this->context->instance->spell(word.c_str());
    
    if (isCorrect) {
      context->unlockRead();
      return env.Null();
    }
    
    char** suggestions = NULL;
    int length = this->context->instance->suggest(&suggestions, word.c_str());
    context->unlockRead();
    
    Napi::Array array = Napi::Array::New(env, length);
    for (int i = 0; i < length; i++) {
      array.Set(i, Napi::String::New(env, suggestions[i]));
    }

    this->context->instance->free_list(&suggestions, length);

    return array;
  }
}

Napi::Value Nodehun::analyze(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

  if (info.Length() != 1) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    deferred.Reject(error.Value());
  } else if (!info[0].IsString()) {
    Napi::Error error = Napi::Error::New(env, INVALID_FIRST_ARGUMENT);
    deferred.Reject(error.Value());
  } else {
    std::string word = info[0].ToString().Utf8Value();

    AnalyzeWorker* worker = new AnalyzeWorker(
      context,
      deferred,
      word
    );

    worker->Queue();
  }

  return deferred.Promise();
}

Napi::Value Nodehun::analyzeSync(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    error.ThrowAsJavaScriptException();
    return error.Value();
  } else if (!info[0].IsString()) {
    Napi::Error error = Napi::Error::New(env, INVALID_FIRST_ARGUMENT);
    error.ThrowAsJavaScriptException();
    return error.Value();
  } else {
    std::string word = info[0].ToString().Utf8Value();

    char** analysis = NULL;
    this->context->lockRead();
    int length = this->context->instance->analyze(&analysis, word.c_str());
    this->context->unlockRead();
    
    Napi::Array array = Napi::Array::New(env, length);
    for (int i = 0; i < length; i++) {
      array.Set(i, Napi::String::New(env, analysis[i]));
    }

    context->instance->free_list(&analysis, length);

    return array;
  }
}

Napi::Value Nodehun::stem(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

  if (info.Length() != 1) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    deferred.Reject(error.Value());
  } else if (!info[0].IsString()) {
    Napi::Error error = Napi::Error::New(env, INVALID_FIRST_ARGUMENT);
    deferred.Reject(error.Value());
  } else {
    std::string word = info[0].ToString().Utf8Value();

    StemWorker* worker = new StemWorker(
      context,
      deferred,
      word
    );

    worker->Queue();
  }

  return deferred.Promise();
}

Napi::Value Nodehun::stemSync(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    error.ThrowAsJavaScriptException();
    return error.Value();
  } else if (!info[0].IsString()) {
    Napi::Error error = Napi::Error::New(env, INVALID_FIRST_ARGUMENT);
    error.ThrowAsJavaScriptException();
    return error.Value();
  } else {
    std::string word = info[0].ToString().Utf8Value();

    char** stems = NULL;
    context->lockRead();
    int length = this->context->instance->stem(&stems, word.c_str());
    context->unlockRead();
    
    Napi::Array array = Napi::Array::New(env, length);
    for (int i = 0; i < length; i++) {
      array.Set(i, Napi::String::New(env, stems[i]));
    }

    context->instance->free_list(&stems, length);

    return array;
  }
}

Napi::Value Nodehun::generate(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

  if (info.Length() != 2) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    deferred.Reject(error.Value());
  } else if (!info[0].IsString()) {
    Napi::Error error = Napi::Error::New(env, INVALID_FIRST_ARGUMENT);
    deferred.Reject(error.Value());
  } else if (!info[1].IsString()) {
    Napi::Error error = Napi::Error::New(env, INVALID_SECOND_ARGUMENT);
    deferred.Reject(error.Value());
  } else {
    std::string word = info[0].ToString().Utf8Value();
    std::string example = info[1].ToString().Utf8Value();

    GenerateWorker* worker = new GenerateWorker(
      context,
      deferred,
      word,
      example
    );

    worker->Queue();
  }

  return deferred.Promise();
}

Napi::Value Nodehun::generateSync(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 2) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    error.ThrowAsJavaScriptException();
    return error.Value();
  } else if (!info[0].IsString()) {
    Napi::Error error = Napi::Error::New(env, INVALID_FIRST_ARGUMENT);
    error.ThrowAsJavaScriptException();
    return error.Value();
  } else if (!info[1].IsString()) {
    Napi::Error error = Napi::Error::New(env, INVALID_SECOND_ARGUMENT);
    error.ThrowAsJavaScriptException();
    return error.Value();
  } else {
    std::string word = info[0].ToString().Utf8Value();
    std::string example = info[1].ToString().Utf8Value();

    char** generates = NULL;
    context->lockRead();
    int length = this->context->instance->generate(
      &generates,
      word.c_str(),
      example.c_str()
    );
    context->unlockRead();
    
    Napi::Array array = Napi::Array::New(env, length);
    for (int i = 0; i < length; i++) {
      array.Set(i, Napi::String::New(env, generates[i]));
    }
    
    context->instance->free_list(&generates, length);

    return array;
  }
}

Napi::Value Nodehun::addSync(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    error.ThrowAsJavaScriptException();

    return error.Value();
  } else if (!info[0].IsString()) {
    Napi::Error error = Napi::Error::New(env, INVALID_FIRST_ARGUMENT);
    error.ThrowAsJavaScriptException();
    
    return error.Value();
  } else {
    std::string word = info[0].ToString().Utf8Value();

    context->lockWrite();
    context->instance->add(word.c_str());
    context->unlockWrite();
    
    return env.Undefined();
  }

}

Napi::Value Nodehun::add(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(info.Env());

  if (info.Length() != 1) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    deferred.Reject(error.Value());
  } else if (!info[0].IsString()) {
    Napi::Error error = Napi::Error::New(env, INVALID_FIRST_ARGUMENT);
    deferred.Reject(error.Value());
  } else {
    std::string word = info[0].ToString().Utf8Value();

    AddWorker* worker = new AddWorker(
      context,
      deferred,
      word
    );

    worker->Queue();
  }

  return deferred.Promise();
}

Napi::Value Nodehun::addWithAffixSync(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 2) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    error.ThrowAsJavaScriptException();

    return error.Value();
  } else if (!info[0].IsString()) {
    Napi::Error error = Napi::Error::New(env, INVALID_FIRST_ARGUMENT);
    error.ThrowAsJavaScriptException();
    
    return error.Value();
  } else if (!info[1].IsString()) {
    Napi::Error error = Napi::Error::New(env, INVALID_SECOND_ARGUMENT);
    error.ThrowAsJavaScriptException();

    return error.Value();
  } else {
    std::string word = info[0].ToString().Utf8Value();
    std::string example = info[1].ToString().Utf8Value();

    context->lockWrite();
    context->instance->add_with_affix(word.c_str(), example.c_str());
    context->unlockWrite();
    
    return env.Undefined();
  }

}

Napi::Value Nodehun::addWithAffix(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(info.Env());

  if (info.Length() != 2) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    deferred.Reject(error.Value());
  } else if (!info[0].IsString()) {
    Napi::Error error = Napi::Error::New(env, INVALID_FIRST_ARGUMENT);
    deferred.Reject(error.Value());
  } else if (!info[1].IsString()) {
    Napi::Error error = Napi::Error::New(env, INVALID_SECOND_ARGUMENT);
    deferred.Reject(error.Value());
  } else {
    std::string word = info[0].ToString().Utf8Value();
    std::string example = info[1].ToString().Utf8Value();

    AddWithAffixWorker* worker = new AddWithAffixWorker(
      context,
      deferred,
      word,
      example
    );

    worker->Queue();
  }

  return deferred.Promise();
}

Napi::Value Nodehun::removeSync(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    error.ThrowAsJavaScriptException();

    return error.Value();
  } else if (!info[0].IsString()) {
    Napi::Error error = Napi::Error::New(env, INVALID_FIRST_ARGUMENT);
    error.ThrowAsJavaScriptException();
    
    return error.Value();
  } else {
    std::string word = info[0].ToString().Utf8Value();

    context->lockWrite();
    context->instance->remove(word.c_str());
    context->unlockWrite();
    
    return env.Undefined();
  }

}

Napi::Value Nodehun::remove(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(info.Env());

  if (info.Length() != 1) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    deferred.Reject(error.Value());
  } else if (!info[0].IsString()) {
    Napi::Error error = Napi::Error::New(env, INVALID_FIRST_ARGUMENT);
    deferred.Reject(error.Value());
  } else {
    std::string word = info[0].ToString().Utf8Value();

    RemoveWorker* worker = new RemoveWorker(
      context,
      deferred,
      word
    );

    worker->Queue();
  }

  return deferred.Promise();
}

Napi::Value Nodehun::getDictionaryEncoding(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() > 0) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    error.ThrowAsJavaScriptException();
    return error.Value();
  }

  char* encoding = this->context->instance->get_dic_encoding();
  
  if (encoding == NULL) {
    return env.Undefined();
  } else {
    return Napi::String::New(env, encoding);
  }

}

Napi::Value Nodehun::getWordCharacters(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() > 0) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    error.ThrowAsJavaScriptException();
    return error.Value();
  }

  const char* wordCharacters = this->context->instance->get_wordchars();
  
  if (wordCharacters == NULL) {
    return env.Undefined();
  } else {
    return Napi::String::New(env, wordCharacters);
  }
}

Napi::Value Nodehun::getWordCharactersUTF16(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() > 0) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    error.ThrowAsJavaScriptException();
    return error.Value();
  }

  int length = 0;
  unsigned short* chars = this->context->instance->get_wordchars_utf16(&length);

  if (chars == NULL) {
    return env.Undefined();
  } else {
    return Napi::String::New(env, ((char16_t*) chars));
  }
}

Napi::Value Nodehun::getVersion(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() > 0) {
    Napi::Error error = Napi::Error::New(env, INVALID_NUMBER_OF_ARGUMENTS);
    error.ThrowAsJavaScriptException();
    return error.Value();
  }

  const char* v = this->context->instance->get_version();

  if (v == NULL) {
    return env.Undefined();
  } else {
    return Napi::String::New(env, v);
  }
}

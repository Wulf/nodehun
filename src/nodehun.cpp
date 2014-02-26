#include "nodehun.hpp"
#include <cstring>
#include <iostream>
using namespace v8;
using node::Buffer;

Persistent<FunctionTemplate> Nodehun::SpellDictionary::constructor;

void Nodehun::SpellDictionary::Init(Handle<Object> exports, Handle<Object> module) {
  HandleScope scope;
  
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  
  constructor = Persistent<FunctionTemplate>::New(tpl);
  constructor->InstanceTemplate()->SetInternalFieldCount(6);
  constructor->SetClassName(String::NewSymbol("NodehunDictionary"));
  
  NODE_SET_PROTOTYPE_METHOD(constructor, "spellSuggest", spellSuggest);
  NODE_SET_PROTOTYPE_METHOD(constructor, "spellSuggestions", spellSuggestions);
  NODE_SET_PROTOTYPE_METHOD(constructor,"addDictionary", addDictionary);
  NODE_SET_PROTOTYPE_METHOD(constructor,"addWord", addWord);
  NODE_SET_PROTOTYPE_METHOD(constructor,"removeWord", removeWord);
  NODE_SET_PROTOTYPE_METHOD(constructor,"analyze", analyze);
  NODE_SET_PROTOTYPE_METHOD(constructor,"stem", stem);
  //NODE_SET_PROTOTYPE_METHOD(constructor,"generate", generate);
  
  module->Set(String::NewSymbol("exports"), constructor->GetFunction());
}

Nodehun::SpellDictionary::SpellDictionary(const char *affbuf, const char *dictbuf){
  spellClass = new Hunspell(affbuf, dictbuf,NULL,true);
}

Handle<Value> Nodehun::SpellDictionary::New(const Arguments& args) {
  HandleScope scope;
  int argl = args.Length();
  if (!args.IsConstructCall())
    return ThrowException(Exception::Error(String::New("Use the new operator to create an instance of this object.")));
  if(argl < 2)
    return ThrowException(Exception::Error(String::New("Constructor requires two arguments.")));
  if(!Buffer::HasInstance(args[0]))
    return ThrowException(Exception::TypeError(String::New("First argument must be a buffer")));
  if(!Buffer::HasInstance(args[1]))
    return ThrowException(Exception::TypeError(String::New("Second argument must be a buffer")));

  Nodehun::SpellDictionary *obj = new Nodehun::SpellDictionary(Buffer::Data(args[0].As<Object>()), Buffer::Data(args[1].As<Object>()));
  obj->Wrap(args.This());  
  return args.This();
}

Handle<Value> Nodehun::SpellDictionary::spellSuggest(const Arguments& args) {
  HandleScope scope;
  if (args.Length() < 2) 
    return ThrowException(Exception::TypeError(String::New("Missing required arguments.")));
  if(!args[0]->IsString())
    return ThrowException(Exception::TypeError(String::New("First argument must be a string.")));
  if(!args[1]->IsFunction())
    return ThrowException(Exception::TypeError(String::New("Second argument must be a function.")));

  Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
  
  String::Utf8Value arg0(args[0]->ToString());
  Local<Function> callback = Local<Function>::Cast(args[1]);
  
  Nodehun::SpellData* spellData = new Nodehun::SpellData();
  spellData->request.data = spellData;
  spellData->callback = Persistent<Function>::New(callback);
  spellData->word.append(*arg0);
  
  spellData->obj = obj;
  spellData->multiple = false;
  uv_queue_work(uv_default_loop(), &spellData->request,
		Nodehun::SpellDictionary::checkSuggestions, Nodehun::SpellDictionary::sendSuggestions);
  return Undefined();
}

Handle<Value> Nodehun::SpellDictionary::spellSuggestions(const Arguments& args) {
  HandleScope scope;
  if (args.Length() < 2) 
    return ThrowException(Exception::TypeError(String::New("Missing required arguments.")));
  if(!args[0]->IsString())
    return ThrowException(Exception::TypeError(String::New("First argument must be a string.")));
  if(!args[1]->IsFunction())
    return ThrowException(Exception::TypeError(String::New("Second argument must be a function.")));

  Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
  
  String::Utf8Value arg0(args[0]->ToString());
  Local<Function> callback = Local<Function>::Cast(args[1]);
  
  Nodehun::SpellData* spellData = new Nodehun::SpellData();
  spellData->request.data = spellData;
  spellData->callback = Persistent<Function>::New(callback);
  spellData->word.append(*arg0);
  
  spellData->obj = obj;
  spellData->multiple = true;
  uv_queue_work(uv_default_loop(), &spellData->request,
		Nodehun::SpellDictionary::checkSuggestions, Nodehun::SpellDictionary::sendSuggestions);
  return Undefined();
}

void Nodehun::SpellDictionary::checkSuggestions(uv_work_t* request) {
  Nodehun::SpellData* spellData = static_cast<Nodehun::SpellData*>(request->data);
  char** suggestions;
  spellData->wordCorrect = spellData->obj->spellClass->spell(spellData->word.c_str());
  if (!spellData->wordCorrect)
    spellData->numSuggest = spellData->obj->spellClass->suggest(&suggestions, spellData->word.c_str());
  else
    spellData->numSuggest = 0;
  spellData->suggestions = new char*[spellData->numSuggest];
  for(int i = 0, l = spellData->numSuggest; i < l; i++){
    spellData->suggestions[i] = new char[strlen(suggestions[i]) + 1];
    strcpy(spellData->suggestions[i],suggestions[i]);
  }
  spellData->obj->spellClass->free_list(&suggestions,spellData->numSuggest);
}

void Nodehun::SpellDictionary::sendSuggestions(uv_work_t* request, int i){
  HandleScope scope;
  Nodehun::SpellData* spellData = static_cast<Nodehun::SpellData*>(request->data);
  
  const unsigned argc = 2;
  Local<Value> argv[argc];
  argv[0] = Local<Value>::New(Boolean::New(spellData->wordCorrect));
  if(spellData->wordCorrect || spellData->numSuggest == 0){
    if(spellData->multiple)
      argv[1] = Array::New(0);
    else
      argv[1] = Local<Value>::New(Null());
  }
  else if(spellData->numSuggest > 0){
    if(spellData->multiple){
      Local<Array> suglist = Array::New(spellData->numSuggest);
      for(int i = 0; i < spellData->numSuggest; i++){
	suglist->Set(i,String::New(spellData->suggestions[i]));
	delete spellData->suggestions[i];
      }
      delete spellData->suggestions;
      argv[1] = suglist;
    }
    else{
      argv[1] = String::New(spellData->suggestions[0]);
    }
  }
  TryCatch try_catch;
  spellData->callback->Call(Context::GetCurrent()->Global(), argc, argv);
  if (try_catch.HasCaught())
    node::FatalException(try_catch);
  spellData->callback.Dispose();
  delete spellData;
}

Handle<Value> Nodehun::SpellDictionary::addDictionary(const Arguments& args) {
  HandleScope scope;
  int argl = args.Length();
  if(argl < 1)
    return ThrowException(Exception::Error(String::New("addDictionary requires at least one argument.")));
  if(!Buffer::HasInstance(args[0]))
    return ThrowException(Exception::TypeError(String::New("First argument must be a buffer")));
  
  Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
  Nodehun::DictData* dictData = new Nodehun::DictData();
  dictData->callbackExists = false;

  if(argl > 1 && args[1]->IsFunction()){
    Local<Function> callback = Local<Function>::Cast(args[1]);
    dictData->callback = Persistent<Function>::New(callback);
    dictData->callbackExists = true;
  }
  dictData->dict = new char[Buffer::Length(args[0]) + 1];
  strcpy(dictData->dict, Buffer::Data(args[0].As<Object>()));
  dictData->obj = obj;
  dictData->request.data = dictData;

  uv_queue_work(uv_default_loop(), &dictData->request,
		Nodehun::SpellDictionary::addDictionaryWork, Nodehun::SpellDictionary::addDictionaryFinish);
  return Undefined();
}

void Nodehun::SpellDictionary::addDictionaryWork(uv_work_t* request){
  Nodehun::DictData* dictData = static_cast<Nodehun::DictData*>(request->data);
  int status = dictData->obj->spellClass->add_dic(dictData->dict, true);
  dictData->success = status == 0;
}

void Nodehun::SpellDictionary::addDictionaryFinish(uv_work_t* request, int i){
  HandleScope scope;
  Nodehun::DictData* dictData = static_cast<Nodehun::DictData*>(request->data);
  
  if(dictData->callbackExists){
    const unsigned argc = 1;
    Local<Value> argv[argc];
    argv[0] = Local<Value>::New(Boolean::New(dictData->success));
    TryCatch try_catch;
    dictData->callback->Call(Context::GetCurrent()->Global(), argc, argv);
    if (try_catch.HasCaught()) {
      node::FatalException(try_catch);
    }
    dictData->callback.Dispose();
  }
  delete dictData->dict;
  delete dictData;
}

Handle<Value> Nodehun::SpellDictionary::addWord(const Arguments& args) {
  HandleScope scope;
  
  if (args.Length() < 1 || !args[0]->IsString())
    return ThrowException(Exception::TypeError(String::New("First argument must be a string.")));

  Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
  String::Utf8Value arg0(args[0]->ToString());
  Nodehun::WordData* wordData = new Nodehun::WordData();
  if(args.Length() > 1 && args[1]->IsFunction()){
    Local<Function> callback = Local<Function>::Cast(args[1]);
    wordData->callback = Persistent<Function>::New(callback);
    wordData->callbackExists = true;
  }
  else{
    wordData->callbackExists = false;
  }
  //add word
  wordData->removeWord = false;
  wordData->word.append(*arg0);
  wordData->obj = obj;
  wordData->request.data = wordData;
  
  uv_queue_work(uv_default_loop(), &wordData->request,
		Nodehun::SpellDictionary::addRemoveWordWork, Nodehun::SpellDictionary::addRemoveWordFinish);
  return Undefined();
}

Handle<Value> Nodehun::SpellDictionary::removeWord(const Arguments& args) {
  HandleScope scope;
  
  if (args.Length() < 1 || !args[0]->IsString())
    return ThrowException(Exception::TypeError(String::New("First argument must be a string.")));

  Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
  String::Utf8Value arg0(args[0]->ToString());
  Nodehun::WordData* wordData = new Nodehun::WordData();
  if(args.Length() > 1 && args[1]->IsFunction()){
    Local<Function> callback = Local<Function>::Cast(args[1]);
    wordData->callback = Persistent<Function>::New(callback);
    wordData->callbackExists = true;
  }
  else{
    wordData->callbackExists = false;
  }
  //remove word
  wordData->removeWord = true;
  wordData->word.append(*arg0);
  wordData->obj = obj;
  wordData->request.data = wordData;
  
  uv_queue_work(uv_default_loop(), &wordData->request,
		Nodehun::SpellDictionary::addRemoveWordWork, Nodehun::SpellDictionary::addRemoveWordFinish);
  return Undefined();
}

void Nodehun::SpellDictionary::addRemoveWordWork(uv_work_t* request){
  Nodehun::WordData* wordData = static_cast<Nodehun::WordData*>(request->data);
  int status;
  if(wordData->removeWord)
    status = wordData->obj->spellClass->remove(wordData->word.c_str());
  else
    status = wordData->obj->spellClass->add(wordData->word.c_str());
  wordData->success = status == 0;
}

void Nodehun::SpellDictionary::addRemoveWordFinish(uv_work_t* request, int i){
  HandleScope scope;
  Nodehun::WordData* wordData = static_cast<Nodehun::WordData*>(request->data);
  
  if(wordData->callbackExists){
    const unsigned argc = 2;
    Local<Value> argv[argc];
    argv[0] = Local<Value>::New(Boolean::New(wordData->success));
    argv[1] = Local<Value>::New(String::New(wordData->word.c_str()));
    TryCatch try_catch;
    wordData->callback->Call(Context::GetCurrent()->Global(), argc, argv);

    if (try_catch.HasCaught())
      node::FatalException(try_catch);

    wordData->callback.Dispose();
  }
  delete wordData;
}

Handle<Value> Nodehun::SpellDictionary::analyze(const Arguments& args) {
  HandleScope scope;  
  if (args.Length() < 1 || !args[0]->IsString())
    return ThrowException(Exception::TypeError(String::New("First argument must be a string.")));

  Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
  String::Utf8Value arg0(args[0]->ToString());
  Nodehun::AnalyzeData* analyzeData = new Nodehun::AnalyzeData();
  if(args.Length() > 1 && args[1]->IsFunction()){
    Local<Function> callback = Local<Function>::Cast(args[1]);
    analyzeData->callback = Persistent<Function>::New(callback);
    analyzeData->callbackExists = true;
  }
  else{
    analyzeData->callbackExists = false;
  }
  analyzeData->word.append(*arg0);
  analyzeData->obj = obj;
  analyzeData->request.data = analyzeData;
  
  uv_queue_work(uv_default_loop(), &analyzeData->request,
		Nodehun::SpellDictionary::analyzeWork, Nodehun::SpellDictionary::analyzeFinish);
  return Undefined();
}

void Nodehun::SpellDictionary::analyzeWork(uv_work_t* request){
  Nodehun::AnalyzeData* analyzeData = static_cast<Nodehun::AnalyzeData*>(request->data);
  analyzeData->numResults = analyzeData->obj->spellClass->analyze(&analyzeData->results, analyzeData->word.c_str());
}

void Nodehun::SpellDictionary::analyzeFinish(uv_work_t* request, int i){
  HandleScope scope;
  Nodehun::AnalyzeData* analyzeData = static_cast<Nodehun::AnalyzeData*>(request->data);
  
  if(analyzeData->callbackExists){
    const unsigned int argc = 1;
    Local<Value> argv[argc];
    Local<Array> suglist = Array::New(analyzeData->numResults);
    for(int i = 0; i < analyzeData->numResults; i++)
      suglist->Set(i,String::New(analyzeData->results[i]));
    analyzeData->obj->spellClass->free_list(&analyzeData->results,analyzeData->numResults);
    argv[0] = suglist;
    TryCatch try_catch;
    analyzeData->callback->Call(Context::GetCurrent()->Global(), argc, argv);
  
    if(try_catch.HasCaught())
      node::FatalException(try_catch);
    analyzeData->callback.Dispose();
  }
  else{
    analyzeData->obj->spellClass->free_list(&analyzeData->results,analyzeData->numResults);
  }
  delete analyzeData;
}

Handle<Value> Nodehun::SpellDictionary::stem(const Arguments& args) {
  HandleScope scope;  
  if (args.Length() < 1 || !args[0]->IsArray())
    return ThrowException(Exception::TypeError(String::New("First argument must be an array.")));

  Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
  Local<Array> arg0 = Local<Array>::Cast(args[0]);
  int l = arg0->Length();
  char **words = new char*[l];
  int numWords = 0;
  Local<Value> val;
  for(int i = 0; i < l; i++){
    Local<Value> val = arg0->Get(i);
    if(val->IsString()){
      v8::String::Utf8Value str(val->ToString());
      words[i] = new char[str.length()];
      strcpy(words[i], *str);
      numWords++;
    }
  }
  Nodehun::StemData* stemData = new Nodehun::StemData();
  stemData->words = words;
  stemData->numWords = numWords;
  if(args.Length() > 1 && args[1]->IsFunction()){
    Local<Function> callback = Local<Function>::Cast(args[1]);
    stemData->callback = Persistent<Function>::New(callback);
    stemData->callbackExists = true;
  }
  else{
    stemData->callbackExists = false;
  }

  stemData->obj = obj;
  stemData->request.data = stemData;
  
  uv_queue_work(uv_default_loop(), &stemData->request,
		Nodehun::SpellDictionary::stemWork, Nodehun::SpellDictionary::stemFinish);
  return Undefined();
}

void Nodehun::SpellDictionary::stemWork(uv_work_t* request){
  Nodehun::StemData* stemData = static_cast<Nodehun::StemData*>(request->data);
  stemData->numResults = stemData->obj->spellClass->stem(&stemData->results, stemData->words, stemData->numWords);
  for(int i = 0; i < stemData->numWords; i++)
    delete stemData->words[i];
  delete stemData->words;
}

void Nodehun::SpellDictionary::stemFinish(uv_work_t* request, int i){
  HandleScope scope;
  Nodehun::StemData* stemData = static_cast<Nodehun::StemData*>(request->data);
  
  if(stemData->callbackExists){
    const unsigned int argc = 1;
    Local<Value> argv[argc];
    Local<Array> suglist = Array::New(stemData->numResults);
    int i;
    for(i = 0; i < stemData->numResults; i++)
      suglist->Set(i,String::New(stemData->results[i]));
    stemData->obj->spellClass->free_list(&stemData->results,stemData->numResults);
    argv[0] = suglist;
    TryCatch try_catch;
    stemData->callback->Call(Context::GetCurrent()->Global(), argc, argv);
  
    if(try_catch.HasCaught())
      node::FatalException(try_catch);
    stemData->callback.Dispose();
  }
  else{
    stemData->obj->spellClass->free_list(&stemData->results,stemData->numResults);
  }
  delete stemData;
}

NODE_MODULE(nodehun, Nodehun::SpellDictionary::Init);

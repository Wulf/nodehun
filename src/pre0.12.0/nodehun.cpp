#include "nodehun.hpp"

using namespace v8;
using node::Buffer;

Persistent<FunctionTemplate> Nodehun::SpellDictionary::constructor;

void Nodehun::SpellDictionary::Init(Handle<Object> exports, Handle<Object> module) 
{
  HandleScope scope;
  
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  constructor = Persistent<FunctionTemplate>::New(tpl);
  constructor->InstanceTemplate()->SetInternalFieldCount(1);
  constructor->SetClassName(String::NewSymbol("NodehunDictionary"));
  //static
  NODE_SET_METHOD(constructor, "createNewNodehun" , createNewNodehun);    
  //prototype
  NODE_SET_PROTOTYPE_METHOD(constructor, "isCorrect", isCorrect);
  NODE_SET_PROTOTYPE_METHOD(constructor, "isCorrectSync", isCorrectSync);
  NODE_SET_PROTOTYPE_METHOD(constructor, "spellSuggest", spellSuggest);
  NODE_SET_PROTOTYPE_METHOD(constructor, "spellSuggestSync", spellSuggestSync);
  NODE_SET_PROTOTYPE_METHOD(constructor, "spellSuggestions", spellSuggestions);
  NODE_SET_PROTOTYPE_METHOD(constructor, "spellSuggestionsSync", spellSuggestionsSync);
  NODE_SET_PROTOTYPE_METHOD(constructor, "addDictionary", addDictionary);
  NODE_SET_PROTOTYPE_METHOD(constructor, "addDictionarySync", addDictionarySync);
  NODE_SET_PROTOTYPE_METHOD(constructor, "addWord", addWord);
  NODE_SET_PROTOTYPE_METHOD(constructor, "addWordSync", addWordSync);
  NODE_SET_PROTOTYPE_METHOD(constructor, "removeWord", removeWord);
  NODE_SET_PROTOTYPE_METHOD(constructor, "removeWordSync", removeWordSync);
  NODE_SET_PROTOTYPE_METHOD(constructor, "stem", stem);
  NODE_SET_PROTOTYPE_METHOD(constructor, "stemSync", stemSync);
  NODE_SET_PROTOTYPE_METHOD(constructor, "generate", generate);
  NODE_SET_PROTOTYPE_METHOD(constructor, "generateSync", generateSync);
  NODE_SET_PROTOTYPE_METHOD(constructor, "analyze", analyze);
  NODE_SET_PROTOTYPE_METHOD(constructor, "analyzeSync", analyzeSync);
  
  module->Set(String::NewSymbol("exports"), constructor->GetFunction());
}

Handle<Value> Nodehun::SpellDictionary::createNewNodehun(const v8::Arguments& args)
{
  HandleScope scope;
  int argl = args.Length();
  if(argl > 2 && args[2]->IsFunction()) {      
    Persistent<Function> callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));
    const unsigned argc = 2;
    Local<Value> argv[argc];
    argv[1] = Local<Value>::New(Null());
    if(!Buffer::HasInstance(args[0])) {
      TryCatch try_catch;
      argv[0] = Exception::TypeError(String::New("First argument must be a buffer"));
      callback->Call(Context::GetCurrent()->Global(), argc, argv);
      if (try_catch.HasCaught())
	node::FatalException(try_catch);
      callback.Dispose();
    }
    else if(!Buffer::HasInstance(args[1])) {
      TryCatch try_catch;
      argv[0] = Exception::TypeError(String::New("Second argument must be a buffer"));
      callback->Call(Context::GetCurrent()->Global(), argc, argv);
      if (try_catch.HasCaught())
	node::FatalException(try_catch);
      callback.Dispose();
    }
    else {
      Nodehun::NodehunData* nodeData = new Nodehun::NodehunData();
      nodeData->callback = callback;
      nodeData->aff = new char[Buffer::Length(args[0])];
      strcpy(nodeData->aff, Buffer::Data(args[0].As<Object>()));
      nodeData->dict = new char[Buffer::Length(args[1])];
      strcpy(nodeData->dict, Buffer::Data(args[1].As<Object>()));
      nodeData->request.data = nodeData;
      uv_queue_work(uv_default_loop(), &nodeData->request,
		    Nodehun::SpellDictionary::createNewNodehunWork, Nodehun::SpellDictionary::createNewNodehunFinish);
    }
  }
  return scope.Close(Undefined());
}

void Nodehun::SpellDictionary::createNewNodehunWork(uv_work_t* request)
{  
  Nodehun::NodehunData* nodeData = static_cast<Nodehun::NodehunData*>(request->data);
  nodeData->obj = new Hunspell(nodeData->aff, nodeData->dict, NULL, true);
  delete nodeData->aff;
  delete nodeData->dict;
}

void Nodehun::SpellDictionary::createNewNodehunFinish(uv_work_t* request, int i)
{
  HandleScope scope;
  Nodehun::NodehunData* nodeData = static_cast<Nodehun::NodehunData*>(request->data);  
  const unsigned argc = 2;
  Local<Value> argv[argc];
  Handle<Value> ext = External::New(nodeData->obj);
  argv[0] = Local<Value>::New(Null());
  argv[1] = constructor->GetFunction()->NewInstance(1, &ext);
  TryCatch try_catch;
  nodeData->callback->Call(Context::GetCurrent()->Global(), argc, argv);
  if (try_catch.HasCaught())
    node::FatalException(try_catch);
  nodeData->callback.Dispose();
  delete nodeData;
}


Nodehun::SpellDictionary::SpellDictionary(const char *affbuf, const char *dictbuf)
{
  spellClass = new Hunspell(affbuf, dictbuf, NULL, true);
}

Nodehun::SpellDictionary::SpellDictionary(Hunspell *obj)
{
  spellClass = obj;
}

Handle<Value> Nodehun::SpellDictionary::New(const Arguments& args) 
{
  HandleScope scope;
  int argl = args.Length();
  if (!args.IsConstructCall())
    return scope.Close(ThrowException(Exception::Error(String::New("Use the new operator to create an instance of this object."))));
  if(argl > 0 && args[0]->IsExternal()) {
    Local<External> ext = Local<External>::Cast(args[0]);
    void *ptr = ext->Value();
    Nodehun::SpellDictionary *obj = new Nodehun::SpellDictionary(static_cast<Hunspell*>(ptr));
    uv_rwlock_init(&(obj->rwlock));
    obj->Wrap(args.This());
  }
  else {
    if(argl < 2)
      return scope.Close(ThrowException(Exception::Error(String::New("Constructor requires two arguments."))));
    if(!Buffer::HasInstance(args[0]))
      return scope.Close(ThrowException(Exception::TypeError(String::New("First argument must be a buffer"))));
    if(!Buffer::HasInstance(args[1]))
      return scope.Close(ThrowException(Exception::TypeError(String::New("Second argument must be a buffer"))));

    Nodehun::SpellDictionary *obj = new Nodehun::SpellDictionary(Buffer::Data(args[0].As<Object>()), Buffer::Data(args[1].As<Object>()));
    uv_rwlock_init(&(obj->rwlock));
    obj->Wrap(args.This());  
  }
  return scope.Close(args.This());
}

Handle<Value> Nodehun::SpellDictionary::isCorrect(const Arguments& args) 
{
  HandleScope scope;
  int argl = args.Length();
  if(argl > 1 && args[1]->IsFunction()) {
    Persistent<Function> callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));
    const unsigned argc = 3;
    Local<Value> argv[argc];
    argv[1] = Local<Value>::New(Null());
    argv[2] = Local<Value>::New(Null());
    if(argl < 1 || !args[0]->IsString()) {
      TryCatch try_catch;
      argv[0] = Exception::TypeError(String::New("First argument must be a string"));
      callback->Call(Context::GetCurrent()->Global(), argc, argv);
      if (try_catch.HasCaught())
	node::FatalException(try_catch);
      callback.Dispose();
    }
    else {
      Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
      Nodehun::CorrectData* corrData = new Nodehun::CorrectData();  
      String::Utf8Value arg0(args[0]->ToString());

      corrData->callback = callback;
      corrData->request.data = corrData;
      corrData->word.append(*arg0);
      corrData->obj = obj;
      corrData->isCorrect = false;
      uv_queue_work(uv_default_loop(), &corrData->request,
      	    Nodehun::SpellDictionary::checkCorrect, Nodehun::SpellDictionary::sendCorrect);
    }
  }
  return scope.Close(Undefined());
}

Handle<Value> Nodehun::SpellDictionary::isCorrectSync(const Arguments& args) 
{
  HandleScope scope;
  if(args.Length() > 0 && args[0]->IsString()) {
    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
    String::Utf8Value arg0(args[0]->ToString());
    return scope.Close(Boolean::New(obj->spell(*arg0)));
  }
  else {
    return scope.Close(ThrowException(Exception::TypeError(String::New("First argument must be a string."))));
  }
}

bool Nodehun::SpellDictionary::spell(const char* word) 
{
  uv_rwlock_rdlock(&(this->rwlock));
  bool correct = this->spellClass->spell(word);
  uv_rwlock_rdunlock(&(this->rwlock));
  return correct;
}

void Nodehun::SpellDictionary::checkCorrect(uv_work_t* request) 
{
  Nodehun::CorrectData* corrData = static_cast<Nodehun::CorrectData*>(request->data);
  corrData->isCorrect = corrData->obj->spell(corrData->word.c_str());
}

void Nodehun::SpellDictionary::sendCorrect(uv_work_t* request, int i)
{
  HandleScope scope;
  Nodehun::CorrectData* corrData = static_cast<Nodehun::CorrectData*>(request->data);
  const unsigned argc = 3;
  Local<Value> argv[argc];
  argv[0] = Local<Value>::New(Null());
  argv[1] = Local<Value>::New(Boolean::New(corrData->isCorrect));
  argv[2] = Local<Value>::New(String::New(corrData->word.c_str()));
  TryCatch try_catch;
  corrData->callback->Call(Context::GetCurrent()->Global(), argc, argv);
  if (try_catch.HasCaught())
    node::FatalException(try_catch);
  corrData->callback.Dispose();
  delete corrData;
}

Handle<Value> Nodehun::SpellDictionary::spellSuggest(const Arguments& args) 
{
  HandleScope scope;
  int argl = args.Length();
  if(argl > 1 && args[1]->IsFunction()) {
    Persistent<Function> callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));
    const unsigned argc = 3;
    Local<Value> argv[argc];
    argv[1] = Local<Value>::New(Null());
    argv[2] = Local<Value>::New(Null());
    if(argl < 1 || !args[0]->IsString()) {
      TryCatch try_catch;
      argv[0] = Exception::TypeError(String::New("First argument must be a string"));
      callback->Call(Context::GetCurrent()->Global(), argc, argv);
      if (try_catch.HasCaught())
	node::FatalException(try_catch);
      callback.Dispose();
    }
    else {
      Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
      Nodehun::SpellData* spellData = new Nodehun::SpellData();  
      String::Utf8Value arg0(args[0]->ToString());

      spellData->callback = callback;
      spellData->request.data = spellData;
      spellData->word.append(*arg0);
      spellData->obj = obj;
      spellData->multiple = false;
      uv_queue_work(uv_default_loop(), &spellData->request,
      	    Nodehun::SpellDictionary::checkSuggestions, Nodehun::SpellDictionary::sendSuggestions);
    }
  }
  return scope.Close(Undefined());
}

Handle<Value> Nodehun::SpellDictionary::spellSuggestSync(const Arguments& args) 
{
  HandleScope scope;
  if(args.Length() && args[0]->IsString()) {
    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
    String::Utf8Value arg0(args[0]->ToString());
    bool corr = false;
    char** arr = NULL;
    int num = obj->spellCheck(&corr, &arr, *arg0);
    if(num > 0){
      Local<String> s = String::New(arr[0]);
      obj->spellClass->free_list(&arr, num);
      return scope.Close(s);
    }
    else{
      obj->spellClass->free_list(&arr, num);
      return scope.Close(Local<Value>::New(Null()));
    }
  }
  else {
    return scope.Close(ThrowException(Exception::TypeError(String::New("First argument must be a string."))));  
  }
}

Handle<Value> Nodehun::SpellDictionary::spellSuggestions(const Arguments& args) 
{
  HandleScope scope;
  int argl = args.Length();
  if(argl > 1 && args[1]->IsFunction()) {
    Persistent<Function> callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));
    const unsigned argc = 3;
    Local<Value> argv[argc];
    argv[1] = Local<Value>::New(Null());
    argv[2] = Local<Value>::New(Null());
    if(argl < 1 || !args[0]->IsString()) {
      TryCatch try_catch;
      argv[0] = Exception::TypeError(String::New("First argument must be a string"));
      callback->Call(Context::GetCurrent()->Global(), argc, argv);
      if (try_catch.HasCaught())
	node::FatalException(try_catch);
      callback.Dispose();
    }
    else {

      Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
      Nodehun::SpellData* spellData = new Nodehun::SpellData();
      String::Utf8Value arg0(args[0]->ToString());  

      spellData->callback = callback;
      spellData->request.data = spellData;
      
      spellData->word.append(*arg0);
      spellData->obj = obj;
      spellData->multiple = true;
      uv_queue_work(uv_default_loop(), &spellData->request,
		    Nodehun::SpellDictionary::checkSuggestions, Nodehun::SpellDictionary::sendSuggestions);
    }
  }
  return scope.Close(Undefined());
}

Handle<Value> Nodehun::SpellDictionary::spellSuggestionsSync(const Arguments& args) 
{
  HandleScope scope;
  if(args.Length() > 0 && args[0]->IsString()) {
    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
    String::Utf8Value arg0(args[0]->ToString());
    bool corr = false;
    char** arr = NULL;
    int num = obj->spellCheck(&corr, &arr, *arg0);
    Local<Array> a = Array::New(num);
    for(int i = 0; i < num; i++)
      a->Set(i, String::New(arr[i]));
    obj->spellClass->free_list(&arr, num);
    return scope.Close(a);
  }
  else {
    return scope.Close(ThrowException(Exception::TypeError(String::New("First argument must be a string."))));  
  }
}

int Nodehun::SpellDictionary::spellCheck(bool* correct, char*** arr, const char* word)
{
  int sugg = 0;
  uv_rwlock_rdlock(&(this->rwlock));
  *correct = this->spellClass->spell(word);
  if(!*correct)
    sugg = this->spellClass->suggest(arr, word);
  uv_rwlock_rdunlock(&(this->rwlock));
  return sugg;
}

void Nodehun::SpellDictionary::checkSuggestions(uv_work_t* request) 
{
  Nodehun::SpellData* spellData = static_cast<Nodehun::SpellData*>(request->data);
  uv_rwlock_rdlock(&(spellData->obj->rwlock));
  spellData->wordCorrect = spellData->obj->spellClass->spell(spellData->word.c_str());
  if (!spellData->wordCorrect)
    spellData->numSuggest = spellData->obj->spellClass->suggest(&(spellData->suggestions), spellData->word.c_str());
  else
    spellData->numSuggest = 0;
  uv_rwlock_rdunlock(&(spellData->obj->rwlock));
}

void Nodehun::SpellDictionary::sendSuggestions(uv_work_t* request, int i)
{
  HandleScope scope;
  Nodehun::SpellData* spellData = static_cast<Nodehun::SpellData*>(request->data);
  const unsigned argc = 4;
  Local<Value> argv[argc];
  argv[0] = Local<Value>::New(Null());
  argv[1] = Local<Value>::New(Boolean::New(spellData->wordCorrect));
  argv[3] = Local<Value>::New(String::New(spellData->word.c_str()));
  if(spellData->wordCorrect || spellData->numSuggest == 0) {
    if(spellData->multiple)
      argv[2] = Array::New(0);
    else
      argv[2] = Local<Value>::New(Null());
  }
  else if(spellData->numSuggest > 0) {
    if(spellData->multiple) {
      Local<Array> suglist = Array::New(spellData->numSuggest);
      for(int t = 0; t < spellData->numSuggest; t++)
	suglist->Set(t,String::New(spellData->suggestions[t]));
      argv[2] = suglist;
    }
    else {
      argv[2] = String::New(spellData->suggestions[0]);
    }
  }
  spellData->obj->spellClass->free_list(&(spellData->suggestions), spellData->numSuggest);
  TryCatch try_catch;
  spellData->callback->Call(Context::GetCurrent()->Global(), argc, argv);
  if (try_catch.HasCaught())
    node::FatalException(try_catch);
  spellData->callback.Dispose();
  delete spellData;
}

Handle<Value> Nodehun::SpellDictionary::addDictionary(const Arguments& args) 
{
  HandleScope scope;
  int argl = args.Length();
  if(argl > 0) {
    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
    Nodehun::DictData* dictData = new Nodehun::DictData();
    dictData->callbackExists = false;
    if(argl > 1 && args[1]->IsFunction()) {
      Persistent<Function> callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));
      const unsigned argc = 1;
      Local<Value> argv[argc];
      if(!Buffer::HasInstance(args[0])) {
	TryCatch try_catch;
	argv[0] = Exception::TypeError(String::New("First argument must be a buffer"));
	callback->Call(Context::GetCurrent()->Global(), argc, argv);
	if (try_catch.HasCaught())
	  node::FatalException(try_catch);
	callback.Dispose();
	delete dictData;
	return scope.Close(Undefined());
      }
      dictData->callback = callback;
      dictData->callbackExists = true;
    }
    if(!Buffer::HasInstance(args[0])) {
      delete dictData;
      return scope.Close(ThrowException(Exception::TypeError(String::New("First argument must be a buffer"))));
    }
    dictData->dict = new char[Buffer::Length(args[0])];
    strcpy(dictData->dict, Buffer::Data(args[0].As<Object>()));
    dictData->obj = obj;
    dictData->request.data = dictData;
  
    uv_queue_work(uv_default_loop(), &dictData->request,
		  Nodehun::SpellDictionary::addDictionaryWork, Nodehun::SpellDictionary::addDictionaryFinish);

  }
  return scope.Close(Undefined());
}

Handle<Value> Nodehun::SpellDictionary::addDictionarySync(const Arguments& args) 
{
  HandleScope scope;
  if(args.Length() > 0 && Buffer::HasInstance(args[0])) {
    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
    return scope.Close(Boolean::New(obj->addDict(node::Buffer::Data(args[0].As<Object>()))));
  }
  else {
    return scope.Close(ThrowException(Exception::TypeError(String::New("First argument must be a buffer"))));
  }
}

bool Nodehun::SpellDictionary::addDict(const char* dictionary)
{
  uv_rwlock_wrlock(&(this->rwlock));
  int status = this->spellClass->add_dic(dictionary);
  uv_rwlock_wrunlock(&(this->rwlock));
  return status == 0;
}

void Nodehun::SpellDictionary::addDictionaryWork(uv_work_t* request)
{
  Nodehun::DictData* dictData = static_cast<Nodehun::DictData*>(request->data);
  dictData->success = dictData->obj->addDict(dictData->dict);
}

void Nodehun::SpellDictionary::addDictionaryFinish(uv_work_t* request, int i)
{
  HandleScope scope;
  Nodehun::DictData* dictData = static_cast<Nodehun::DictData*>(request->data);
  
  if(dictData->callbackExists) {
    const unsigned argc = 1;
    Local<Value> argv[argc];
    argv[0] = dictData->success ? Local<Value>::New(Null()) : Exception::TypeError(String::New("There was an error adding the dictionary to the nodehun class, because the buffer was deformed."));
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

Handle<Value> Nodehun::SpellDictionary::addWord(const Arguments& args) 
{
  return Nodehun::SpellDictionary::addRemoveWordInit(args, false);
}

Handle<Value> Nodehun::SpellDictionary::addWordSync(const Arguments& args) 
{
  return Nodehun::SpellDictionary::addRemoveWordInitSync(args, false);
}

Handle<Value> Nodehun::SpellDictionary::removeWord(const Arguments& args) 
{
  return Nodehun::SpellDictionary::addRemoveWordInit(args, true);
}

Handle<Value> Nodehun::SpellDictionary::removeWordSync(const Arguments& args) 
{
  return Nodehun::SpellDictionary::addRemoveWordInitSync(args, true);
}

Handle<Value> Nodehun::SpellDictionary::addRemoveWordInit(const Arguments& args, bool remove)
{
  HandleScope scope;
  int argl = args.Length();
  if(argl > 0) {
    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
    String::Utf8Value arg0(args[0]->ToString());
    Nodehun::WordData* wordData = new Nodehun::WordData();
    wordData->callbackExists = false;
    if(argl > 1 && args[1]->IsFunction()) {
      Persistent<Function> callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));
      const unsigned argc = 2;
      Local<Value> argv[argc];
      wordData->callback = callback;
      wordData->callbackExists = true;      
      if (!args[0]->IsString()) {
	TryCatch try_catch;
	argv[0] = Exception::TypeError(String::New("First argument must be a string"));
	argv[1] = Local<Value>::New(Null());
	callback->Call(Context::GetCurrent()->Global(), argc, argv);
	if (try_catch.HasCaught())
	  node::FatalException(try_catch);
	callback.Dispose();
	delete wordData;
	return scope.Close(Undefined());  
      }
    }
    if (!args[0]->IsString()) {
      delete wordData;
      return scope.Close(ThrowException(Exception::TypeError(String::New("First argument must be a string."))));
    }
    wordData->word.append(*arg0);
    wordData->removeWord = remove;
    wordData->obj = obj;
    wordData->request.data = wordData;
    uv_queue_work(uv_default_loop(), &wordData->request,
		  Nodehun::SpellDictionary::addRemoveWordWork, Nodehun::SpellDictionary::addRemoveWordFinish);

  }
  return scope.Close(Undefined());  
}

Handle<Value> Nodehun::SpellDictionary::addRemoveWordInitSync(const Arguments& args, bool remove)
{
  HandleScope scope;
  if(args.Length() > 0 && args[0]->IsString()) {
    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
    String::Utf8Value arg0(args[0]->ToString());
    return scope.Close(Boolean::New(obj->addRemoveWord(*arg0, remove)));
  }
  else {
    return scope.Close(ThrowException(Exception::TypeError(String::New("First argument must be a string."))));  
  }
}

bool Nodehun::SpellDictionary::addRemoveWord(const char* word, bool remove)
{
  int status;
  uv_rwlock_wrlock(&(this->rwlock));
  if(remove)
    status = this->spellClass->remove(word);
  else
    status = this->spellClass->add(word);
  uv_rwlock_wrunlock(&(this->rwlock));
  return status == 0;
}

void Nodehun::SpellDictionary::addRemoveWordWork(uv_work_t* request)
{
  Nodehun::WordData* wordData = static_cast<Nodehun::WordData*>(request->data);
  wordData->success = wordData->obj->addRemoveWord(wordData->word.c_str(), wordData->removeWord);
}

void Nodehun::SpellDictionary::addRemoveWordFinish(uv_work_t* request, int i)
{
  HandleScope scope;
  Nodehun::WordData* wordData = static_cast<Nodehun::WordData*>(request->data);
  
  if(wordData->callbackExists) {
    const unsigned argc = 2;
    Local<Value> argv[argc];
    argv[0] = wordData->success ? Local<Value>::New(Null()) : Exception::TypeError(String::New("There was an error changing the status of the word. The dictionary may be corrupted, or the word may be malfored."));
    argv[1] = wordData->success ? Local<Value>::New(String::New(wordData->word.c_str())) : Local<Value>::New(Null());
    TryCatch try_catch;
    wordData->callback->Call(Context::GetCurrent()->Global(), argc, argv);
    if (try_catch.HasCaught())
      node::FatalException(try_catch);
    wordData->callback.Dispose();
  }
  delete wordData;
}

Handle<Value> Nodehun::SpellDictionary::stem(const Arguments& args) 
{
  HandleScope scope;  
  int argl = args.Length();
  if(argl > 1 && args[1]->IsFunction()) {
    Persistent<Function> callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));
    if (!args[0]->IsString()) {
      const unsigned argc = 2;
      Local<Value> argv[argc];
      TryCatch try_catch;
      argv[0] = Exception::TypeError(String::New("First argument must be a string"));
      argv[1] = Local<Value>::New(Null());
      callback->Call(Context::GetCurrent()->Global(), argc, argv);
      if (try_catch.HasCaught())
	node::FatalException(try_catch);
      callback.Dispose();
      return scope.Close(Undefined());
    }

    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
    Nodehun::StemData* stemData = new Nodehun::StemData();
    v8::String::Utf8Value arg0(args[0]->ToString());
    stemData->word.append(*arg0);
    stemData->callback = callback;
    stemData->obj = obj;
    stemData->request.data = stemData;
    uv_queue_work(uv_default_loop(), &stemData->request,
		  Nodehun::SpellDictionary::stemWork, Nodehun::SpellDictionary::stemFinish);
  }
  return scope.Close(Undefined());
}

Handle<Value> Nodehun::SpellDictionary::stemSync(const Arguments& args) 
{
  HandleScope scope;  
  if(args.Length() > 0 && args[0]->IsString()) {
    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
    v8::String::Utf8Value arg0(args[0]->ToString());
    char** arr = NULL;
    int num = obj->stemWord(&arr, *arg0);
    Local<Array> a = Array::New(num);
    for(int i = 0; i < num; i++)
      a->Set(i, String::New(arr[i]));
    obj->spellClass->free_list(&arr, num);
    return scope.Close(a);    
  }
  else {
    return scope.Close(ThrowException(Exception::TypeError(String::New("First argument must be a string."))));
  }
}

int Nodehun::SpellDictionary::stemWord(char*** arr, const char* word)
{
  uv_rwlock_rdlock(&(this->rwlock));
  int res = this->spellClass->stem(arr, word);
  uv_rwlock_rdunlock(&(this->rwlock));
  return res;
}

void Nodehun::SpellDictionary::stemWork(uv_work_t* request)
{
  Nodehun::StemData* stemData = static_cast<Nodehun::StemData*>(request->data);
  stemData->numResults = stemData->obj->stemWord(&stemData->results, stemData->word.c_str());
}

void Nodehun::SpellDictionary::stemFinish(uv_work_t* request, int i)
{
  HandleScope scope;
  Nodehun::StemData* stemData = static_cast<Nodehun::StemData*>(request->data);
  
  const unsigned int argc = 2;
  Local<Value> argv[argc];
  Local<Array> suglist = Array::New(stemData->numResults);

  for(int t = 0; t < stemData->numResults; t++)
    suglist->Set(t,String::New(stemData->results[t]));
  stemData->obj->spellClass->free_list(&stemData->results,stemData->numResults);
  argv[0] = Local<Value>::New(Null());
  argv[1] = suglist;
  TryCatch try_catch;
  stemData->callback->Call(Context::GetCurrent()->Global(), argc, argv);
  
  if(try_catch.HasCaught())
    node::FatalException(try_catch);
  stemData->callback.Dispose();
  delete stemData;
}

Handle<Value> Nodehun::SpellDictionary::generate(const Arguments& args) 
{
  HandleScope scope;  
  int argl = args.Length();
  if(argl > 2 && args[2]->IsFunction()) {
    Persistent<Function> callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));
    if (!args[0]->IsString()) {
      const unsigned argc = 2;
      Local<Value> argv[argc];
      TryCatch try_catch;
      argv[0] = Exception::TypeError(String::New("First argument must be a string"));
      argv[1] = Local<Value>::New(Null());
      callback->Call(Context::GetCurrent()->Global(), argc, argv);
      if (try_catch.HasCaught())
	node::FatalException(try_catch);
      callback.Dispose();
      return scope.Close(Undefined());
    }
	
    if (!args[1]->IsString()) {
      const unsigned argc = 2;
      Local<Value> argv[argc];
      TryCatch try_catch;
      argv[0] = Exception::TypeError(String::New("Second argument must be a string"));
      argv[1] = Local<Value>::New(Null());
      callback->Call(Context::GetCurrent()->Global(), argc, argv);
      if (try_catch.HasCaught())
	node::FatalException(try_catch);
      callback.Dispose();
      return scope.Close(Undefined());
    }

    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
    Nodehun::GenerateData* generateData = new Nodehun::GenerateData();
    v8::String::Utf8Value arg0(args[0]->ToString());
    v8::String::Utf8Value arg1(args[1]->ToString());
    generateData->word.append(*arg0);
    generateData->word2.append(*arg1);
    generateData->callback = callback;
    generateData->obj = obj;
    generateData->request.data = generateData;
    uv_queue_work(uv_default_loop(), &generateData->request,
		  Nodehun::SpellDictionary::generateWork, Nodehun::SpellDictionary::generateFinish);
  }
  return scope.Close(Undefined());
}

Handle<Value> Nodehun::SpellDictionary::generateSync(const Arguments& args) 
{
  HandleScope scope;  
  if(args.Length() > 1 && args[0]->IsString() && args[1]->IsString()) {
    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
    v8::String::Utf8Value arg0(args[0]->ToString());
    v8::String::Utf8Value arg1(args[1]->ToString());
    char** arr = NULL;
    int num = obj->generateWord(&arr, *arg0, *arg1);
    Local<Array> a = Array::New(num);
    for(int i = 0; i < num; i++)
      a->Set(i, String::New(arr[i]));
    obj->spellClass->free_list(&arr, num);
    return scope.Close(a);        
  }
  else {
    return scope.Close(ThrowException(Exception::TypeError(String::New("First and second arguments must be a string."))));
  }
}

int Nodehun::SpellDictionary::generateWord(char*** arr, const char* word, const char* word2)
{
  uv_rwlock_rdlock(&(this->rwlock));
  int num = this->spellClass->generate(arr, word, word2);
  uv_rwlock_rdunlock(&(this->rwlock));
  return num;
}

void Nodehun::SpellDictionary::generateWork(uv_work_t* request)
{
  Nodehun::GenerateData* generateData = static_cast<Nodehun::GenerateData*>(request->data);
  generateData->numResults = generateData->obj->generateWord(&generateData->results, generateData->word.c_str(), generateData->word2.c_str());
}

void Nodehun::SpellDictionary::generateFinish(uv_work_t* request, int i)
{
  HandleScope scope;
  Nodehun::GenerateData* generateData = static_cast<Nodehun::GenerateData*>(request->data);
  
  const unsigned int argc = 2;
  Local<Value> argv[argc];
  Local<Array> suglist = Array::New(generateData->numResults);

  for(int t = 0; t < generateData->numResults; t++)
    suglist->Set(t,String::New(generateData->results[t]));
  generateData->obj->spellClass->free_list(&generateData->results,generateData->numResults);
  argv[0] = Local<Value>::New(Null());
  argv[1] = suglist;
  TryCatch try_catch;
  generateData->callback->Call(Context::GetCurrent()->Global(), argc, argv);
  
  if(try_catch.HasCaught())
    node::FatalException(try_catch);
  generateData->callback.Dispose();
  delete generateData;
}

Handle<Value> Nodehun::SpellDictionary::analyze(const Arguments& args) 
{
  HandleScope scope;  
  int argl = args.Length();
  if(argl > 1 && args[1]->IsFunction()) {
    Persistent<Function> callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));
    if (!args[0]->IsString()) {
      const unsigned argc = 2;
      Local<Value> argv[argc];
      TryCatch try_catch;
      argv[0] = Exception::TypeError(String::New("First argument must be a string"));
      argv[1] = Local<Value>::New(Null());
      callback->Call(Context::GetCurrent()->Global(), argc, argv);
      if (try_catch.HasCaught())
	node::FatalException(try_catch);
      callback.Dispose();
      return scope.Close(Undefined());
    }

    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
    Nodehun::AnalyzeData* analyzeData = new Nodehun::AnalyzeData();
    v8::String::Utf8Value arg0(args[0]->ToString());
    analyzeData->word.append(*arg0);
    analyzeData->callback = callback;
    analyzeData->obj = obj;
    analyzeData->request.data = analyzeData;
    uv_queue_work(uv_default_loop(), &analyzeData->request,
		  Nodehun::SpellDictionary::analyzeWork, Nodehun::SpellDictionary::analyzeFinish);
  }
  return scope.Close(Undefined());
}

Handle<Value> Nodehun::SpellDictionary::analyzeSync(const Arguments& args) 
{
  HandleScope scope;  
  if(args.Length() > 0 && args[0]->IsString()) {
    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
    v8::String::Utf8Value arg0(args[0]->ToString());
    char** arr = NULL;
    int num = obj->analyzeWord(&arr, *arg0);
    Local<Array> a = Array::New(num);
    for(int i = 0; i < num; i++)
      a->Set(i, String::New(arr[i]));
    obj->spellClass->free_list(&arr, num);
    return scope.Close(a);    
  }
  else {
    return scope.Close(ThrowException(Exception::TypeError(String::New("First argument must be a string."))));
  }
}

int Nodehun::SpellDictionary::analyzeWord(char*** arr, const char* word)
{
  uv_rwlock_rdlock(&(this->rwlock));
  int num = this->spellClass->analyze(arr, word);
  uv_rwlock_rdunlock(&(this->rwlock));
  return num;
}

void Nodehun::SpellDictionary::analyzeWork(uv_work_t* request)
{
  Nodehun::AnalyzeData* analyzeData = static_cast<Nodehun::AnalyzeData*>(request->data);
  analyzeData->numResults = analyzeData->obj->analyzeWord(&analyzeData->results, analyzeData->word.c_str());
}

void Nodehun::SpellDictionary::analyzeFinish(uv_work_t* request, int i)
{
  HandleScope scope;
  Nodehun::AnalyzeData* analyzeData = static_cast<Nodehun::AnalyzeData*>(request->data);
  
  const unsigned int argc = 2;
  Local<Value> argv[argc];
  Local<Array> suglist = Array::New(analyzeData->numResults);

  for(int t = 0; t < analyzeData->numResults; t++)
    suglist->Set(t,String::New(analyzeData->results[t]));
  analyzeData->obj->spellClass->free_list(&analyzeData->results,analyzeData->numResults);
  argv[0] = Local<Value>::New(Null());
  argv[1] = suglist;
  TryCatch try_catch;
  analyzeData->callback->Call(Context::GetCurrent()->Global(), argc, argv);
  
  if(try_catch.HasCaught())
    node::FatalException(try_catch);
  analyzeData->callback.Dispose();
  delete analyzeData;
}

NODE_MODULE(nodehun, Nodehun::SpellDictionary::Init);

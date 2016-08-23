#include "nodehun.hpp"

using namespace v8;

Persistent<Function> Nodehun::SpellDictionary::constructor;

void Nodehun::SpellDictionary::Init(Handle<Object> exports, Handle<Object> module)
{
  Isolate* isolate = Isolate::GetCurrent();
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "NodehunDictionary"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  //static
  NODE_SET_METHOD((v8::Local<v8::Template>) tpl, "createNewNodehun" , createNewNodehun);    
  //prototype
  NODE_SET_PROTOTYPE_METHOD(tpl, "isCorrect", isCorrect);
  NODE_SET_PROTOTYPE_METHOD(tpl, "isCorrectSync", isCorrectSync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "spellSuggest", spellSuggest);
  NODE_SET_PROTOTYPE_METHOD(tpl, "spellSuggestSync", spellSuggestSync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "spellSuggestions", spellSuggestions);
  NODE_SET_PROTOTYPE_METHOD(tpl, "spellSuggestionsSync", spellSuggestionsSync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "addDictionary", addDictionary);
  NODE_SET_PROTOTYPE_METHOD(tpl, "addDictionarySync", addDictionarySync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "addWord", addWord);
  NODE_SET_PROTOTYPE_METHOD(tpl, "addWordSync", addWordSync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "removeWord", removeWord);
  NODE_SET_PROTOTYPE_METHOD(tpl, "removeWordSync", removeWordSync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "stem", stem);
  NODE_SET_PROTOTYPE_METHOD(tpl, "stemSync", stemSync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "generate", generate);
  NODE_SET_PROTOTYPE_METHOD(tpl, "generateSync", generateSync);
  NODE_SET_PROTOTYPE_METHOD(tpl, "analyze", analyze);
  NODE_SET_PROTOTYPE_METHOD(tpl, "analyzeSync", analyzeSync);

  constructor.Reset(isolate, tpl->GetFunction());
  module->Set(String::NewFromUtf8(isolate, "exports"), tpl->GetFunction());
}

void Nodehun::SpellDictionary::createNewNodehun(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);

  int argl = args.Length();
  if(argl > 2 && args[2]->IsFunction()) {
    Local<Function> callback = Local<Function>::Cast(args[2]);
    const unsigned argc = 2;
    Local<Value> argv[argc];
    argv[1] = Local<Value>::New(isolate, Null(isolate));
    if(!node::Buffer::HasInstance(args[0])) {
      argv[0] = Exception::TypeError(String::NewFromUtf8(isolate, "First argument must be a buffer"));
      callback->Call(isolate->GetCurrentContext()->Global(), argc, argv);
    }
    else if(!node::Buffer::HasInstance(args[1])) {
      argv[0] = Exception::TypeError(String::NewFromUtf8(isolate, "Second argument must be a buffer"));
      callback->Call(isolate->GetCurrentContext()->Global(), argc, argv);
    }
    else {
      Nodehun::NodehunData* nodeData = new Nodehun::NodehunData();
      nodeData->isolate = isolate;
      nodeData->callback.Reset(isolate, callback);
      nodeData->aff = new char[node::Buffer::Length(args[0])];
      strcpy(nodeData->aff, node::Buffer::Data(args[0].As<Object>()));
      nodeData->dict = new char[node::Buffer::Length(args[1])];
      strcpy(nodeData->dict, node::Buffer::Data(args[1].As<Object>()));
      nodeData->request.data = nodeData;
      uv_queue_work(uv_default_loop(), &nodeData->request,
		    Nodehun::SpellDictionary::createNewNodehunWork, Nodehun::SpellDictionary::createNewNodehunFinish);
    }
  }
  args.GetReturnValue().SetUndefined();
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
  Nodehun::NodehunData* nodeData = static_cast<Nodehun::NodehunData*>(request->data);
  Isolate *isolate = nodeData->isolate;;
  HandleScope scope(isolate);
  const unsigned argc = 2;
  Local<Value> argv[argc];
  Handle<Value> ext = External::New(isolate, nodeData->obj);
  argv[0] = Local<Value>::New(isolate, Null(isolate));
  Local<Function> cons = Local<Function>::New(isolate, constructor);
  argv[1] = cons->NewInstance(1, &ext);
  Local<Function> cb = Local<Function>::New(isolate, nodeData->callback);
  cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
  nodeData->callback.Reset();
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

void Nodehun::SpellDictionary::New(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  int argl = args.Length();
  if (!args.IsConstructCall()){
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Use the new operator to create an instance of this object.")));
    return;
  }
  if(argl > 0 && args[0]->IsExternal()) {
    Local<External> ext = Local<External>::Cast(args[0]);
    void *ptr = ext->Value();
    Nodehun::SpellDictionary *obj = new Nodehun::SpellDictionary(static_cast<Hunspell*>(ptr));
    uv_rwlock_init(&(obj->rwlock));
    obj->Wrap(args.Holder());
  }
  else {
    if(argl < 2){
      isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Constructor requires two arguments.")));
      return;
    }
    if(!node::Buffer::HasInstance(args[0])){
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "First argument must be a buffer")));
      return;
    }
    if(!node::Buffer::HasInstance(args[1])){
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Second argument must be a buffer")));
      return;
    }

    Nodehun::SpellDictionary *obj = new Nodehun::SpellDictionary(node::Buffer::Data(args[0].As<Object>()), node::Buffer::Data(args[1].As<Object>()));
    uv_rwlock_init(&(obj->rwlock));
    obj->Wrap(args.Holder());
  }
  args.GetReturnValue().Set(args.Holder());
}

void Nodehun::SpellDictionary::isCorrect(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  int argl = args.Length();
  if(argl > 1 && args[1]->IsFunction()) {
    Local<Function> callback = Local<Function>::Cast(args[1]);
    const unsigned argc = 3;
    Local<Value> argv[argc];
    argv[1] = Local<Value>::New(isolate, Null(isolate));
    argv[2] = Local<Value>::New(isolate, Null(isolate));
    if(argl < 1 || !args[0]->IsString()) {
      argv[0] = Exception::TypeError(String::NewFromUtf8(isolate, "First argument must be a string"));
      callback->Call(isolate->GetCurrentContext()->Global(), argc, argv);
    }
    else {
      Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.Holder());
      Nodehun::CorrectData* corrData = new Nodehun::CorrectData();
      String::Utf8Value arg0(args[0]->ToString());

      corrData->isolate = isolate;
      corrData->callback.Reset(isolate, callback);
      corrData->request.data = corrData;
      corrData->word.append(*arg0);
      corrData->obj = obj;
      corrData->isCorrect = false;
      uv_queue_work(uv_default_loop(), &corrData->request,
      	    Nodehun::SpellDictionary::checkCorrect, Nodehun::SpellDictionary::sendCorrect);
    }
  }
  args.GetReturnValue().SetUndefined();
}

void Nodehun::SpellDictionary::isCorrectSync(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  if(args.Length() > 0 && args[0]->IsString()) {
      Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.Holder());
      String::Utf8Value arg0(args[0]->ToString());
      args.GetReturnValue().Set(obj->spell(*arg0));
  }
  else {
    args.GetReturnValue().SetUndefined();
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "First argument must be a string.")));
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
  Nodehun::CorrectData* corrData = static_cast<Nodehun::CorrectData*>(request->data);
  Isolate* isolate = corrData->isolate;
  HandleScope scope(isolate);

  const unsigned argc = 3;
  Local<Value> argv[argc];
  argv[0] = Local<Value>::New(isolate, Null(isolate));
  argv[1] = Local<Value>::New(isolate, Boolean::New(isolate, corrData->isCorrect));
  argv[2] = Local<Value>::New(isolate, String::NewFromUtf8(isolate, corrData->word.c_str()));
  Local<Function> cb = Local<Function>::New(isolate, corrData->callback);
  cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
  corrData->callback.Reset();
  delete corrData;
}


void Nodehun::SpellDictionary::spellSuggest(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  int argl = args.Length();
  if(argl > 1 && args[1]->IsFunction()) {
    Local<Function> callback = Local<Function>::Cast(args[1]);
    const unsigned argc = 3;
    Local<Value> argv[argc];
    argv[1] = Local<Value>::New(isolate, Null(isolate));
    argv[2] = Local<Value>::New(isolate, Null(isolate));
    if(argl < 1 || !args[0]->IsString()) {
      argv[0] = Exception::TypeError(String::NewFromUtf8(isolate, "First argument must be a string"));
      callback->Call(isolate->GetCurrentContext()->Global(), argc, argv);
    }
    else {
      Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.Holder());
      Nodehun::SpellData* spellData = new Nodehun::SpellData();
      String::Utf8Value arg0(args[0]->ToString());

      spellData->isolate = isolate;
      spellData->callback.Reset(isolate, callback);
      spellData->request.data = spellData;
      spellData->word.append(*arg0);
      spellData->obj = obj;
      spellData->multiple = false;
      uv_queue_work(uv_default_loop(), &spellData->request,
      	    Nodehun::SpellDictionary::checkSuggestions, Nodehun::SpellDictionary::sendSuggestions);
    }
  }
  args.GetReturnValue().SetUndefined();
}

void Nodehun::SpellDictionary::spellSuggestSync(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  if(args.Length() > 0 && args[0]->IsString()) {
    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.Holder());
    String::Utf8Value arg0(args[0]->ToString());
    bool corr = false;
    char** arr = NULL;
    int num = obj->spellCheck(&corr, &arr, *arg0);
    if(num > 0)
      args.GetReturnValue().Set(Local<Value>::New(isolate, String::NewFromUtf8(isolate, arr[0])));
    else
      args.GetReturnValue().SetNull();
    obj->spellClass->free_list(&arr, num);
  }
  else {
    args.GetReturnValue().SetUndefined();
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "First argument must be a string.")));
  }

}

void Nodehun::SpellDictionary::spellSuggestions(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  int argl = args.Length();
  if(argl > 1 && args[1]->IsFunction()) {
    Local<Function> callback = Local<Function>::Cast(args[1]);
    const unsigned argc = 3;
    Local<Value> argv[argc];
    argv[1] = Local<Value>::New(isolate, Null(isolate));
    argv[2] = Local<Value>::New(isolate, Null(isolate));
    if(argl < 1 || !args[0]->IsString()) {
      argv[0] = Exception::TypeError(String::NewFromUtf8(isolate, "First argument must be a string"));
      callback->Call(isolate->GetCurrentContext()->Global(), argc, argv);
    }
    else {
      Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.Holder());
      Nodehun::SpellData* spellData = new Nodehun::SpellData();
      String::Utf8Value arg0(args[0]->ToString());

      spellData->isolate = isolate;
      spellData->callback.Reset(isolate, callback);
      spellData->request.data = spellData;
      spellData->word.append(*arg0);
      spellData->obj = obj;
      spellData->multiple = true;
      uv_queue_work(uv_default_loop(), &spellData->request,
		    Nodehun::SpellDictionary::checkSuggestions, Nodehun::SpellDictionary::sendSuggestions);
    }
  }
  args.GetReturnValue().SetUndefined();
}

void Nodehun::SpellDictionary::spellSuggestionsSync(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  if(args.Length() > 0 && args[0]->IsString()) {
    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.Holder());
    String::Utf8Value arg0(args[0]->ToString());
    bool corr = false;
    char** arr = NULL;
    int num = obj->spellCheck(&corr, &arr, *arg0);
    Local<Array> a = Array::New(isolate, num);
    for(int i = 0; i < num; i++)
      a->Set(i, String::NewFromUtf8(isolate, arr[i]));
    args.GetReturnValue().Set(a);
    obj->spellClass->free_list(&arr, num);
  }
  else {
    args.GetReturnValue().SetUndefined();
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "First argument must be a string.")));
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
  spellData->numSuggest = spellData->obj->spellCheck(&(spellData->wordCorrect), &(spellData->suggestions), spellData->word.c_str());
}

void Nodehun::SpellDictionary::sendSuggestions(uv_work_t* request, int i)
{
  Nodehun::SpellData* spellData = static_cast<Nodehun::SpellData*>(request->data);
  Isolate* isolate = spellData->isolate;
  HandleScope scope(isolate);

  const unsigned argc = 4;
  Local<Value> argv[argc];
  argv[0] = Local<Value>::New(isolate, Null(isolate));
  argv[1] = Local<Value>::New(isolate, Boolean::New(isolate, spellData->wordCorrect));
  argv[3] = Local<Value>::New(isolate, String::NewFromUtf8(isolate, spellData->word.c_str()));
  if(spellData->wordCorrect || spellData->numSuggest == 0) {
    if(spellData->multiple)
      argv[2] = Array::New(isolate, 0);
    else
      argv[2] = Local<Value>::New(isolate, Null(isolate));
  }
  else if(spellData->numSuggest > 0) {
    if(spellData->multiple) {
      Local<Array> suglist = Array::New(isolate, spellData->numSuggest);
      for(int t = 0; t < spellData->numSuggest; t++)
	suglist->Set(t,String::NewFromUtf8(isolate, spellData->suggestions[t]));
      argv[2] = suglist;
    }
    else {
      argv[2] = String::NewFromUtf8(isolate, spellData->suggestions[0]);
    }
  }
  spellData->obj->spellClass->free_list(&(spellData->suggestions), spellData->numSuggest);
  Local<Function> cb = Local<Function>::New(isolate, spellData->callback);
  cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
  spellData->callback.Reset();
  delete spellData;
}

void Nodehun::SpellDictionary::addDictionary(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  int argl = args.Length();
  if(argl > 0) {
    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.Holder());
    Nodehun::DictData* dictData = new Nodehun::DictData();
    dictData->callbackExists = false;
    if(argl > 1 && args[1]->IsFunction()) {
      Local<Function> callback = Local<Function>::Cast(args[1]);
      const unsigned argc = 1;
      Local<Value> argv[argc];
      if(!node::Buffer::HasInstance(args[0])) {
	argv[0] = Exception::TypeError(String::NewFromUtf8(isolate, "First argument must be a buffer"));
	callback->Call(isolate->GetCurrentContext()->Global(), argc, argv);
	delete dictData;
	args.GetReturnValue().SetUndefined();
	return;
      }
      dictData->callback.Reset(isolate, callback);
      dictData->callbackExists = true;
    }
    if(!node::Buffer::HasInstance(args[0])) {
      delete dictData;
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "First argument must be a buffer")));
      return;
    }
    dictData->isolate = isolate;
    dictData->dict = new char[node::Buffer::Length(args[0])];
    strcpy(dictData->dict, node::Buffer::Data(args[0].As<Object>()));
    dictData->obj = obj;
    dictData->request.data = dictData;

    uv_queue_work(uv_default_loop(), &dictData->request,
		  Nodehun::SpellDictionary::addDictionaryWork, Nodehun::SpellDictionary::addDictionaryFinish);
  }
  args.GetReturnValue().SetUndefined();
}

void Nodehun::SpellDictionary::addDictionarySync(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  if(args.Length() > 0 && node::Buffer::HasInstance(args[0])) {
    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.Holder());
    args.GetReturnValue().Set(obj->addDict(node::Buffer::Data(args[0].As<Object>())));
  }
  else {
    args.GetReturnValue().SetUndefined();
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "First argument must be a buffer.")));
  }
  args.GetReturnValue().SetUndefined();
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
  Nodehun::DictData* dictData = static_cast<Nodehun::DictData*>(request->data);
  Isolate* isolate = dictData->isolate;
  HandleScope scope(isolate);

  if(dictData->callbackExists) {
    const unsigned argc = 1;
    Local<Value> argv[argc];
    argv[0] = dictData->success ? Local<Value>::New(isolate, Null(isolate)) : Exception::TypeError(String::NewFromUtf8(isolate, "There was an error adding the dictionary to the nodehun class, because the buffer was deformed."));
    Local<Function> cb = Local<Function>::New(isolate, dictData->callback);
    cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
    dictData->callback.Reset();
  }
  delete dictData->dict;
  delete dictData;
}

void Nodehun::SpellDictionary::addWord(const FunctionCallbackInfo<Value>& args)
{
  Nodehun::SpellDictionary::addRemoveWordInit(args, false);
}

void Nodehun::SpellDictionary::addWordSync(const FunctionCallbackInfo<Value>& args)
{
  Nodehun::SpellDictionary::addRemoveWordInitSync(args, false);
}

void Nodehun::SpellDictionary::removeWord(const FunctionCallbackInfo<Value>& args)
{
  Nodehun::SpellDictionary::addRemoveWordInit(args, true);
}

void Nodehun::SpellDictionary::removeWordSync(const FunctionCallbackInfo<Value>& args)
{
  Nodehun::SpellDictionary::addRemoveWordInitSync(args, true);
}

void Nodehun::SpellDictionary::addRemoveWordInit(const FunctionCallbackInfo<Value>& args, bool remove)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  int argl = args.Length();
  if(argl > 0) {
    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.Holder());
    String::Utf8Value arg0(args[0]->ToString());
    Nodehun::WordData* wordData = new Nodehun::WordData();
    wordData->callbackExists = false;
    if(argl > 1 && args[1]->IsFunction()) {
      Local<Function> callback = Local<Function>::New(isolate, Local<Function>::Cast(args[1]));
      const unsigned argc = 2;
      Local<Value> argv[argc];
      if (!args[0]->IsString()) {
	argv[0] = Exception::TypeError(String::NewFromUtf8(isolate, "First argument must be a string"));
	argv[1] = Local<Value>::New(isolate, Null(isolate));
	callback->Call(isolate->GetCurrentContext()->Global(), argc, argv);
	delete wordData;
	args.GetReturnValue().SetUndefined();
	return;
      }
      wordData->callback.Reset(isolate, callback);
      wordData->callbackExists = true;
    }
    else if (!args[0]->IsString()) {
      delete wordData;
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "First argument must be a string.")));
      return;
    }
    wordData->isolate = isolate;
    wordData->word.append(*arg0);
    wordData->removeWord = remove;
    wordData->obj = obj;
    wordData->request.data = wordData;
    uv_queue_work(uv_default_loop(), &wordData->request,
		  Nodehun::SpellDictionary::addRemoveWordWork, Nodehun::SpellDictionary::addRemoveWordFinish);

  }
  args.GetReturnValue().SetUndefined();
}

void Nodehun::SpellDictionary::addRemoveWordInitSync(const FunctionCallbackInfo<Value>& args, bool remove)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  if(args.Length() > 0 && args[0]->IsString()) {
    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.Holder());
    String::Utf8Value arg0(args[0]->ToString());
    args.GetReturnValue().Set(obj->addRemoveWord(*arg0, remove));
  }
  else {
    args.GetReturnValue().SetUndefined();
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "First argument must be a string.")));
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
  Nodehun::WordData* wordData = static_cast<Nodehun::WordData*>(request->data);
  Isolate* isolate = wordData->isolate;
  HandleScope scope(isolate);

  if(wordData->callbackExists) {
    const unsigned argc = 2;
    Local<Value> argv[argc];
    argv[0] = wordData->success ? Local<Value>::New(isolate, Null(isolate)) : Exception::TypeError(String::NewFromUtf8(isolate, "There was an error changing the status of the word. The dictionary may be corrupted, or the word may be malfored."));
    argv[1] = wordData->success ? Local<Value>::New(isolate, String::NewFromUtf8(isolate, wordData->word.c_str())) : Local<Value>::New(isolate, Null(isolate));
    Local<Function> cb = Local<Function>::New(isolate, wordData->callback);
    cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
    wordData->callback.Reset();
  }
  delete wordData;
}

void Nodehun::SpellDictionary::stem(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  int argl = args.Length();
  if(argl > 1 && args[1]->IsFunction()) {
    Local<Function> callback = Local<Function>::Cast(args[1]);
    if (!args[0]->IsString()) {
      const unsigned argc = 2;
      Local<Value> argv[argc];
      argv[0] = Exception::TypeError(String::NewFromUtf8(isolate, "First argument must be a string"));
      argv[1] = Local<Value>::New(isolate, Null(isolate));
      callback->Call(isolate->GetCurrentContext()->Global(), argc, argv);
    }
    else{
      Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.Holder());
      Nodehun::StemData* stemData = new Nodehun::StemData();
      v8::String::Utf8Value arg0(args[0]->ToString());
      stemData->isolate = isolate;
      stemData->word.append(*arg0);
      stemData->callback.Reset(isolate, callback);
      stemData->obj = obj;
      stemData->request.data = stemData;
      uv_queue_work(uv_default_loop(), &stemData->request,
		    Nodehun::SpellDictionary::stemWork, Nodehun::SpellDictionary::stemFinish);
    }
  }
  args.GetReturnValue().SetUndefined();
}

void Nodehun::SpellDictionary::stemSync(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  if(args.Length() > 0 && args[0]->IsString()) {
    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.Holder());
    v8::String::Utf8Value arg0(args[0]->ToString());
    char** arr = NULL;
    int num = obj->stemWord(&arr, *arg0);
    Local<Array> a = Array::New(isolate, num);
    for(int i = 0; i < num; i++)
      a->Set(i, String::NewFromUtf8(isolate, arr[i]));
    args.GetReturnValue().Set(a);
    obj->spellClass->free_list(&arr, num);
  }
  else {
    args.GetReturnValue().SetUndefined();
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "First argument must be a string.")));
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
  Nodehun::StemData* stemData = static_cast<Nodehun::StemData*>(request->data);
  Isolate* isolate = stemData->isolate;
  HandleScope scope(isolate);

  const unsigned int argc = 2;
  Local<Value> argv[argc];
  Local<Array> suglist = Array::New(isolate, stemData->numResults);

  for(int t = 0; t < stemData->numResults; t++)
    suglist->Set(t,String::NewFromUtf8(isolate, stemData->results[t]));
  stemData->obj->spellClass->free_list(&stemData->results,stemData->numResults);
  argv[0] = Local<Value>::New(isolate, Null(isolate));
  argv[1] = suglist;
  Local<Function> cb = Local<Function>::New(isolate, stemData->callback);
  cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
  stemData->callback.Reset();
  delete stemData;
}

void Nodehun::SpellDictionary::generate(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  int argl = args.Length();
  if(argl > 2 && args[2]->IsFunction()) {
    Local<Function> callback = Local<Function>::Cast(args[2]);
    if (!args[0]->IsString()) {
      const unsigned argc = 2;
      Local<Value> argv[argc];
      argv[0] = Exception::TypeError(String::NewFromUtf8(isolate, "First argument must be a string"));
      argv[1] = Local<Value>::New(isolate, Null(isolate));
      callback->Call(isolate->GetCurrentContext()->Global(), argc, argv);
    }
    else if (!args[1]->IsString()) {
      const unsigned argc = 2;
      Local<Value> argv[argc];
      argv[0] = Exception::TypeError(String::NewFromUtf8(isolate, "Second argument must be a string"));
      argv[1] = Local<Value>::New(isolate, Null(isolate));
      callback->Call(isolate->GetCurrentContext()->Global(), argc, argv);
    }
    else{
      Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.Holder());
      Nodehun::GenerateData* generateData = new Nodehun::GenerateData();
      v8::String::Utf8Value arg0(args[0]->ToString());
      v8::String::Utf8Value arg1(args[1]->ToString());
      generateData->isolate = isolate;
      generateData->word.append(*arg0);
      generateData->word2.append(*arg1);
      generateData->callback.Reset(isolate, callback);
      generateData->obj = obj;
      generateData->request.data = generateData;
      uv_queue_work(uv_default_loop(), &generateData->request,
		    Nodehun::SpellDictionary::generateWork, Nodehun::SpellDictionary::generateFinish);
    }
  }
  args.GetReturnValue().SetUndefined();
}

void Nodehun::SpellDictionary::generateSync(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  if(args.Length() > 1 && args[0]->IsString() && args[1]->IsString()) {
    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.Holder());
    v8::String::Utf8Value arg0(args[0]->ToString());
    v8::String::Utf8Value arg1(args[1]->ToString());
    char** arr = NULL;
    int num = obj->generateWord(&arr, *arg0, *arg1);
    Local<Array> a = Array::New(isolate, num);
    for(int i = 0; i < num; i++)
      a->Set(i, String::NewFromUtf8(isolate, arr[i]));
    args.GetReturnValue().Set(a);
    obj->spellClass->free_list(&arr, num);
  }
  else {
    args.GetReturnValue().SetUndefined();
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "First and second arguments must be a string.")));
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
  Nodehun::GenerateData* generateData = static_cast<Nodehun::GenerateData*>(request->data);
  Isolate* isolate = generateData->isolate;
  HandleScope scope(isolate);

  const unsigned int argc = 2;
  Local<Value> argv[argc];
  Local<Array> suglist = Array::New(isolate, generateData->numResults);

  for(int t = 0; t < generateData->numResults; t++)
    suglist->Set(t,String::NewFromUtf8(isolate, generateData->results[t]));
  generateData->obj->spellClass->free_list(&generateData->results,generateData->numResults);
  argv[0] = Local<Value>::New(isolate, Null(isolate));
  argv[1] = suglist;
  Local<Function> cb = Local<Function>::New(isolate, generateData->callback);
  cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
  generateData->callback.Reset();
  delete generateData;
}

void Nodehun::SpellDictionary::analyze(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  int argl = args.Length();
  if(argl > 1 && args[1]->IsFunction()) {
    Local<Function> callback = Local<Function>::Cast(args[1]);
    if (!args[0]->IsString()) {
      const unsigned argc = 2;
      Local<Value> argv[argc];
      argv[0] = Exception::TypeError(String::NewFromUtf8(isolate, "First argument must be a string"));
      argv[1] = Local<Value>::New(isolate, Null(isolate));
      callback->Call(isolate->GetCurrentContext()->Global(), argc, argv);
    }
    else{
      Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.Holder());
      Nodehun::AnalyzeData* analyzeData = new Nodehun::AnalyzeData();
      v8::String::Utf8Value arg0(args[0]->ToString());
      analyzeData->isolate = isolate;
      analyzeData->word.append(*arg0);
      analyzeData->callback.Reset(isolate, callback);
      analyzeData->obj = obj;
      analyzeData->request.data = analyzeData;
      uv_queue_work(uv_default_loop(), &analyzeData->request,
		    Nodehun::SpellDictionary::analyzeWork, Nodehun::SpellDictionary::analyzeFinish);
    }
  }
  args.GetReturnValue().SetUndefined();
}

void Nodehun::SpellDictionary::analyzeSync(const FunctionCallbackInfo<Value>& args)
{
  Isolate* isolate = args.GetIsolate();
  HandleScope scope(isolate);
  if(args.Length() > 0 && args[0]->IsString()) {
    Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.Holder());
    v8::String::Utf8Value arg0(args[0]->ToString());
    char** arr = NULL;
    int num = obj->analyzeWord(&arr, *arg0);
    Local<Array> a = Array::New(isolate, num);
    for(int i = 0; i < num; i++)
      a->Set(i, String::NewFromUtf8(isolate, arr[i]));
    args.GetReturnValue().Set(a);
    obj->spellClass->free_list(&arr, num);
  }
  else {
    args.GetReturnValue().SetUndefined();
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "First argument must be a string.")));
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
  Nodehun::AnalyzeData* analyzeData = static_cast<Nodehun::AnalyzeData*>(request->data);
  Isolate* isolate = analyzeData->isolate;
  HandleScope scope(isolate);

  const unsigned int argc = 2;
  Local<Value> argv[argc];
  Local<Array> suglist = Array::New(isolate, analyzeData->numResults);

  for(int t = 0; t < analyzeData->numResults; t++)
    suglist->Set(t,String::NewFromUtf8(isolate, analyzeData->results[t]));
  analyzeData->obj->spellClass->free_list(&analyzeData->results,analyzeData->numResults);
  argv[0] = Local<Value>::New(isolate, Null(isolate));
  argv[1] = suglist;
  Local<Function> cb = Local<Function>::New(isolate, analyzeData->callback);
  cb->Call(isolate->GetCurrentContext()->Global(), argc, argv);
  analyzeData->callback.Reset();
  delete analyzeData;
}

NODE_MODULE(nodehun, Nodehun::SpellDictionary::Init);

#include <uv.h>
#include <node.h>
#include <string>
#include <hunspell.hxx>
#include "nodehun.hpp"

#ifdef _WIN32
#define __SLASH__ "\\"
#else
#define __SLASH__ "/"
#endif

using namespace v8;
Persistent<FunctionTemplate> SpellDictionary::constructor;
std::string _dictionariesPath;


bool dictionaryDirectoryExists(const char *file){
	static uv_loop_t* loop = uv_default_loop();
	uv_fs_t open_req;
	uv_fs_t close_req;
	uv_fs_open(loop, &open_req, file, O_RDONLY, 0, NULL);
	if(open_req.result == -1){
		uv_fs_close(loop, &close_req, open_req.result, NULL);
		uv_fs_req_cleanup(&open_req);
		uv_fs_req_cleanup(&close_req);
		return false;
	}
	uv_fs_close(loop, &close_req, open_req.result, NULL);
	if(close_req.result == -1){
		uv_fs_req_cleanup(&open_req);
		uv_fs_req_cleanup(&close_req);
		return false;
	}
	uv_fs_req_cleanup(&open_req);
	uv_fs_req_cleanup(&close_req);
	return true;
}

void SpellDictionary::Init(Handle<Object> target) {
	HandleScope scope;

	Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
	Local<String> name = String::NewSymbol("Dictionary");

	constructor = Persistent<FunctionTemplate>::New(tpl);
	constructor->InstanceTemplate()->SetInternalFieldCount(5);
	constructor->SetClassName(name);
	
	NODE_SET_PROTOTYPE_METHOD(constructor, "spellSuggest", spellSuggest);
	NODE_SET_PROTOTYPE_METHOD(constructor, "spellSuggestions", spellSuggestions);
	NODE_SET_PROTOTYPE_METHOD(constructor,"addDictionary", addDictionary);
	NODE_SET_PROTOTYPE_METHOD(constructor,"addWord", addWord);
	NODE_SET_PROTOTYPE_METHOD(constructor,"removeWord", removeWord);
	
	target->Set(name, constructor->GetFunction());
}

SpellDictionary::SpellDictionary(const char *language){
	std::string path, affPath, dicPath;
	path.append(_dictionariesPath);
	path.append(language);
	path.append(__SLASH__);
	path.append(language);
	
	affPath.append(path);
	dicPath.append(path);
	affPath.append(".aff");
	dicPath.append(".dic");

	pathsExist = dictionaryDirectoryExists(affPath.c_str()) && dictionaryDirectoryExists(dicPath.c_str());
	if(pathsExist){
		spellClass = new Hunspell(affPath.c_str(), dicPath.c_str());
	}
}

Handle<Value> SpellDictionary::New(const Arguments& args) {
	HandleScope scope;

	if (!args.IsConstructCall()) {
		return ThrowException(Exception::TypeError(
		String::New("Use the new operator to create instances of this object.")));
	}
	if(args.Length() < 1 || !args[0]->IsString()){
		return ThrowException(Exception::TypeError(
		String::New("First argument must be a string.")));
	}

	String::Utf8Value arg0(args[0]->ToString());
	SpellDictionary* obj = new SpellDictionary(*arg0);
	
	if(!obj->pathsExist){
		return ThrowException(Exception::TypeError(
		String::New("No such dictionary exists."))
		);
	}
	obj->Wrap(args.This());

	return args.This();
}

struct SpellData {
	uv_work_t request;
	Persistent<Function> callback;

	std::string word;
	bool multiple;
	Hunspell *spellClass;
	bool wordCorrect;
	char **suggestions;
	int numSuggest;
};

Handle<Value> SpellDictionary::spellSuggest(const Arguments& args) {
	HandleScope scope;
	if (args.Length() < 2) {
		return ThrowException(Exception::TypeError(
		String::New("Missing required arguments.")));
	}
	if(!args[0]->IsString()){
		return ThrowException(Exception::TypeError(
		String::New("First argument must be a string.")));
	}
	if(!args[1]->IsFunction()){
		return ThrowException(Exception::TypeError(
		String::New("Second argument must be a function.")));
	}
	SpellDictionary* obj = ObjectWrap::Unwrap<SpellDictionary>(args.This());
	
	String::Utf8Value arg0(args[0]->ToString());
	Local<Function> callback = Local<Function>::Cast(args[1]);

	SpellData* spellData = new SpellData();
	spellData->request.data = spellData;
	spellData->callback = Persistent<Function>::New(callback);
	spellData->word.append(*arg0);
	
	spellData->spellClass = obj->spellClass;
	spellData->multiple = false;
	uv_queue_work(uv_default_loop(), &spellData->request,
					SpellDictionary::CheckSuggestions, SpellDictionary::SendSuggestions);
	return Undefined();
}

Handle<Value> SpellDictionary::spellSuggestions(const Arguments& args) {
	HandleScope scope;
	if (args.Length() < 2) {
		return ThrowException(Exception::TypeError(
		String::New("Missing required arguments.")));
	}
	if(!args[0]->IsString()){
		return ThrowException(Exception::TypeError(
		String::New("First argument must be a string.")));
	}
	if(!args[1]->IsFunction()){
		return ThrowException(Exception::TypeError(
		String::New("Second argument must be a function.")));
	}
	SpellDictionary* obj = ObjectWrap::Unwrap<SpellDictionary>(args.This());
	
	String::Utf8Value arg0(args[0]->ToString());
	Local<Function> callback = Local<Function>::Cast(args[1]);

	SpellData* spellData = new SpellData();
	spellData->request.data = spellData;
	spellData->callback = Persistent<Function>::New(callback);
	spellData->word.append(*arg0);
	
	
	spellData->spellClass = obj->spellClass;
	spellData->multiple = true;
	uv_queue_work(uv_default_loop(), &spellData->request,
					SpellDictionary::CheckSuggestions, SpellDictionary::SendSuggestions);
	return Undefined();
}

void SpellDictionary::CheckSuggestions(uv_work_t* request) {
	SpellData* spellData = static_cast<SpellData*>(request->data);

	spellData->wordCorrect = spellData->spellClass->spell(spellData->word.c_str());
	if (!spellData->wordCorrect){
		spellData->numSuggest = spellData->spellClass->suggest(&(spellData->suggestions),spellData->word.c_str());
	}
}

void SpellDictionary::SendSuggestions(uv_work_t* request){
	HandleScope scope;
	SpellData* spellData = static_cast<SpellData*>(request->data);
	
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
			for(int i = 0; i < spellData->numSuggest; i++)
				suglist->Set(i,String::New(spellData->suggestions[i]));
			argv[1] = suglist;
		}
		else{
			argv[1] = String::New(spellData->suggestions[0]);
		}
		spellData->spellClass->free_list(&(spellData->suggestions),spellData->numSuggest);
		spellData->suggestions = NULL;
	}

	TryCatch try_catch;
	spellData->callback->Call(Context::GetCurrent()->Global(), argc, argv);
	if (try_catch.HasCaught()) {
		node::FatalException(try_catch);
	}
	spellData->callback.Dispose();
	delete spellData;
}

struct DictData {
	uv_work_t request;
	Persistent<Function> callback;
	bool callbackExists;
	std::string path;
	std::string dict;
	bool success;
	Hunspell *spellClass;
};

Handle<Value> SpellDictionary::addDictionary(const Arguments& args) {
	HandleScope scope;
	if (args.Length() < 1 || !args[0]->IsString()) {
		return ThrowException(Exception::TypeError(
		String::New("First argument must be a string.")));
	}
	SpellDictionary* obj = ObjectWrap::Unwrap<SpellDictionary>(args.This());
	DictData* dictData = new DictData();
	String::Utf8Value arg0(args[0]->ToString());
	if(args.Length() > 1 && args[1]->IsFunction()){
		Local<Function> callback = Local<Function>::Cast(args[1]);
		dictData->callback = Persistent<Function>::New(callback);
		dictData->callbackExists = true;
	}
	else{
		dictData->callbackExists = false;
	}
	
	dictData->path.append(_dictionariesPath);
	dictData->path.append(*arg0);
	dictData->path.append(__SLASH__);
	dictData->path.append(*arg0);
	dictData->path.append(".dic");
	dictData->dict.append(*arg0);
	
	dictData->spellClass = obj->spellClass;
	dictData->request.data = dictData;
	
	uv_queue_work(uv_default_loop(), &dictData->request,
					SpellDictionary::addDictionaryWork, SpellDictionary::addDictionaryFinish);
	return Undefined();

}

void SpellDictionary::addDictionaryWork(uv_work_t* request){
	DictData* dictData = static_cast<DictData*>(request->data);
	
	if(!dictionaryDirectoryExists(dictData->path.c_str())){
		dictData->success = false;
	}
	else{
		int status = dictData->spellClass->add_dic(dictData->path.c_str());
		dictData->success = status == 0;
	}
	
}

void SpellDictionary::addDictionaryFinish(uv_work_t* request){
	HandleScope scope;
	DictData* dictData = static_cast<DictData*>(request->data);

	if(dictData->callbackExists){
		const unsigned argc = 2;
		Local<Value> argv[argc];
		argv[0] = Local<Value>::New(Boolean::New(dictData->success));
		argv[1] = Local<Value>::New(String::New(dictData->dict.c_str()));
		TryCatch try_catch;
		dictData->callback->Call(Context::GetCurrent()->Global(), argc, argv);
		if (try_catch.HasCaught()) {
			node::FatalException(try_catch);
		}
		dictData->callback.Dispose();
	}
	delete dictData;
}

struct WordData {
	uv_work_t request;
	Persistent<Function> callback;
	bool removeWord;
	bool callbackExists;
	bool success;
	std::string word;
	Hunspell *spellClass;
};

Handle<Value> SpellDictionary::addWord(const Arguments& args) {
	HandleScope scope;
	
	if (args.Length() < 1 || !args[0]->IsString()) {
		return ThrowException(Exception::TypeError(
		String::New("First argument must be a string.")));
	}
	SpellDictionary* obj = ObjectWrap::Unwrap<SpellDictionary>(args.This());
	String::Utf8Value arg0(args[0]->ToString());
	WordData* wordData = new WordData();
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
	wordData->spellClass = obj->spellClass;
	wordData->request.data = wordData;
	
	uv_queue_work(uv_default_loop(), &wordData->request,
					SpellDictionary::addRemoveWordWork, SpellDictionary::addRemoveWordFinish);
	return Undefined();
}

Handle<Value> SpellDictionary::removeWord(const Arguments& args) {
	HandleScope scope;
	
	if (args.Length() < 1 || !args[0]->IsString()) {
		return ThrowException(Exception::TypeError(
		String::New("First argument must be a string.")));
	}
	SpellDictionary* obj = ObjectWrap::Unwrap<SpellDictionary>(args.This());
	String::Utf8Value arg0(args[0]->ToString());
	WordData* wordData = new WordData();
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
	wordData->spellClass = obj->spellClass;
	wordData->request.data = wordData;
	
	uv_queue_work(uv_default_loop(), &wordData->request,
					SpellDictionary::addRemoveWordWork, SpellDictionary::addRemoveWordFinish);
	return Undefined();
}

void SpellDictionary::addRemoveWordWork(uv_work_t* request){
	WordData* wordData = static_cast<WordData*>(request->data);
	int status;
	if(wordData->removeWord){
		status = wordData->spellClass->remove(wordData->word.c_str());
	}
	else{
		status = wordData->spellClass->add(wordData->word.c_str());
	}
	wordData->success = status == 0;
}

void SpellDictionary::addRemoveWordFinish(uv_work_t* request){
	HandleScope scope;
	WordData* wordData = static_cast<WordData*>(request->data);

	if(wordData->callbackExists){
		const unsigned argc = 2;
		Local<Value> argv[argc];
		argv[0] = Local<Value>::New(Boolean::New(wordData->success));
		argv[1] = Local<Value>::New(String::New(wordData->word.c_str()));
		TryCatch try_catch;
		wordData->callback->Call(Context::GetCurrent()->Global(), argc, argv);
		if (try_catch.HasCaught()) {
			node::FatalException(try_catch);
		}
		wordData->callback.Dispose();
	}
	delete wordData;
}
// Other Members
Handle<Value> SetDictionariesPath(const Arguments& args) {
	HandleScope scope;
	if (args.Length() < 1 || !args[0]->IsString()) {
		return ThrowException(Exception::TypeError(
		String::New("First argument must be a string.")));
	}
	String::Utf8Value arg0(args[0]->ToString());
	_dictionariesPath = *arg0;
	return scope.Close(Undefined());
}

#include "addwordperm.cpp"
void RegisterModule(Handle<Object> target) {
	HandleScope scope;
	SpellDictionary::Init(target);
	target->Set(String::NewSymbol("_setDictionariesPath"),
		FunctionTemplate::New(SetDictionariesPath)->GetFunction());
	target->Set(String::NewSymbol("addWordsPerm"),
		FunctionTemplate::New(AddWordsPermanently)->GetFunction());
}
NODE_MODULE(nodehun, RegisterModule);
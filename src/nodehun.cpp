/*Copyright (c) 2012 Nathan Sweet, DataSphere Inc.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "nodehun.hpp"
#include "addwordperm.hpp"


using namespace v8;
Persistent<FunctionTemplate> Nodehun::SpellDictionary::constructor;

bool Nodehun::dictionaryDirectoryExists(const char *file){
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

void Nodehun::SpellDictionary::Init(Handle<Object> target) {
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

Nodehun::SpellDictionary::SpellDictionary(const char *language){
	std::string path, affPath, dicPath;
	path.append(Nodehun::_dictionariesPath);
	path.append(language);
	path.append(__SLASH__);
	path.append(language);
	
	affPath.append(path);
	dicPath.append(path);
	affPath.append(".aff");
	dicPath.append(".dic");

	pathsExist = Nodehun::dictionaryDirectoryExists(affPath.c_str()) && Nodehun::dictionaryDirectoryExists(dicPath.c_str());
	if(pathsExist){
		spellClass = new Hunspell(affPath.c_str(), dicPath.c_str());
	}
}

Handle<Value> Nodehun::SpellDictionary::New(const Arguments& args) {
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
	Nodehun::SpellDictionary* obj = new Nodehun::SpellDictionary(*arg0);
	
	if(!obj->pathsExist){
		return ThrowException(Exception::TypeError(
		String::New("No such dictionary exists."))
		);
	}
	obj->Wrap(args.This());

	return args.This();
}

Handle<Value> Nodehun::SpellDictionary::spellSuggest(const Arguments& args) {
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
	Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
	
	String::Utf8Value arg0(args[0]->ToString());
	Local<Function> callback = Local<Function>::Cast(args[1]);

	Nodehun::SpellData* spellData = new Nodehun::SpellData();
	spellData->request.data = spellData;
	spellData->callback = Persistent<Function>::New(callback);
	spellData->word.append(*arg0);
	
	spellData->spellClass = obj->spellClass;
	spellData->multiple = false;
	uv_queue_work(uv_default_loop(), &spellData->request,
					Nodehun::SpellDictionary::CheckSuggestions, Nodehun::SpellDictionary::SendSuggestions);
	return Undefined();
}

Handle<Value> Nodehun::SpellDictionary::spellSuggestions(const Arguments& args) {
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
	Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
	
	String::Utf8Value arg0(args[0]->ToString());
	Local<Function> callback = Local<Function>::Cast(args[1]);

	Nodehun::SpellData* spellData = new Nodehun::SpellData();
	spellData->request.data = spellData;
	spellData->callback = Persistent<Function>::New(callback);
	spellData->word.append(*arg0);
	
	
	spellData->spellClass = obj->spellClass;
	spellData->multiple = true;
	uv_queue_work(uv_default_loop(), &spellData->request,
					Nodehun::SpellDictionary::CheckSuggestions, Nodehun::SpellDictionary::SendSuggestions);
	return Undefined();
}

void Nodehun::SpellDictionary::CheckSuggestions(uv_work_t* request) {
	Nodehun::SpellData* spellData = static_cast<Nodehun::SpellData*>(request->data);
	spellData->wordCorrect = spellData->spellClass->spell(spellData->word.c_str());
	if (!spellData->wordCorrect){
		spellData->numSuggest = spellData->spellClass->suggest(&(spellData->suggestions),spellData->word.c_str());
	}
	else{
		spellData->numSuggest = 0;
	}
}

void Nodehun::SpellDictionary::SendSuggestions(uv_work_t* request){
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

Handle<Value> Nodehun::SpellDictionary::addDictionary(const Arguments& args) {
	HandleScope scope;
	if (args.Length() < 1 || !args[0]->IsString()) {
		return ThrowException(Exception::TypeError(
		String::New("First argument must be a string.")));
	}
	Nodehun::SpellDictionary* obj = ObjectWrap::Unwrap<Nodehun::SpellDictionary>(args.This());
	Nodehun::DictData* dictData = new Nodehun::DictData();
	String::Utf8Value arg0(args[0]->ToString());
	if(args.Length() > 1 && args[1]->IsFunction()){
		Local<Function> callback = Local<Function>::Cast(args[1]);
		dictData->callback = Persistent<Function>::New(callback);
		dictData->callbackExists = true;
	}
	else{
		dictData->callbackExists = false;
	}
	
	dictData->path.append(Nodehun::_dictionariesPath);
	dictData->path.append(*arg0);
	dictData->path.append(__SLASH__);
	dictData->path.append(*arg0);
	dictData->path.append(".dic");
	dictData->dict.append(*arg0);
	
	dictData->spellClass = obj->spellClass;
	dictData->request.data = dictData;
	
	uv_queue_work(uv_default_loop(), &dictData->request,
					Nodehun::SpellDictionary::addDictionaryWork, Nodehun::SpellDictionary::addDictionaryFinish);
	return Undefined();

}

void Nodehun::SpellDictionary::addDictionaryWork(uv_work_t* request){
	Nodehun::DictData* dictData = static_cast<Nodehun::DictData*>(request->data);
	
	if(!Nodehun::dictionaryDirectoryExists(dictData->path.c_str())){
		dictData->success = false;
	}
	else{
		int status = dictData->spellClass->add_dic(dictData->path.c_str());
		dictData->success = status == 0;
	}
	
}

void Nodehun::SpellDictionary::addDictionaryFinish(uv_work_t* request){
	HandleScope scope;
	Nodehun::DictData* dictData = static_cast<Nodehun::DictData*>(request->data);

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

Handle<Value> Nodehun::SpellDictionary::addWord(const Arguments& args) {
	HandleScope scope;
	
	if (args.Length() < 1 || !args[0]->IsString()) {
		return ThrowException(Exception::TypeError(
		String::New("First argument must be a string.")));
	}
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
	wordData->spellClass = obj->spellClass;
	wordData->request.data = wordData;
	
	uv_queue_work(uv_default_loop(), &wordData->request,
					Nodehun::SpellDictionary::addRemoveWordWork, Nodehun::SpellDictionary::addRemoveWordFinish);
	return Undefined();
}

Handle<Value> Nodehun::SpellDictionary::removeWord(const Arguments& args) {
	HandleScope scope;
	
	if (args.Length() < 1 || !args[0]->IsString()) {
		return ThrowException(Exception::TypeError(
		String::New("First argument must be a string.")));
	}
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
	wordData->spellClass = obj->spellClass;
	wordData->request.data = wordData;
	
	uv_queue_work(uv_default_loop(), &wordData->request,
					Nodehun::SpellDictionary::addRemoveWordWork, Nodehun::SpellDictionary::addRemoveWordFinish);
	return Undefined();
}

void Nodehun::SpellDictionary::addRemoveWordWork(uv_work_t* request){
	Nodehun::WordData* wordData = static_cast<Nodehun::WordData*>(request->data);
	int status;
	if(wordData->removeWord){
		status = wordData->spellClass->remove(wordData->word.c_str());
	}
	else{
		status = wordData->spellClass->add(wordData->word.c_str());
	}
	wordData->success = status == 0;
}

void Nodehun::SpellDictionary::addRemoveWordFinish(uv_work_t* request){
	HandleScope scope;
	Nodehun::WordData* wordData = static_cast<Nodehun::WordData*>(request->data);

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

Handle<Value> Nodehun::SetDictionariesPath(const Arguments& args) {
	HandleScope scope;
	if (args.Length() < 1 || !args[0]->IsString()) {
		return ThrowException(Exception::TypeError(
		String::New("First argument must be a string.")));
	}
	String::Utf8Value arg0(args[0]->ToString());
	Nodehun::_dictionariesPath = *arg0;
	return scope.Close(Undefined());
}

void Nodehun::RegisterModule(Handle<Object> target) {
	HandleScope scope;
	SpellDictionary::Init(target);
	target->Set(String::NewSymbol("_setDictionariesPath"),
		FunctionTemplate::New(Nodehun::SetDictionariesPath)->GetFunction());
	target->Set(String::NewSymbol("addWordsPerm"),
		FunctionTemplate::New(Nodehun::AddWordsPermanently)->GetFunction());
}
bool Nodehun::sortStringsBool(std::string a, std::string b){
	int i = 0;
	std::string first = a.substr(0,a.find_first_of('/'));
	std::string second = b.substr(0,b.find_first_of('/'));
	while ((i < first.length()) && (i < second.length())){
		if (tolower (first[i]) < tolower (second[i])) return true;
		else if (tolower (first[i]) > tolower (second[i])) return false;
		i++;
	}
	return first.length() < second.length();
}

int Nodehun::sortStringsInt(std::string a, std::string b){
	int i = 0;
	std::string first = a.substr(0,a.find_first_of('/'));
	std::string second = b.substr(0,b.find_first_of('/'));
	while ((i < first.length()) && (i < second.length())){
		if (tolower (first[i]) < tolower (second[i])) return -1;
		else if (tolower (first[i]) > tolower (second[i])) return 1;
		i++;
	}
	if(first.length() < second.length()) return -1;
	else if(second.length() < first.length()) return 1;
	else return 0;
}

void Nodehun::AddWordsPermCloseFile(uv_fs_t *close_req){
	Nodehun::WordsData* wordsData = static_cast<Nodehun::WordsData*>(close_req->data);
	uv_async_send(&wordsData->async);
	uv_fs_req_cleanup(close_req);
}

void Nodehun::AddWordsPermWrite (uv_fs_t *write_req){
	Nodehun::WordsData* wordsData = static_cast<Nodehun::WordsData*>(write_req->data);
	if(write_req->result == -1){
		wordsData->error.append("Error writing dictionary.");
		wordsData->success = false;
	}
	else{
		wordsData->success = true;
	}
	wordsData->close_req.data = wordsData;
	uv_fs_close(uv_default_loop(), &wordsData->close_req, wordsData->open_req.result, Nodehun::AddWordsPermCloseFile);
	uv_fs_req_cleanup(write_req);
}

int Nodehun::AddWordsComp(WordsData * wordsData,std::string item, int i){
	std::string curStr;
	int compInt;
	if(i < wordsData->numWords){
		curStr = wordsData->words.at(i);
		compInt = sortStringsInt(curStr,item);
		if(compInt >= 0){
			if(compInt == 0)
				i++;
			wordsData->file.append(item);
			wordsData->file.append("\n");
		}
		else{
			wordsData->numAdded++;
			wordsData->wordsAdded.push_back(curStr.substr(0,curStr.find_first_of("/")).c_str());
			wordsData->file.append(curStr.c_str());
			wordsData->file.append("\n");
			i++;
			i = AddWordsComp(wordsData,item,i);
		}
	}
	else{
		wordsData->file.append(item);
		wordsData->file.append("\n");
	}
	return i;
}

void Nodehun::AddWordsPermRead(uv_fs_t *read_req) {
	Nodehun::WordsData* wordsData = static_cast<Nodehun::WordsData*>(read_req->data);
	
	if(read_req->result == -1){
		wordsData->error.append("Error reading dictionary.");
		wordsData->success = false;
		wordsData->close_req.data = wordsData;
		uv_fs_close(uv_default_loop(), &wordsData->close_req, wordsData->open_req.result, Nodehun::AddWordsPermCloseFile);
	}
	else {
		std::string in(wordsData->buffer);
		std::stringstream ss(in);
		std::string item;
		wordsData->numAdded = 0;
		bool first = true;
		int i = 0,
		    wordCount;
	    while(std::getline(ss, item)) {
			if(first){
				std::stringstream nss;
				nss << item;
				nss >> wordCount;
				first = false;
			}
			i = AddWordsComp(wordsData,item,i);
		}
		std::string curStr;
		char last = wordsData->file.at(wordsData->file.length() - 1);
		if(last == '\0'){
			wordsData->file.erase((wordsData->file.length() - 1),1);
			last = wordsData->file.at(wordsData->file.length() - 1);
		}
		if(last != '\n'){
			wordsData->file.append("\n");
		}
		while(i < wordsData->numWords){
			curStr = wordsData->words.at(i);
			wordsData->numAdded++;
			wordsData->wordsAdded.push_back(curStr.substr(0,curStr.find_first_of("/")).c_str());
			wordsData->file.append(curStr.c_str());
			wordsData->file.append("\n");
			i++;
		}
		if(wordsData->numAdded > 0){
			wordCount = wordCount + wordsData->numAdded;
			std::size_t lineEnd = wordsData->file.find_first_not_of("0123456789");
			wordsData->file.erase(0,lineEnd);
			std::stringstream ss;
			ss << wordCount;
			wordsData->file.insert(0,ss.str());
			wordsData->file.erase((wordsData->file.length() - 1),1);
			wordsData->file.append("\0");
		}
		wordsData->write_req.data = wordsData;
		wordsData->fileSize = wordsData->file.size();
		uv_fs_write(uv_default_loop(), &wordsData->write_req, wordsData->open_req.result,
			(void*)wordsData->file.c_str(), wordsData->fileSize, 0, Nodehun::AddWordsPermWrite);
		
	}
	if(wordsData->buffer)
		free(wordsData->buffer);
	uv_fs_req_cleanup(read_req);
}

void Nodehun::AddWordsPermStat(uv_fs_t* stat_req){
	Nodehun::WordsData* wordsData = static_cast<Nodehun::WordsData*>(stat_req->data);
	if(stat_req->result == -1){
		wordsData->error.append("Dictionary does not exist");
		wordsData->success = false;
		wordsData->close_req.data = wordsData;
		uv_fs_close(uv_default_loop(), &wordsData->close_req, wordsData->open_req.result, Nodehun::AddWordsPermCloseFile);
	}
	else{
		wordsData->read_req.data = wordsData;
		wordsData->fileSize = ((struct stat*)stat_req->ptr)->st_size;
		wordsData->buffer = (char*)malloc(wordsData->fileSize);
		uv_fs_read(uv_default_loop(), &wordsData->read_req, wordsData->open_req.result, wordsData->buffer,
					wordsData->fileSize, -1, Nodehun::AddWordsPermRead);
	}
	uv_fs_req_cleanup(stat_req);
}

void Nodehun::AddWordsPermOpen(uv_fs_t* open_req){
	Nodehun::WordsData* wordsData = static_cast<Nodehun::WordsData*>(open_req->data);
	if(open_req->result == -1){
		wordsData->error.append("Dictionary does not exist");
		wordsData->success = false;
		wordsData->close_req.data = wordsData;
		uv_fs_close(uv_default_loop(), &wordsData->close_req, wordsData->open_req.result, Nodehun::AddWordsPermCloseFile);
	}
	else{
		wordsData->stat_req.data = wordsData;
		uv_fs_fstat(uv_default_loop(), &wordsData->stat_req, open_req->result, Nodehun::AddWordsPermStat);	
	}
	uv_fs_req_cleanup(open_req);
}

void Nodehun::AddWordsPermWork(void* arg){
	uv_async_t* async = (uv_async_t*)arg;
	Nodehun::WordsData* wordsData = static_cast<Nodehun::WordsData*>(async->data);

	std::sort(wordsData->words.begin(), wordsData->words.end(),sortStringsBool);
	
	wordsData->success = true;

	std::string dictionaryDir;
	dictionaryDir.append(Nodehun::_dictionariesPath);
	dictionaryDir.append(wordsData->dictionary);
	dictionaryDir.append(__SLASH__);
	dictionaryDir.append(wordsData->dictionary);
	dictionaryDir.append(".dic");
	wordsData->open_req.data = wordsData;
	uv_fs_open(uv_default_loop(), &wordsData->open_req, dictionaryDir.c_str(),O_RDWR,O_EXCL,Nodehun::AddWordsPermOpen);
}

void Nodehun::AddWordsPermFinish(uv_async_t *async, int status) {
	HandleScope scope;
	Nodehun::WordsData* wordsData = static_cast<Nodehun::WordsData*>(async->data);
	if(wordsData->callbackExists){
		const unsigned argc = 2;
		Local<Value> argv[argc];
		if(wordsData->success){
			argv[0] = Local<Value>::New(Null());
			Local<Value> jsArr = Array::New(wordsData->numAdded);
			Array* jsArrT = Array::Cast(*jsArr);
			std::vector<std::string>::iterator wordIt;
			for(int i = 0; i < wordsData->numAdded; i++)
				jsArrT->Set(i,Local<Value>::New(String::New(wordsData->wordsAdded.at(i).c_str())));
			argv[1] = Local<Value>::New(jsArr);
		}
		else{
			argv[0] = Local<Value>::New(String::New(wordsData->error.c_str()));
			argv[1] = Local<Value>::New(Null());
		}
		TryCatch try_catch;
		wordsData->callback->Call(Context::GetCurrent()->Global(), argc, argv);
		if (try_catch.HasCaught()) {
			node::FatalException(try_catch);
		}
		wordsData->callback.Dispose();
	}
	uv_close((uv_handle_t*) async, NULL);
	uv_thread_join(&wordsData->thread_id);
	delete wordsData;
}

Handle<Value> Nodehun::AddWordsPermanently(const Arguments& args){
	HandleScope scope;
	if(args.Length() < 2){
		return ThrowException(Exception::TypeError(
		String::New("Missing required arguments.")));
	}
	if(!args[0]->IsString()){
		return ThrowException(Exception::TypeError(
		String::New("First argument must be a string.")));
	}
	if(!args[1]->IsArray()){
		return ThrowException(Exception::TypeError(
		String::New("Second argument must be an array.")));
	}

	Nodehun::WordsData* wordsData = new Nodehun::WordsData();
	
	if(args.Length() > 2 && args[2]->IsFunction()){
		Local<Function> callback = Local<Function>::Cast(args[2]);
		wordsData->callback = Persistent<Function>::New(callback);
		wordsData->callbackExists = true;
	}
	else{
		wordsData->callbackExists = false;
	}
	String::Utf8Value arg0(args[0]->ToString());
	wordsData->dictionary.append(*arg0);
	
	Array* jsArr = Array::Cast(*args[1]);
	int arrL = jsArr->Length();
	Local<Value> obj;
	Array* jsArrTmp;
	std::string tStr;
	for(int i = 0; i < arrL; i++){
		obj = jsArr->Get(i);
		tStr = "";
		if(obj->IsString()){
			String::Utf8Value tmpStr(obj->ToString());
			wordsData->words.push_back(*tmpStr);
		}
		else if(obj->IsArray()){
			jsArrTmp = Array::Cast(*obj);
			if(jsArrTmp->Length() > 1 && jsArrTmp->Get(0)->IsString() && jsArrTmp->Get(1)->IsString()){
				String::Utf8Value tmpStr(jsArrTmp->Get(0)->ToString());
				tStr.append(*tmpStr);
				tStr.append("/");
				String::Utf8Value tmpStr2(jsArrTmp->Get(1)->ToString());
				tStr.append(*tmpStr2);
				wordsData->words.push_back(tStr);
			}
		}
	}
	wordsData->numWords = arrL;
	wordsData->async.data = wordsData;
	
	uv_async_init(uv_default_loop(), &wordsData->async, Nodehun::AddWordsPermFinish);
	uv_thread_create(&wordsData->thread_id, Nodehun::AddWordsPermWork, (void *)&wordsData->async);
	
	return Undefined();
}

NODE_MODULE(nodehun, Nodehun::RegisterModule);

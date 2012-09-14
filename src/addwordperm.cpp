#include <algorithm>
#include <vector>
#include <iostream>
#include <sstream>
#include <fcntl.h>

struct WordsData {
	Persistent<Function> callback;
	bool callbackExists;
	std::vector<std::string> words;
	std::vector<std::string> wordsAdded;
	bool success;
	int numWords;
	int numAdded;
	std::string dictionary;
	uv_async_t async;
	uv_fs_t open_req;
	uv_fs_t stat_req;
	uv_fs_t read_req;
	uv_fs_t write_req;
	uv_fs_t close_req;
	std::string error;
	std::string file;
	uv_thread_t thread_id;
	int fileSize;
	char * buffer;
};

bool sortStringsBool(std::string a, std::string b){
	int i = 0;
	std::string first = a.substr(0,a.find_first_of('/'));
	std::string second = b.substr(0,b.find_first_of('/'));
	while ((i < first.length()) && (i < second.length()))
	{
		if (tolower (first[i]) < tolower (second[i])) return true;
		else if (tolower (first[i]) > tolower (second[i])) return false;
		i++;
	}
	return first.length() < second.length();
}

int sortStringsInt(std::string a, std::string b){
	int i = 0;
	std::string first = a.substr(0,a.find_first_of('/'));
	std::string second = b.substr(0,b.find_first_of('/'));
	while ((i < first.length()) && (i < second.length()))
	{
		if (tolower (first[i]) < tolower (second[i])) return -1;
		else if (tolower (first[i]) > tolower (second[i])) return 1;
		i++;
	}
	if(first.length() < second.length()) return -1;
	else if(second.length() < first.length()) return 1;
	else return 0;
}

void AddWordsPermCloseFile(uv_fs_t *close_req){
	WordsData* wordsData = static_cast<WordsData*>(close_req->data);
	uv_async_send(&wordsData->async);
	uv_fs_req_cleanup(close_req);
}

void AddWordsPermWrite (uv_fs_t *write_req){
	WordsData* wordsData = static_cast<WordsData*>(write_req->data);
	if(write_req->result == -1){
		wordsData->error.append("Error writing dictionary.");
		wordsData->success = false;
	}
	else{
		wordsData->success = true;
	}
	wordsData->close_req.data = wordsData;
	uv_fs_close(uv_default_loop(), &wordsData->close_req, wordsData->open_req.result, AddWordsPermCloseFile);
	uv_fs_req_cleanup(write_req);
}

int AddWordsComp(WordsData * wordsData,std::string item, int i){
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

void AddWordsPermRead(uv_fs_t *read_req) {
	WordsData* wordsData = static_cast<WordsData*>(read_req->data);
	
	if(read_req->result == -1){
		wordsData->error.append("Error reading dictionary.");
		wordsData->success = false;
		wordsData->close_req.data = wordsData;
		uv_fs_close(uv_default_loop(), &wordsData->close_req, wordsData->open_req.result, AddWordsPermCloseFile);
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
			(void*)wordsData->file.c_str(), wordsData->fileSize, 0, AddWordsPermWrite);
		
	}
	if(wordsData->buffer)
		free(wordsData->buffer);
	uv_fs_req_cleanup(read_req);
}

void AddWordsPermStat(uv_fs_t* stat_req){
	WordsData* wordsData = static_cast<WordsData*>(stat_req->data);
	if(stat_req->result == -1){
		wordsData->error.append("Dictionary does not exist");
		wordsData->success = false;
		wordsData->close_req.data = wordsData;
		uv_fs_close(uv_default_loop(), &wordsData->close_req, wordsData->open_req.result, AddWordsPermCloseFile);
	}
	else{
		wordsData->read_req.data = wordsData;
		wordsData->fileSize = ((struct stat*)stat_req->ptr)->st_size;
		wordsData->buffer = (char*)malloc(wordsData->fileSize);
		uv_fs_read(uv_default_loop(), &wordsData->read_req, wordsData->open_req.result, wordsData->buffer,
					wordsData->fileSize, -1, AddWordsPermRead);
	}
	uv_fs_req_cleanup(stat_req);
}

void AddWordsPermOpen(uv_fs_t* open_req){
	WordsData* wordsData = static_cast<WordsData*>(open_req->data);
	if(open_req->result == -1){
		wordsData->error.append("Dictionary does not exist");
		wordsData->success = false;
		wordsData->close_req.data = wordsData;
		uv_fs_close(uv_default_loop(), &wordsData->close_req, wordsData->open_req.result, AddWordsPermCloseFile);
	}
	else{
		wordsData->stat_req.data = wordsData;
		uv_fs_fstat(uv_default_loop(), &wordsData->stat_req, open_req->result, AddWordsPermStat);	
	}
	uv_fs_req_cleanup(open_req);
}

void AddWordsPermWork(void* arg){
	uv_async_t* async = (uv_async_t*)arg;
	WordsData* wordsData = static_cast<WordsData*>(async->data);

	std::sort(wordsData->words.begin(), wordsData->words.end(),sortStringsBool);
	
	wordsData->success = true;

	std::string dictionaryDir;
	dictionaryDir.append(_dictionariesPath);
	dictionaryDir.append(wordsData->dictionary);
	dictionaryDir.append(__SLASH__);
	dictionaryDir.append(wordsData->dictionary);
	dictionaryDir.append(".dic");
	wordsData->open_req.data = wordsData;
	uv_fs_open(uv_default_loop(), &wordsData->open_req, dictionaryDir.c_str(),O_RDWR,O_EXCL,AddWordsPermOpen);
}

void AddWordsPermFinish(uv_async_t *async, int status) {
	HandleScope scope;
	WordsData* wordsData = static_cast<WordsData*>(async->data);
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

Handle<Value> AddWordsPermanently(const Arguments& args){
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

	WordsData* wordsData = new WordsData();
	
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
	
	uv_async_init(uv_default_loop(), &wordsData->async, AddWordsPermFinish);
	uv_thread_create(&wordsData->thread_id, AddWordsPermWork, (void *)&wordsData->async);
	
	return Undefined();
}
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
#include <uv.h>
#include <string>
#include <hunspell.hxx>
#include <node.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include <sstream>
#include <fcntl.h>
#ifdef _WIN32
#define __SLASH__ "\\"
#else
#define __SLASH__ "/"
#endif

namespace Nodehun{
	//
	// The folder in which the dictionaries are contained 
	//
	std::string _dictionariesPath;
	//
	// This is the JS object that binds to hunspell:
	// its internal methods are simply proxies to the
	// related hunspell methods.
	//
	class SpellDictionary;
	//
	// Checks to see if a dictionary exists
	// based on whether it exists in the dictionary
	// directory.
	//
	bool dictionaryDirectoryExists(const char *file);
	//
	// Sets where the dictionaries' folder is.
	//
	v8::Handle<v8::Value> SetDictionariesPath(const v8::Arguments& args);
	//
	// This registers all of the correct methods and objects
	// with nodeJS.
	//
	void RegisterModule(v8::Handle<v8::Object> target);
	//
	// This is a baton for the asynchronous work of adding
	// or removing a word from the dictionary object at runtime.
	//
	struct WordData {
		uv_work_t request;
		v8::Persistent<v8::Function> callback;
		bool removeWord;
		bool callbackExists;
		bool success;
		std::string word;
		Hunspell *spellClass;
	};
	//
	// represents a work baton to asynchronously add a new
	// new dictionary, during runtime, to the object of 
	// an existing dictionary.
	//
	struct DictData {
		uv_work_t request;
		v8::Persistent<v8::Function> callback;
		bool callbackExists;
		std::string path;
		std::string dict;
		bool success;
		Hunspell *spellClass;
	};
	//
	// This is a baton for the asynchronous work of allowing
	// the hunspell object to process a string to see if it is
	// a defined word and/or if not what the correct spelling might be.
	//
	struct SpellData {
		uv_work_t request;
		v8::Persistent<v8::Function> callback;
		std::string word;
		bool multiple;
		Hunspell *spellClass;
		bool wordCorrect;
		char **suggestions;
		int numSuggest;
	};
	//
	// This struct is the baton that sees words added permanently
	// to a file from start to finish. The many data types represent
	// other data structures used to accomplish asynchronous file i/o
	// tasks like opening a file and reading a file descriptor.
	//
	struct WordsData {
		v8::Persistent<v8::Function> callback;
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
	//
	// This method compares two strings and sorts
	// for the "sort" algorithm and sorts them alphabeticaly
	// ignoring case. This is for sorting the received string
	// arguments
	//
	bool sortStringsBool(std::string a, std::string b);
	//
	// This method compares two strings and sorts them alphabetically
	// regardles of case but does regard possible duplication.
	//
	int sortStringsInt(std::string a, std::string b);
	//
	// After a file has been written to or reading/writing
	// to it has failed this method closes it.
	//
	void AddWordsPermCloseFile(uv_fs_t *close_req);
	//
	// This method writes the buffer to the file
	// that has been created by the sorting algorithm.
	//
	void AddWordsPermWrite (uv_fs_t *write_req);
	//
	// This method carefully compares two words to make sure
	// they are in the right order in relation to one another.
	//
	int AddWordsComp(WordsData * wordsData,std::string item, int i);
	//
	// This method receives the buffer of the file
	// then inserts the words that have been received
	// as an argument into the appropriate place in the stream.
	//
	void AddWordsPermRead(uv_fs_t *read_req);
	//
	// This method gets the size of the file and then
	// allocates memory for a buffer, then calls the read function.
	//
	void AddWordsPermStat(uv_fs_t* stat_req);
	//
	// This method simply opens the file then
	// pases the file descriptor to the Stat method
	// to get it's size.
	//
	void AddWordsPermOpen(uv_fs_t* open_req);
	//
	// This is the method the starts the thread
	// out of the thread that v8 is in to do the 
	// file i/o work (all asynchronously).
	//
	void AddWordsPermWork(void* arg);
	//
	// When all the work is done this file
	// receives the word baton and calls the JS callback
	// function that was passed as an argument
	// with the result of the execution of adding
	// words permanently.
	//
	void AddWordsPermFinish(uv_async_t *async, int status);
	//
	// This is the JavaScript function that proxys the 
	// add words functionality.
	//
	v8::Handle<v8::Value> AddWordsPermanently(const v8::Arguments& args);

};

class Nodehun::SpellDictionary : public node::ObjectWrap {
	public:
		//
		// The function that gets called by JavaScript
		// when a new object is being created.
		//
		static v8::Persistent<v8::FunctionTemplate> constructor;
		static void Init(v8::Handle<v8::Object> target);
		SpellDictionary(const char *language);
		//
		// The destructor has to elimintate it's reference
		// to the spellClass object (Hunspell) otherwise
		// the object's reference count won't go down to zero.
		//
		~SpellDictionary(){
			if (spellClass != NULL)
				delete spellClass;
			spellClass = NULL;
		};
		bool pathsExist;
		// The pointer to the Hunspell Object.
		Hunspell *spellClass;
	protected:
		//
		// When a new JS object is created
		//
		static v8::Handle<v8::Value> New(const v8::Arguments& args);
		//
		// Suggest a singularly correct spelling from a string.
		//
		static v8::Handle<v8::Value> spellSuggest(const v8::Arguments& args);
		//
		// Suggest a list of possible spellings from a string.
		// Ordered by correctness.
		//
		static v8::Handle<v8::Value> spellSuggestions(const v8::Arguments& args);
		//
		// Add a new dictionary to an existing dictionary object at runtime (ephemerally).
		//
		static v8::Handle<v8::Value> addDictionary(const v8::Arguments& args);
		//
		// Add a word to a dictionary object at runtime (ephemerally).
		//
		static v8::Handle<v8::Value> addWord(const v8::Arguments& args);
		//
		// Remove a word from a dictionary object at runtime (ephemerally).
		//
		static v8::Handle<v8::Value> removeWord(const v8::Arguments& args);
		//
		// The work (threaded) functionality to add a new dictionary
		// to the current dictionary object.
		//
		static void addDictionaryWork(uv_work_t* request);
		//
		// The call back to merge the thread back and return the result
		// of a successful addition of a dictionary to the dictionary
		// at runtime.
		//
		static void addDictionaryFinish(uv_work_t* request);
		//
		// add/remove a word work (threaded) to the dictionary
		// object at runtime.
		//
		static void addRemoveWordWork(uv_work_t* request);
		//
		// the call back to merge the thread that added/removed
		// a word from the dictionary object.
		//
		static void addRemoveWordFinish(uv_work_t* request);
		//
		// The work (threaded) to check to see if a given
		// string and if not what any possible suggestions might be.
		//
		static void CheckSuggestions(uv_work_t* request);
		//
		// The call back to merge the thread that checked for spelling
		// suggestions from the dictionary object to return the result
		// of the work.
		//
		static void SendSuggestions(uv_work_t* request);
};

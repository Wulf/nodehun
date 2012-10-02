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
namespace Nodehun{
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
}

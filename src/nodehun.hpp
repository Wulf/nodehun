#include "license.nodehun"
#include <uv.h>
#include <string>
#include <hunspell.hxx>
#include <node.h>
#include <node_buffer.h>

namespace Nodehun {
  //
  // This is the JS object that binds to hunspell:
  // its internal methods are simply proxies to the
  // related hunspell methods.
  //
  class SpellDictionary;
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
    Nodehun::SpellDictionary* obj;
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
    char * dict;
    bool success;
    Nodehun::SpellDictionary* obj;
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
    Nodehun::SpellDictionary* obj;
    bool wordCorrect;
    char** suggestions;
    int numSuggest;
  };
  //
  // This is a baton for the obtaining analyze data.
  //
  struct AnalyzeData{
    uv_work_t request;
    v8::Persistent<v8::Function> callback;
    bool callbackExists;
    std::string word;
    Nodehun::SpellDictionary *obj;
    char** results;
    int numResults;
  };
  //
  // This is a baton for obtaining stem data.
  //
  struct StemData{
    uv_work_t request;
    v8::Persistent<v8::Function> callback;
    bool callbackExists;
    Nodehun::SpellDictionary *obj;
    char** words;
    int numWords;
    char** results;
    int numResults;
  };
}

class Nodehun::SpellDictionary : public node::ObjectWrap {
public:
  //
  // The function that gets called by JavaScript
  // when a new object is being created.
  //
  static v8::Persistent<v8::FunctionTemplate> constructor;
  static void Init(v8::Handle<v8::Object> exports, v8::Handle<v8::Object> module);
  SpellDictionary(const char *affbuf, const char *dictbuf);
  //
  // The destructor!
  //
  ~SpellDictionary(){
    if (spellClass){
      delete spellClass;
      spellClass = NULL;
    }
  };
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
  static void addDictionaryFinish(uv_work_t* request, int i = -1);
  //
  // add/remove a word work (threaded) to the dictionary
  // object at runtime.
  //
  static void addRemoveWordWork(uv_work_t* request);  
  //
  // the call back to merge the thread that added/removed
  // a word from the dictionary object.
  //
  static void addRemoveWordFinish(uv_work_t* request, int i = -1);
  //
  // The work (threaded) to check to see if a given
  // string and if not what any possible suggestions might be.
  //
  static void checkSuggestions(uv_work_t* request);
  //
  // The call back to merge the thread that checked for spelling
  // suggestions from the dictionary object to return the result
  // of the work.
  //
  static void sendSuggestions(uv_work_t* request, int i = -1);
  //
  // The node function to call to analyze a word's stems
  //
  static v8::Handle<v8::Value> analyze(const v8::Arguments& args);
  //
  // Threaded work on hunspell to analyze a word's stems
  //
  static void analyzeWork(uv_work_t* request);
  //
  // Join thread and return stems from the hunspell work
  //
  static void analyzeFinish(uv_work_t* request, int i);
  //
  // node wrapped hunspell stem function
  //
  static v8::Handle<v8::Value> stem(const v8::Arguments& args);
  //
  // Threaded work on hunspell to get stem results from hunspell
  //
  static void stemWork(uv_work_t* request);
  //
  // Join thread and return stemming results
  //
  static void stemFinish(uv_work_t* request, int i);/*
  //
  // Generate stemming information from work
  //
  static v8::Handle<v8::Value> generate(const v8::Arguments& args);
  //
  // Threaded generate work
  //
  static void generateWork(uv_work_t* request);
  //
  // Join generate work results to ui thread.
  //
  static void generateFinish(uv_work_t* request, int i);*/
};

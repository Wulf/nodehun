#include <uv.h>
#include <string>
#include <cstring>
#include <hunspell.hxx>
#include <node.h>
#include <node_buffer.h>
#include <node_object_wrap.h>

using namespace v8;
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
    v8::Isolate* isolate;
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
    v8::Isolate* isolate;
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
    v8::Isolate* isolate;
    std::string word;
    bool multiple;
    Nodehun::SpellDictionary* obj;
    bool wordCorrect;
    char** suggestions;
    int numSuggest;
  };
  //
  // This is a baton for obtaining stem data.
  //
  struct StemData{
    uv_work_t request;
    v8::Persistent<v8::Function> callback;
    v8::Isolate *isolate;
    Nodehun::SpellDictionary *obj;
    std::string word;
    char** results;
    int numResults;
  };
  //
  // This is a baton for obtaining generate data.
  //
  struct GenerateData{
    uv_work_t request;
    v8::Persistent<v8::Function> callback;
    v8::Isolate *isolate;
    Nodehun::SpellDictionary *obj;
    std::string word;
    std::string word2;
    char** results;
    int numResults;
  };
  //
  // This is a baton for obtaining analyze data.
  //
  struct AnalyzeData{
    uv_work_t request;
    v8::Persistent<v8::Function> callback;
    v8::Isolate *isolate;
    Nodehun::SpellDictionary *obj;
    std::string word;
    char** results;
    int numResults;
  };
  //
  // represents a work baton to asynchronously create a new
  // nodehun instance. 
  //
  struct NodehunData {
    uv_work_t request;
    v8::Persistent<v8::Function> callback;
    v8::Isolate *isolate;
    char * aff;
    char * dict;
    Hunspell *obj;
  };
}

class Nodehun::SpellDictionary : public node::ObjectWrap {
public:
  //
  // The function that gets called by JavaScript
  // when a new object is being created.
  //
  static v8::Persistent<v8::Function> constructor;
  static void Init(v8::Handle<v8::Object>, v8::Handle<v8::Object>);
  SpellDictionary(const char*, const char*);
  SpellDictionary(Hunspell*);
  //
  // The destructor!
  //
  ~SpellDictionary(){
    if (spellClass){
      delete spellClass;
      spellClass = NULL;
    }
    uv_rwlock_destroy(&rwlock);
  };
  // The pointer to the Hunspell Object.
  Hunspell *spellClass;
  uv_rwlock_t rwlock;
protected:
  //
  // Creates a new nodehun, asynchronously
  //
  static void createNewNodehun(const FunctionCallbackInfo<Value>&);
  //
  // When a new JS object is created
  //
  static void New(const FunctionCallbackInfo<Value>&);
  static void New();
  //
  // Suggest a singularly correct spelling from a string.
  //
  static void spellSuggest(const FunctionCallbackInfo<Value>&);
  //
  // Suggest a list of possible spellings from a string.
  // Ordered by correctness.
  //
  static void spellSuggestions(const FunctionCallbackInfo<Value>&);
  //
  // Add a new dictionary to an existing dictionary object at runtime (ephemerally).
  //
  static void addDictionary(const FunctionCallbackInfo<Value>&);
  //
  // Add a word to a dictionary object at runtime (ephemerally).
  //
  static void addWord(const FunctionCallbackInfo<Value>&);
  //
  // Remove a word from a dictionary object at runtime (ephemerally).
  //
  static void removeWord(const FunctionCallbackInfo<Value>&);
  //
  // Does the bulk of the work for adding and removing a word
  //
  static void addRemoveWordInit(const FunctionCallbackInfo<Value>&, bool);
  //
  // new nodehun work
  //
  static void createNewNodehunWork(uv_work_t*);  
  //
  // new nodehun finish
  //
  static void createNewNodehunFinish(uv_work_t*, int i = -1);
  //
  // The work (threaded) functionality to add a new dictionary
  // to the current dictionary object.
  //
  static void addDictionaryWork(uv_work_t*);
  //
  // The call back to merge the thread back and return the result
  // of a successful addition of a dictionary to the dictionary
  // at runtime.
  //
  static void addDictionaryFinish(uv_work_t*, int i = -1);
  //
  // add/remove a word work (threaded) to the dictionary
  // object at runtime.
  //
  static void addRemoveWordWork(uv_work_t*);  
  //
  // the call back to merge the thread that added/removed
  // a word from the dictionary object.
  //
  static void addRemoveWordFinish(uv_work_t*, int i = -1);
  //
  // The work (threaded) to check to see if a given
  // string and if not what any possible suggestions might be.
  //
  static void checkSuggestions(uv_work_t*);
  //
  // The call back to merge the thread that checked for spelling
  // suggestions from the dictionary object to return the result
  // of the work.
  //
  static void sendSuggestions(uv_work_t*, int i = -1);
  //
  // node wrapped hunspell stem function
  //
  static void stem(const FunctionCallbackInfo<Value>&);
  //
  // Threaded work on hunspell to get stem results from hunspell
  //
  static void stemWork(uv_work_t*);
  //
  // Join thread and return stemming results
  //
  static void stemFinish(uv_work_t*, int i = -1);
  //
  // node wrapped hunspell generate function
  //
  static void generate(const FunctionCallbackInfo<Value>&);
  //
  // Threaded work on hunspell to get generate results from hunspell
  //
  static void generateWork(uv_work_t*);
  //
  // Join thread and return generateming results
  //
  static void generateFinish(uv_work_t*, int i = -1);
  //
  // node wrapped hunspell analyze function
  //
  static void analyze(const FunctionCallbackInfo<Value>&);
  //
  // Threaded work on hunspell to get analyze results from hunspell
  //
  static void analyzeWork(uv_work_t*);
  //
  // Join thread and return analyzeming results
  //
  static void analyzeFinish(uv_work_t*, int i = -1);

};

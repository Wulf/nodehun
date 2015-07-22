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
  // This is a baton for the asynchronous work of checking
  // whether a word is spelled correctly or not.
  //
  struct CorrectData {
    uv_work_t request;
    v8::Persistent<v8::Function> callback;
    v8::Isolate* isolate;
    bool isCorrect;
    std::string word;
    Nodehun::SpellDictionary* obj;
  };
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
  static void createNewNodehunWork(uv_work_t*);  
  static void createNewNodehunFinish(uv_work_t*, int i = -1);
  //
  // When a new JS object is created
  //
  static void New(const FunctionCallbackInfo<Value>&);
  static void New();
  //
  // Returns a boolean value as to whether a word is spelled correctly
  // or not (i.e. whether it exists in the dictionary or not).
  //
  static void isCorrect(const FunctionCallbackInfo<Value>&);
  static void isCorrectSync(const FunctionCallbackInfo<Value>&);
  static void checkCorrect(uv_work_t*);
  static void sendCorrect(uv_work_t*, int i = -1);
  bool spell(const char*);
  //
  // Suggest a singularly correct spelling from a string. `spellSuggestions`
  // sends multiple suggestions.
  //
  static void spellSuggest(const FunctionCallbackInfo<Value>&);
  static void spellSuggestSync(const FunctionCallbackInfo<Value>&);
  static void spellSuggestions(const FunctionCallbackInfo<Value>&);
  static void spellSuggestionsSync(const FunctionCallbackInfo<Value>&);
  static void checkSuggestions(uv_work_t*);
  static void sendSuggestions(uv_work_t*, int i = -1);
  int spellCheck(bool*, char***, const char*);
  //
  // Add a new dictionary to an existing dictionary object at runtime (ephemerally).
  //
  static void addDictionary(const FunctionCallbackInfo<Value>&);
  static void addDictionarySync(const FunctionCallbackInfo<Value>&);
  static void addDictionaryWork(uv_work_t*);
  static void addDictionaryFinish(uv_work_t*, int i = -1);
  bool addDict(const char*);
  //
  // Add or remove a word to a dictionary object at runtime (ephemerally).
  //
  static void addWord(const FunctionCallbackInfo<Value>&);
  static void addWordSync(const FunctionCallbackInfo<Value>&);
  static void removeWord(const FunctionCallbackInfo<Value>&);
  static void removeWordSync(const FunctionCallbackInfo<Value>&);
  static void addRemoveWordInit(const FunctionCallbackInfo<Value>&, bool);
  static void addRemoveWordInitSync(const FunctionCallbackInfo<Value>&, bool);
  static void addRemoveWordWork(uv_work_t*);  
  static void addRemoveWordFinish(uv_work_t*, int i = -1);
  bool addRemoveWord(const char*, bool);
  //
  // node wrapped hunspell stem function
  //
  static void stem(const FunctionCallbackInfo<Value>&);
  static void stemSync(const FunctionCallbackInfo<Value>&);
  static void stemWork(uv_work_t*);
  static void stemFinish(uv_work_t*, int i = -1);
  int stemWord(char***, const char*);
  //
  // node wrapped hunspell generate function
  //
  static void generate(const FunctionCallbackInfo<Value>&);
  static void generateSync(const FunctionCallbackInfo<Value>&);
  static void generateWork(uv_work_t*);
  static void generateFinish(uv_work_t*, int i = -1);
  int generateWord(char***, const char*, const char*);
  //
  // node wrapped hunspell analyze function
  //
  static void analyze(const FunctionCallbackInfo<Value>&);
  static void analyzeSync(const FunctionCallbackInfo<Value>&);
  static void analyzeWork(uv_work_t*);
  static void analyzeFinish(uv_work_t*, int i = -1);
  int analyzeWord(char***, const char*);
};

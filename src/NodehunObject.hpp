#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <string>
#include <cstring>
#include <hunspell.hxx>

#include <nan.h>

class MyObject : public Nan::ObjectWrap {
  public:
    static void Init(v8::Local<v8::Object> exports, v8::Local<v8::Object> module);

  private:
    static Nan::Persistent<v8::Function> constructor;

    Hunspell* hunspellInstance;

    explicit MyObject(const char* affbuf, const char* dicbuf);
    ~MyObject();

    static NAN_METHOD(New); // constructor

    // Spelling API
    static NAN_METHOD(suggest);
    static NAN_METHOD(suggestSync);
    static NAN_METHOD(suggestOne);
    static NAN_METHOD(suggestOneSync);
    static NAN_METHOD(corret);
    static NAN_METHOD(correctSync);

    // Analysis API
    static NAN_METHOD(stem);
    static NAN_METHOD(stemSync);
    static NAN_METHOD(generate);
    static NAN_METHOD(generateSync);
    static NAN_METHOD(analyze);
    static NAN_METHOD(analyzeSync);

    // Dictionary Management API
    static NAN_METHOD(addWord);
    static NAN_METHOD(addWordSync);
    static NAN_METHOD(removeWord);
    static NAN_METHOD(removeWordSync);
    static NAN_METHOD(addDictionary);
    static NAN_METHOD(addDictionarySync);
};

#endif

#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <string>
#include <cstring>
#include <hunspell.hxx>

#include <nan.h>

class Nodehun : public Nan::ObjectWrap {
  public:
    static void Init(v8::Local<v8::Object> exports, v8::Local<v8::Object> module);
    class CorrectWorker : public Nan::AsyncWorker {
      public:
        CorrectWorker(Nan::Callback *callback, Hunspell *hunspellInstance, std::string *word)
        : Nan::AsyncWorker(callback), word(word), hunspellInstance(hunspellInstance), correct(false) {}

        void Execute () {
          correct = hunspellInstance->spell(*word);
        }

        void HandleOKCallback () {
          Nan::HandleScope scope;

          v8::Local<v8::Value> argv[] = {
              Nan::Null(),
              Nan::New<v8::Boolean>(correct)
          };

          callback->Call(2, argv);
        }

        void Destroy() {
          delete word;
        }

      private:
        std::string *word;
        bool correct;
        Hunspell *hunspellInstance;
    };

  private:
    static Nan::Persistent<v8::Function> constructor;

    Hunspell* hunspellInstance;

    explicit Nodehun(const char* affbuf, const char* dicbuf);
    ~Nodehun();

    static NAN_METHOD(New); // constructor

    // Spelling API
    static NAN_METHOD(suggest);
    static NAN_METHOD(suggestSync);
    static NAN_METHOD(suggestOne);
    static NAN_METHOD(suggestOneSync);
    static NAN_METHOD(correct);
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

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
    class SuggestWorker : public Nan::AsyncWorker {
      public:
        SuggestWorker(Nan::Callback *callback, Hunspell *hunspellInstance, std::string *word)
        : Nan::AsyncWorker(callback), word(word), hunspellInstance(hunspellInstance), suggestionsFound(NULL) {}

        void Execute () {
          correct = hunspellInstance->spell(*word);
          if(correct) {
            size = 0;
          } else {
            size = hunspellInstance->suggest(&suggestionsFound, word->c_str());
          }
        }

        void HandleOKCallback () {
          Nan::HandleScope scope;

          v8::Local<v8::Array> suggestions = Nan::New<v8::Array>(size);
          v8::Local<v8::Boolean> isCorrect = Nan::New<v8::Boolean>(correct);
          if(!correct) {
            for(int i = 0; i < size; i++) {
              Nan::MaybeLocal<v8::String> maybeSuggestion = Nan::New(suggestionsFound[i]);

              if(maybeSuggestion.IsEmpty()) {
                Nan::Set(suggestions, i, Nan::Undefined());
              } else {
                Nan::Set(suggestions, i, maybeSuggestion.ToLocalChecked());
              }
            }

            hunspellInstance->free_list(&suggestionsFound, size);
          }


          v8::Local<v8::Value> argv[] = {
              Nan::Null(),
              isCorrect,
              suggestions,
          };

          callback->Call(3, argv);
        }

        void Destroy() {
          delete word;
        }

      private:
        std::string *word;
        int size;
        bool correct;
        char** suggestionsFound;
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

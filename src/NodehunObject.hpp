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

    static void New(const Nan::FunctionCallbackInfo<v8::Value>& info);
};

#endif

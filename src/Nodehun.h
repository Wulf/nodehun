#ifndef Nodehun_H
#define Nodehun_H

#include <napi.h>
#include "HunspellContext.h"

class Nodehun : public Napi::ObjectWrap<Nodehun> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  static Napi::Object NewInstance(const Napi::CallbackInfo& info);
  Nodehun(const Napi::CallbackInfo& info);
  ~Nodehun();

 private:
  static Napi::FunctionReference constructor;
  HunspellContext* context;
  
  // (dictionary: Buffer) => void
  Napi::Value addDictionary(const Napi::CallbackInfo& info);
  Napi::Value addDictionarySync(const Napi::CallbackInfo& info);

  // (word: string) => boolean
  Napi::Value spell(const Napi::CallbackInfo& info);
  Napi::Value spellSync(const Napi::CallbackInfo& info);
  
  // (word: string) => string[] | null
  Napi::Value suggest(const Napi::CallbackInfo& info);
  Napi::Value suggestSync(const Napi::CallbackInfo& info);
  
  // (word: string) => string[]
  Napi::Value analyze(const Napi::CallbackInfo& info);
  Napi::Value analyzeSync (const Napi::CallbackInfo& info);

  // (word: string) => string[]
  Napi::Value stem(const Napi::CallbackInfo& info);
  Napi::Value stemSync(const Napi::CallbackInfo& info);

  // (word: string, example: string) => string[]
  Napi::Value generate(const Napi::CallbackInfo& info);
  Napi::Value generateSync(const Napi::CallbackInfo& info);

  // (word: string) => void
  Napi::Value add(const Napi::CallbackInfo& info);
  Napi::Value addSync(const Napi::CallbackInfo& info);

  // (word: string, example: string) => void
  Napi::Value addWithAffix(const Napi::CallbackInfo& info);
  Napi::Value addWithAffixSync(const Napi::CallbackInfo& info);

  // (word: string) => void
  Napi::Value remove(const Napi::CallbackInfo& info);
  Napi::Value removeSync(const Napi::CallbackInfo& info);

  // () => string | undefined
  Napi::Value getDictionaryEncoding(const Napi::CallbackInfo& info);
  // () => string | undefined
  Napi::Value getWordCharacters(const Napi::CallbackInfo& info);
  // () => string | undefined
  Napi::Value getWordCharactersUTF16(const Napi::CallbackInfo& info);
  // () => string | undefined
  Napi::Value getVersion(const Napi::CallbackInfo& info);
};

#endif
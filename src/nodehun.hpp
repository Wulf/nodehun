#ifndef NODEHUN_HPP
#define NODEHUN_HPP
#define MAX_PATH 1024
#include <node.h>
class SpellDictionary : public node::ObjectWrap {
	public:
		static v8::Persistent<v8::FunctionTemplate> constructor;
		static void Init(v8::Handle<v8::Object> target);
		SpellDictionary(const char *language);
		~SpellDictionary(){
			if (spellClass != NULL)
				delete spellClass;
			spellClass = NULL;
		};
		bool pathsExist;
		Hunspell *spellClass;
	protected:
		
		static v8::Handle<v8::Value> New(const v8::Arguments& args);
		static v8::Handle<v8::Value> spellSuggest(const v8::Arguments& args);
		static v8::Handle<v8::Value> spellSuggestions(const v8::Arguments& args);
		static v8::Handle<v8::Value> addDictionary(const v8::Arguments& args);
		static v8::Handle<v8::Value> addWord(const v8::Arguments& args);
		static v8::Handle<v8::Value> removeWord(const v8::Arguments& args);
		static void addDictionaryWork(uv_work_t* request);
		static void addDictionaryFinish(uv_work_t* request);
		static void addRemoveWordWork(uv_work_t* request);
		static void addRemoveWordFinish(uv_work_t* request);
		static void CheckSuggestions(uv_work_t* request);
		static void SendSuggestions(uv_work_t* request);
};
#endif

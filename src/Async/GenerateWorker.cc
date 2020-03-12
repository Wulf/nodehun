#include <napi.h>
#include <hunspell.hxx>
#include "Worker.cc"

class GenerateWorker : public Worker {
    public:
        GenerateWorker(
            HunspellContext* context,
            Napi::Promise::Deferred d,
            std::string word,
            std::string example)
        : Worker(context, d), word(word), example(example) {}

    void Execute() {
        // Worker thread; don't use N-API here
        context->lockRead();
        length = context->instance->generate(
            &generates,
            word.c_str(),
            example.c_str()
        );
        context->unlockRead();
    }

    void Resolve(Napi::Promise::Deferred const &deferred) {
        Napi::Env env = deferred.Env();
        
        Napi::Array array = Napi::Array::New(env, length);
        for (int i = 0; i < length; i++) {
            array.Set(i, Napi::String::New(env, generates[i]));
        }
        
        context->instance->free_list(&generates, length);

        deferred.Resolve(array);
    }

    private:
        int length = 0;
        char** generates = NULL;
        std::string word;
        std::string example;
};
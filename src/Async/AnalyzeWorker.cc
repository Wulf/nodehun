#include <napi.h>

#include <chrono>
#include <thread>
#include <hunspell.hxx>
#include "Worker.cc"

class AnalyzeWorker : public Worker {
    public:
        AnalyzeWorker(
            HunspellContext* context,
            Napi::Promise::Deferred d,
            const char* word)
        : Worker(context, d), word(word) {}

    void Execute() {
        // Worker thread; don't use N-API here
        context->lock();
        length = this->context->instance->analyze(&analysis, word);
        context->unlock();
    }

    void Resolve(Napi::Promise::Deferred const &deferred) {
        Napi::Env env = deferred.Env();
        
        Napi::Array array = Napi::Array::New(env, length);
        for (int i = 0; i < length; i++) {
            array.Set(i, Napi::String::New(env, *(analysis+i)));
        }

        deferred.Resolve(array);
    }

    private:
        int length = 0;
        const char* word;
        char** analysis;
};
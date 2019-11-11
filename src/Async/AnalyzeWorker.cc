#include <napi.h>
#include <hunspell.hxx>
#include "Worker.cc"

class AnalyzeWorker : public Worker {
    public:
        AnalyzeWorker(
            HunspellContext* context,
            Napi::Promise::Deferred d,
            std::string word)
        : Worker(context, d), word(word) {}

    void Execute() {
        // Worker thread; don't use N-API here
        context->lockRead();
        length = this->context->instance->analyze(&analysis, word.c_str());
        context->unlockRead();
    }

    void Resolve(Napi::Promise::Deferred const &deferred) {
        Napi::Env env = deferred.Env();
        
        Napi::Array array = Napi::Array::New(env, length);
        for (int i = 0; i < length; i++) {
            array.Set(i, Napi::String::New(env, analysis[i]));
        }

        context->instance->free_list(&analysis, length);

        deferred.Resolve(array);
    }

    private:
        int length = 0;
        std::string word;
        char** analysis = NULL;
};
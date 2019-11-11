#include <napi.h>
#include <hunspell.hxx>
#include "Worker.cc"

class SuggestWorker : public Worker {
    public:
        SuggestWorker(
            HunspellContext* context,
            Napi::Promise::Deferred d,
            std::string word)
        : Worker(context, d), word(word) {}

    void Execute() {
        // Worker thread; don't use N-API here
        context->lockRead();
        bool correct = context->instance->spell(word.c_str());
        if (!correct) {
            length = this->context->instance->suggest(&suggestions, word.c_str());
        }
        context->unlockRead();
    }

    void Resolve(Napi::Promise::Deferred const &deferred) {
        Napi::Env env = deferred.Env();

        if (length == -1) {
            deferred.Resolve(env.Null());
            return;
        }

        Napi::Array array = Napi::Array::New(env, length);
        for (int i = 0; i < length; i++) {
            array.Set(i, Napi::String::New(env, suggestions[i]));
        }

        context->instance->free_list(&suggestions, length);

        deferred.Resolve(array);
    }

    private:
        int length = -1;
        std::string word;
        char** suggestions = NULL;
};
#include <napi.h>
#include <hunspell.hxx>
#include "Worker.cc"

class SpellWorker : public Worker {
    public:
        SpellWorker(
            HunspellContext* context,
            Napi::Promise::Deferred d,
            std::string word)
        : Worker(context, d), word(word) {}

    void Execute() {
        // Worker thread; don't use N-API here
        context->lockRead();
        correct = context->instance->spell(word.c_str());
        context->unlockRead();
    }

    void Resolve(Napi::Promise::Deferred const &deferred) {
        Napi::Env env = deferred.Env();

        deferred.Resolve(Napi::Boolean::New(env, correct));
    }

    private:
        bool correct = false;
        std::string word;
};
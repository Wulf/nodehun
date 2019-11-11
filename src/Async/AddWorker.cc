#include <napi.h>
#include <hunspell.hxx>
#include "Worker.cc"

class AddWorker : public Worker {
    public:
        AddWorker(
            HunspellContext* context,
            Napi::Promise::Deferred d,
            std::string word)
        : Worker(context, d), word(word) {}

    void Execute() {
        // Worker thread; don't use N-API here
        context->lockWrite();
        context->instance->add(word.c_str());
        context->unlockWrite();
    }

    void Resolve(Napi::Promise::Deferred const &deferred) {
        Napi::Env env = deferred.Env();
        
        deferred.Resolve(env.Undefined());
    }

    private:
        std::string word;
};
#include <napi.h>
#include <hunspell.hxx>
#include <string>
#include "Worker.cc"

class RemoveWorker : public Worker {
    public:
        RemoveWorker(
            HunspellContext* context,
            Napi::Promise::Deferred d,
            std::string word)
        : Worker(context, d), word(std::move(word)) {}

    void Execute() {
        // Worker thread; don't use N-API here
        context->lockRead();
        context->instance->remove(word.c_str());
        context->unlockRead();
    }

    void Resolve(Napi::Promise::Deferred const &deferred) {
        Napi::Env env = deferred.Env();
        
        deferred.Resolve(env.Undefined());
    }

    private:
        std::string word;
};
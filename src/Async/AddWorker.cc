#include <napi.h>

#include <chrono>
#include <thread>
#include <hunspell.hxx>
#include "Worker.cc"

class AddWorker : public Worker {
    public:
        AddWorker(
            HunspellContext* context,
            Napi::Promise::Deferred d,
            const char* word)
        : Worker(context, d), word(word) {}

    void Execute() {
        // Worker thread; don't use N-API here
        context->lock();
        context->instance->add(word);
        context->unlock();
    }

    void Resolve(Napi::Promise::Deferred const &deferred) {
        Napi::Env env = deferred.Env();
        
        deferred.Resolve(env.Undefined());
    }

    private:
        const char* word;
};
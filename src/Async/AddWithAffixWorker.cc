#include <napi.h>

#include <chrono>
#include <thread>
#include <hunspell.hxx>
#include "Worker.cc"

class AddWithAffixWorker : public Worker {
    public:
        AddWithAffixWorker(
            HunspellContext* context,
            Napi::Promise::Deferred d,
            const char* word,
            const char* example)
        : Worker(context, d), word(word), example(example) {}

    void Execute() {
        // Worker thread; don't use N-API here
        context->lock();
        context->instance->add_with_affix(word, example);
        context->unlock();
    }

    void Resolve(Napi::Promise::Deferred const &deferred) {
        Napi::Env env = deferred.Env();

        deferred.Resolve(env.Undefined());
    }

    private:
        const char* word;
        const char* example;
};
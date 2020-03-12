#include <napi.h>
#include <hunspell.hxx>
#include "Worker.cc"

class AddWithAffixWorker : public Worker {
    public:
        AddWithAffixWorker(
            HunspellContext* context,
            Napi::Promise::Deferred d,
            std::string word,
            std::string example)
        : Worker(context, d), word(word), example(example) {}

    void Execute() {
        // Worker thread; don't use N-API here
        context->lockWrite();
        context->instance->add_with_affix(word.c_str(), example.c_str());
        context->unlockWrite();
    }

    void Resolve(Napi::Promise::Deferred const &deferred) {
        Napi::Env env = deferred.Env();

        deferred.Resolve(env.Undefined());
    }

    private:
        std::string word;
        std::string example;
};
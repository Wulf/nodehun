#include <napi.h>
#include <hunspell.hxx>
#include "Worker.cc"

class AddDictionaryWorker : public Worker {
    public:
        AddDictionaryWorker(
            HunspellContext* context,
            Napi::Promise::Deferred d,
            const char* dictionary)
        : Worker(context, d), dictionary(dictionary) {}

    void Execute() {
        // Worker thread; don't use N-API here
        context->lockWrite();
        context->instance->add_dic(dictionary);
        context->unlockWrite();
    }

    void Resolve(Napi::Promise::Deferred const &deferred) {
        Napi::Env env = deferred.Env();
        
        deferred.Resolve(env.Undefined());
    }

    private:
        const char* dictionary;
};
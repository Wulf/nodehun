#ifndef Worker_cc
#define Worker_cc

// Thanks @jaubourg, @Superlokkus, and @greg9504.
// https://github.com/nodejs/node-addon-api/issues/231
#include <napi.h>
#include "../HunspellContext.h"

class Worker : public Napi::AsyncWorker {
public:
    Worker(HunspellContext* context, Napi::Promise::Deferred const &d, const char* resource_name) : AsyncWorker(get_fake_callback(d.Env()).Value(), resource_name), deferred(d), context(context) {}
    Worker(HunspellContext* context, Napi::Promise::Deferred const &d) : AsyncWorker(get_fake_callback(d.Env()).Value()), deferred(d), context(context) {}

    virtual void Resolve(Napi::Promise::Deferred const &deferred) = 0;

    void OnOK() override {
        Resolve(deferred);
    }

    void OnError(Napi::Error const &error) override {
        deferred.Reject(error.Value());
    }

protected:
    HunspellContext* context;
    
private:
    static Napi::Value noop(Napi::CallbackInfo const &info) {
        return info.Env().Undefined();
    }

    Napi::Reference<Napi::Function> const &get_fake_callback(Napi::Env const &env) {
        static Napi::Reference<Napi::Function> fake_callback
                = Napi::Reference<Napi::Function>::New(Napi::Function::New(env, noop), 1);
        fake_callback.SuppressDestruct();

        return fake_callback;
    }

    Napi::Promise::Deferred deferred;
};

#endif
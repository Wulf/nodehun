#ifndef HunspellContext_H
#define HunspellContext_H

#include <hunspell.hxx>
#include <napi.h>
#include <mutex>

class HunspellContext {
 public:
  Hunspell* instance;
 
  HunspellContext(Hunspell* instance): instance(instance) {};

  ~HunspellContext() {
      delete instance;
  }

  void lock() {
    asyncLock.lock();
  }

  void unlock() {
    asyncLock.unlock();
  }

 private:
  /*
   * The Hunspell instance is not thread safe, so we use a mutex
   * to manage asynchronous usage.
   */
  std::mutex asyncLock;
};

#endif
#ifndef HunspellContext_H
#define HunspellContext_H

#include <hunspell.hxx>
#include <napi.h>
#include <mutex>
#include <shared_mutex>

class HunspellContext {
 public:
  Hunspell* instance;
 
  HunspellContext(Hunspell* instance): instance(instance) {};

  ~HunspellContext() {
      if (instance) {
        delete instance;
        instance = NULL;
      }
  }

  void lockRead() {
    rwLock.lock_shared();
  }

  void unlockRead() {
    rwLock.unlock_shared();
  }

  void lockWrite() {
    rwLock.lock();
  }

  void unlockWrite() {
    rwLock.unlock();
  }

 private:
  /*
   * The Hunspell instance is not thread safe, so we use a mutex
   * to manage asynchronous usage.
   */
  std::shared_mutex rwLock;
};

#endif
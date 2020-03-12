#ifndef HunspellContext_H
#define HunspellContext_H

#include <hunspell.hxx>
#include <napi.h>
#include <mutex>
#include <shared_mutex>
#include <uv.h>

class HunspellContext {
 public:
  Hunspell* instance;
 
  HunspellContext(Hunspell* instance): instance(instance) {
    uv_rwlock_init(&rwLock);
  };

  ~HunspellContext() {
      if (instance) {
        delete instance;
        instance = NULL;
      }

      uv_rwlock_destroy(&rwLock);
  }

  void lockRead() {
    uv_rwlock_rdlock(&rwLock);
  }

  void unlockRead() {
    uv_rwlock_rdunlock(&rwLock);
  }

  void lockWrite() {
    uv_rwlock_wrlock(&rwLock);
  }

  void unlockWrite() {
    uv_rwlock_wrunlock(&rwLock);
  }

 private:
  /*
   * The Hunspell instance is not thread safe, so we use a mutex
   * to manage asynchronous usage.
   */
  uv_rwlock_t rwLock;
};

#endif
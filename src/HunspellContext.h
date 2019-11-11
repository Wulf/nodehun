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

  void lockRead() {
    readLock.lock();
  }

  void unlockRead() {
    readLock.unlock();
  }

  void lockWrite() {
    writeLock.lock();
  }

  void unlockWrite() {
    writeLock.unlock();
  }

 private:
  /*
   * The Hunspell instance is not thread safe, so we use a mutex
   * to manage asynchronous usage.
   */
  std::mutex writeLock;
  std::mutex readLock;
};

#endif
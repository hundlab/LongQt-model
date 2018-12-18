#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <iterator>
#include <list>
#include <mutex>
#include <queue>
#include <thread>
#include <tuple>
#include <vector>

namespace LongQt {

class ThreadPool {
  struct Runable {
    Runable(std::function<void(void)> fn) : fn(fn) {}
    Runable() : exists(false) {}
    bool exists = true;
    std::function<void(void)> fn;
  };

  std::atomic<int> idle = 0;
  std::mutex jobs_mtx;
  std::condition_variable more_jobs;
  std::queue<Runable> jobs;
  //  std::mutex threads_mtx;
  //  std::list<std::thread> threads;

  int numthreads = 0;
  int maxthreads = 0;

  std::function<void(void)> __finished;

  void thread_main();

 public:
  ThreadPool(int maxthreads = -1);
  ~ThreadPool();
  void resize(int maxthreads);
  int idleThreads();
  void clear();

  template <class Fn, class... Args>
  void push(Fn &&fn, Args &&... args);

  template <class Fn, class InputIterator>
  void pushAll(Fn &&fn, InputIterator begin, InputIterator end);

  void finishedCallback(std::function<void(void)> fn);
};

template <class Fn, class... Args>
void ThreadPool::push(Fn &&fn, Args &&... args) {
  Runable run(std::bind(fn, args...));
  std::lock_guard<std::mutex> jobs_lock(jobs_mtx);
  jobs.push(run);
  more_jobs.notify_all();
  for (int i = 0; i < this->jobs.size() && this->numthreads < this->maxthreads;
       i++) {
    std::thread(&ThreadPool::thread_main, this).detach();
    this->numthreads++;
  }
}

template <class Fn, class InputIterator>
void ThreadPool::pushAll(Fn &&fn, InputIterator begin, InputIterator end) {
  std::lock_guard<std::mutex> jobs_lock(jobs_mtx);
  for (auto it = begin; it != end; it++) {
    Runable run(std::bind(fn, *it));
    jobs.push(run);
  }
  more_jobs.notify_all();
  for (int i = 0; i < this->jobs.size() && this->numthreads < this->maxthreads;
       i++) {
    std::thread(&ThreadPool::thread_main, this).detach();
    this->numthreads++;
  }
}
}  // namespace LongQt
#endif  // THREADPOOL_H

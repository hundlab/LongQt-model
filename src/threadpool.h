#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <iterator>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <tuple>
#include <vector>

namespace LongQt {

class ThreadPool {
  class Thread;

  struct Shared {
    std::atomic<int> idle = 0;
    std::mutex jobs_mtx;
    std::mutex threads_mtx;
    std::condition_variable_any more_jobs;
    std::condition_variable_any finished_jobs;
    std::queue<std::function<void(void)>> jobs;
    std::list<std::shared_ptr<Thread>> threads;
    std::function<void(void)> __finished;
    int numthreads() const;
  } shared;

  int clean_threads();

  int maxthreads = std::thread::hardware_concurrency();

  class Thread {
   public:
    Thread(ThreadPool *p);
    ~Thread();
    std::shared_future<void> finished;
    std::atomic<bool> kill = false;
    void main();

   private:
    void signal_jobs_finished();
    void work_loop(std::unique_lock<std::mutex> &jobs_lck, std::unique_lock<std::mutex> &th_lck);
    decltype(shared) *d;
    std::promise<void> __finished;
    //    std::atomic<int> *numthreads;
  };

  void createThreads();
  void deleteThreads(int num);

 public:
  ThreadPool(int maxthreads = -1);
  ~ThreadPool();
  void resize(int maxthreads);
  int numThreads();
  int idleThreads();
  void clear();
  void wait();

  template <class Fn, class... Args>
  void push(Fn &&fn, Args &&... args);

  template <class Fn, class InputIterator>
  void pushAll(Fn &&fn, InputIterator begin, InputIterator end);

  template <class Fn, class InputIterator>
  void pushAllpnt(Fn &&fn, InputIterator begin, InputIterator end);

  void finishedCallback(std::function<void(void)> fn);
};

template <class Fn, class... Args>
void ThreadPool::push(Fn &&fn, Args &&... args) {
  std::lock_guard<std::mutex> jobs_lock(shared.jobs_mtx);
  shared.jobs.push(std::bind(fn, args...));
  shared.more_jobs.notify_all();
  std::lock_guard<std::mutex> th_lck(shared.threads_mtx);
  this->createThreads();
}

template <class Fn, class InputIterator>
void ThreadPool::pushAll(Fn &&fn, InputIterator begin, InputIterator end) {
  std::lock_guard<std::mutex> jobs_lock(shared.jobs_mtx);
  for (auto it = begin; it != end; it++) {
    shared.jobs.push(std::bind(fn, *it));
  }
  shared.more_jobs.notify_all();
  std::lock_guard<std::mutex> th_lock(shared.threads_mtx);
  this->createThreads();
}

template <class Fn, class InputIterator>
void ThreadPool::pushAllpnt(Fn &&fn, InputIterator begin, InputIterator end) {
  std::lock_guard<std::mutex> jobs_lock(shared.jobs_mtx);
  for (auto it = begin; it != end; it++) {
      auto temp = &(*it);
    shared.jobs.push([fn,temp] () {fn(*temp);});
  }
  shared.more_jobs.notify_all();
  std::lock_guard<std::mutex> th_lock(shared.threads_mtx);
  this->createThreads();
}
}  // namespace LongQt
#endif  // THREADPOOL_H

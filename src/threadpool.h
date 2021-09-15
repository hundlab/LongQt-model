#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <iterator>
#include <list>
#include <map>
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
    std::atomic<int> working = 0;
    std::mutex jobs_mtx;
    std::condition_variable_any more_jobs;
    std::condition_variable_any finished_jobs;
    std::queue<std::pair<int,std::function<void(void)>>> jobs; //jobs_mtx
    std::map<int,int> n_jobs_unfinished; //jobs_mtx
    std::function<void(void)> __finishedCallback; //jobs_mtx
  } shared;

  class Thread {
   public:
    Thread(Shared* shared);
    ~Thread();
    std::shared_future<void> finished;
    std::atomic<bool> kill = false;
    void main();

  private:
    decltype(shared) *d;
    std::promise<void> __finished;
    std::thread thread;
    void work_loop();
    std::pair<int,std::function<void(void)>> findWork(std::unique_lock<std::mutex>& jobs_lck, bool release_job_lock = true);
    int work(std::function<void(void)> fn);
    void finishWork(std::unique_lock<std::mutex>& jobs_lck, int id, bool release_job_lock = true);
    void signal_jobs_finished(std::unique_lock<std::mutex>& jobs_lck,  bool release_job_lock = true);
  };

  int nextJobID = 0;
  int maxthreads = std::thread::hardware_concurrency();
  std::list<std::shared_ptr<Thread>> threads;
  void createThreads();
  void deleteThreads(int num);
  int clean_threads();
  int numthreads() const;

 public:
  ThreadPool(int maxthreads = -1);
  ~ThreadPool();
  void resize(int maxthreads);
  int numThreads();
  int idleThreads();
  void clear();
  bool wait_for(std::chrono::seconds dur);
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
  int job_id = (this->nextJobID)++;
  shared.jobs.push(std::make_pair(job_id, std::bind(fn, args...)));
  shared.n_jobs_unfinished[job_id] = 1;
  shared.more_jobs.notify_one();
  this->createThreads();
}

template <class Fn, class InputIterator>
void ThreadPool::pushAll(Fn &&fn, InputIterator begin, InputIterator end) {
  std::lock_guard<std::mutex> jobs_lock(shared.jobs_mtx);
  int job_id = (this->nextJobID)++;
  int n_work = 0;
  for (auto it = begin; it != end; it++) {
    shared.jobs.push(std::make_pair(job_id, std::bind(fn, *it)));
    n_work++;
  }
  shared.n_jobs_unfinished[job_id] = n_work;
  shared.more_jobs.notify_all();
  this->createThreads();
}

template <class Fn, class InputIterator>
void ThreadPool::pushAllpnt(Fn &&fn, InputIterator begin, InputIterator end) {
  std::lock_guard<std::mutex> jobs_lock(shared.jobs_mtx);
  int job_id = (this->nextJobID)++;
  int n_work = 0;
  for (auto it = begin; it != end; it++) {
    auto temp = &(*it);
    shared.jobs.push(std::make_pair(job_id, [fn,temp] () {fn(*temp);}));
    n_work++;
  }
  shared.more_jobs.notify_all();
  this->createThreads();
}
}  // namespace LongQt
#endif  // THREADPOOL_H

#include "threadpool.h"

using namespace LongQt;

void ThreadPool::thread_main() {
  Runable run;
  std::unique_lock<std::mutex> jobs_lck(this->jobs_mtx);
  jobs_lck.unlock();
  while (true) {
    int curr_idle = this->idle.fetch_add(1) + 1;
    jobs_lck.lock();
    while (this->jobs.empty()) {
      if (curr_idle == this->numthreads) {
        try {
          this->__finished();
        } catch (std::bad_function_call&) {
        }
      }
      this->more_jobs.wait(jobs_lck);
    }
    run = this->jobs.front();
    this->jobs.pop();
    jobs_lck.unlock();
    this->idle--;
    if (run.exists) {
      run.fn();
    } else {
      return;
    }
  }
}

ThreadPool::ThreadPool(int maxthreads) {
  this->maxthreads =
      maxthreads > 0 ? maxthreads : std::thread::hardware_concurrency();
  this->maxthreads = this->maxthreads > 0 ? this->maxthreads : 1;
}

ThreadPool::~ThreadPool() {
  std::lock_guard<std::mutex> jobs_lock(jobs_mtx);
  for (int i = 0; i < this->numthreads; i++) {
    jobs.push(Runable());
  }
}

void ThreadPool::resize(int maxthreads) {
  std::lock_guard<std::mutex> jobs_lock(jobs_mtx);
  if (maxthreads >= 0) {
    this->maxthreads = maxthreads;
  }
  while (this->numthreads > this->maxthreads) {
    jobs.push(Runable());
    this->numthreads--;
  }
  for (int i = 0; i < this->jobs.size() && this->numthreads <= this->maxthreads;
       i++) {
    std::thread(&ThreadPool::thread_main, this).detach();
    this->numthreads++;
  }
}

int ThreadPool::idleThreads() { return this->idle; }

void ThreadPool::finishedCallback(std::function<void(void)> fn) {
  this->__finished = fn;
}

void ThreadPool::clear()
{
  std::lock_guard<std::mutex> jobs_lock(jobs_mtx);
  decltype(this->jobs) empty;
  this->jobs.swap(empty);
}

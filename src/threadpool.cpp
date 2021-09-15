#include "threadpool.h"
#include <assert.h>
#include <iostream>
#include "logger.h"

using namespace LongQt;

ThreadPool::Thread::Thread(Shared* shared) : d(shared) {
  this->finished = this->__finished.get_future();
  this->thread = std::thread(&Thread::main, this);
  this->thread.detach();
}

ThreadPool::Thread::~Thread() {
  this->kill = true;
  this->d->more_jobs.notify_all();
  this->finished.wait();
}

void ThreadPool::Thread::main() {
  this->work_loop();
  this->__finished.set_value_at_thread_exit();
}

void ThreadPool::Thread::work_loop() {
  std::unique_lock<std::mutex> jobs_lck(d->jobs_mtx, std::defer_lock);

  while (true) {
      try {
          auto[id, fn] = this->findWork(jobs_lck);

          this->work(fn);

          this->finishWork(jobs_lck, id, false);
      }  catch (std::exception) {
          return;
      }
  }
}

std::pair<int,std::function<void(void)>> ThreadPool::Thread::findWork(std::unique_lock<std::mutex>& jobs_lck, bool release_job_lock) {
    if(!jobs_lck.owns_lock())
        jobs_lck.lock();

    while (d->jobs.empty()) {
      if (this->kill) {
        throw std::exception();
      }
      d->more_jobs.wait(jobs_lck);
    }
    auto job = d->jobs.front();
    d->jobs.pop();

    if(release_job_lock)
        jobs_lck.unlock();
     return job;
}

int ThreadPool::Thread::work(std::function<void(void)> fn) {
    ++(d->working);
    try {
      fn();
    } catch(std::exception& e) {
        Logger::getInstance()->write("Threadpool: Exception thrown while "
                                     "running worker: {}", e.what());
    }
    int n_working = --(d->working);
    return n_working;
}

void ThreadPool::Thread::finishWork(std::unique_lock<std::mutex>& jobs_lck, int id, bool release_job_lock) {
    if(!jobs_lck.owns_lock())
        jobs_lck.lock();
    try {
        int n_left = --(d->n_jobs_unfinished.at(id));
        if(n_left == 0) {
            this->signal_jobs_finished(jobs_lck, false);
            d->n_jobs_unfinished.erase(id);
        }
    }  catch (std::out_of_range) {
        Logger::getInstance()->write("ThreadPool: Job ID out of range");
    }
    if(release_job_lock)
        jobs_lck.unlock();
}

void ThreadPool::Thread::signal_jobs_finished(std::unique_lock<std::mutex>& jobs_lck,  bool release_job_lock) {
  if(!jobs_lck.owns_lock())
    jobs_lck.lock();
  d->finished_jobs.notify_all();
  try {
    d->__finishedCallback();
  } catch (std::bad_function_call&) {
  }
  if(release_job_lock)
      jobs_lck.unlock();
}

void ThreadPool::createThreads() {
  int num =
      std::min((int)shared.jobs.size(), this->maxthreads) - this->clean_threads();
  for (int i = 0; i < num; i++) {
    this->threads.push_back(std::make_shared<Thread>(&this->shared));
  }
  this->clean_threads();
}

void ThreadPool::deleteThreads(int num) {
  int n_threads = this->threads.size();
  int n_after_clean = this->clean_threads();
  int n_cleaned = n_threads - n_after_clean;
  int count = std::max(num - n_cleaned, 0);
  auto it = this->threads.begin();
  const auto end = this->threads.end();
  while (count > 0 && it != end) {
    (*it)->kill = true;
    --count;
    ++it;
  }
  shared.more_jobs.notify_all();
}

ThreadPool::ThreadPool(int maxthreads) {
  if (maxthreads >= 1) {
      this->maxthreads = maxthreads;
  }
  else {
      this->maxthreads = std::thread::hardware_concurrency();
  }
  if (this->maxthreads < 1) this->maxthreads = 1;
}

ThreadPool::~ThreadPool() {
  this->deleteThreads(this->numthreads());
}

void ThreadPool::resize(int maxthreads) {
  if (maxthreads > 0) {
    this->maxthreads = maxthreads;
  }
  std::lock_guard<std::mutex> jobs_lck(shared.jobs_mtx);
  int change = this->maxthreads - this->clean_threads();
  if (change > 0) {
    this->createThreads();
  } else {
    this->deleteThreads(-change);
  }
}

int ThreadPool::numThreads() { return this->maxthreads; }

int ThreadPool::idleThreads() {
  return this->maxthreads - shared.working;
}

void ThreadPool::finishedCallback(std::function<void(void)> fn) {
  std::lock_guard(shared.jobs_mtx);
  shared.__finishedCallback = fn;
}

void ThreadPool::clear() {
  std::lock_guard<std::mutex> jobs_lock(shared.jobs_mtx);
  decltype(shared.jobs) empty;
  shared.jobs.swap(empty);
}

void ThreadPool::wait() {
  std::unique_lock<std::mutex> jobs_lck(shared.jobs_mtx);
  while (shared.working > 0 || shared.jobs.size() > 0) {
    shared.finished_jobs.wait_for(jobs_lck, std::chrono::microseconds(500));
  }
}

bool ThreadPool::wait_for(std::chrono::seconds dur) {
    std::unique_lock<std::mutex> jobs_lck(shared.jobs_mtx);
    if (shared.working > 0 || shared.jobs.size() > 0) {
      shared.finished_jobs.wait_for(jobs_lck, dur);
    }
    return !(shared.working > 0 || shared.jobs.size() > 0);
}

int LongQt::ThreadPool::clean_threads() {
  this->threads.remove_if([](auto th) {
    if (th->finished.valid()) {
      auto status = th->finished.wait_for(std::chrono::seconds(0));
      return status == std::future_status::ready;
    }
    return false;
  });
  return this->threads.size();
}

int ThreadPool::numthreads() const {
  int num = 0;
  for (auto& th : this->threads) {
    if (th->finished.valid()) {
      auto status = th->finished.wait_for(std::chrono::seconds(0));
      if (status != std::future_status::ready) {
        ++num;
      }
    }
  }
  return num;
}

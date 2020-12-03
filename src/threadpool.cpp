#include "threadpool.h"
#include <assert.h>
#include <iostream>
#include "logger.h"

using namespace LongQt;

ThreadPool::Thread::Thread(ThreadPool* p) : d(&p->shared) {
  this->finished = this->__finished.get_future();
  std::thread(&Thread::main, this).detach();
}

ThreadPool::Thread::~Thread() {
  this->kill = true;
  this->d->more_jobs.notify_all();
  this->finished.wait();
}

void ThreadPool::Thread::main() {
  std::unique_lock<std::mutex> jobs_lck(d->jobs_mtx);
  std::unique_lock<std::mutex> th_lck(d->threads_mtx);
  this->work_loop(jobs_lck, th_lck);
  th_lck.lock();
  this->__finished.set_value_at_thread_exit();
}

void ThreadPool::Thread::work_loop(std::unique_lock<std::mutex>& jobs_lck,
                                   std::unique_lock<std::mutex>& th_lck) {
  jobs_lck.unlock();
  th_lck.unlock();
  while (true) {
//    if (this->kill) return;
    int curr_idle = ++(d->idle);
    jobs_lck.lock();
    th_lck.lock();
    if (d->jobs.empty() && curr_idle == d->numthreads()) {
      this->signal_jobs_finished();
    }
    th_lck.unlock();
    while (d->jobs.empty()) {
      if (this->kill) {
        --(d->idle);
        return;
      }
      d->more_jobs.wait(jobs_lck);
    }
    auto fn = d->jobs.front();
    d->jobs.pop();
    jobs_lck.unlock();
    --(d->idle);
    try {
      fn();
    } catch(std::exception& e) {
        Logger::getInstance()->write("Threadpool: Exception thrown while "
                                     "running worker: {}", e.what());
    }
  }
}

void ThreadPool::Thread::signal_jobs_finished() {
  d->finished_jobs.notify_all();
  try {
    d->__finished();
  } catch (std::bad_function_call&) {
  }
}

// th_mtx && jobs_mtx
void ThreadPool::createThreads() {
  int num =
      std::min((int)shared.jobs.size(), this->maxthreads) - shared.numthreads();
  for (int i = 0; i < num; i++) {
    shared.threads.push_back(std::make_shared<Thread>(this));
  }
  this->clean_threads();
}

// th_mtx
void ThreadPool::deleteThreads(int num) {
  if (num > this->clean_threads()) num = shared.numthreads();
  int count = 0;
  auto it = shared.threads.begin();
  const auto end = shared.threads.end();
  while (count < num && it != end) {
    (*it)->kill = true;
    ++count;
    ++it;
  }
  shared.more_jobs.notify_all();
}

ThreadPool::ThreadPool(int maxthreads) {
  if (this->maxthreads < 1) this->maxthreads = 1;
  if (maxthreads >= 1) this->maxthreads = maxthreads;
}

ThreadPool::~ThreadPool() {
  std::lock_guard<std::mutex> th_lck(shared.threads_mtx);
  this->deleteThreads(shared.numthreads());
}

void ThreadPool::resize(int maxthreads) {
  if (maxthreads > 0) {
    this->maxthreads = maxthreads;
  }
  std::lock_guard<std::mutex> jobs_lck(shared.jobs_mtx);
  std::lock_guard<std::mutex> th_lck(shared.threads_mtx);
  int change = this->maxthreads - this->clean_threads();
  if (change > 0) {
    this->createThreads();
  } else {
    this->deleteThreads(-change);
  }
}

int ThreadPool::numThreads() { return this->maxthreads; }

int ThreadPool::idleThreads() {
  std::lock_guard<std::mutex> th_lck(shared.threads_mtx);
  return shared.idle + (this->maxthreads - shared.numthreads());
}

void ThreadPool::finishedCallback(std::function<void(void)> fn) {
  shared.__finished = fn;
}

void ThreadPool::clear() {
  std::lock_guard<std::mutex> jobs_lock(shared.jobs_mtx);
  decltype(shared.jobs) empty;
  shared.jobs.swap(empty);
}

void ThreadPool::wait() {
  std::unique_lock<std::mutex> jobs_lck(shared.jobs_mtx);
  std::unique_lock<std::mutex> th_lck(shared.threads_mtx);
  while (this->clean_threads() > shared.idle || shared.jobs.size() > 0) {
    th_lck.unlock();
    shared.finished_jobs.wait_for(jobs_lck, std::chrono::milliseconds(500));
    th_lck.lock();
  }
}

// th_mtx
int LongQt::ThreadPool::clean_threads() {
  shared.threads.remove_if([](auto th) {
    if (th->finished.valid()) {
      auto status = th->finished.wait_for(std::chrono::seconds(0));
      return status == std::future_status::ready;
    }
    return false;
  });
  return shared.threads.size();
}

// th_mtx
int ThreadPool::Shared::numthreads() const {
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

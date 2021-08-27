#ifndef THREADPOOL_TEST
#define THREADPOOL_TEST
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <thread>
#include "threadpool.h"

using namespace testing;
using namespace LongQt;

TEST(threadpool, construct_neg) {
  ThreadPool pool(-10);
  EXPECT_GT(pool.numThreads(), 0);
}

TEST(threadpool, construct_pos) {
  ThreadPool pool(30);
  EXPECT_EQ(pool.numThreads(), 30);
}

TEST(threadpool, construct_zero) {
  ThreadPool pool(0);
  EXPECT_GT(pool.numThreads(), 0);
}

TEST(threadpool, wait_none) {
  ThreadPool pool;
  pool.wait();
  EXPECT_EQ(pool.idleThreads(), pool.numThreads());
}

TEST(threadpool, wait) {
  ThreadPool pool;
  pool.push([]() {});
  pool.wait();
  EXPECT_EQ(pool.idleThreads(), pool.numThreads());
}

TEST(threadpool, clear) {
  ThreadPool pool(5);
  std::atomic<int> to_set_a = 0;
  std::atomic<int> to_set_b = 0;
  std::mutex mtx;
  std::condition_variable cv;
  auto fn = [&mtx, &cv, &to_set_a, &to_set_b]() {
    to_set_a++;
    std::unique_lock<std::mutex> lck(mtx);
    cv.wait(lck);
    to_set_b++;
  };
  for (int i = 0; i < 10; i++) {
    pool.push(fn);
  }
  int counter = 0;
  for (; counter < 5 && to_set_a < 5; counter++) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  EXPECT_LT(counter, 5);
  pool.clear();
  cv.notify_all();
  pool.wait();
  EXPECT_EQ(pool.idleThreads(), pool.numThreads());
  EXPECT_EQ(to_set_b, 5);
}

TEST(threadpool, resize_smaller) {
  ThreadPool pool(10);
  std::atomic<int> to_set_b = 0;
  std::mutex mtx;
  std::condition_variable cv;
  auto fn = [&mtx, &cv, &to_set_b]() { to_set_b++; };
  for (int i = 0; i < 10; i++) {
    pool.push(fn);
  }
  pool.resize(5);
  pool.wait();
  EXPECT_EQ(pool.idleThreads(), 5);
  EXPECT_EQ(pool.numThreads(), 5);
  EXPECT_EQ(to_set_b, 10);
}

TEST(threadpool, resize_smaller_multiple) {
  ThreadPool pool(10);
  std::atomic<int> to_set_b = 0;
  std::mutex mtx;
  std::condition_variable cv;
  auto fn = [&mtx, &cv, &to_set_b]() {
	  to_set_b++;
	  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  };
  for (int i = 0; i < 10; i++) {
    pool.push(fn);
  }
  pool.resize(7);
  pool.resize(7);
  pool.resize(7);
  pool.resize(7);
  pool.wait();
  EXPECT_EQ(pool.idleThreads(), 7);
  EXPECT_EQ(pool.numThreads(), 7);
  EXPECT_EQ(to_set_b, 10);
}

TEST(threadpool, resize_larger) {
  ThreadPool pool(5);
  std::atomic<int> to_set_b = 0;
  std::mutex mtx;
  std::condition_variable cv;
  auto fn = [&mtx, &cv, &to_set_b]() { to_set_b++; };
  for (int i = 0; i < 10; i++) {
    pool.push(fn);
  }
  pool.resize(7);
  pool.wait();
  EXPECT_EQ(pool.idleThreads(), 7);
  EXPECT_EQ(pool.numThreads(), 7);
  EXPECT_EQ(to_set_b, 10);
}

TEST(threadpool, resize_larger_multiple) {
  ThreadPool pool(1);
  std::atomic<int> to_set_b = 0;
  std::mutex mtx;
  std::condition_variable cv;
  auto fn = [&mtx, &cv, &to_set_b]() { to_set_b++; };
  for (int i = 0; i < 10; i++) {
    pool.push(fn);
  }
  pool.resize(7);
  pool.resize(7);
  pool.resize(7);
  pool.resize(7);
  pool.wait();
  EXPECT_EQ(pool.idleThreads(), 7);
  EXPECT_EQ(pool.numThreads(), 7);
  EXPECT_EQ(to_set_b, 10);
}

TEST(threadpool, finishedCallback) {
  ThreadPool pool;
  std::atomic<int> to_set = 0;
  pool.finishedCallback([&to_set]() { ++to_set; });
  pool.push([]() {});
  pool.wait();
  EXPECT_EQ(pool.idleThreads(), pool.numThreads());
  EXPECT_EQ(to_set, 1);
}

TEST(threadpool, finishedCallback_resize_gt) {
  ThreadPool pool(2);
  std::atomic<int> to_set = 0;
  pool.finishedCallback([&to_set]() { ++to_set; });
  auto fn = []() {};
  for (int i = 0; i < 10; ++i) {
    pool.push(fn);
  }
  pool.resize(7);
  pool.wait();
  EXPECT_EQ(pool.idleThreads(), pool.numThreads());
  EXPECT_EQ(to_set, 10);
}

TEST(threadpool, finishedCallback_resize_lt) {
  ThreadPool pool(7);
  std::atomic<int> to_set = 0;
  pool.finishedCallback([&to_set]() { ++to_set; });
  auto fn = []() {};
  for (int i = 0; i < 10; ++i) {
    pool.push(fn);
  }
  pool.wait();
  pool.resize(6);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  pool.resize(5);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  pool.resize(4);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  pool.resize(3);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  pool.resize(2);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  pool.resize(1);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  EXPECT_EQ(pool.idleThreads(), pool.numThreads());
  EXPECT_EQ(to_set, 10);
}

TEST(threadpool, finishedCallback_mult) {
  ThreadPool pool(2);
  std::atomic<int> to_set = 0;
  pool.finishedCallback([&to_set]() { ++to_set; });
  std::vector<int> args;
  auto fn = [](int) {};
  for (int i = 0; i < 10; ++i) {
    args.push_back(1);
  }
  pool.pushAll(fn, args.begin(), args.end());
  pool.wait();
  pool.pushAll(fn, args.begin(), args.end());
  pool.wait();
  EXPECT_EQ(pool.idleThreads(), pool.numThreads());
  EXPECT_EQ(to_set, 2);
}

TEST(threadpool, push_no_args) {
  ThreadPool pool;
  std::atomic<bool> to_set = false;
  pool.push([&to_set]() { to_set = true; });
  pool.wait();
  EXPECT_EQ(pool.idleThreads(), pool.numThreads());
  EXPECT_TRUE(to_set);
}

TEST(threadpool, push_args) {
  ThreadPool pool;
  std::atomic<int> to_set = 0;
  auto fn = [](auto *to_set) { (*to_set)++; };
  for (int i = 0; i < 5; i++) {
    pool.push(fn, &to_set);
  }
  pool.wait();
  EXPECT_EQ(pool.idleThreads(), pool.numThreads());
  EXPECT_EQ(to_set, 5);
}

#endif  // THREADPOOL_TEST

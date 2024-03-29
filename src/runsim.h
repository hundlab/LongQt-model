#ifndef RUNSIM_H
#define RUNSIM_H

#include "protocol.h"
#include "threadpool.h"
#include <vector>
#include <chrono>

namespace LongQt {

class RunSim {
 public:
  RunSim();
  RunSim(std::shared_ptr<Protocol> proto);
  RunSim(std::vector<std::shared_ptr<Protocol>> protoList);

  ~RunSim();

  void run();
  void cancel();
  bool finished();
  std::pair<double,double> progressRange();
  double progress();
  void finishedCallback(std::function<void(void)> fn);
  void startCallback(std::function<void(void)> fn);
  bool wait_for(std::chrono::seconds dur);
  void wait();

  int numThreads();
  void numThreads(int maxthreads);

  void setSims(std::shared_ptr<Protocol> proto);
  void setSims(std::vector<std::shared_ptr<Protocol>> protoList);
  void appendSims(std::shared_ptr<Protocol> proto);
  void appendSims(std::vector<std::shared_ptr<Protocol>> protoList);
  void clear();
//  QFuture<void>& getFuture();

 private:
  void finishedCallbackFn();
  void startedCallbackFn();
//  QFuture<void> next;
  std::vector<std::shared_ptr<Protocol>> simulations;
  ThreadPool pool;
  bool __finished = true;
  std::function<void(void)> __finishedCall;
  std::function<void(void)> __startCall;
};
}  // namespace LongQt

#endif  // RUNSIM_H

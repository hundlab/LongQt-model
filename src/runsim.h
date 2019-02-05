#ifndef RUNSIM_H
#define RUNSIM_H

#include "protocol.h"
#include "threadpool.h"
#include <vector>

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
  void wait();

  void setSims(std::shared_ptr<Protocol> proto);
  void setSims(std::vector<std::shared_ptr<Protocol>> protoList);
  void appendSims(std::shared_ptr<Protocol> proto);
  void appendSims(std::vector<std::shared_ptr<Protocol>> protoList);
  void clear();
//  QFuture<void>& getFuture();

 private:
  void poolCallback();
//  QFuture<void> next;
  std::vector<std::shared_ptr<Protocol>> simulations;
  ThreadPool pool;
  bool __finished = true;
  std::function<void(void)> __finishedCall;
  std::function<void(void)> __startCall;
};
}  // namespace LongQt

#endif  // RUNSIM_H

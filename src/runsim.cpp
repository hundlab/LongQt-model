#include "runsim.h"
#include <QThreadPool>
#include <QtConcurrent>
using namespace LongQt;
using namespace std;

RunSim::RunSim() {
  this->pool.finishedCallback(std::bind(&RunSim::poolCallback, this));
}

RunSim::RunSim(shared_ptr<Protocol> proto) {
  this->setSims(proto);
  this->pool.finishedCallback(std::bind(&RunSim::poolCallback, this));
}

RunSim::RunSim(std::vector<shared_ptr<Protocol>> protoList) {
  this->setSims(protoList);
  this->pool.finishedCallback(std::bind(&RunSim::poolCallback, this));
}

RunSim::~RunSim() {}

void RunSim::appendSims(shared_ptr<Protocol> proto) {
  int i = 0;
  proto->mkDirs();
  simulations.clear();

  for (i = 0; i < proto->numtrials; i++) {
    proto->trial(i);
    simulations.push_back(shared_ptr<Protocol>(proto->clone()));
  }
}

void RunSim::appendSims(std::vector<shared_ptr<Protocol>> protoList) {
  for (auto& proto : protoList) {
    proto->mkDirs();
    simulations.push_back(proto);
  }
}

void RunSim::setSims(shared_ptr<Protocol> proto) {
  simulations.clear();
  this->appendSims(proto);
}

void RunSim::setSims(std::vector<shared_ptr<Protocol>> protoList) {
  simulations.clear();
  this->appendSims(protoList);
}

void RunSim::clear() { simulations.clear(); }

void RunSim::poolCallback() {
  this->__finished = true;
  try {
    this->__finishedCall();
  } catch (std::bad_function_call&) {
  }
}

// QFuture<void>& RunSim::getFuture() { return this->next; }

bool RunSim::finished() { return this->__finished; /*next.isFinished();*/ }

std::pair<double, double> RunSim::progressRange() {
  double progressTotal = 0;
  for (auto& p : this->simulations) {
    progressTotal += p->tMax;
  }
  return {0, progressTotal};
}

double RunSim::progress() {
  //  qInfo() << "Threads: " <<
  //  QThreadPool::globalInstance()->activeThreadCount()
  //          << '/' << QThreadPool::globalInstance()->maxThreadCount();
  //  qInfo() << "Progress: " << next.progressValue() - next.progressMinimum()
  //          << '/' << next.progressMaximum();
  double progress = 0;
  for (auto& p : this->simulations) {
    progress += p->cell()->t;
  }
  return progress;
}

void RunSim::finishedCallback(std::function<void()> fn) {
  this->__finishedCall = fn;
}

void RunSim::startCallback(std::function<void()> fn) { this->__startCall = fn; }

void RunSim::run() {
  if (!this->finished()) return;
  this->__finished = false;
  try {
    this->__startCall();
  } catch (std::bad_function_call&) {
  }
  //    for(auto& p: vector) {
  //        p->runTrial();
  //    }
  //    return;
  //  next = QtConcurrent::map(vector, [](shared_ptr<Protocol> p) {
  //    if (p != NULL) {
  //      p->runTrial();
  //    }
  //  });  // pass vector of protocols to QtConcurrent
  this->pool.pushAll(&Protocol::runTrial, this->simulations.begin(),
                     this->simulations.end());
  //  for (auto& proto : this->simulations) {
  //    this->pool.push(&Protocol::runTrial, proto);
  //  }
}

void RunSim::cancel() {
  for (auto& p : this->simulations) {
    p->stopTrial();
  }
  this->pool.clear();
}

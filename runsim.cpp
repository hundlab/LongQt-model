#include "runsim.h"
#include <QtConcurrent>

RunSim::RunSim() {}

RunSim::RunSim(shared_ptr<Protocol> proto)
{
    this->setSims(proto);
}

RunSim::RunSim(std::vector<shared_ptr<Protocol>> protoList)
{
    this->setSims(protoList);
}

RunSim::~RunSim() {}

void RunSim::appendSims(shared_ptr<Protocol> proto) {
    int i = 0;
    proto->mkDirs();
    vector.clear();

    for(i = 0; i < proto->numtrials; i++) {
        proto->trial(i);
        vector.append(shared_ptr<Protocol>(proto->clone()));
    }
}

void RunSim::appendSims(std::vector<shared_ptr<Protocol>> protoList) {
    for(auto& proto: protoList) {
        proto->mkDirs();
        vector.append(proto);
    }
}

void RunSim::setSims(shared_ptr<Protocol> proto) {
    vector.clear();
    this->appendSims(proto);
}

void RunSim::setSims(std::vector<shared_ptr<Protocol>> protoList) {
    vector.clear();
    this->appendSims(protoList);
}

void RunSim::clear() {
    vector.clear();
}

QFuture<void>& RunSim::getFuture() {
    return this->next;
}

bool RunSim::finished()
{
    return next.isFinished();
}

void RunSim::run() {
    next = QtConcurrent::map(vector,[] (shared_ptr<Protocol> p) {
            if(p != NULL) {
            p->runTrial();
            }
            });  // pass vector of protocols to QtConcurrent
}

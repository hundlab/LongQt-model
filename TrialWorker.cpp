#include "TrialWorker.h"

TrialWorker::TrialWorker(Protocol* proto, SharedNumber trialnum, QObject* parent) : QObject(parent), trialnum(trialnum) {
    this->proto = proto;
}
void TrialWorker::work() {
    bool work = proto->setTrial(trialnum++);
    while(work) {
        proto->runTrial();
        work = proto->setTrial(trialnum++);
    }
}

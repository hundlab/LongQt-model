#include "TrialWorker.h"
#include <QThread>

TrialWorker::TrialWorker(Protocol* proto, SharedNumber* trialnum, QObject* parent) : QObject(parent), trialnum(trialnum) {
    this->trialnum->setParent(this);
    this->proto = proto;
}
void TrialWorker::work() {
    bool work = proto->setTrial(((*trialnum)++));
    QThread* currentThread = QThread::currentThread();
    while(work && !currentThread->isInterruptionRequested()) {
        proto->runTrial();
        emit currentProtoChanged(trialnum->getNum()+1);
        if(currentThread->isInterruptionRequested()) {
            break;
        }
        work = proto->setTrial(((*trialnum)++));
    }
    if(!currentThread->isInterruptionRequested()) {
        emit finished();
    }
}

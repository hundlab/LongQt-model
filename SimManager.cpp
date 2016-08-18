#include "TrialWorker.h"
#include "SimManager.h"
#define THREAD_COUNT_DEFAULT 4

SimManager::SimManager(Protocol* proto, QThread::Priority priority) {
    this->proto = proto;
    this->priority = priority;
    proto->propertyoutfile = "dt%d_%s" + string(".dat");
    proto->dvarsoutfile = "dt%d_dvars" + string(".dat");
    proto->finalpropertyoutfile = "dss%d_%s" + string(".dat");
    proto->finaldvarsoutfile = "dss%d_pvars" + string(".dat");
//    connect(this, &SimManager::quit, this, &QObject::deleteLater);
}
void SimManager::runSims() {
    this->constructPool();
    for(auto drop = localPool.begin();drop != localPool.end();drop++) {
        drop.value()->start(this->priority);
    }    
    emit started();
}
void SimManager::checkFinished() {
    this->numThreadsRunning--;
    if(this->numThreadsRunning <= 0) {
        emit finished();
    }
}
void SimManager::constructPool() {
    QMutex* trialLock = new QMutex();
    int* trialnum = new int;
    *trialnum = this->proto->getTrial();
    (*trialnum)--;
    int numThreads = QThread::idealThreadCount();
    if(numThreads <= 0) {
        numThreads = THREAD_COUNT_DEFAULT;
    }
    this->numThreadsRunning = numThreads;
    for(int i = 0; i < numThreads; i++) {
        QThread* thread = new QThread();
        SharedNumber* sharedTrialnum = new SharedNumber(trialnum, trialLock);
        TrialWorker* worker = new TrialWorker(this->proto->clone(), sharedTrialnum);
        worker->moveToThread(thread);
        localPool.insert(worker,thread);

        connect(this, &SimManager::finished, sharedTrialnum, &QObject::deleteLater);
        connect(worker, &TrialWorker::currentProtoChanged, this, &SimManager::currentTrialChanged);
        connect(thread, &QThread::started, worker, &TrialWorker::work);
        connect(this, &SimManager::quit, thread, &QThread::requestInterruption);
        connect(worker, &TrialWorker::finished, this, &SimManager::checkFinished);
        
    }
    emit numTrialsChanged(0,this->proto->numtrials);
}

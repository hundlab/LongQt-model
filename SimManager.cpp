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
    this->constructPool();
}
void SimManager::constructPool() {
    QMutex* trialLock = new QMutex();
    int* trialnum = new int;
    *trialnum = this->proto->getTrial();
    SharedNumber sharedTrialnum(trialnum, trialLock);
    int numThreads = QThread::idealThreadCount();
    if(numThreads <= 0) {
        numThreads = THREAD_COUNT_DEFAULT;
    }
    for(int i = 0; i < numThreads; i++) {
        TrialWorker* worker = new TrialWorker(this->proto->clone(), sharedTrialnum, this);
        QThread* thread = new QThread();
        worker->moveToThread(thread);
        connect(thread, &QThread::finished, worker, &QObject::deleteLater);
        connect(this, &SimManager::runSims, worker, &TrialWorker::work);
        connect(this, &SimManager::quit, thread, &QThread::quit);
        thread->start(this->priority);
        localPool.insert(worker,thread);
    }
}

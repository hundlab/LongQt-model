#ifndef SIMTHREADMANAGER_H
#define SIMTHREADMANAGER_H

#include "protocol.h"
#include "TrialWorker.h"
#include <QThread>
#include <QObject>
#include <QMap>

class SimManager : public QObject {
Q_OBJECT
  public:
    SimManager(Protocol* proto, QThread::Priority priority = QThread::InheritPriority);
    ~SimManager() {
        for(auto drop = localPool.begin(); drop != localPool.end();drop++) {
            drop.key()->deleteLater();
            drop.value()->exit();
            drop.value()->deleteLater();
        }
    };
  public slots:
    void runSims();
    void checkFinished();
  signals:
    void finished();
    void started();
    void numTrialsChanged(int,int);
    void currentTrialChanged(int);
    void quit();
  private:
    Protocol* proto;
    QThread::Priority priority;
    void constructPool();
    QMap<TrialWorker*,QThread*> localPool;
    int numThreadsRunning;
};

#endif

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
    ~SimManager() {};
  signals:
    void finished();
    void started();
    void progressRangeChanged(int,int);
    void progressValueChanged(int);
    void runSims();
    void quit();
  private:
    Protocol* proto;
    QThread::Priority priority;
    void constructPool();
    QMap<TrialWorker*,QThread*> localPool;
};

#endif

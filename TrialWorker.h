#ifndef TRIALWORKER_H
#define TRIALWORKER_H

#include <QMutex>
#include <QObject>
#include "protocol.h"

class SharedNumber {
  public:
    SharedNumber(int* num, QMutex* lock) {
        this->num = num;
        this->lock = lock;
    }
    ~SharedNumber() {}
    int getNum() {
        return this->current;
    }
    int operator++(int) {
        lock->lock();
         this->current = *num++;
        lock->unlock();
        return this->current;
    }
  private:
    int* num;
    int current;
    QMutex* lock;
};
class TrialWorker : public QObject {
Q_OBJECT
  public:
    TrialWorker(Protocol* proto, SharedNumber trialnum, QObject* parent = Q_NULLPTR);
    ~TrialWorker() {}
  public slots:
    void work(); 
  private:
    Protocol* proto;
    SharedNumber trialnum;
};

#endif

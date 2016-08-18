#ifndef TRIALWORKER_H
#define TRIALWORKER_H

#include <QMutex>
#include <QObject>
#include "protocol.h"

class SharedNumber : public QObject {
Q_OBJECT
  public:
    SharedNumber(int* num, QMutex* lock, QObject* parent = Q_NULLPTR) : QObject(parent) {
        this->num = num;
        this->lock = lock;
    }
    ~SharedNumber() {}
    int getNum() {
        lock->lock();
         this->current = *num;
        lock->unlock();
        return this->current;
    }
    int operator++(int) {
        lock->lock();
         this->current = (*num)++;
        lock->unlock();
        emit valueChanged(this->current);
        return this->current;
    }
    int operator--(int) {
        lock->lock();
         this->current = (*num)--;
        lock->unlock();
        emit valueChanged(this->current);
        return this->current;
    }
  private:
    int* num;
    int current;
    QMutex* lock;
  signals:
    void valueChanged(int);
};
class TrialWorker : public QObject {
Q_OBJECT
  public:
    TrialWorker(Protocol* proto, SharedNumber* trialnum, QObject* parent = Q_NULLPTR);
    ~TrialWorker() {}
  public slots:
    void work(); 
  signals:
    void currentProtoChanged(int);
    void finished();
  private:
    Protocol* proto;
    SharedNumber* trialnum;
};

#endif

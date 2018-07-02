#ifndef RUNSIM_H
#define RUNSIM_H

#include <QFutureWatcher>
#include <QVector>
#include <protocol.h>



class RunSim
{
public:
    RunSim();
    RunSim(shared_ptr<Protocol> proto);
    RunSim(std::vector<shared_ptr<Protocol> > protoList);

    ~RunSim();

    void run();
    bool finished();
    void setSims(shared_ptr<Protocol> proto);
    void setSims(std::vector<shared_ptr<Protocol> > protoList);
    void appendSims(shared_ptr<Protocol> proto);
    void appendSims(std::vector<shared_ptr<Protocol> > protoList);
    void clear();
    QFuture<void>& getFuture();
private:

    QFuture<void> next;
    QVector<shared_ptr<Protocol>> vector;
};

#endif // RUNSIM_H

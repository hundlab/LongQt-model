#ifndef RUNSIM_H
#define RUNSIM_H

#include <QFutureWatcher>
#include <QVector>
#include <protocol.h>


namespace LongQt {

class RunSim
{
public:
    RunSim();
    RunSim(std::shared_ptr<Protocol> proto);
    RunSim(std::vector<std::shared_ptr<Protocol> > protoList);

    ~RunSim();

    void run();
    bool finished();
    bool progress();
    void setSims(std::shared_ptr<Protocol> proto);
    void setSims(std::vector<std::shared_ptr<Protocol> > protoList);
    void appendSims(std::shared_ptr<Protocol> proto);
    void appendSims(std::vector<std::shared_ptr<Protocol> > protoList);
    void clear();
    QFuture<void>& getFuture();
private:

    QFuture<void> next;
    QVector<std::shared_ptr<Protocol>> vector;
};
} //LongQt

#endif // RUNSIM_H

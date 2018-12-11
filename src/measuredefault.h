#ifndef MEASUREDEFAULT_H
#define MEASUREDEFAULT_H

#include "measure.h"

#define INF std::numeric_limits<double>::infinity()
#define Q_NAN std::numeric_limits<double>::quiet_NaN()

namespace LongQt {

class MeasureDefault: public Measure
{
public:
    MeasureDefault(std::set<std::string> selected = {});
    MeasureDefault(const MeasureDefault& toCopy);
    MeasureDefault(MeasureDefault &&toCopy);
    virtual ~MeasureDefault() = default;

    MeasureDefault& operator=(const MeasureDefault& toCopy) = delete;

    virtual bool measure(double time,double var);  //measures props related to var; returns 1 when ready for output.
    virtual void reset();   //resets params to init vals

protected:
    virtual void beforeOutput();

private:
    virtual void calcMeasure(double time, double var);
    virtual void updateOld(double time, double var);

    double varold = Q_NAN;
    double told = Q_NAN;
    double derivold = Q_NAN; //dv/dt from prev. time step

    double deriv = 0;
    double amp = Q_NAN;
    double avg = 0; //mean
    double sd = 0; //standard deviation
    double count = 0; //# of calls since last reset
    double avg_2 = 0; //E[X^2]
    std::pair<double,double> maxderiv = {Q_NAN,-INF};   // time of max deriv.
    std::pair<double,double> max = {Q_NAN,-INF};      // max var value
    std::pair<double,double> min = {Q_NAN,INF};       // min var value

    std::set<std::string> __selection; // map for refing properties that will be output.
};
} //LongQt
#undef INF
#undef Q_NAN
#endif // MEASUREDEFAULT_H

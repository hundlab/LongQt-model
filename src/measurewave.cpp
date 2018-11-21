#include "measurewave.h"
using namespace LongQt;
using namespace std;

#define INF std::numeric_limits<double>::infinity()
#define Q_NAN std::numeric_limits<double>::quiet_NaN()

MeasureWave::MeasureWave(set<string> selected, double percrepol):
    Measure(selected),
    __percrepol(percrepol)
{
    varmap.insert({{"cl",&cl},{"amp",&amp},{"ddr",&ddr},{"dur",&dur},
        {"durtime1",&durtime1},{"vartakeoff",&vartakeoff},
        {"deriv2ndt",&maxderiv2d.first}});
    this->selection(selected);
};

MeasureWave::MeasureWave(const MeasureWave& toCopy):
    Measure(toCopy)
{
    this->copy(toCopy);
};

MeasureWave::MeasureWave(MeasureWave&& toCopy):
    Measure(toCopy)
{
    this->copy(toCopy);
};

void MeasureWave::copy(const MeasureWave& toCopy) {
    vartakeoff= toCopy.vartakeoff;
    repol = toCopy.repol;
    amp = toCopy.amp;
    maxderiv2d.second= toCopy.maxderiv2d.second;
    cl= toCopy.cl;
    told = toCopy.told;
    varold = toCopy.varold;
    derivold = toCopy.derivold;
    maxfound = toCopy.maxfound;
    ampfound = toCopy.ampfound;
    ddrfound = toCopy.ddrfound;
    maxderiv_prev = toCopy.maxderiv_prev;
    inAP = toCopy.inAP;
    __percrepol = toCopy.__percrepol;
    resetflag = toCopy.resetflag;
    dur = toCopy.dur;
    __selection = toCopy.__selection;
};

void MeasureWave::calcMeasure(double time, double var) {
//    if(minflag&&abs(var)>peak){          // Track value and time of peak
    if(minfound && ddrfound && !maxfound && var>max.second) {
        max={time,var};
    }
    if(!std::isnan(max.first) && var <= max.second-.1*abs(max.second)) {
        maxfound=true;
    }

    if(var<min.second && !maxfound){ // Track value and time of min
        min= {time,var};
    } else
        minfound=true;

    if(std::isnan(told)) {
        return;
    }

    resetflag = false;  //default for return...set to 1 when props ready for output

    double deriv=(var-varold)/(time-told);
    double deriv2nd=(deriv-derivold)/(time-told);

    if(deriv>maxderiv.second){             // Track value and time of max 1st deriv
        maxderiv={time,deriv};
    }

   if(deriv2nd>.02 && var>(0.01*abs(min.second)+min.second) && !ddrfound){   // Track 2nd deriv for SAN ddr
   //not functioning for cal as it is has small values and does not meet the 0.02
   //threshold
        vartakeoff=var;
        maxderiv2d={time,deriv2nd};
        if(!std::isnan(min.first)) {
            ddr=(vartakeoff-min.second)/(time-min.first);
        }
        ddrfound=true;
    }

    if(var>repol && !inAP){          // t1 for dur calculation = first time var crosses repol.
        durtime1=time;            // will depend on __percrepol default is 50 but can be changed.
        inAP=true;
    }

    if(minfound && !std::isnan(max.first) && !maxfound) { //&&!ampfound && maxfound
        amp=max.second-min.second;
    }
    if(maxfound && !ampfound) {
        ampfound = true;
        cl=maxderiv.first-maxderiv_prev;
        maxderiv_prev=maxderiv.first;
        repol = (1-__percrepol*0.01)*amp+min.second;
    }

    if(inAP && var<repol){
        dur=time-durtime1;
        inAP=false;
        resetflag = true;  // lets calling fxn know that it is time to output and reset.
    }
}

void MeasureWave::reset()
{
    maxderiv2d.second=-INF;
    //told is still valid after reset
//    told = 0.0;
    minfound = false;
    maxfound = false;
    ampfound = false;
    ddrfound = false;
    this->Measure::reset();
};
void MeasureWave::percrepol(double val) {
    this->__percrepol = val;
}

double MeasureWave::percrepol() const {
    return this->__percrepol;
}
#undef INF
#undef Q_NAN

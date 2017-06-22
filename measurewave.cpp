#include "measurewave.h"

MeasureWave::MeasureWave(set<string> selected, double percrepol):
    Measure(selected),
    __percrepol(percrepol)
{
    varmap.insert({{"cl",&cl},{"amp",&amp},{"ddr",&ddr},{"dur",&dur},
        {"durtime1",&durtime1},{"vartakeoff",&vartakeoff},
        {"deriv2ndt",&deriv2ndt}});

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
    maxderiv2nd= toCopy.maxderiv2nd;
    cl= toCopy.cl;
    told = toCopy.told;
    varold = toCopy.varold;
    derivold = toCopy.derivold;
    maxflag = toCopy.maxflag;
    ampflag = toCopy.ampflag;
    ddrflag = toCopy.ddrflag;
    derivt2 = toCopy.derivt2;
    derivt1 = toCopy.derivt1;
    durflag = toCopy.durflag;
    deriv2ndt = toCopy.deriv2ndt;
    __percrepol = toCopy.__percrepol;
    returnflag = toCopy.returnflag;
    dur = toCopy.dur;
    __selection = toCopy.__selection;
};

void MeasureWave::calcMeasure(double time, double var) {
//    if(minflag&&abs(var)>peak){          // Track value and time of peak
    if(minflag&&var>peak) {
        peak=var;
        maxt=time;
    } else if((peak-min)>0.3*abs(min))    // Assumes true max is more than 30% greater than the min.
        maxflag=true;

    if(var<min){                        // Track value and time of min
        min=var;
        mint=time;
    } else
        minflag=true;

    if(std::isnan(told)) {
        return;
    }

    returnflag = false;  //default for return...set to 1 when props ready for output

    double deriv=(var-varold)/(time-told);
    double deriv2nd=(deriv-derivold)/(time-told);

    if(deriv>maxderiv){             // Track value and time of max 1st deriv
        maxderiv=deriv;
        derivt=time;
    }

   if(deriv2nd>.02&&var>(0.01*abs(min)+min)&&!ddrflag){   // Track 2nd deriv for SAN ddr
        vartakeoff=var;
        maxderiv2nd=deriv2nd;
        deriv2ndt=time;
        if(!std::isnan(mint)) {
            ddr=(vartakeoff-min)/(time-mint);
        }
        ddrflag=true;
    }

    if(var>repol&&!durflag){          // t1 for dur calculation = first time var crosses repol.
        durtime1=time;            // will depend on __percrepol - default is 50 but can be changed.
        durflag=true;
    }

    if(maxflag&&minflag&&!ampflag){
        amp=peak-min;
        ampflag = true;
        cl=derivt-derivt1;
        derivt2=derivt1;
        derivt1=derivt;
        repol = (1-__percrepol*0.01)*amp+min;
    }

    if(durflag&&var<repol){
        dur=time-durtime1;
        durflag=false;
        returnflag = true;  // lets calling fxn know that it is time to output and reset.
    }
}

void MeasureWave::reset()
{
    maxderiv2nd=0.0;
    //told is still valid after reset
//    told = 0.0;
    minflag = false;
    maxflag = false;
    ampflag = false;
    ddrflag = false;
};
void MeasureWave::percrepol(double val) {
    this->__percrepol = val;
}

double MeasureWave::percrepol() const {
    return this->__percrepol;
}



//################################################
// This code file
// defines Measure class used to track properties
// (e.g. peak, min, duration) of specified state variable.
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015
// Email thomas.hund@osumc.edu
//#################################################

#include "measure_kernel.h"

//#############################################################
// Measure class constructor and destructor
//#############################################################

MeasureKernel::MeasureKernel(string varname)
{
    peak=-100.0;
    min=100.0;
    vartakeoff=-100.0;
    amp = 70.0;
    repol50 = -25.0;
    repol75 = -25.0;
    maxderiv=0.0;
    maxderiv2nd=0.0;
    cl=0.0;
    told = -10000.0;
    mint = 0.0;
    maxt = 0.0;
    varold = 100.0;
    derivold = 0.0;
    minflag = false;
    maxflag = false;
    ampflag = false;
    ddrflag = false;
    derivt2 = 0.0;
    derivt1 = 0.0;
    derivt = 0.0;
    deriv2ndt = 0.0;
    dur50flag = false;
    dur75flag = false;
    dur90flag = false;
    percrepol = 50.0;
    returnflag = 0;

    this->varname = varname;
    
    mkmap();
    
    };

MeasureKernel::MeasureKernel(const MeasureKernel& toCopy) {
    this->copy(toCopy);
};

MeasureKernel::MeasureKernel( MeasureKernel&& toCopy) {
    this->copy(toCopy); 
};

MeasureKernel& MeasureKernel::operator=(const MeasureKernel& toCopy) {
    this->copy(toCopy);
    return *this;
};

MeasureKernel::~MeasureKernel()
{
};

set<string> MeasureKernel::getVariables() {
    set<string> toReturn;
    map<string,double*>::iterator it;
    for(it = varmap.begin(); it != varmap.end(); it++) {
        toReturn.insert(it->first);
    }
    return toReturn;
}
 
map<string,double> MeasureKernel::getVariablesMap() {
    map<string,double> toReturn;
    for(map<string,double*>::iterator it = varmap.begin(); it != varmap.end(); it++) {
        toReturn.insert(std::pair<string,double>(it->first, *it->second));
    }
    return toReturn;
}

void MeasureKernel::copy(const MeasureKernel& toCopy) {
    std::map<string, double*>::iterator it;

    peak= toCopy.peak;
    min= toCopy.min;
    vartakeoff= toCopy.vartakeoff;
    repol50 = toCopy.repol50;
    repol75 = toCopy.repol75;
    amp = toCopy.amp;
    maxderiv= toCopy.maxderiv;
    maxderiv2nd= toCopy.maxderiv2nd;
    cl= toCopy.cl;
    told = toCopy.told;
    mint = toCopy.mint;
    maxt = toCopy.maxt;
    varold = toCopy.varold;
    derivold = toCopy.derivold;
    minflag = toCopy.minflag;
    maxflag = toCopy.maxflag;
    ampflag = toCopy.ampflag;
    ddrflag = toCopy.ddrflag;
    derivt2 = toCopy.derivt2;
    derivt1 = toCopy.derivt1;
    derivt = toCopy.derivt;
    deriv2ndt = toCopy.deriv2ndt;
    dur50flag = toCopy.dur50flag;
    dur75flag = toCopy.dur75flag;
    dur90flag = toCopy.dur90flag;
    percrepol = toCopy.percrepol;
    returnflag = toCopy.returnflag;
    dur50 = toCopy.dur50;
    dur75 = toCopy.dur75;
    dur90 = toCopy.dur90;
    varname = toCopy.varname;    

    mkmap();
};

//################################################################
// Function to track properties (e.g. peak, min, duration) of
// specified state variable and return status flag to calling fxn.
//################################################################
bool MeasureKernel::measure(double time, double var)
{
    double deriv,deriv2nd;
    
    returnflag = false;  //default for return...set to 1 when props ready for output
    
    deriv=(var-varold)/(time-told);
    deriv2nd=(deriv-derivold)/(time-told);
    
    if(deriv>maxderiv){             // Track value and time of max 1st deriv
        maxderiv=deriv;
        derivt=time;
    }
    
    if(deriv2nd>.02&&var>(0.01*abs(min)+min)&&!ddrflag){   // Track 2nd deriv for SAN ddr
        vartakeoff=var;
        deriv2ndt=time;
        ddr=(vartakeoff-min)/(time-mint);
        ddrflag=true;
    }
    
    if(minflag&&var>peak){          // Track value and time of peak
        peak=var;
        maxt=time;
    }
    else if((peak-min)>0.3*abs(min))    // Assumes true max is more than 30% greater than the min.
        maxflag=true;
    
    if(var<min){                        // Track value and time of min
        min=var;
        mint=time;
    }
    else
        minflag=true;
    
    if(var>repol50&&!dur50flag){          // t1 for dur calculation = first time var crosses repol.
        dur50time1=time;            // will depend on percrepol - default is 50 but can be changed.
        dur50flag=true;
    }
    if(var>repol75&&!dur75flag){          // t1 for dur calculation = first time var crosses repol.
        dur75time1=time;
        dur75flag=true;
    }
    if(var>repol75&&!dur90flag){          // t1 for dur calculation = first time var crosses repol.
        dur90time1=time;
        dur90flag=true;
    }
    
    if(maxflag&&minflag&&!ampflag){
        amp=peak-min;
        ampflag = true;
        cl=derivt-derivt1;
        derivt2=derivt1;
        derivt1=derivt;
        repol50 = (1-percrepol*0.01)*amp+min;
        repol75 = (1-0.75)*amp+min;
    }
    
    if(dur50flag&&var<repol50){
        dur50=time-dur50time1;
        dur50flag=false;
        returnflag = true;  // lets calling fxn know that it is time to output and reset.
    }
    if(dur75flag&&var<repol75){
        dur75=time-dur75time1;
        dur75flag=false;
    }
    if(dur90flag&&var<repol75){
        dur90=time-dur90time1;
        dur90flag=false;
    }
    told=time;
    varold=var;
    derivold=deriv;
    
    return (returnflag);
};

void MeasureKernel::reset()
{
    peak=-100.0;
    min=100.0;
    maxderiv=0.0;
    maxderiv2nd=0.0;
    told = 0.0;
    minflag = 0;
    maxflag = 0;
    ampflag = 0;
    ddrflag = 0;
};

void MeasureKernel::mkmap() {
    varmap["peak"]=&peak;
    varmap["amp"]=&amp;
    varmap["cl"]=&cl;
    varmap["ddr"]=&ddr;
    varmap["maxderiv"]=&maxderiv;
    varmap["dur50"]=&dur50;
    varmap["dur75"]=&dur75;
    varmap["dur90"]=&dur90;
    varmap["dur50time1"]=&dur50time1;
    varmap["dur75time1"]=&dur75time1;
    varmap["dur90time1"]=&dur90time1;
    varmap["vartakeoff"]=&vartakeoff;
    varmap["mint"]=&mint;
    varmap["derivt"]=&derivt;
    varmap["deriv2ndt"]=&deriv2ndt;
};

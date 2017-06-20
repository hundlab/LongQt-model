//################################################
// This code file
// defines Measure class used to track properties
// (e.g. peak, min, duration) of specified state variable.
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015
// Email thomas.hund@osumc.edu
//#################################################

#include "measure.h"
#include <cmath>

//#############################################################
// Measure class constructor and destructor
//#############################################################

Measure::Measure(set<string> selected, double percrepol):
    __percrepol(percrepol),
    __selection(selected)
{};

Measure::Measure(const Measure& toCopy) {
    this->copy(toCopy);
};

Measure::Measure( Measure&& toCopy) {
    this->copy(toCopy);
};

/*Measure& Measure::operator=(const Measure& toCopy) {
this->copy(toCopy);
return *this->;
};*/

set<string> Measure::variables() {
    set<string> toReturn;
    for(auto& var: varmap) {
        toReturn.insert(var.first);
    }
    return toReturn;
}

map<string,double> Measure::variablesMap() {
    map<string,double> toReturn;
    for(auto& var: varmap) {
        toReturn.insert(std::pair<string,double>(var.first, *var.second));
    }
    return toReturn;
}

void Measure::copy(const Measure& toCopy) {
    peak= toCopy.peak;
    min= toCopy.min;
    vartakeoff= toCopy.vartakeoff;
    repol = toCopy.repol;
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
    durflag = toCopy.durflag;
    __percrepol = toCopy.__percrepol;
    returnflag = toCopy.returnflag;
    dur = toCopy.dur;
    __selection = toCopy.__selection;
};

//################################################################
// Function to track properties (e.g. peak, min, duration) of
// specified state variable and return status flag to calling fxn.
//################################################################
bool Measure::measure(double time, double var)
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

    if(minflag&&abs(var)>peak){          // Track value and time of peak
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

    told=time;
    varold=var;
    derivold=deriv;

    return (returnflag);
};


void Measure::reset()
{
    /*	for(auto var: this->varmap) {
        this->lastMap[var.first] = *var.second;
        }*/
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
void Measure::percrepol(double val) {
    this->__percrepol = val;
}

double Measure::percrepol() const {
    return this->__percrepol;
}

string Measure::getNameString(string name) {
    string nameStr = "";
    for(auto& sel: this->__selection) {
        nameStr += name+"/"+sel+"\t";
    }
    return nameStr;
}
string Measure::getValueString() {
    string valStr = "";
    for(auto& sel: this->__selection) {
        valStr += to_string(*this->varmap.at(sel))+"\t";
    }
    return valStr;
}
/*
   void Measure::restoreLast() {
   for(auto var: lastMap) {
 *this->varmap[var.first] = var.second;
 }
 }*/


set<string> Measure::selection() {
    return this->__selection;
}
void Measure::selection(set<string> new_selection) {
    this->__selection.clear();
    for(auto& select: new_selection) {
        if(varmap.count(select) == 1) {
            __selection.insert(select);
        }
    }
}

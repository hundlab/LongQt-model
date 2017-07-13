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

//#############################################################
// Measure class constructor and destructor
//#############################################################

Measure::Measure(set<string> selected)
{
    for(auto& select: selected) {
        if(varmap.count(select)>0) {
            __selection.insert(select);
        }
    }
};

Measure::Measure(const Measure& toCopy) {
    this->copy(toCopy);
};

Measure::Measure( Measure&& toCopy) {
    this->copy(toCopy);
};

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
    told = toCopy.told;
    mint = toCopy.mint;
    maxt = toCopy.maxt;
    varold = toCopy.varold;
    derivold = toCopy.derivold;
    derivt = toCopy.derivt;
    returnflag = toCopy.returnflag;
    __selection = toCopy.__selection;
};

//################################################################
// Function to track properties (e.g. peak, min, duration) of
// specified state variable and return status flag to calling fxn.
//################################################################
bool Measure::measure(double time, double var) {
    this->calcMeasure(time,var);
    this->updateOld(time, var);
    return this->returnflag;
}

void Measure::calcMeasure(double time, double var)
{
//    if(minflag&&abs(var)>peak){          // Track value and time of peak
    if(var>peak) {
        peak=var;
        maxt=time;
    }

    if(var<min){                        // Track value and time of min
        min=var;
        mint=time;
    }
    if(std::isnan(told)) {
        return;
    }

    returnflag = false;  //default for return...set to 1 when props ready for output

    deriv=(var-varold)/(time-told);

    if(deriv>maxderiv){             // Track value and time of max 1st deriv
        maxderiv=deriv;
        derivt=time;
    }
};

void Measure::updateOld(double time, double var) {
    told=time;
    varold=var;
    derivold=deriv;
}


void Measure::reset()
{
    peak=-100.0;
    min=100.0;
    maxderiv=0.0;
//    maxderiv2nd=0.0;
    //told is still valid after reset
//    told = 0.0;
    returnflag = false;
};
string Measure::getNameString(string name) const {
    string nameStr = "";
    for(auto& sel: this->__selection) {
        nameStr += name+"/"+sel+"\t";
    }
    return nameStr;
}
string Measure::getValueString() const {
    string valStr = "";
    for(auto& sel: this->__selection) {
        valStr += to_string(*this->varmap.at(sel))+"\t";
    }
    return valStr;
}

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

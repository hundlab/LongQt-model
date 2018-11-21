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
#include <iomanip>
#include <sstream>

using namespace LongQt;
using namespace std;

#define INF std::numeric_limits<double>::infinity()
#define Q_NAN std::numeric_limits<double>::quiet_NaN()
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
    max = toCopy.max;
    told = toCopy.told;
    min = toCopy.min;
    varold = toCopy.varold;
    derivold = toCopy.derivold;
    maxderiv = toCopy.maxderiv;
    resetflag = toCopy.resetflag;
    __selection = toCopy.__selection;
};

//################################################################
// Function to track properties (e.g. peak, min, duration) of
// specified state variable and return status flag to calling fxn.
//################################################################
bool Measure::measure(double time, double var) {
    this->calcMeasure(time,var);
    this->updateOld(time, var);
    return this->resetflag;
}

void Measure::calcMeasure(double time, double var)
{
//    if(minflag&&abs(var)>peak){          // Track value and time of peak
    if(var>max.second) {
        max= {time, var};
    }

    if(var<min.second){                        // Track value and time of min
        min= {time, var};
    }
    if(std::isnan(told)) {
        return;
    }

    resetflag = false;  //default for return...set to 1 when props ready for output

    deriv=(var-varold)/(time-told);

    if(deriv>maxderiv.second){             // Track value and time of max 1st deriv
        maxderiv= {time,deriv};
    }
};

void Measure::updateOld(double time, double var) {
    told=time;
    varold=var;
    derivold=deriv;
}


void Measure::reset()
{
    max.second=-INF;
    min.second=INF;
    maxderiv.second=-INF;
//    maxderiv2nd=0.0;
    //told is still valid after reset
//    told = 0.0;
    resetflag = false;
};
string Measure::getNameString(string name) const {
    string nameStr = "";
    for(auto& sel: this->__selection) {
        nameStr += name+"/"+sel+"\t";
    }
    return nameStr;
}
string Measure::getValueString() const {
    std::ostringstream valStr;
    for(auto& sel: this->__selection) {
        valStr << std::scientific << *this->varmap.at(sel) << "\t";
    }
    return valStr.str();
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
#undef INF
#undef Q_NAN

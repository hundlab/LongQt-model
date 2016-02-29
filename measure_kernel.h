//################################################
// This header file
// declares Measure class used to track properties
// (e.g. peak, min, duration) of specified state variable.
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015
// Email thomas.hund@osumc.edu
//#################################################

#ifndef MEASURE_KERNEL_H
#define MEASURE_KERNEL_H

#include <sstream>
#include <exception>
#include <string>
#include <cmath>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <iostream>

#include "iobase.h"

using namespace std;

class MeasureKernel
{
public:
    MeasureKernel();
    MeasureKernel(const MeasureKernel& toCopy);
    MeasureKernel( MeasureKernel&& toCopy);
    ~MeasureKernel();

    MeasureKernel& operator=(const MeasureKernel& toCopy);
    
    bool measure(double time,double var);  //measures props related to var; returns 1 when ready for output.
    void reset();   //resets params to init vals

    string varname;
    
    set<string> getVariables();
    map<string,double> getVariablesMap();

protected:   
    map<string, double*> varmap; // map for refing properties that can be measured.

    double* var;
    double varold;
    double vartakeoff; //var value at point of max deflection.
    double told;
    double mint;    //time of min value.
    double maxt;    //time of max value.
    double durtime1;
    double derivold; //dv/dt from prev. time step
    double derivt;   // time of max deriv.
    double derivt1;  // time of prev. cycle max deriv.
    double derivt2;
    double deriv2ndt;  // time of max 2nd deriv.
    double peak;      // max var value
    double min;       // min var value
    double amp;
    double ddr;      //diastolic depol. rate
    double maxderiv;
    double maxderiv1;
    double maxderiv2;
    double maxderiv2nd;
    double cl;
    double dur;   //duration
    double percrepol;   //specify percent repolarization
    double repol;           // repol var val for duration measure.
    bool minflag;
    bool maxflag;
    bool durflag;    //1 while measuring duration.
    bool ampflag;
    bool ddrflag;
    bool returnflag;
    
    void copy(const MeasureKernel& toCopy);    
};

#endif

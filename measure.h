//################################################
// This header file
// declares Measure class used to track properties
// (e.g. peak, min, duration) of specified state variable.
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015
// Email thomas.hund@osumc.edu
//#################################################

#ifndef MEASURE_H
#define MEASURE_H

#include "measure_kernel.h"


class Measure : public MeasureKernel, IOBase {
public:
    Measure() : MeasureKernel() {
        selection.insert("peak");
        selection.insert("min");
        selection.insert("maxderiv");
        selection.insert("dur");
    };

    Measure(const Measure& toCopy) : MeasureKernel(toCopy) {
    };
    Measure( Measure&& toCopy) : MeasureKernel( toCopy) {
    };
    ~Measure() {};

    Measure& operator=(const Measure& toCopy) {
        this->copy(toCopy);
        return *this;
    };
    
    bool write(bool useFlags = true, bool reset = true);
    bool setOutputfile(string filename);
    set<string> getSelection();
    bool setSelection(set<string> new_selection);
private:
    void copy(const Measure& toCopy);
    set<string> selection; // map for refing properties that will be output.
    ofstream ofile;
};

#endif

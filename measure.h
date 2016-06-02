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

#include <functional> 

#include "measure_kernel.h"
#include "iobase.h"


class Measure : public MeasureKernel, public IOBase {
public:
    Measure(string varname = "") : MeasureKernel(varname) {
//        selection.insert("peak");
//        selection.insert("min");
//        selection.insert("maxderiv");
//        selection.insert("dur");
    };

    Measure(const Measure& toCopy) : MeasureKernel(toCopy) {
        selection = toCopy.selection;
    };
    Measure( Measure&& toCopy) : MeasureKernel( toCopy) {
        selection = toCopy.selection;
    };

    ~Measure() {
        if(ofile.is_open()) {
            ofile.close();
        }
    };

    Measure& operator=(const Measure& toCopy) {
        this->copy(toCopy);
        this->selection = toCopy.selection;
        return *this;
    };
    
    bool write(bool useFlags = true, bool reset = true);
    bool setOutputfile(string filename);

    const set<string>& Selection = cref(selection);
    bool setSelection(set<string> new_selection);
    bool addToMeasureSelection(string new_select);
    void removeFromSelection(string to_remove);
    void closeFiles();
private:
    set<string> selection; // map for refing properties that will be output.
    ofstream ofile;
};

#endif

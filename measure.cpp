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


void copy(const Measure& toCopy) {
    MeasureKernel::copy(toCopy);
    selection = toCopy.selection; 
};

bool Measure::setOutputfile(string filename) {
    IOBase::setOutputfile(filename, this->selection, &ofile);
}

set<string> Measure::getSelection() {
    return selection;
}

bool Measure::setSelection(set<string> new_selection) {
    bool toReturn = true;
    set<string>::iterator set_it;
    map<string,double*>::iterator map_it;
    
    selection = set<string>();

    for(set_it = new_selection.begin(); set_it != new_selection.end(); set_it++) {
        map_it = varmap.find(*set_it);
        if(map_it != varmap.end()) {
            selection.insert(*set_it);
        } else {
            toReturn = false;
        }
    }
    if(!ofile.good()) {
        return toReturn;
    }
    ofile.seekp(0);
    for(set<string>::iterator it = selection.begin(); it != selection.end(); it++) {
        ofile << *it << "\t";
    }
    ofile << endl;
    
    return toReturn;
}
bool Measure::write(bool useFlags, bool reset) {
    bool toReturn = false;

    if(useFlags&&!returnflag) {
        return toReturn;
    }

    toReturn = IOBase::write(this->selection, this->varmap, &ofile);

    if(reset&&toReturn) {
        this->reset();
    }
    return toReturn;
}



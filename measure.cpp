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

bool Measure::setOutputfile(string filename) {
    return IOBase::setOutputfile(filename, this->selection, &ofile);
}

bool Measure::setSelection(set<string> new_selection) {
    bool toReturn = true;
    set<string>::iterator set_it;
    map<string,double*>::iterator map_it;
    
    selection.clear();

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

bool Measure::addToMeasureSelection(string new_select) {
    if(varmap.find(new_select) != varmap.end()) {
        return selection.insert(new_select).second;
    } else {
        return false;
    }
}

void Measure::removeFromSelection(string to_remove) {
    selection.erase(selection.find(to_remove));
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




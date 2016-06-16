//################################################
// This code file
// defines an abstract class used to read and write
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015
// Email thomas.hund@osumc.edu
//#################################################


#include "iobase.h"
bool IOBase::setOutputfile(string filename, set<string> set, ofstream* ofile) {
    ifstream test;
    bool exists;
    std::set<string>::iterator it;
    test.open(filename);
    exists = test.good();
    test.close();
    if(ofile->is_open()) {
        ofile->close();
    }
    ofile->precision(10);
    ofile->open(filename,ios_base::app);
    if(!ofile->good()) {
        cout << "Error Opening " << filename << endl;
    } else if(!exists) {
        for(it = set.begin(); it != set.end(); it++) {
            *ofile << *it << "\t";
        }
        *ofile << endl;
    return true;
    }
return false;
};
 

bool IOBase::write(set<string> selection, map<string, double*> map, ofstream* ofile) {
    if(!ofile->good()) {
        return false;
    }
    for(set<string>::iterator it = selection.begin(); it != selection.end(); it++) {
        *ofile << *map[*it] << "\t";
    }
    *ofile << endl;
    return true;
};

void IOBase::closeFile(ofstream* ofile) {
    if(ofile->is_open()) {
	ofile->close();
    }
}

//####################################################
// This file contains general function definitions 
// for simulation of excitable cell activity read and
// write functions.
//
// Copyright (C) 2011 Thomas J. Hund.
// Updated 11/21/2012
//####################################################

#include "cell.h"
#include <sstream>

bool Cell::setOuputfileVariables(string filename) {
    return setOutputfile(filename, this->varsSelection, &varsofile);
};

bool Cell::setOutputfileConstants(string filename) {
    return setOutputfile(filename, this->parsSelection, &parsofile);
};

void Cell::writeVariables() {
    write(varsSelection, this->vars, &varsofile);
};

void Cell::writeConstants() {
    write(parsSelection, this->pars, &parsofile);
};

bool Cell::setConstantSelection(set<string> selection) {
    return setSelection(this->pars, &this->parsSelection, selection, &parsofile);
};

bool Cell::setVariableSelection(set<string> selection) {
    return setSelection(this->vars, &this->varsSelection, selection, &varsofile);
};

set<string> Cell::getConstantSelection() {
    return parsSelection;
};

set<string> Cell::getVariableSelection() {
    return varsSelection;
};

bool Cell::setSelection(map<string, double*> map, set<string>* old_selection, set<string> new_selection, ofstream* ofile) {
    bool toReturn = true;
    set<string>::iterator set_it;
    
    *old_selection = set<string>();

    for(set_it = new_selection.begin(); set_it != new_selection.end(); set_it++) {
        if(map.find(*set_it) != map.end()) {
            old_selection->insert(*set_it);
        } else {
            toReturn = false;
        }
    }
    if(!ofile->good()) {
        return toReturn;
    }
    ofile->seekp(0);
    for(set<string>::iterator it = old_selection->begin(); it != old_selection->end(); it++) {
        *ofile << *it << "\t";
    }
    *ofile << endl;
    
    return toReturn;
};

void Cell::closeFiles() {
    IOBase::closeFile(&parsofile);
    IOBase::closeFile(&varsofile);
}
bool Cell::writeCellState(string filename) {
    ofstream ofile;
    ofile.open(filename,ios_base::app);
    if(!ofile.good()) {
        cout << "Error Opening " << filename << endl;
        return false;
    }
    bool ret = this->writeCellState(ofile);
    ofile.close();
    return ret;
}
bool Cell::writeCellState(ofstream& ofile) {
    if(!ofile.good()) {
        cout << "Error Opening File for writing cell state" << endl;
        return false;
    }
    ofile << "##BEGIN VARS\n";
    for(auto val : vars) {
        ofile << val.first << "\t" << *val.second << "\n";
    }
    ofile << "##END VARS\n";
    ofile << "##BEGIN PARS\n";
    for(auto val : pars) {
        ofile << val.first << "\t" << *val.second << "\n";
    }
    ofile << "##END PARS\n";
    return true;

}
bool Cell::readCellState(string filename) {
    ifstream ifile;
    ifile.open(filename);
    if(!ifile.good()) {
        cout << "Error Opening " << filename << "\n";
        return false;
    }
    bool ret = this->readCellState(ifile);
    ifile.close();
    return ret;
}
bool Cell::readCellState(ifstream& ifile) {
    if(!ifile.good()) {
        cout << "Error Opening File For Reading Cell State" << "\n";
        return false;
    }
    string temp, name, val;
    bool Nvars = false;
    bool Npars = false;
    while(!ifile.eof()&&!(Nvars&&Npars)) {
        getline(ifile,temp);
        if(temp == "##BEGIN VARS"&&!Nvars) {
            Nvars = true;
            while(!ifile.eof() && temp != "##END VARS") {
                stringstream linestream(temp);
                linestream >> name >> val;
                try {
                    *vars.at(name) = std::stod(val);
                } catch(out_of_range&) {}
                getline(ifile,temp);
            }
        }
        if(temp == "##BEGIN PARS"&&!Npars) {
            Npars = true;
            while(!ifile.eof() && temp != "##END PARS") {
                stringstream linestream(temp);
                linestream >> name >> val;
                try {
                    *vars.at(name) = std::stod(val);
                } catch(out_of_range&) {}
                getline(ifile,temp);
            }
        }
    }
    return true;
}

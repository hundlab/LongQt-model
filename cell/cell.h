//################################################
// This header file contains class definition for
// simulation of excitable cell activity, with
// functions for input and output. 
//
// Copyright (C) 2011 Thomas J. Hund.
// Updated 11/21/2012
// Email thomas.hund@osumc.edu
//#################################################

#ifndef MODEL
#define MODEL

#include <cmath>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>

#include "iobase.h"
#include "cell_kernel.h"

using namespace std;


//######################################################
//Define class for parent cell.
//######################################################
class Cell : public CellKernel, public IOBase
{
public:
    Cell() : CellKernel() {
        varsSelection.insert("t");
        varsSelection.insert("vOld");
    };

    Cell(const Cell& toCopy) : CellKernel(toCopy) {
        parsSelection = toCopy.parsSelection;
        varsSelection = toCopy.varsSelection;
    };
    ~Cell() {};

    virtual Cell* clone() {//public copy function
        return new Cell(*this);
    }

    virtual bool setOutputfileConstants(string filename);
    virtual bool setOuputfileVariables(string filename);
    virtual bool setConstantSelection(set<string> new_selection);
    virtual bool setVariableSelection(set<string> new_selection);
    virtual set<string> getConstantSelection();
    virtual set<string> getVariableSelection();
    virtual void writeConstants();
    virtual void writeVariables();
    virtual void closeFiles();
    virtual bool writeCellState(string filename);
    virtual bool writeCellState(ofstream& ofile);
    virtual bool readCellState(string filename);
    virtual bool readCellState(ifstream& ifile);
protected:
    virtual bool setSelection(map<string, double*> map, set<string>* old_selection, set<string> new_selection, ofstream* ofile);
    ofstream parsofile;
    ofstream varsofile;
    set<string> parsSelection;
    set<string> varsSelection;

};
#endif

//################################################
// This header file contains class definition for
// simulation of excitable cell activity, with
// functions for input and output. 
//
// Copyright (C) 2011 Thomas J. Hund.
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
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include "iobase.h"
#include "cell_kernel.h"

namespace LongQt {

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
    }
    virtual ~Cell() {}
    virtual Cell* clone() = 0;

    virtual bool setOutputfileConstants(std::string filename);
    virtual bool setOuputfileVariables(std::string filename);
    virtual bool setConstantSelection(std::set<std::string> new_selection);
    virtual bool setVariableSelection(std::set<std::string> new_selection);
    virtual std::set<std::string> getConstantSelection();
    virtual std::set<std::string> getVariableSelection();
    virtual void writeConstants();
    virtual void writeVariables();
    virtual void closeFiles();
    virtual bool writeCellState(std::string file);
    virtual bool writeCellState(QXmlStreamWriter& xml);
    virtual bool readCellState(std::string file);
    virtual bool readCellState(QXmlStreamReader& xml);
protected:
    virtual bool setSelection(std::map<std::string, double*> map, std::set<std::string>* old_selection, std::set<std::string> new_selection, std::ofstream* ofile);
    std::ofstream parsofile;
    std::ofstream varsofile;
    std::set<std::string> parsSelection;
    std::set<std::string> varsSelection;

};
} //LongQt

#endif

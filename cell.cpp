//####################################################
// This file contains general function definitions 
// for simulation of excitable cell activity.
//
// Copyright (C) 2011 Thomas J. Hund.
// Updated 11/21/2012
//####################################################

#include "cell.h"

//######################################################
// Constructor for parent cell class - the basis for 
// any excitable cell model.  
//######################################################
Cell::Cell()
{
    //##### Assign default parameters ##################
    dtmin = 0.005;  // ms
    dtmed = 0.01;
    dtmax = 0.1;
    dvcut = 1.0;    // mV/ms
    Cm = 1.0;  // uF/cm^s
    Rcg = 1.0;
    cellRadius = 0.001; // cm
    cellLength = 0.01;  // cm
    type = "Cell";  // class name
    RGAS = 8314.0;
    TEMP = 310.0;
    FDAY = 96487.0;

    //##### Initialize variables ##################
    dVdt=dVdtmax=0.0;
    t=0.0;
    dt=dtmin;
    vOld = vNew =  -88.0;
    iTot = iTotold = 0.0;
    iNat = iKt = iCat = 0.0;
    
    // make map of state vars
    vars["vOld"]=&vOld;
    vars["t"]=&t;
    vars["dVdt"]=&dVdt;
    vars["iTot"]=&iTot;
    vars["iKt"]=&iKt;
    vars["iNat"]=&iNat;
    vars["iCat"]=&iCat;
    
    // make map of params
    pars["dtmin"]=&dtmin;
    pars["dtmed"]=&dtmed;
    pars["dtmax"]=&dtmax;
    pars["Cm"]=&Cm;
    pars["Rcg"]=&Rcg;
    pars["RGAS"]=&RGAS;
    pars["TEMP"]=&TEMP;
    pars["FDAY"]=&FDAY;
    pars["cellRadius"]=&cellRadius;
    pars["cellLength"]=&cellLength;
    pars["Vcell"]=&Vcell;
    pars["Vmyo"]=&Vmyo;
    pars["AGeo"]=&AGeo;
    pars["ACap"]=&ACap;
    
};

//#####################################################
//Constructor for deep copy from annother Cell
//#####################################################
Cell::Cell(const Cell& toCopy)
{

    //##### Assign default parameters ##################
    dtmin = toCopy.dtmin;
    dtmed = toCopy.dtmed;
    dtmax = toCopy.dtmax;
    dvcut = toCopy.dvcut;    // mV/ms
    Cm = toCopy.Cm ;  // uF/cm^s
    Rcg = toCopy.Rcg;
    cellRadius = toCopy.cellRadius; // cm
    cellLength = toCopy.cellLength;  // cm
    type = "Cell";  // class name
    RGAS = toCopy.RGAS;
    TEMP = toCopy.TEMP;
    FDAY = toCopy.FDAY;

    //##### Initialize variables ##################
    dVdt= toCopy.dVdt;
    dVdtmax= toCopy.dVdtmax;
    t= toCopy.t;
    dt=toCopy.dt;
    vOld = toCopy.vOld;
    vNew = toCopy.vNew;
    iTot = toCopy.iTot;
    iTotold = toCopy.iTotold;
    iNat = toCopy.iNat;
    iKt = toCopy.iKt;
    iCat = toCopy.iCat;
    
    parsSelection = toCopy.parsSelection;
    varsSelection = toCopy.varsSelection;

    // make map of state vars
    vars["vOld"]=&vOld;
    vars["t"]=&t;
    vars["dVdt"]=&dVdt;
    vars["iTot"]=&iTot;
    vars["iKt"]=&iKt;
    vars["iNat"]=&iNat;
    vars["iCat"]=&iCat;
    
    // make map of params
    pars["dtmin"]=&dtmin;
    pars["dtmed"]=&dtmed;
    pars["dtmax"]=&dtmax;
    pars["Cm"]=&Cm;
    pars["Rcg"]=&Rcg;
    pars["RGAS"]=&RGAS;
    pars["TEMP"]=&TEMP;
    pars["FDAY"]=&FDAY;
    pars["cellRadius"]=&cellRadius;
    pars["cellLength"]=&cellLength;
    pars["Vcell"]=&Vcell;
    pars["Vmyo"]=&Vmyo;
    pars["AGeo"]=&AGeo;
    pars["ACap"]=&ACap;
    
};

//######################################################
// Destructor for parent cell class.
//#####################################################
Cell::~Cell()
{
};

//default copy function
Cell* Cell::clone() {
    return new Cell(*this);
};

// Transmembrane potential 
double Cell::updateV()
{
  vNew=vOld-iTot*dt;
  
  dVdt=(vNew-vOld)/dt;
  if(dVdt>dVdtmax)
    dVdtmax=dVdt;
  return vNew;
};

void Cell::setV(double v)
{
  vNew=v;	
  vOld=v;
};

// Dynamic time step  
double Cell::tstep(double stimt)
{
  t=t+dt;
  
  if((dVdt>=dvcut)||((t>stimt-2.0)&&(t<stimt+10.0))||(apTime<5.0))
  {
    dt=dtmin;
  }
  else if(apTime<40)
  {
    dt=dtmed;
  }
  else
  {
    dt=dtmax;
  }
  
    return t;
};

// External stimulus.
int Cell::externalStim(double stimval)
{
    iTot = iTot+stimval;   // If [ion] change, also should add stimval to specific ion total (e.g. iKt)
    
  return 1;
};

// Overwritten in child classes with calls to fxns that update currents
void Cell::updateCurr()
{
};

// Ditto but for intracellular ion concentration changes
void Cell::updateConc()
{
};

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


//####################################################
// This file contains general function definitions 
// for simulation of excitable cell activity.
//
// Copyright (C) 2011 Thomas J. Hund.
// Updated 11/21/2012
//####################################################

#include "cell_kernel.h"

//######################################################
// Constructor for parent cell class - the basis for 
// any excitable cell model.  
//######################################################
CellKernel::CellKernel()
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
    iTot = iTotold = 0.000000000001;
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
//Constructor for deep copy from annother CellKernel
//#####################################################
CellKernel::CellKernel(const CellKernel& toCopy)
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
CellKernel::~CellKernel()
{
};

//default copy function
CellKernel* CellKernel::clone() {
    return new CellKernel(*this);
};

// Transmembrane potential 
double CellKernel::updateV()
{
  vNew=vOld-iTot*dt;
  
  dVdt=(vNew-vOld)/dt;
  if(dVdt>dVdtmax)
    dVdtmax=dVdt;
  return vNew;
};

void CellKernel::setV(double v)
{
  vNew=v;	
  vOld=v;
};

// Dynamic time step  
double CellKernel::tstep(double stimt)
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
dt=dtmax;
  return t;
};

// External stimulus.
int CellKernel::externalStim(double stimval)
{
    iTot = iTot+stimval;   // If [ion] change, also should add stimval to specific ion total (e.g. iKt)
    
  return 1;
};

// Overwritten in child classes with calls to fxns that update currents
void CellKernel::updateCurr()
{
};

// Ditto but for intracellular ion concentration changes
void CellKernel::updateConc()
{
};

set<string> CellKernel::getVariables() {
    set<string> toReturn;
    for(auto it = vars.begin(); it != vars.end(); it++) {
        toReturn.insert(it->first);
    }
    return toReturn;
};

set<string> CellKernel::getConstants() {
    set<string> toReturn;
    for(auto it = pars.begin(); it != pars.end(); it++) {
        toReturn.insert(it->first);
    }
    return toReturn;
};

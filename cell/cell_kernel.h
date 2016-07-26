//################################################
// This header file contains class definition for
// simulation of excitable cell activity. 
//
// Copyright (C) 2011 Thomas J. Hund.
// Updated 11/21/2012
// Email thomas.hund@osumc.edu
//#################################################

#ifndef MODEL_KERNEL
#define MODEL_KERNEL

#include <cmath>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>

#include "iobase.h"

using namespace std;


//######################################################
//Define class for parent cell.
//######################################################
class CellKernel
{
  public:
    CellKernel();
    CellKernel(const CellKernel& toCopy);
    ~CellKernel(); 
    
    virtual CellKernel* clone(); //public copy function
  //##### Declare class functions ##############
    virtual double updateV();
    virtual void setV(double v);
    virtual void updateCurr();
    virtual void updateConc();
    virtual double tstep(double stimt);
    virtual int externalStim(double stimval);

    virtual double var(string name);
    virtual bool setVar(string name, double val);
    virtual double par(string name);
    virtual bool setPar(string name, double val);
    virtual set<string> getVariables();
    virtual set<string> getConstants();
 
    const char* type;
    double vOld;    // Transmembrane potential
    double vNew;
    double t;       // time, ms
    double dt;	  // Time increment
    double apTime;
    double dtmin,dtmed,dtmax,dvcut;
  protected:
    //##### Declare class variables ##############
    double iNat;  // Total transmembrane sodium current.
    double iKt;  // Total transmembrane potassium current.
    double iCat;  // Total transmembrane calcium current.
    double iTot;  // Total transmembrane current.
    double iTotold;  // Total transmembrane current.
    
    //##### Declare class params ##############
    double Cm;    // Specific membrane capacitance, uF/cm^2
    double Vcell;  // Total cell Volume, uL.
    double Vmyo;  //  Myoplasmic volume, uL.
    double AGeo;   // Geometric cell surface area.
    double ACap;   // Capacitive cell surface area.
    double Rcg;    // Ratio of capacitive to geometric area.
    double cellRadius, cellLength;
    double dVdt;
    double dVdtmax;
    
    
    double RGAS;
    double TEMP;
    double FDAY;

    
    //##### Declare maps for vars/params ##############
    map<string, double*> vars;  // map of state vars
    map<string, double*> pars;  // map of params
    void copyVarPar(const CellKernel& toCopy);
};
#endif

//################################################
// This header file contains class definition for
// simulation of excitable cell activity. 
//
// Copyright (C) 2011 Thomas J. Hund.
// Updated 11/21/2012
// Email thomas.hund@osumc.edu
//#################################################

#ifndef MODEL
#define MODEL

#include <cmath>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;


//######################################################
//Define class for parent cell.
//######################################################
class Cell
{
  public:
    Cell();
    ~Cell(); 
  
  //##### Declare class functions ##############
    virtual double updateV();
    virtual void setV(double v);
    virtual void updateCurr();
    virtual void updateConc();
    virtual double tstep(double stimt);
    virtual int externalStim(double stimval);
    //##### Declare class variables ##############
    double vOld;    // Transmembrane potential
    double vNew;
    double t;       // time, ms
    double dt;	  // Time increment
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
    
    double dtmin,dtmed,dtmax,dvcut;
    double apTime;
    
    double RGAS;
    double TEMP;
    double FDAY;

    const char* type;
    
    //##### Declare maps for vars/params ##############
    map<string, double*> vars;  // map of state vars
    map<string, double*> pars;  // map of params
    
};


#endif

//################################################
// This header file contains class definition for
// simulation of excitable cell activity. 
//
// Copyright (C) 2011 Thomas J. Hund.
// Email thomas.hund@osumc.edu
//#################################################

#ifndef MODEL_KERNEL
#define MODEL_KERNEL

#include <cmath>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <memory>

#include "iobase.h"
#include "logger.h"

namespace LongQt {

//######################################################
//Define class for parent cell.
//######################################################
class CellKernel : public std::enable_shared_from_this<CellKernel>
{
  public:
    CellKernel();
    CellKernel(const CellKernel& toCopy);
    virtual ~CellKernel(); 
    
    virtual CellKernel* clone() = 0; //public copy function
  //##### Declare class functions ##############
    virtual double updateV();
    virtual void setV(double v);
    virtual void updateCurr() = 0;
    virtual void updateConc() = 0;
    virtual double tstep(double stimt);
    virtual void externalStim(double stimval);

    //##### Declare class variables ##############
    double vOld;    // Transmembrane potential from previous iteration
//    double vNew;    // new Transmembrane potential in calculation
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
//    double dVdtmax;
    double Rmyo = 150; //Myoplasmic resistivity.
    
    double dtmin,dtmed,dtmax,dvcut;
    double apTime;
    
    double RGAS;
    double TEMP;
    double FDAY;
    
    virtual double var(std::string name);
    virtual bool setVar(std::string name, double val);
    virtual bool hasVar(std::string name);
    virtual double par(std::string name);
    virtual bool setPar(std::string name, double val);
    virtual bool hasPar(std::string name);
    virtual std::set<std::string> vars();
    virtual std::set<std::string> pars();
    virtual const char* type() const = 0;
//    void reset();
    //class options eg ISO
    virtual std::map<std::string,int> optionsMap() const;
    virtual int option() const;
    virtual std::string optionStr() const;
    virtual void setOption(std::string opt);
    virtual void setOption(int opt);

protected:
    std::map<std::string, double*> __vars;  // map of state vars
    std::map<std::string, double*> __pars;  // map of params

    std::list<std::list<int>> conflicts; //list of conflicting options

    virtual int removeConflicts(int opt);
    void copyVarPar(const CellKernel& toCopy);
    virtual void Initialize();
private:
    void mkmap();
};
} //LongQt
#include "cell_kernel.hpp"
#endif

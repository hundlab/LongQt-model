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
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "logger.h"

namespace LongQt {

//######################################################
// Define class for parent cell.
//######################################################
class CellKernel : public std::enable_shared_from_this<CellKernel> {
  using dlim = std::numeric_limits<double>;

 public:
  CellKernel();
  CellKernel(const CellKernel& toCopy);
  virtual ~CellKernel();

  virtual void setup();
  virtual CellKernel* clone() = 0;  // public copy function
  //##### Declare class functions ##############
  virtual double updateV();
  virtual void setV(double v = dlim::quiet_NaN());
  virtual void updateCurr() = 0;
  virtual void updateConc() = 0;
  virtual double tstep(double stimt);
  virtual void externalStim(double stimval);

  //##### Declare class variables ##############
  double vOld = -88.0;  // Transmembrane potential from previous iteration
  double vNew = -88.0;  // Transmembrane potential from current iteration
  //    double vNew;    // new Transmembrane potential in calculation
  double t = 0;          // time, ms
  double dtmin = 0.005;  // ms
  double dtmed = 0.01;   // ms
  double dtmax = 0.1;    // ms
  double dvcut = 1.0;    // mV/ms
  double apTime = 0.0;
  double dt;               // Time increment
  double iNat = 0;         // Total transmembrane sodium current.
  double iKt = 0;          // Total transmembrane potassium current.
  double iCat = 0;         // Total transmembrane calcium current.
  double iTot = 1e-12;     // Total transmembrane current.
  double iTotold = 1e-12;  // Total transmembrane current.

  //##### Declare class params ##############
  double Cm = 1.0;           // Specific membrane capacitance, uF/cm^2
  double Rcg = 1;            // Ratio of capacitive to geometric area.
  double cellRadius = 1e-3;  // cm
  double cellLength = 1e-2;  // cm
  double Vcell;              // Total cell Volume, uL.
  double Vmyo;               //  Myoplasmic volume, uL.
  double AGeo;               // Geometric cell surface area.
  double ACap;               // Capacitive cell surface area.
  double dVdt = 0;
  //    double dVdtmax;
  double Rmyo = 150;  // Myoplasmic resistivity.

  double RGAS = 8314.0;
  double TEMP = 310.0;
  double FDAY = 96487.0;

  virtual double var(std::string name);
  virtual bool setVar(std::string name, double val);
  virtual bool hasVar(std::string name);
  virtual double par(std::string name);
  virtual bool setPar(std::string name, double val);
  virtual bool hasPar(std::string name);
  virtual std::set<std::string> vars();
  virtual std::set<std::string> pars();
  virtual const char* type() const = 0;
  // class options eg ISO
  virtual std::map<std::string, int> optionsMap() const;
  virtual int option() const;
  virtual std::string optionStr() const;
  virtual void setOption(std::string opt);
  virtual void setOption(int opt);

 protected:
  std::map<std::string, double*> __vars;  // map of state vars
  std::map<std::string, double*> __pars;  // map of params

  std::list<std::list<int>> conflicts;  // list of conflicting options

  virtual int removeConflicts(int opt);
  void copyVarPar(const CellKernel& toCopy);

 private:
  void Initialize();
  void mkmap();
};
}  // namespace LongQt
#include "cell_kernel.hpp"
#endif

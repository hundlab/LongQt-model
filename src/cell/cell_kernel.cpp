//####################################################
// This file contains general function definitions
// for simulation of excitable cell activity.
//
// Copyright (C) 2011 Thomas J. Hund.
//####################################################

#include "cell_kernel.h"
#include "cellutils.h"

#include <typeinfo>
using namespace LongQt;
using namespace std;

void CellKernel::Initialize() { this->mkmap(); }

//######################################################
// Constructor for parent cell class - the basis for
// any excitable cell model.
//######################################################
CellKernel::CellKernel() { this->Initialize(); }

//#####################################################
// Constructor for deep copy from annother CellKernel
//#####################################################
CellKernel::CellKernel(const CellKernel& o) { this->Initialize(); }

CellKernel::~CellKernel() {}

// setup variables for simulation
// to be called directly before running simulation
void CellKernel::setup() {
  dt = dtmin;
  Vcell = 1000 * 3.14 * cellRadius * cellRadius * cellLength;
  Vmyo = 0.66 * Vcell;
  AGeo =
      2 * 3.14 * cellRadius * cellRadius + 2 * 3.14 * cellRadius * cellLength;
  ACap = AGeo * Rcg;
  t = 0;
}

// Transmembrane potential
double CellKernel::updateV() {
  vNew = vOld - iTot * dt;

  dVdt = (vNew - vOld) / dt;
  //  if(dVdt>dVdtmax)
  //    dVdtmax=dVdt;
  return vNew;
};

void CellKernel::setV(double v) {
  if (std::isnan(v)) {
    vOld = vNew;
  } else {
    vOld = v;
    vNew = v;
  }
}

// Dynamic time step
double CellKernel::tstep(double stimt) {
  t += dt;

  if ((dVdt >= dvcut) || ((t > stimt - 2.0) && (t < stimt + 10.0)) ||
      (apTime < 5.0)) {
    dt = dtmin;
  } else if (apTime < 40) {
    dt = dtmed;
  } else {
    dt = dtmax;
  }

  this->apTime += dt;

  return t;
};

// External stimulus.
void CellKernel::externalStim(double stimval) {
  iTot += stimval;  // If [ion] change, also should add stimval to
                    // specific ion total (e.g. iKt)
  apTime = 0;
}
double CellKernel::var(string name) { return *__vars.at(name); }
bool CellKernel::setVar(string name, double val) {
  try {
    *__vars.at(name) = val;
  } catch (out_of_range&) {
    Logger::getInstance()->write<out_of_range>("{} not in cell vars", name);
    return false;
  }
  return true;
}

bool CellKernel::hasVar(string name) { return __vars.count(name) > 0; }
double CellKernel::par(string name) { return *__pars.at(name); }
bool CellKernel::setPar(string name, double val) {
  try {
    *__pars.at(name) = val;
  } catch (out_of_range&) {
    Logger::getInstance()->write<out_of_range>("{} not in cell pars", name);
    return false;
  }
  return true;
}

bool CellKernel::hasPar(string name) { return __pars.count(name) > 0; }
set<string> CellKernel::vars() {
  set<string> toReturn;
  for (auto it = __vars.begin(); it != __vars.end(); it++) {
    toReturn.insert(it->first);
  }
  return toReturn;
};

set<string> CellKernel::pars() {
  set<string> toReturn;
  for (auto it = __pars.begin(); it != __pars.end(); it++) {
    toReturn.insert(it->first);
  }
  return toReturn;
}

void CellKernel::insertOpt(string name, bool* valptr, std::string label) {
  __opts[name] = valptr;
}

// const char* CellKernel::name() const
//{
//#if !defined(_WIN32) && !defined(_WIN64)
//    #include <cxxabi.h>
//    int status;
//    return abi::__cxa_demangle(typeid(*this).name(),0,0,&status);
//#else
//    return typeid(*this).name();
//#endif
//}

void CellKernel::copyVarPar(const CellKernel& toCopy) {
  for (auto it : __vars) {
    try {
      *it.second = *toCopy.__vars.at(it.first);
    } catch (const std::out_of_range&) {
      Logger::getInstance()->write("{} not in cell vars", it.first);
    }
  }
  for (auto it : __pars) {
    try {
      *it.second = *toCopy.__pars.at(it.first);
    } catch (const std::out_of_range&) {
      Logger::getInstance()->write("{} not in cell pars", it.first);
    }
  }
  for (auto& opt : __opts) {
    try {
      *opt.second = *toCopy.__opts.at(opt.first);
    } catch (const std::out_of_range&) {
      Logger::getInstance()->write("{} not in cell options", opt.first);
    }
  }
}
void CellKernel::mkmap() {
  // make map of state vars
  CellKernel::insertVar("vOld", &vOld);
  CellKernel::insertVar("t", &t);
  CellKernel::insertVar("dVdt", &dVdt);
  CellKernel::insertVar("iTot", &iTot);
  CellKernel::insertVar("iKt", &iKt);
  CellKernel::insertVar("iNat", &iNat);
  CellKernel::insertVar("iCat", &iCat);

  // make map of params
  CellKernel::insertPar("dtmin", &dtmin);
  CellKernel::insertPar("dtmed", &dtmed);
  CellKernel::insertPar("dtmax", &dtmax);
  CellKernel::insertPar("Cm", &Cm);
  CellKernel::insertPar("Rcg", &Rcg);
  CellKernel::insertPar("RGAS", &RGAS);
  CellKernel::insertPar("TEMP", &TEMP);
  CellKernel::insertPar("FDAY", &FDAY);
  CellKernel::insertPar("cellRadius", &cellRadius);
  CellKernel::insertPar("cellLength", &cellLength);
  CellKernel::insertPar("Vcell", &Vcell);
  CellKernel::insertPar("Vmyo", &Vmyo);
  CellKernel::insertPar("AGeo", &AGeo);
  CellKernel::insertPar("ACap", &ACap);

  // add potenttially needed values to pars
  //    CellKernel::insertPar("vNew",&vNew);
  //    CellKernel::insertPar("dVdtmax",&dVdtmax);
}
/*void CellKernel::reset() {
    int opt = this->option();
    this->Initialize();
    this->setOption(opt);
}*/

map<string, bool> CellKernel::optionsMap() const {
  map<string, bool> optsMap;
  for (auto opt : __opts) {
    optsMap[opt.first] = *opt.second;
  }
  return optsMap;
}

bool CellKernel::option(string name) const {
  try {
    return __opts.at(name);
  } catch (std::out_of_range&) {
    return false;
  }
}

void CellKernel::setOption(string name, bool val) {
  try {
    if (val) {
      auto conflictsList = this->getConflicts(name);
      for (auto& confl : conflictsList) {
        *(__opts[confl]) = false;
      }
    }
    *(__opts.at(name)) = val;

  } catch (std::out_of_range&) {
    return;
  }
}

void CellKernel::insertPar(std::string name, double* valptr) {
  this->__pars[name] = valptr;
}

void CellKernel::insertVar(std::string name, double* valptr) {
  this->__vars[name] = valptr;
}

void CellKernel::insertConflicts(std::list<std::string> conflicts) {
  std::set<std::string> conflSet;
  for (auto& conflict : conflicts) {
    if (this->__opts.count(conflict) > 0) {
      conflSet.insert(conflict);
    }
  }
  this->__conflicts.push_back(conflSet);
}

std::list<std::string> CellKernel::getConflicts(std::string name) {
  std::list<std::string> conflictsList;
  for (auto& conflSet : this->__conflicts) {
    if (conflSet.count(name) > 0) {
      for (auto& conflItem : conflSet) {
        if (this->__opts.count(conflItem) > 0) {
          conflictsList.push_back(conflItem);
        }
      }
    }
  }
  return conflictsList;
}

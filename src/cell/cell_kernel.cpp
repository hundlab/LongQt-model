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

void CellKernel::Initialize() {
    //##### Assign default parameters ##################
    dtmin = 0.005;  // ms
    dtmed = 0.01;
    dtmax = 0.1;
    dvcut = 1.0;    // mV/ms
    Cm = 1.0;  // uF/cm^s
    Rcg = 1.0;
    cellRadius = 0.001; // cm
    cellLength = 0.01;  // cm
    RGAS = 8314.0;
    TEMP = 310.0;
    FDAY = 96487.0;

    //##### Initialize variables ##################
    dVdt=/*dVdtmax=*/0.0;
    t=0.0;
    dt=dtmin;
    vOld = /*vNew =*/  -88.0;
    iTot = iTotold = 0.000000000001;
    iNat = iKt = iCat = 0.0;
    this->mkmap();
}

//######################################################
// Constructor for parent cell class - the basis for 
// any excitable cell model.  
//######################################################
CellKernel::CellKernel() {
    this->Initialize();
};

//#####################################################
//Constructor for deep copy from annother CellKernel
//#####################################################
CellKernel::CellKernel(const CellKernel& toCopy)
{
    this->Initialize();
};

CellKernel::~CellKernel() {
}

// Transmembrane potential 
double CellKernel::updateV()
{
  double vNew=vOld-iTot*dt;
  
  dVdt=(vNew-vOld)/dt;
//  if(dVdt>dVdtmax)
//    dVdtmax=dVdt;
  return vNew;
};

void CellKernel::setV(double v)
{
//  vNew=v;
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

  return t;
};

// External stimulus.
void CellKernel::externalStim(double stimval)
{
    iTot = iTot+stimval;   // If [ion] change, also should add stimval to specific ion total (e.g. iKt)
}
double CellKernel::var(string name) {
    return *__vars.at(name);
}
bool CellKernel::setVar(string name, double val) {
    try {
        *__vars.at(name) = val;
    } catch(out_of_range&) {
        Logger::getInstance()->write<out_of_range>("{} not in cell vars", name);
        return false;
    }
    return true;
}

bool CellKernel::hasVar(string name)
{
    return __vars.count(name) > 0;
}
double CellKernel::par(string name) {
    return *__pars.at(name);
}
bool CellKernel::setPar(string name, double val) {
    try {
        *__pars.at(name) = val;
    } catch(out_of_range&) {
        Logger::getInstance()->write<out_of_range>("{} not in cell pars", name);
        return false;
    }
    return true;
}

bool CellKernel::hasPar(string name)
{
    return __pars.count(name) > 0;
}
set<string> CellKernel::vars() {
    set<string> toReturn;
    for(auto it = __vars.begin(); it != __vars.end(); it++) {
        toReturn.insert(it->first);
    }
    return toReturn;
};

set<string> CellKernel::pars() {
    set<string> toReturn;
    for(auto it = __pars.begin(); it != __pars.end(); it++) {
        toReturn.insert(it->first);
    }
    return toReturn;
}

//const char* CellKernel::name() const
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
    for(auto it : __vars) {
        try {
            *it.second = *toCopy.__vars.at(it.first);
        } catch(const std::out_of_range&) {
            Logger::getInstance()->write("{} not in cell vars", it.first);
        }
    }
    for(auto it : __pars) {
        try {
            *it.second = *toCopy.__pars.at(it.first);
        } catch(const std::out_of_range&) {
            Logger::getInstance()->write("{} not in cell pars", it.first);
        }
    }
    this->setOption(toCopy.option());
}
void CellKernel::mkmap() {
    // make map of state vars
    __vars["vOld"]=&vOld;
    __vars["t"]=&t;
    __vars["dVdt"]=&dVdt;
    __vars["iTot"]=&iTot;
    __vars["iKt"]=&iKt;
    __vars["iNat"]=&iNat;
    __vars["iCat"]=&iCat;
    
    // make map of params
    __pars["dtmin"]=&dtmin;
    __pars["dtmed"]=&dtmed;
    __pars["dtmax"]=&dtmax;
    __pars["Cm"]=&Cm;
    __pars["Rcg"]=&Rcg;
    __pars["RGAS"]=&RGAS;
    __pars["TEMP"]=&TEMP;
    __pars["FDAY"]=&FDAY;
    __pars["cellRadius"]=&cellRadius;
    __pars["cellLength"]=&cellLength;
    __pars["Vcell"]=&Vcell;
    __pars["Vmyo"]=&Vmyo;
    __pars["AGeo"]=&AGeo;
    __pars["ACap"]=&ACap;
    
		//add potenttially needed values to pars
//    __pars["vNew"]=&vNew;
//    __pars["dVdtmax"]=&dVdtmax;
}
/*void CellKernel::reset() {
    int opt = this->option();
    this->Initialize();
    this->setOption(opt);
}*/

map<string, int> CellKernel::optionsMap() const
{
    return {{"WT",0}};
}

int CellKernel::option() const
{
    return 0;
}

string CellKernel::optionStr() const
{
    return "WT";
}

void CellKernel::setOption(string){}

void CellKernel::setOption(int){}

int CellKernel::removeConflicts(int opt)
{
    int finalOpt = opt;
    for(auto& cOptList: this->conflicts) {
        int first = 0;
        for(auto& cOpt: cOptList) {
            if(!first && (cOpt&finalOpt)) {
                first = cOpt;
            }
            finalOpt &= ~cOpt; //remove cOpt
        }
        finalOpt |= first; //add the first cOpt from conflicts back
    }
    return finalOpt;
}

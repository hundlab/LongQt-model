//####################################################
// This file contains general function definitions 
// for simulation of excitable cell activity.
//
// Copyright (C) 2011 Thomas J. Hund.
//####################################################

#include "cell_kernel.h"

#include <typeinfo>
#include <QDebug>

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
    dVdt=dVdtmax=0.0;
    t=0.0;
    dt=dtmin;
    vOld = vNew =  -88.0;
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

  return t;
};

// External stimulus.
int CellKernel::externalStim(double stimval)
{
    iTot = iTot+stimval;   // If [ion] change, also should add stimval to specific ion total (e.g. iKt)
    return 1;
}
double CellKernel::var(string name) {
    return *vars.at(name);
}
bool CellKernel::setVar(string name, double val) {
    try {
        *vars.at(name) = val;
    } catch(out_of_range&) {
        qDebug("%s not in cell vars", name.c_str());
        return false;
    }
    return true;
}

bool CellKernel::hasVar(string name)
{
    return vars.count(name) > 0;
}
double CellKernel::par(string name) {
    return *pars.at(name);
}
bool CellKernel::setPar(string name, double val) {
    try {
        *pars.at(name) = val;
    } catch(out_of_range&) {
        qDebug("%s not in cell pars", name.c_str());
        return false;
    }
    return true;
}

bool CellKernel::hasPar(string name)
{
    return pars.count(name) > 0;
}
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
    for(auto it : vars) {
        try {
            *it.second = *toCopy.vars.at(it.first);
        } catch(const std::out_of_range&) {
            qDebug("%s not in cell vars", it.first.c_str());
        }
    }
    for(auto it : pars) {
        try {
            *it.second = *toCopy.pars.at(it.first);
        } catch(const std::out_of_range&) {
            qDebug("%s not in cell pars", it.first.c_str());
        }
    }
    this->setOption(toCopy.option());
}
void CellKernel::mkmap() {
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
    
		//add potenttially needed values to pars
    pars["vNew"]=&vNew;
    pars["dVdtmax"]=&dVdtmax;
}
void CellKernel::reset() {
    int opt = this->option();
    this->Initialize();
    this->setOption(opt);
}

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

vector<string> CellKernel::split(string s, char delim) {
    vector<string> v;
    size_t prev_pos = 0;
    size_t pos = s.find(delim,prev_pos);
    std::string token;
    while(pos != std::string::npos) {
        token = s.substr(prev_pos, pos-prev_pos);
        if(token != "") {
            v.push_back(token);
        }
        prev_pos = pos+1;
        pos = s.find(delim,prev_pos);
    }
    token = s.substr(prev_pos);
    if(token != "") {
        v.push_back(token);
    }
    return v;
}

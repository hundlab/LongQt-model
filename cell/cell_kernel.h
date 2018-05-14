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
#include <memory>

#include "iobase.h"

using namespace std;


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
    virtual int externalStim(double stimval);

    //##### Declare class variables ##############
    double vOld;    // Transmembrane potential from previous iteration
    double vNew;    // new Transmembrane potential in calculation
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
    double Rmyo = 150; //Myoplasmic resistivity.
    
    double dtmin,dtmed,dtmax,dvcut;
    double apTime;
    
    double RGAS;
    double TEMP;
    double FDAY;
    
    //##### Declare maps for vars/params ##############
    map<string, double*> vars;  // map of state vars
    map<string, double*> pars;  // map of params
    virtual double var(string name);
    virtual bool setVar(string name, double val);
    virtual double par(string name);
    virtual bool setPar(string name, double val);
    virtual set<string> getVariables();
    virtual set<string> getConstants();
    virtual const char* type() const = 0;
    void reset();
    //class options eg ISO
    virtual map<string,int> optionsMap() const;
    virtual int option() const;
    virtual string optionStr() const;
    virtual void setOption(string opt);
    virtual void setOption(int opt);

protected:
    void copyVarPar(const CellKernel& toCopy);
    virtual void Initialize();
private:
    void mkmap();
};


//Macro to create a flag enum with a map of its values used by subclasses to create
//Options enum
#define EXPAND(x) x
// NUM_ARGS(...) evaluates to the literal number of the passed-in arguments.
#define _GET_NTH_ARG(_1, _2, _3, _4, _5, _6, _7, _9, _10, N, ...) N
#define NUM_ARGS(...) EXPAND(_GET_NTH_ARG(__VA_ARGS__,9,8,7,6,5,4,3,2,1,0))

#define CLASS_FUNCTIONS \
virtual map<string,int> optionsMap() const;\
virtual int option() const;\
virtual string optionStr() const;\
virtual void setOption(string opt);\
virtual void setOption(int opt);

#define REVERSE_1(a) a
#define REVERSE_2(a,b) b,a
#define REVERSE_3(a,...) EXPAND(REVERSE_2(__VA_ARGS__)),a
#define REVERSE_4(a,...) EXPAND(REVERSE_3(__VA_ARGS__)),a
#define REVERSE_5(a,...) EXPAND(REVERSE_4(__VA_ARGS__)),a
#define REVERSE_6(a,...) EXPAND(REVERSE_5(__VA_ARGS__)),a
#define REVERSE_7(a,...) EXPAND(REVERSE_6(__VA_ARGS__)),a
#define REVERSE_8(a,...) EXPAND(REVERSE_7(__VA_ARGS__)),a
#define REVERSE_9(a,...) EXPAND(REVERSE_8(__VA_ARGS__)),a
#define REVERSE_10(a,...) EXPAND(REVERSE_9(__VA_ARGS__)),a
#define REVERSE1(N,...) EXPAND(REVERSE_ ## N(__VA_ARGS__))
#define REVERSE(N, ...) REVERSE1(N, __VA_ARGS__)


#define EN_STR_0() WT = 0
#define EN_STR_1(str) str = 1 << 0, EN_STR_0()
#define EN_STR_2(str,...) str = 1 << 1, EXPAND(EN_STR_1(__VA_ARGS__))
#define EN_STR_3(str,...) str = 1 << 2, EXPAND(EN_STR_2(__VA_ARGS__))
#define EN_STR_4(str,...) str = 1 << 3, EXPAND(EN_STR_3(__VA_ARGS__))
#define EN_STR_5(str,...) str = 1 << 4, EXPAND(EN_STR_4(__VA_ARGS__))
#define EN_STR_6(str,...) str = 1 << 5, EXPAND(EN_STR_5(__VA_ARGS__))
#define EN_STR_7(str,...) str = 1 << 6, EXPAND(EN_STR_6(__VA_ARGS__))
#define EN_STR_8(str,...) str = 1 << 7, EXPAND(EN_STR_7(__VA_ARGS__))
#define EN_STR_9(str,...) str = 1 << 8, EXPAND(EN_STR_8(__VA_ARGS__))
#define EN_STR_10(str,...) str = 1 << 9, EXPAND(EN_STR_9(__VA_ARGS__))

#define MP_STR_0() {"WT",0}
#define MP_STR_1(str) {#str, 1 << 0}, MP_STR_0()
#define MP_STR_2(str,...) {#str, 1 << 1}, EXPAND(MP_STR_1(__VA_ARGS__))
#define MP_STR_3(str,...) {#str, 1 << 2}, EXPAND(MP_STR_2(__VA_ARGS__))
#define MP_STR_4(str,...) {#str, 1 << 3}, EXPAND(MP_STR_3(__VA_ARGS__))
#define MP_STR_5(str,...) {#str, 1 << 4}, EXPAND(MP_STR_4(__VA_ARGS__))
#define MP_STR_6(str,...) {#str, 1 << 5}, EXPAND(MP_STR_5(__VA_ARGS__))
#define MP_STR_7(str,...) {#str, 1 << 6}, EXPAND(MP_STR_6(__VA_ARGS__))
#define MP_STR_8(str,...) {#str, 1 << 7}, EXPAND(MP_STR_7(__VA_ARGS__))
#define MP_STR_9(str,...) {#str, 1 << 8}, EXPAND(MP_STR_8(__VA_ARGS__))
#define MP_STR_10(str,...) {#str, 1 << 9}, EXPAND(MP_STR_9(__VA_ARGS__))


#define _MAKE_MAP1(N, ...) map<string,int> optsMap = {EXPAND(MP_STR_ ## N(__VA_ARGS__))};
#define _MAKE_MAP(N, ...) _MAKE_MAP1(N, __VA_ARGS__)

#define _MAKE_ENUM1(N, ...) enum Options {EXPAND(EN_STR_ ## N(__VA_ARGS__))}; CLASS_FUNCTIONS
#define _MAKE_ENUM(N, ...) _MAKE_ENUM1(N, __VA_ARGS__)

#define _MAKE_OPTIONS1(N, ...) _MAKE_ENUM(N,__VA_ARGS__) _MAKE_MAP(N,__VA_ARGS__)
#define _MAKE_OPTIONS(N, ...) _MAKE_OPTIONS1(N,REVERSE(N,__VA_ARGS__))
//The function to call will set up enum Options
#define MAKE_OPTIONS(...) _MAKE_OPTIONS(NUM_ARGS(__VA_ARGS__),__VA_ARGS__)


#define OPERATORS(CLASS_NAME) \
inline CLASS_NAME::Options operator~ (CLASS_NAME::Options a) { return (CLASS_NAME::Options)~(int)a; }\
inline CLASS_NAME::Options operator| (CLASS_NAME::Options a, CLASS_NAME::Options b) { return (CLASS_NAME::Options)((int)a | (int)b); }\
inline CLASS_NAME::Options operator& (CLASS_NAME::Options a, CLASS_NAME::Options b) { return (CLASS_NAME::Options)((int)a & (int)b); }\
inline CLASS_NAME::Options operator^ (CLASS_NAME::Options a, CLASS_NAME::Options b) { return (CLASS_NAME::Options)((int)a ^ (int)b); }\
inline CLASS_NAME::Options& operator|= (CLASS_NAME::Options& a, CLASS_NAME::Options b) { return (CLASS_NAME::Options&)((int&)a |= (int)b); }\
inline CLASS_NAME::Options& operator&= (CLASS_NAME::Options& a, CLASS_NAME::Options b) { return (CLASS_NAME::Options&)((int&)a &= (int)b); }\
inline CLASS_NAME::Options& operator^= (CLASS_NAME::Options& a, CLASS_NAME::Options b) { return (CLASS_NAME::Options&)((int&)a ^= (int)b); }

#define OPTIONS_FUNCTIONS(CLASS_NAME) \
map<string, int> CLASS_NAME::optionsMap() const\
{\
    return optsMap;\
}\
int CLASS_NAME::option() const\
{\
    return opts;\
}\
string CLASS_NAME::optionStr() const\
{\
    for (auto& it: optsMap) {\
        if (it.second == this->opts) {\
            return it.first;\
        }\
    }\
    return "";\
}\
vector<string> split(string s, char delim) {\
    vector<string> v;\
    size_t pos = 0;\
    size_t prev_pos = 0;\
    std::string token;\
    while((pos = s.find(delim,prev_pos)) != std::string::npos) {\
        token = s.substr(prev_pos, pos);\
        v.push_back(token);\
        prev_pos = pos;\
    }\
    token = s.substr(prev_pos);\
    v.push_back(token);\
    return v;\
}\
void CLASS_NAME::setOption(string opt)\
{\
    Options o = WT;\
    auto splits = split(opt,'|');\
    for(auto& sp: splits) {\
        o |= static_cast<Options>(this->optsMap[sp]);\
    }\
    opts = o;\
}\
void CLASS_NAME::setOption(int opt)\
{\
    opts = static_cast<Options>(opt);\
}

#define MAKE_OPTIONS_FUNCTIONS(CLASS_NAME) OPERATORS(CLASS_NAME) OPTIONS_FUNCTIONS(CLASS_NAME)
#endif

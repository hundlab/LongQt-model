//################################################
// This header file contains protocol class
// definition for simulation of excitable cell activity.
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015
// Email thomas.hund@osumc.edu
//#################################################

#ifndef PROTO
#define PROTO


#include "kurata08.h"
#include <sstream>
#include <random>


//######################################################
//Define class for protocol.
//######################################################
class Output;  // forward declare Output class for use in Protocol
class Measure; // forward declare Measure class for use in Protocol

class Protocol
{
  public:
    Protocol();
    ~Protocol();
  
    //##### Declare class functions ##############
    virtual int stim();
    virtual int assign_cell_pars(std::vector<std::string> pnames, std::vector< std::vector<std::string> > pvals, int trialnum);
    virtual std::map<std::string, double*> resizemap(std::map<std::string,double*> varmap, std::string file);
    virtual std::map<std::string, double*> resizemap(std::map<std::string,double*> varmap, std::vector<std::string> names);
    virtual std::tuple< std::vector<std::string>, std::vector< std::vector<std::string> > > parse2Dmap(std::map<std::string,double*> varmap,std::map<std::string,double*> varmap2, std::string file);
    virtual std::tuple< std::vector<std::string>, std::vector< std::vector<std::string> > > parsemixedmap(std::map<std::string,double*> varmap, std::string file);
    virtual std::map<std::string,double> copymapvals(std::map<std::string, double*> varmap);
    virtual int map2screen(std::map<std::string, double*> varmap);
    virtual int map2screen(std::map<std::string, double> varmap);


    //##### Declare class variables ##############
    Cell* cell;        // pointer to cell class
    Output* douts;     // point to output class for writing data
    Output *ics;        // Output for saving SVs at end of sim.
    Measure* measures; // pointer to measure class for measuring SV props.
    double vM;         // membrane potential, mV
    double time;       // time, ms
    //##### Declare class params ##############
    double bcl,stimval,stimdur,stimt, numstims;   //variables for pacing.
    double meastime,writetime;
    double writeint;
    double doneflag;
    double readflag,saveflag,writeflag,measflag,paceflag;
    double numtrials;
    double stimflag, stimcounter;
    
    double tMax;
    double maxdoutsize,maxmeassize;
    
    int myId;  // used to identify threaded protos

    std::default_random_engine generator;
    
    std::string readfile,savefile,dvarfile,pvarfile, measfile, simvarfile;
    
    //##### Declare maps for vars/params ##############
    std::map<std::string, double*> pars;  // map of params
    
};

//#############################################################
// Declare and define Output class used to read and write
// values of variables supplied as argument (varmap).
//#############################################################
class Output
{
public:
    Output();
    ~Output();
    
    std::ofstream ofile;
    int counter;
    int interval;   //write to file when counter%interval==0
    
    virtual int writevals(std::map<std::string, double*> varmap, std::string file, char type);
    virtual int writevals(std::map<std::string, double*> varmap, std::string file);
    virtual int writevals(std::map<std::string, double> varmap, std::string file, char type);
    virtual int writevals(std::map<std::string, double> varmap, std::string file);
};

//#############################################################
// Declare and define Measure class used to track properties
// (e.g. peak, min, duration) of specified state variable.
//#############################################################

class Measure
{
public:
    Measure();
    ~Measure();
    
    double* var;
    double varold;
    double vartakeoff; //var value at point of max deflection.
    double told;
    double mint;    //time of min value.
    double maxt;    //time of max value.
    double durtime1;
    double derivold; //dv/dt from prev. time step
    double derivt;   // time of max deriv.
    double derivt1;  // time of prev. cycle max deriv.
    double derivt2;
    double deriv2ndt;  // time of max 2nd deriv.
    double peak;      // max var value
    double min;       // min var value
    double amp;
    double ddr;      //diastolic depol. rate
    double maxderiv;
    double maxderiv1;
    double maxderiv2;
    double maxderiv2nd;
    double cl;
    double dur;   //duration
    double percrepol;   //specify percent repolarization
    double repol;           // repol var val for duration measure.
    int minflag;
    int maxflag;
    int durflag;    //1 while measuring duration.
    int ampflag;
    int ddrflag;
    int returnflag;
    
    std::string varname;
    
    int measure(double time,double var);  //measures props related to var; returns 1 when ready for output.
    void reset();   //resets params to init vals
    std::map<std::string, double*> varmap; // map for refing properties that can be measured.
    std::map<std::string, double*> datamap; // map for refing properties that will be output.
    
};


#endif

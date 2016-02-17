//################################################
// This header file contains protocol class
// declaration for simulation of excitable cell activity.
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015
// Email thomas.hund@osumc.edu
//#################################################

#ifndef PROTO
#define PROTO

#include <sstream>
#include <random>
#include <exception>
#include <string>

#include "kurata08.h"
#include "measure.h"
#include "output.h"

using namespace std;

class Protocol
{
  public:
    Protocol();
    Protocol(const Protocol& toCopy);
    ~Protocol();
  
    //##### Declare class functions ##############
    virtual int stim();
    virtual int assign_cell_pars(vector<string> pnames, vector< vector<string> > pvals, int trialnum);
    virtual map<string, double*> resizemap(map<string,double*> varmap, string file);
    virtual map<string, double*> resizemap(map<string,double*> varmap, vector<string> names);
    virtual tuple< vector<string>, vector< vector<string> > > parse2Dmap(map<string,double*> varmap,map<string,double*> varmap2, string file);
    virtual tuple< vector<string>, vector< vector<string> > > parsemixedmap(map<string,double*> varmap, string file);
    virtual map<string,double> copymapvals(map<string, double*> varmap);
    virtual int map2screen(map<string, double*> varmap);
    virtual int map2screen(map<string, double> varmap);
    virtual int initializeMeasure(int measureSize);
    virtual bool readMvars(string file);
    virtual int getNeededDOutputSize(); //get the size needed to construct the output array
    virtual int runSim();
    virtual bool runTrial();
    virtual int readpars(map<string, double*> varmap, string file);
    virtual int parsemixedmap(map<string,double*> varmap, string file, vector<string>* cnames, vector<vector<string>>* twoDrnames);
    virtual int parse2Dmap(map<string,double*> varmap,map<string,double*> varmap2, string file, vector<string>* vnames, vector< vector<string> >* twoDmnames);
    virtual int resizemap(map<string,double*> varmap, string file, map<string, double*>* vars);
    virtual int readpvars();
    virtual bool writepars(map<string, double*> varmap, string file); //write the contence of varmap to a file
    virtual bool writedvars(map<string, double*> varmap, string file); //write varmap keys to a file
    virtual bool write2Dmap(vector<string> vnames, vector< vector<string> > twoDmnames, string file);
    virtual void setTrial(unsigned int current_trial);
    virtual unsigned int getTrial();

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
    int writestd;    
    double tMax;
    double maxdoutsize,maxmeassize;
    
    int myId;  // used to identify threaded protos

    default_random_engine generator;
    
    string readfile,savefile,dvarfile,pvarfile, measfile, simvarfile, propertyoutfile, dvarsoutfile, finalpropertyoutfile, finaldvarsoutfile;


    vector<string> pnames;              // stores cell param names
    vector< vector<string> > pvals;     // stores cell param vals
    vector<string> mvnames;     // vector of var names to be measured (e.g. Vm, Cai)
    vector< vector<string> > mpnames;   // vector of property names to measure (e.g. dur, min)
 
    
    //##### Declare maps for vars/params ##############
    map<string, double*> pars;  // map of params
    map<string, double*> parmap; // map for output params
    map<string, double*> datamap; // map for output state vars
    map<string, double> *tempvals;   // map to store measure vals
  
    private:
    unsigned int trial;
};

#endif

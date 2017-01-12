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
#include <list>
#include <unordered_map>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include "measure.h"
#include "cellUtils.h"

using namespace std;

struct GetSetRef {
    function<string(void)> get;
    function<void(const string&)> set;
    string type;
    GetSetRef Initialize(string type, function<string(void)> get, function<void(const string&)> set) {
        this->type = type;
        this->get = get;
        this->set = set;
        return *this;
    }
};

class Protocol
{
  public:
    Protocol();
    Protocol(const Protocol& toCopy);
    Protocol(Protocol&& toCopy);
    Protocol& operator=(const Protocol& toCopy);
    virtual Protocol* clone();
    ~Protocol();
  
    //##### Declare class functions ##############
    virtual int assign_cell_pars(vector<string> pnames, vector< vector<string> > pvals, int trialnum);
    virtual int runSim();
    virtual bool runTrial();
    virtual int readpars(QXmlStreamReader& xml, set<string> varnames = {});
    virtual int readpars(string file, set<string> varnames = {});
    virtual int parsemixedmap(map<string,double*> varmap, string file, vector<string>* cnames, vector<vector<string>>* twoDrnames);
    virtual int readpvars();
    virtual bool writepars(string file); //write the contence of pars to a file
    virtual bool writepars(QXmlStreamWriter& xml); //write the contence of pars to a file
    virtual bool writedvars(string file); //write varmap keys to a file
    virtual bool readdvars(string file);
    virtual bool write2Dmap(vector<string> vnames, vector< vector<string> > twoDmnames, string file);
    virtual void setTrial(unsigned int current_trial);
    virtual unsigned int getTrial();
    virtual bool writeMVarsFile(string file);
    virtual bool readMvarsFile(string filename);
    virtual bool setCell(const string& type, bool reset = false);
    virtual list<string> cellOptions();

    //#### Declare Static helper functions #####
    static string to_string(const bool& b);
    static bool stob(const string& s);


    //##### Declare class variables ##############
    Cell* cell;        // pointer to cell class
    double vM;         // membrane potential, mV
    double time;       // time, ms
    //##### Declare class params ##############
    double meastime,writetime;
    double writeint;
    double doneflag;
    bool writeflag,measflag;
    bool writeCellState, readCellState;
    int numtrials;
    int writestd;    
    double tMax;
    
    default_random_engine generator;
    
    string readfile,savefile,dvarfile,pvarfile, measfile, simvarfile, propertyoutfile, dvarsoutfile, finalpropertyoutfile, finaldvarsoutfile, cellStateFile;

    string datadir;
    string cellStateDir;

    vector<string> pnames;              // stores cell param names
    vector< vector<string> > pvals;     // stores cell param vals
 
    
    //##### Declare maps for vars/params ##############
    map<string, GetSetRef> pars;

    const map<string,Measure>& Measures = cref(measures);
    virtual bool addMeasure(Measure toInsert);
    virtual void removeMeasure(string measureName);
    virtual bool setMeasures(map<string,Measure> newMeasures);
    virtual bool addToMeasreSelection(string measureName, string property);
    virtual void removeFromMeasureSelection(string measureName, string property);

    map<string, CellInitializer> cellMap;

    protected:
    void copy(const Protocol& toCopy);    
    map<string,Measure> measures; // set of measure class for measuring SV props.
    int trial;
};

#endif

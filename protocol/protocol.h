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
#include <exception>
#include <string>
#include <list>
#include <functional>
#include <memory>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include "measuremanager.h"
#include "cellutils.h"
#include "cellpvars.h"

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
        virtual Protocol* clone() = 0;
        virtual ~Protocol();

        //##### Declare class functions ##############
        virtual int runSim();
        virtual bool runTrial() = 0;
        virtual void setupTrial();
        virtual int readpars(QXmlStreamReader& xml);
        virtual bool writepars(QXmlStreamWriter& xml); //write the contents of pars to a file
        virtual void setTrial(unsigned int current_trial);
        virtual unsigned int getTrial();
        virtual bool setCell(const string& type, bool reset = false);
        virtual list<string> cellOptions();
        virtual void readInCellState(bool read);
        virtual void writeOutCellState(bool write);

        //##### Declare class variables ##############
        Cell* cell = 0;        // pointer to cell class
        double vM;         // membrane potential, mV
        double time;       // time, ms
        const char* type = "protocol";
        //##### Declare class params ##############
        double meastime,writetime;
        double writeint;
        double doneflag;
        bool writeflag,measflag;
        bool writeCellState, readCellState;
        int numtrials;
        int writestd;
        double tMax;

        string readfile,savefile,dvarfile,pvarfile, measfile, simvarfile,
               propertyoutfile, dvarsoutfile, finalpropertyoutfile,
               finaldvarsoutfile, cellStateFile;

        string datadir;
        string cellStateDir;

        unique_ptr<CellPvars> pvars;

        //##### Declare maps for vars/params ##############
        map<string, GetSetRef> pars;

        map<string, CellUtils::CellInitializer> cellMap;
        unique_ptr<MeasureManager> measureMgr; // set of measure class for measuring SV props.

    protected:
        void copy(const Protocol& toCopy);
        int trial;
};

#endif

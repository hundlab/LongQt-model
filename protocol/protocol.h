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
        virtual int readpars(QXmlStreamReader& xml);
        virtual bool writepars(QXmlStreamWriter& xml); //write the contents of pars to a file
        virtual void readInCellState(bool read);
        virtual void writeOutCellState(bool write);

        virtual void trial(unsigned int current_trial);
        virtual unsigned int trial() const;
        virtual bool runTrial() = 0;
        virtual void setupTrial();


        virtual bool cell(const string& type);
        virtual void cell(Cell* cell) = 0;
        virtual Cell* cell() const = 0;
        virtual list<string> cellOptions();

        virtual CellPvars& pvars() = 0;

        virtual MeasureManager& measureMgr() = 0;

        //##### Declare class variables ##############
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


        //##### Declare maps for vars/params ##############
        map<string, GetSetRef> pars;

protected:
        void copy(const Protocol& toCopy);
        int __trial;
};

#endif

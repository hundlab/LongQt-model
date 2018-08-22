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
#include <QDir>

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

class Protocol :  public std::enable_shared_from_this<Protocol>
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
        void setDataDir(string location = "");
        void mkDirs();
        string getDataDir();

        virtual bool cell(const string& type);
        virtual void cell(shared_ptr<Cell> cell) = 0;
        virtual shared_ptr<Cell> cell() const = 0;
        virtual list<string> cellOptions();

        virtual CellPvars& pvars() = 0;

        virtual MeasureManager& measureMgr() = 0;


        //##### Declare class variables ##############
        double vM;         // membrane potential, mV
        double time;       // time, ms
        virtual const char* type() const = 0;
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

        QDir basedir;
        QDir datadir;
        QDir cellStateDir;

        void setRunBefore(function<void(Protocol&)>);
        function<void(Protocol&)> getRunBefore();
        void setRunDuring(function<void(Protocol&)>);
        function<void(Protocol&)> getRunDuring();
        int numruns = 0;
        double firstRun = 0;
        double runEvery = 0;
        void setRunAfter(function<void(Protocol&)>);
        function<void(Protocol&)> getRunAfter();

        //##### Declare maps for vars/params ##############
        map<string, GetSetRef> pars;

protected:
        void copy(const Protocol& toCopy);
        int __trial;
        function<void(Protocol&)> runBefore;
        function<void(Protocol&)> runDuring;
        function<void(Protocol&)> runAfter;
private:
        void mkmap();
};

#endif

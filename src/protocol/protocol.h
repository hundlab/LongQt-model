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
#include "pvarscell.h"
namespace LongQt {

struct GetSetRef {
    std::function<std::string(void)> get;
    std::function<void(const std::string&)> set;
    std::string type;
    GetSetRef Initialize(std::string type, std::function<std::string(void)> get, std::function<void(const std::string&)> set) {
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
        void setDataDir(std::string location = "", std::string basedir = "", std::string appendtxt = "");
        void mkDirs();
        std::string getDataDir();

        virtual bool cell(const std::string& type);
        virtual void cell(std::shared_ptr<Cell> cell) = 0;
        virtual std::shared_ptr<Cell> cell() const = 0;
        virtual std::list<std::string> cellOptions();

        virtual PvarsCell& pvars() = 0;

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

        std::string readfile,savefile,dvarfile,pvarfile, measfile, simvarfile,
               propertyoutfile, dvarsoutfile, finalpropertyoutfile,
               finaldvarsoutfile, cellStateFile;

        QDir basedir;
        QDir datadir;
        QDir cellStateDir;

        void setRunBefore(std::function<void(Protocol&)>);
        std::function<void(Protocol&)> getRunBefore();
        void setRunDuring(std::function<void(Protocol&)>);
        std::function<void(Protocol&)> getRunDuring();
        int numruns = 0;
        double firstRun = 0;
        double runEvery = 0;
        void setRunAfter(std::function<void(Protocol&)>);
        std::function<void(Protocol&)> getRunAfter();

        //##### Declare maps for vars/params ##############
        std::map<std::string, GetSetRef> pars;

protected:
        void copy(const Protocol& toCopy);
        int __trial;
        std::function<void(Protocol&)> runBefore;
        std::function<void(Protocol&)> runDuring;
        std::function<void(Protocol&)> runAfter;
private:
        void mkmap();
};
} //LongQt

#endif

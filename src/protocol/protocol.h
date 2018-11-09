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

        virtual void trial(unsigned int current_trial);
        virtual unsigned int trial() const;
        virtual bool runTrial() = 0;
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

        virtual const char* type() const = 0;
        //##### Declare class params ##############
        double meastime,writetime;
        int writeint;
        bool writeflag,measflag;
        bool writeCellState, readCellState;
        int numtrials;
        double tMax;

        std::string readfile,savefile,dvarfile,pvarfile, measfile, simvarfile,
               propertyoutfile, dvarsoutfile, finalpropertyoutfile,
               finaldvarsoutfile, cellStateFile;

        QDir basedir;
        QDir datadir;
        QDir cellStateDir;

        void setRunBefore(std::function<void(Protocol&)>);
        void setRunDuring(std::function<void(Protocol&)>, double firstRun = -1, double runEvery = -1, int numruns = -1);
        void setRunAfter(std::function<void(Protocol&)>);
        int numruns = 0;
        double firstRun = 0;
        double runEvery = 0;

        std::string parsStr(std::string name);
        void parsStr(std::string name, std::string val);
        std::string parsType(std::string name);
        std::list<std::pair<std::string,std::string>> parsList();


protected:
        void copy(const Protocol& toCopy);

        std::map<std::string, GetSetRef> __pars;

        virtual void readInCellState(bool read);
        virtual void writeOutCellState(bool write);
        virtual void setupTrial();

        int __trial;
        std::function<void(Protocol&)> runBefore;
        std::function<void(Protocol&)> runDuring;
        std::function<void(Protocol&)> runAfter;

        bool runflag; //fix to bool
private:
        void mkmap();
};
} //LongQt

#endif

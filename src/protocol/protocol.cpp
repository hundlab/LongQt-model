//################################################
// This code file contains protocol class
// definition for simulation of excitable cell activity.
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015
// Email thomas.hund@osumc.edu
//#################################################

#include <cstring>

#include "protocol.h"
#include "hrd09.h"
#include "cellutils.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>
#include <QDate>
#include <QTime>
#include <QStandardPaths>

#if defined(_WIN32) || defined(_WIN64)
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif
using namespace LongQt;
using namespace std;

//######################################################
// Default Constructor
//######################################################
Protocol::Protocol()
{
    //##### Assign default parameters ##################
    runflag = true;       // set to 0 to end simulation

    tMax = 10000;   // max simulation time, ms


    readfile = "r.txt"; // File to read SV ICs
    savefile = "s.txt"; // File to save final SV

    __trial = 0;

    writeflag = 1;      // 1 to write data to file during sim
    dvarfile = "dvars.txt";  // File with SV to write.
    writetime = 0;      // time to start writing.
    writeint = 20;     // interval for writing.

    pvarfile = "pvars.txt"; // File to specify cell params
    simvarfile = "simvars.xml";  // File to specify sim params

    propertyoutfile = "dt%d.tsv";
    dvarsoutfile = "dt%d_dvars.tsv";
    finalpropertyoutfile = "dss%d.tsv";
    finaldvarsoutfile = "dss%d_pvars.tsv";
    cellStateFile = "cellstate.xml";//"dss%d_state.dat";


    measflag = 1;       // 1 to track SV props during sim
    measfile = "mvars.txt"; // File containing property names to track
    meastime = 0;       // time to start tracking props

    numtrials = 1;
    writeCellState = readCellState = false;
    //######## Params for pacing protocol #########################


    //##### Initialize variables ##################
    this->runBefore = ([] (Protocol&) {});
    this->runDuring = [] (Protocol&) {};
    this->runAfter = [] (Protocol&) {};
    // make map of params
    this->mkmap();
    basedir = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();
    this->setDataDir();
    cellStateDir = datadir;
};

Protocol::~Protocol() {}

//######################################################
// Deep Copy Constructor
//######################################################
Protocol::Protocol(const Protocol& toCopy) : std::enable_shared_from_this<Protocol>(toCopy) {
    this->copy(toCopy);
};

Protocol::Protocol(Protocol&& toCopy) {
    this->copy(toCopy);
};

Protocol& Protocol::operator=(const Protocol& toCopy) {
    this->copy(toCopy);
    return *this;
};

/*Protocol* Protocol::clone() {//public copy function
  return new Protocol(*this);
  };*/


void Protocol::copy(const Protocol& toCopy) {
    //##### Assign default parameters ##################

    this->mkmap();
    datadir = toCopy.datadir;
    cellStateDir = toCopy.cellStateDir;

    runflag = toCopy.runflag;       // set to 0 to end simulation

    tMax = toCopy.tMax;   // max simulation time, ms

    readfile = toCopy.readfile; // File to read SV ICs
    savefile = toCopy.savefile; // File to save final SV

    __trial = toCopy.__trial;
    dvarfile = toCopy.dvarfile;  // File with SV to write.
    writetime = toCopy.writetime;      // time to start writing.
    writeint = toCopy.writeint;     // interval for writing.
    writeflag = toCopy.writeflag;

    pvarfile = toCopy.pvarfile; // File to specify cell params
    simvarfile = toCopy.simvarfile;  // File to specify sim params

    propertyoutfile = toCopy.propertyoutfile;
    dvarsoutfile = toCopy.dvarsoutfile;
    finalpropertyoutfile = toCopy.finalpropertyoutfile;
    finaldvarsoutfile = toCopy.finaldvarsoutfile;
    cellStateFile = toCopy.cellStateFile;

    measflag = toCopy.measflag;       // 1 to track SV props during sim
    measfile = toCopy.measfile; // File containing property names to track
    meastime = toCopy.meastime;       // time to start tracking props

    numtrials = toCopy.numtrials;
    writeCellState = toCopy.writeCellState;
    readCellState = toCopy.readCellState;

    //##### Initialize variables ##################
    numruns = toCopy.numruns;
    firstRun = toCopy.firstRun;
    runEvery = toCopy.runEvery;
    runBefore = toCopy.runBefore;
    runDuring = toCopy.runDuring;
    runAfter = toCopy.runAfter;

    //###### Duplicate cells, measures outputs and maps######
}
//############################################################
// Run the cell simulation
//############################################################
int Protocol::runSim() {
    int return_val = 0;

    //###############################################################
    // Loop over number of trials
    //###############################################################
    for(;__trial<numtrials;trial(trial() +1))
    {
        return_val = (int)runTrial();
    }
    return return_val;
};

void Protocol::setupTrial() {
    this->mkDirs();
    cell()->reset();
};

//#############################################################
// Read values of variables in varmap from file.
// Format of file should be "variable name" tab "value"
//#############################################################
int Protocol::readpars(QXmlStreamReader& xml) {
    string name = "";
    if(!CellUtils::readNext(xml, "pars")) return 1;
    map<string,string> readValues;
    while(!xml.atEnd() && xml.readNextStartElement()){
        name = xml.attributes().value("name").toString().toStdString();
        xml.readNext();
        if(name != "datadir")
            readValues[name] = xml.text().toString().toStdString();
        xml.readNext();
    }
    //run twice to fix dependancy issues
    for(int i = 0; i <2; ++i) {
        for(auto& pair: readValues) {
            try {
                __pars.at(pair.first).set(pair.second);
            } catch (const std::out_of_range&) {
                if(i == 1)
                    qWarning("Protocol: %s not in pars", name.c_str());
            }
        }
    }
    return 0;
}
//############################################################
// Write the contents of varmap to a file
//############################################################
bool Protocol::writepars(QXmlStreamWriter& xml) {
    xml.writeStartElement("pars");
    for(auto it = __pars.begin(); it != __pars.end(); it++){
        xml.writeStartElement("par");
        xml.writeAttribute("name",it->first.c_str());
        xml.writeCharacters(it->second.get().c_str());
        xml.writeEndElement();
    }
    xml.writeEndElement();
    return 0;
}
void Protocol::trial(unsigned int current_trial) {
    if(current_trial < 0 || current_trial >= numtrials) return;
    __trial = current_trial;
}

unsigned int Protocol::trial() const {
    return __trial;
}

bool Protocol::cell(const string& type) {
    if(cell() != NULL && type == cell()->type()) {
        return false;
    }
    try {
        this->cell((CellUtils::cellMap.at(type))());
        return true;
    } catch(const std::out_of_range&) {
        qDebug("Protocol: %s is not a valid cell type",type.c_str());
        return false;
    }
}

list<string> Protocol::cellOptions() {
    list<string> options;
    for(auto& cellOpt : CellUtils::cellMap) {
        options.push_back(cellOpt.first);
    }
    return options;
}

string Protocol::parsStr(string name)
{
    try {
        return __pars.at(name).get();
    } catch(std::out_of_range) {
        qDebug("Protocol: Par ", name, " not in pars");
    }
    return "";
}

void Protocol::parsStr(string name, string val)
{
    try {
        __pars.at(name).set(val);
    } catch(std::out_of_range) {
        qDebug("Protocol: Par ", name, " not in pars");
    }
}

string Protocol::parsType(string name)
{
    try {
        return __pars.at(name).type;
    } catch(std::out_of_range) {
        qDebug("Protocol: Par ", name, " not in pars");
    }
    return "";
}

list<pair<string, string>> Protocol::parsList()
{
    list<pair<string,string>> parsL;
    for(auto& par: __pars) {
        parsL.push_back({par.first,par.second.type});
    }
    return parsL;
}

bool Protocol::hasPar(string name)
{
    return this->__pars.count(name) > 0;
}

void Protocol::readInCellState(bool read) {
    if(read) {
        cell()->readCellState(cellStateDir.absolutePath().toStdString()+"/"+cellStateFile+std::to_string(__trial)+".xml");
        this->tMax += this->cell()->t;
    }
}

void Protocol::writeOutCellState(bool write) {
    if(write) {
        cell()->writeCellState(getDataDir()+"/"+cellStateFile+std::to_string(__trial)+".xml");
    }
}

void Protocol::setDataDir(string location, string basedir, string appendtxt) {
    if(basedir.length() > 0) {
        this->basedir = basedir.c_str();
    }
    QDir working_dir = QDir(QString(location.c_str()));
    if(working_dir == QDir::currentPath()) {
        auto date_time = QDate::currentDate().toString("MMddyy") + "-" + QTime::currentTime().toString("hhmm");
        working_dir = (this->basedir.absolutePath() + "/data" + date_time + QString(appendtxt.c_str()));
        for(int i = 1; working_dir.exists(); i++) {
            working_dir = (this->basedir.absolutePath() + "/data" + date_time + "_" + QString::number(i));
        }
    }
    this->datadir = working_dir;
}

void Protocol::mkDirs() {
    this->datadir.mkpath(datadir.absolutePath());
}

string Protocol::getDataDir() {
    return this->datadir.absolutePath().toStdString();
}

void Protocol::mkmap() {
    GetSetRef toInsert;
    __pars["tMax"] = toInsert.Initialize("double",[this] () {return std::to_string(tMax);},[this] (const string& value) {tMax = std::stod(value);});
    __pars["numtrials"]= toInsert.Initialize("int", [this] () {return std::to_string(numtrials);}, [this] (const string& value) {numtrials = std::stoi(value);});
    __pars["writeint"]= toInsert.Initialize("int", [this] () {return std::to_string(writeint);}, [this] (const string& value) {writeint = std::stoi(value);});
    __pars["writetime"]= toInsert.Initialize("double", [this] () {return std::to_string(writetime);}, [this] (const string& value) {writetime = std::stod(value);});
    __pars["meastime"]= toInsert.Initialize("double", [this] () {return std::to_string(meastime);}, [this] (const string& value) {meastime = std::stod(value);});
    __pars["writeCellState"]= toInsert.Initialize("bool", [this] () {return CellUtils::to_string(writeCellState);}, [this] (const string& value) {writeCellState = CellUtils::stob(value);});
    __pars["readCellState"]= toInsert.Initialize("bool", [this] () {return CellUtils::to_string(readCellState);}, [this] (const string& value) {readCellState = CellUtils::stob(value);});
    __pars["datadir"]= toInsert.Initialize("directory", [this] () {return getDataDir();}, [this] (const string& value) {setDataDir(value);});
    __pars["cellStateDir"]= toInsert.Initialize("directory", [this] () {return cellStateDir.absolutePath().toStdString();}, [this] (const string& value) {cellStateDir.setPath(QString(value.c_str()));});
    //	pars["pvarfile"]= toInsert.Initialize("file", [this] () {return pvarfile;}, [this] (const string& value) {pvarfile = value;});
    //	pars["dvarfile"]= toInsert.Initialize("file", [this] () {return dvarfile;}, [this] (const string& value) {dvarfile = value;});
    //	pars["measfile"]= toInsert.Initialize("file", [this] () {return measfile;}, [this] (const string& value) {measfile = value;});
    __pars["simvarfile"]= toInsert.Initialize("file", [this] () {return simvarfile;}, [this] (const string& value) {simvarfile = value;});
    __pars["cellStateFile"]= toInsert.Initialize("file", [this] () {return cellStateFile;}, [this] (const string& value) {cellStateFile = value;});
    __pars["celltype"]= toInsert.Initialize("cell", [this] () {return cell()->type();}, [this] (const string& value) {this->cell(value);});
    __pars["cell_option"] = toInsert.Initialize("cell_option", [this] () {return cell()->optionStr();}, [this] (const string& value) {this->cell()->setOption(value);});
}

void Protocol::setRunBefore(function<void(Protocol&)> p) {
    this->runBefore = p;
}
void Protocol::setRunDuring(function<void(Protocol&)> p,  double firstRun, double runEvery, int numruns) {
    this->runDuring = p;
    if(firstRun >= 0) {
        this->firstRun = firstRun;
    }
    if(runEvery >= 0) {
        this->runEvery = runEvery;
    }
    if(numruns >= 0) {
        this->numruns = numruns;
    }
}
void Protocol::setRunAfter(function<void(Protocol&)> p) {
    this->runAfter = p;
}

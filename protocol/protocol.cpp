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

#if defined(_WIN32) || defined(_WIN64)
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif



//######################################################
// Default Constructor
//######################################################
Protocol::Protocol()
{
    //##### Assign default parameters ##################
    cell = new Cell();
    measureMgr = new MeasureManager(cell);
    doneflag = 1;       // set to 0 to end simulation

    tMax = 10000;   // max simulation time, ms

    datadir = "./data/";
    cellStateDir = datadir;

    readfile = "r.txt"; // File to read SV ICs
    savefile = "s.txt"; // File to save final SV

    trial = 0;

    writestd = 0;
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
    time=0.0;
    vM =  -88.0;
    //type = "Cell";  // class name

    // make map of params
    GetSetRef toInsert;
    pars["tMax"] = toInsert.Initialize("double",[this] () {return std::to_string(tMax);},[this] (const string& value) {tMax = std::stod(value);});
    pars["numtrials"]= toInsert.Initialize("int", [this] () {return std::to_string(numtrials);}, [this] (const string& value) {numtrials = std::stoi(value);});
    pars["writeint"]= toInsert.Initialize("int", [this] () {return std::to_string(writeint);}, [this] (const string& value) {writeint = std::stoi(value);});
    pars["writetime"]= toInsert.Initialize("double", [this] () {return std::to_string(writetime);}, [this] (const string& value) {writetime = std::stod(value);});
    pars["meastime"]= toInsert.Initialize("double", [this] () {return std::to_string(meastime);}, [this] (const string& value) {meastime = std::stod(value);});
    pars["writeCellState"]= toInsert.Initialize("bool", [this] () {return CellUtils::to_string(writeCellState);}, [this] (const string& value) {writeCellState = CellUtils::stob(value);});
    pars["readCellState"]= toInsert.Initialize("bool", [this] () {return CellUtils::to_string(readCellState);}, [this] (const string& value) {readCellState = CellUtils::stob(value);});
    pars["datadir"]= toInsert.Initialize("directory", [this] () {return datadir;}, [this] (const string& value) {datadir = value;});
    pars["cellStateDir"]= toInsert.Initialize("directory", [this] () {return cellStateDir;}, [this] (const string& value) {cellStateDir = value;});
    //	pars["pvarfile"]= toInsert.Initialize("file", [this] () {return pvarfile;}, [this] (const string& value) {pvarfile = value;});
    //	pars["dvarfile"]= toInsert.Initialize("file", [this] () {return dvarfile;}, [this] (const string& value) {dvarfile = value;});
    //	pars["measfile"]= toInsert.Initialize("file", [this] () {return measfile;}, [this] (const string& value) {measfile = value;});
    pars["simvarfile"]= toInsert.Initialize("file", [this] () {return simvarfile;}, [this] (const string& value) {simvarfile = value;});
    pars["cellStateFile"]= toInsert.Initialize("file", [this] () {return cellStateFile;}, [this] (const string& value) {cellStateFile = value;});
    pars["celltype"]= toInsert.Initialize("cell", [this] () {return cell->type;}, [this] (const string& value) {this->setCell(value);});

    cellMap = CellUtils::cellMap;
    this->setCell(HRD09Control().type);
};
//######################################################
// Deep Copy Constructor
//######################################################
Protocol::Protocol(const Protocol& toCopy) {
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
    std::map<string, double*>::iterator it;

    //##### Assign default parameters ##################

    datadir = toCopy.datadir;
    cellStateDir = toCopy.cellStateDir;

    doneflag = toCopy.doneflag;       // set to 0 to end simulation

    tMax = toCopy.tMax;   // max simulation time, ms

    readfile = toCopy.readfile; // File to read SV ICs
    savefile = toCopy.savefile; // File to save final SV

    trial = toCopy.trial;
    writestd = toCopy.writestd;
    writeflag = toCopy.writeflag;      // 1 to write data to file during sim
    dvarfile = toCopy.dvarfile;  // File with SV to write.
    writetime = toCopy.writetime;      // time to start writing.
    writeint = toCopy.writeint;     // interval for writing.

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
    time= toCopy.time;
    vM = toCopy.vM;

    // make map of params
    pars = toCopy.pars;

    //###### Duplicate cells, measures outputs and maps######
    cell = toCopy.cell->clone();

    pvars = toCopy.pvars->clone();
    measureMgr = toCopy.measureMgr->clone();
    measureMgr->cell(cell);
}
//############################################################
// Run the cell simulation
//############################################################
int Protocol::runSim() {
    int return_val = 0;

    //###############################################################
    // Loop over number of trials
    //###############################################################
    for(;trial<numtrials;setTrial(getTrial() +1))
    {
        return_val = (int)runTrial();
    }
    return return_val;
};

void Protocol::setupTrial() {};

//#############################################################
// Read values of variables in varmap from file.
// Format of file should be "variable name" tab "value"
//#############################################################
int Protocol::readpars(QXmlStreamReader& xml) {
    string name = "";
    if(!CellUtils::readNext(xml, "pars")) return 1;
    while(!xml.atEnd() && xml.readNextStartElement()){
        name = xml.attributes().value("name").toString().toStdString();
        try {
            xml.readNext();
            pars.at(name).set(xml.text().toString().toStdString());
        } catch (const std::out_of_range&) {
            qWarning("Protocol: %s not in pars", name.c_str());
        }
        xml.readNext();
    }
    return 0;
}
//############################################################
// Write the contents of varmap to a file
//############################################################
bool Protocol::writepars(QXmlStreamWriter& xml) {
    xml.writeStartElement("pars");
    for(auto it = pars.begin(); it != pars.end(); it++){
        xml.writeStartElement("par");
        xml.writeAttribute("name",it->first.c_str());
        xml.writeCharacters(it->second.get().c_str());
        xml.writeEndElement();
    }
    xml.writeEndElement();
    return 0;
}
void Protocol::setTrial(unsigned int current_trial) {
    trial = current_trial;
    //	assign_cell_pars(pnames,pvals,trial);   // Assign cell pars
}

unsigned int Protocol::getTrial() {
    return trial;
}

bool Protocol::setCell(const string& type, bool reset) {
    if(cell != NULL && type == cell->type && !reset) {
        return false;
    }
    try {
        cell = (cellMap.at(type))();
        measureMgr->clear();
        measureMgr->cell(cell);
        if(pvars)
            pvars->clear();
        return true;
    } catch(const std::out_of_range&) {
        qWarning("Protocol: %s is not a valid cell type",type.c_str());
        return false;
    }
}

list<string> Protocol::cellOptions() {
    list<string> options;
    for(auto it = cellMap.begin(); it != cellMap.end(); it++) {
        options.push_back(it->first);
    }
    return options;
}

void Protocol::readInCellState(bool read) {
    if(read) {
        cell->readCellState(cellStateDir+"/"+cellStateFile+std::to_string(trial)+".xml");
        this->tMax += this->cell->t;
    }
}

void Protocol::writeOutCellState(bool write) {
    if(write) {
        cell->writeCellState(datadir+"/"+cellStateFile+std::to_string(trial)+".xml");
    }
}

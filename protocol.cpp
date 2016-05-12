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
#include "gpbatrial.h"
#include "hrd09.h"

#if defined(_WIN32) || defined(_WIN64)
  #define snprintf _snprintf
  #define vsnprintf _vsnprintf
  #define strcasecmp _stricmp
  #define strncasecmp _strnicmp
#endif

//helper functions
string Protocol::to_string(const bool& b) {
    return b ? "true" : "false";
}

bool Protocol::stob(const string& s) {
    return (strcasecmp("true",s.c_str()) == 0);
}

//######################################################
// Default Constructor 
//######################################################
Protocol::Protocol()
{
    //##### Assign default parameters ##################
    cell = new Cell();
    doneflag = 1;       // set to 0 to end simulation
    
    tMax = 10000;   // max simulation time, ms
    
    datadir = "./data/";

    readfile = "r.txt"; // File to read SV ICs
    savefile = "s.txt"; // File to save final SV

    trial = 0;
   
    writestd = 0; 
    writeflag = 1;      // 1 to write data to file during sim
    dvarfile = "dvars.txt";  // File with SV to write.
    writetime = 0;      // time to start writing.
    writeint = 50;     // interval for writing.
    
    pvarfile = "pvars.txt"; // File to specify cell params
    simvarfile = "simvars.txt";  // File to specify sim params

    propertyoutfile = "dt%d_%s.dat";
    dvarsoutfile = "dt%d_dvars.dat";
    finalpropertyoutfile = "dss%d_%s.dat";
    finaldvarsoutfile = "dss%d_pvars.dat";


    measflag = 1;       // 1 to track SV props during sim
    measfile = "mvars.txt"; // File containing property names to track
    meastime = 0;       // time to start tracking props
    
    numtrials = 1;
    //######## Params for pacing protocol #########################
    
    
    //##### Initialize variables ##################
    time=0.0;
    vM =  -88.0;
    //type = "Cell";  // class name
    
    // make map of params
    GetSetRef toInsert;
    pars["tMax"] = toInsert.Initialize("double",[this] () {return std::to_string(tMax);},[this] (const string& value) {tMax = std::stod(value);});
    pars["numtrials"]= toInsert.Initialize("int", [this] () {return std::to_string(numtrials);}, [this] (const string& value) {numtrials = std::stoi(value);});
    pars["writetime"]= toInsert.Initialize("double", [this] () {return std::to_string(writetime);}, [this] (const string& value) {writetime = std::stod(value);});
    pars["meastime"]= toInsert.Initialize("double", [this] () {return std::to_string(meastime);}, [this] (const string& value) {meastime = std::stod(value);});
    pars["datadir"]= toInsert.Initialize("directory", [this] () {return datadir;}, [this] (const string& value) {datadir = value;});
    pars["pvarfile"]= toInsert.Initialize("file", [this] () {return pvarfile;}, [this] (const string& value) {pvarfile = value;});
    pars["dvarfile"]= toInsert.Initialize("file", [this] () {return dvarfile;}, [this] (const string& value) {dvarfile = value;});
    pars["measfile"]= toInsert.Initialize("file", [this] () {return measfile;}, [this] (const string& value) {measfile = value;});
    pars["simvarfile"]= toInsert.Initialize("file", [this] () {return simvarfile;}, [this] (const string& value) {simvarfile = value;});
    pars["celltype"]= toInsert.Initialize("cell", [this] () {return cell->type;}, [this] (const string& value) {this->setCell(value);}); 
    //initalize cellMap
    //will only have an effect the first time it is called
    cellMap["Cell"] = [] () {return new Cell;};
    cellMap["ControlSa"] = [] () {return (Cell*) new ControlSa;};
    cellMap["GpbAtrial"] = [] () {return (Cell*) new GpbAtrial;};
    cellMap["HRD09Control"] = [] () {return (Cell*) new HRD09Control;};
    cellMap["HRD09BorderZone"] = [] () {return (Cell*) new HRD09BorderZone;};
};



//######################################################
// Destructor for parent cell class.
//#####################################################
Protocol::~Protocol(){};

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

Protocol* Protocol::clone() {//public copy function
        return new Protocol(*this);
};


void Protocol::copy(const Protocol& toCopy) {
    unsigned int i = 0;
    std::map<string, double*>::iterator it;

    //##### Assign default parameters ##################
    
    datadir = toCopy.datadir;

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

    measflag = toCopy.measflag;       // 1 to track SV props during sim
    measfile = toCopy.measfile; // File containing property names to track
    meastime = toCopy.meastime;       // time to start tracking props
    
    numtrials = toCopy.numtrials;
    
    
    //##### Initialize variables ##################
    time= toCopy.time;
    vM = toCopy.vM;

    // make map of params
    pars = toCopy.pars;
    
    pnames = vector<string>(toCopy.pnames);
    for(i = 0; i < toCopy.pvals.size(); i++) {
        pvals.push_back(vector<string>(toCopy.pvals.at(i)));
    }
    //###### Duplicate cells, measures outputs and maps######
    cell = toCopy.cell->clone();

    measures = toCopy.measures;

}

//############################################################
// Assign parameter values based on pnames and 2D vector pvals
//############################################################

int Protocol::assign_cell_pars(vector<string> pnames, vector< vector<string> > pvals, int trialnum)
{
    unsigned int j;
    double logmean = 0.0;
    double logstdev = 0.2;
    double mean = 1.0;
    double stdev = 0.2;
    double increment;
    
    lognormal_distribution<double> logdistribution(logmean,logstdev);
    normal_distribution<double> distribution(mean,stdev);
    
    for (j=0; j<pnames.size(); j++) {
        if (pvals[j][0] == "random" ) {  // if first arg is "random" = choose rand val
            if(pvals[j].size()>1){
                if (pvals[j][1] == "lognormal"){
                    if (pvals[j].size()>3) {
                        logmean = atof(pvals[j][2].c_str());
                        logstdev = atof(pvals[j][3].c_str());
                        lognormal_distribution<double> logdistribution(logmean,logstdev);
                        *cell->pars[pnames[j]] = logdistribution(generator) ;
                    }
                    else
                        *cell->pars[pnames[j]] = logdistribution(generator) ;
                }
                else
                    *cell->pars[pnames[j]] = distribution(generator) ;
            }
            else
                *cell->pars[pnames[j]] = distribution(generator) ;
        }
        
        else if (pvals[j][0] == "iter"){  // if first arg is "iter" = iterate
            if (pvals[j].size()>2) {     // if 3 args, 2nd arg is initial val, 3rd arg is inc
                if (trialnum>0){
                    increment = atof(pvals[j][2].c_str());
                    *cell->pars[pnames[j]] = *cell->pars[pnames[j]] - increment;
                }
                else
                    *cell->pars[pnames[j]] = atof(pvals[j][1].c_str());
            }
            else if (pvals[j].size()>1){  // if only two args, 2nd arg is inc
                if (trialnum>0){
                    increment = atof(pvals[j][1].c_str());
                    *cell->pars[pnames[j]] = *cell->pars[pnames[j]] - increment;
                }
            }
            else {
                if (trialnum>0){     // if only one arg, set incr to vary param over range
                    increment = *cell->pars[pnames[j]]/(numtrials-trialnum);
                    *cell->pars[pnames[j]] = *cell->pars[pnames[j]] - increment;
                }
            }
        }
        
        else                  // if first arg is just a number
            *cell->pars[pnames[j]]=atof(pvals[j][0].c_str());
    }

    return 1;
};

bool Protocol::write2Dmap(vector<string> vnames, vector< vector<string> > twoDmnames, string file) {
    int ret = 0;
    unsigned int i,j;
    ofstream out;
    out.open(file);
    if(!out.is_open()){
        cout << "Error opening " << file << endl;
        return 1;
    }

    for(i = 0; i < vnames.size(); i++) {
        out << vnames[i] << "\t";
        for(j = 0; j < twoDmnames[i].size(); j++){
            out << twoDmnames[i][j] << "\t";
        }
        out << endl;
    }

    return ret;
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

bool Protocol::runTrial() { return true;}

//#############################################################
// Read values of variables in varmap from file.
// Format of file should be "variable name" tab "value"
//#############################################################

int Protocol::readpars(string file)
{
    ifstream ifile;
    
    string name;
    string value;
    map<string, double*>::iterator p;
    
    if(!ifile.is_open())
        ifile.open(file);
    if(!ifile.is_open()){
        cout << "Error opening " << file << endl;
        return 1;
    }
    
    while(!ifile.eof()){
        ifile >> name;
        ifile >> value;
        try {
            pars.at(name).set(value);
        } catch (const std::out_of_range& oor) {}
    }

    
    ifile.close();
    return 0;
  
};

//#############################################################
// Create vectors based on mixed list in file.  First row element is checked in varmap
// and gets stored in vector, rest of row used to create row in 2D vector.
// Writes paramiter names into cnames and paramiter values into twoDrnames.
//#############################################################
int Protocol::parsemixedmap(map<string,double*> varmap, string file, vector<string>* cnames, vector<vector<string>>* twoDrnames)
{
    ifstream ifile;
    string line, name;
    
    vector<string> rnames;
    
    ifile.open(file);
    if(!ifile.is_open()){
        cout << "Error opening " << file << endl;
        return 1;
    }
    
    int counter = 0;
    
    while(!ifile.eof()){
        getline(ifile,line);
        stringstream linestream(line);
        linestream >> name;
        if(varmap.count(name)>0){
            (*cnames).push_back(name);
            
            while (!linestream.eof()) {
                linestream >> name;
                rnames.push_back(name);
            }
            if (rnames.size()>0)
                (*twoDrnames).push_back(rnames);
            else
                (*cnames).pop_back();
            
            rnames.clear();
        }
        counter++;
        
    }
    ifile.close();
   
    return 0;
};

//############################################################
// write a mvars file for the measures list
//############################################################
bool Protocol::writeMVarsFile(string file) {
    ofstream out;
    int ret = false;

    out.open(file);
    if(!out.good()){
        cout << "Error opening " << file << endl;
        return ret;
    }

    for(auto im = measures.begin(); im != measures.end(); im++) {
        out << im->second.varname << " \t";
        set<string> selection = im->second.Selection;
        for(set<string>::iterator it = selection.begin(); it != selection.end(); it++){
            out << *it << " \t";
        }
        out << endl;
    }
    out.close();
    ret = true;
    return ret;
}

bool Protocol::addMeasure(Measure toInsert) {
    if(cell->vars.find(toInsert.varname) != cell->vars.end()) {
        return measures.insert(pair<string,Measure>(toInsert.varname, toInsert)).second;
    }
    return false;
}

void Protocol::removeMeasure(string measureName) {
    measures.erase(measureName);
}

bool Protocol::setMeasures(map<string,Measure> newMeasures) {
    bool toReturn = true;
    measures.clear();
    measures.insert(newMeasures.begin(), newMeasures.end());
    for(auto it = measures.begin(); it != measures.end(); it++) {
        if(cell->vars.find(it->first) == cell->vars.end()) {
            measures.erase(it);
            toReturn = false;
        }
    }
    return toReturn;
}

bool Protocol::addToMeasreSelection(string measureName, string property) {
    try {
        return measures.at(measureName).addToMeasureSelection(property);
    } catch (const std::out_of_range& oor) {
        return false;
    }
}

void Protocol::removeFromMeasureSelection(string measureName, string property) {
    try {
        measures.at(measureName).removeFromSelection(property);
    } catch ( const std::out_of_range& oor) {

    }
}


//#############################################################
//read and mvars style file and use it to set measures list 
//where varname is the first word on the line
//and all sequential words are properties to measure (the selection)
//#############################################################
bool Protocol::readMvarsFile(string filename)
{
    ifstream ifile;
    string line, name;
    set<string> possible_vars = cell->getVariables();
    
    
    ifile.open(filename);
    if(!ifile.good()){
        cout << "Error opening " << filename << endl;
        return false;
    }
    
    while(!ifile.eof()) {
        getline(ifile,line);
        stringstream linestream(line);
        Measure temp;
        linestream >> temp.varname;
        while(!linestream.eof()) {
           linestream >> name;
           temp.addToMeasureSelection(name);
        }
        if(possible_vars.find(temp.varname) != possible_vars.end()) {
            measures.insert(pair<string,Measure>(temp.varname,temp));
        }
    }
    
    ifile.close();
    return true;
};

//#############################################################
// Set params for reading/saving model params.
//#############################################################
int Protocol::readpvars(){

    int ret = 0;
    ret = parsemixedmap(cell->pars, pvarfile ,&pnames, &pvals);
    return ret;
};

//############################################################
// Write the contents of varmap to a file
//############################################################
bool Protocol::writepars(string file)
{
    ofstream ofile;
    string name;
    
    if(!ofile.is_open())
        ofile.open(file);
    if(!ofile.is_open()){
        cout << "Error opening " << file << endl;
        return 1;
    }
    
    for(auto it = pars.begin(); it != pars.end(); it++){
        ofile << it->first << "\t" << it->second.get() << endl;
    }

    ofile.close();
    return 0;
  
};
//############################################################
//Wirte the keys from varmap to a file
//############################################################
bool Protocol::writedvars(string file)
{
    set<string> selection = cell->getVariableSelection();
    ofstream ofile;
    string name;
    
    if(!ofile.is_open())
        ofile.open(file, ios_base::trunc);
    if(!ofile.is_open()){
        cout << "Error opening " << file << endl;
        return false;
    }

    for(auto it = selection.begin(); it != selection.end(); it++){
        ofile << *it << endl;
    }

    ofile.close();
    return true;

}
bool Protocol::readdvars(string file) {
    bool toReturn = true;
    ifstream ifile;
    string name;
    set<string> selection;
    
    if(!ifile.is_open()) {
        ifile.open(file);
    }
    if(!ifile.good()){
        cout << "Error opening " << file << endl;
        return false;
    }

    while(!ifile.eof()) {
        ifile >> name;
        if(!selection.insert(name).second) {
            toReturn = false;
        }
    }
    cell->setVariableSelection(selection);
    return toReturn;
}
void Protocol::setTrial(unsigned int current_trial) {
    trial = current_trial;
    assign_cell_pars(pnames,pvals,trial);   // Assign cell pars
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
        measures.clear();
        pvals.clear();
        pnames.clear();
        return true;
    } catch(const std::out_of_range& oor) {
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

//####################################################
// This file contains general function definitions 
// for simulation class.
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015 by TJH
//####################################################

#include "proto.h"
#include <QDebug>

//######################################################
// Constructor for parent cell class - the basis for 
// any excitable cell model.  SV = state variable
//######################################################
Protocol::Protocol()
{
    //##### Assign default parameters ##################
    
    doneflag = 1;       // set to 0 to end simulation
    
    tMax = 10000;   // max simulation time, ms
    
    readflag = 0;       // 1 to read SV ICs from file
    saveflag = 0;       // 1 to save final SV vals to file
    readfile = "./data/r.txt"; // File to read SV ICs
    savefile = "./data/s.txt"; // File to save final SV

    myId = 1;
    
    writeflag = 0;      // 1 to write data to file during sim
    dvarfile = "dvars.txt";  // File with SV to write.
    writetime = 0;      // time to start writing.
    writeint = 50;     // interval for writing.
    maxdoutsize = 10;   // Max number of dout files.
    
    pvarfile = "pvars.txt"; // File to specify cell params
    simvarfile = "simvars.txt";  // File to specify sim params
    
    measflag = 0;       // 1 to track SV props during sim
    measfile = "mvars.txt"; // File containing property names to track
    meastime = 0;       // time to start tracking props
    maxmeassize = 10;   // Max number of SVs to track
    
    numtrials = 1;
    //######## Params for pacing protocol #########################
    paceflag = 0;   // 1 to pace cell.
    numstims = 1;   // # of stimuli to apply
    bcl = 1000;     // basic cycle length, ms
    stimval = 0.0;  // stim current amplitude, uA/uF
    stimdur = 1.0;  // stim duration, ms
    stimt = 0.0;    // time of first stim, ms
    
    stimcounter = 0;
    stimflag = 0;
    
    //##### Initialize variables ##################
    time=0.0;
    vM =  -88.0;
    //type = "Cell";  // class name
    
    // make map of params
    pars["tMax"]=&tMax;
    pars["bcl"]=&bcl;
    pars["stimval"]=&stimval;
    pars["stimdur"]=&stimdur;
    pars["stimt"]=&stimt;
    pars["numstims"]=&numstims;
    pars["numtrials"]=&numtrials;
    pars["readflag"]=&readflag;
    pars["saveflag"]=&saveflag;
    pars["writeflag"]=&writeflag;
    pars["writetime"]=&writetime;
    pars["measflag"]=&measflag;
    pars["meastime"]=&meastime;
    pars["paceflag"]=&paceflag;
    pars["maxdoutsize"]=&maxdoutsize;
};
//######################################################
// Destructor for parent cell class.
//#####################################################
Protocol::~Protocol()
{
};

// External stimulus.
int Protocol::stim()
{
  if(cell->t>=stimt&&cell->t<(stimt+stimdur)){
      if(stimflag==0){
          std::cout << "Stimulus to " << cell->type << " at t = " << cell->t << std::endl;
          stimcounter++;
          stimflag=1;
          if(stimcounter>=int(numstims)){
              doneflag = 0;
              return 0;
          }
      }
      cell->externalStim(stimval);
    }
    else if(stimflag==1){     //trailing edge of stimulus
        stimt=stimt+bcl;
        stimflag=0;
        cell->apTime = 0.0;
    }

    cell->apTime = cell->apTime+cell->dt;

    doneflag = 1;
    return 1;
};

//############################################################
// Assign parameter values based on pnames and 2D vector pvals
//############################################################

int Protocol::assign_cell_pars(std::vector<std::string> pnames, std::vector< std::vector<std::string> > pvals, int trialnum)
{
    int j;
    double logmean = 0.0;
    double logstdev = 0.2;
    double mean = 1.0;
    double stdev = 0.2;
    double increment;
    
    std::lognormal_distribution<double> logdistribution(logmean,logstdev);
    std::normal_distribution<double> distribution(mean,stdev);
    
    for (j=0; j<pnames.size(); j++) {
        if (pvals[j][0] == "random" ) {  // if first arg is "random" = choose rand val
            if(pvals[j].size()>1){
                if (pvals[j][1] == "lognormal"){
                    if (pvals[j].size()>3) {
                        logmean = atof(pvals[j][2].c_str());
                        logstdev = atof(pvals[j][3].c_str());
                        std::lognormal_distribution<double> logdistribution(logmean,logstdev);
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



//#############################################################
// Resize varmap based on list in file.
//#############################################################
std::map<std::string, double*> Protocol::resizemap(std::map<std::string,double*> varmap, std::string file)
{
    std::ifstream ifile;
    std::map<std::string, double*> vars;
    std::string name;
    
    ifile.open(file);
    if(!ifile.is_open()){
        std::cout << "Error opening " << file << std::endl;
        exit(1);
    }
    
    while(!ifile.eof()){
        ifile >> name;
        if(varmap.count(name)>0)
            vars[name] = varmap[name];
    }
    ifile.close();
    return vars;
};

// Resize varmap based on list.
std::map<std::string, double*> Protocol::resizemap(std::map<std::string,double*> varmap, std::vector<std::string> names)
{
    int i;
    std::map<std::string, double*> vars;
    
    for (i=0; i<names.size(); i++){
        if(varmap.count(names[i])>0)
            vars[names[i]] = varmap[names[i]];
    }
    
    return vars;
};

//#############################################################
// Create vectors based on 2D list in file.  First row element is checked in varmap
// and gets stored in vector, rest of row checked in varmap2 and used to create row in 2D vector.
// Returns vector with first column and rest of file in 2D vector.
//#############################################################
std::tuple< std::vector<std::string>, std::vector< std::vector<std::string> > > Protocol::parse2Dmap(std::map<std::string,double*> varmap,std::map<std::string,double*> varmap2, std::string file)
{
    std::ifstream ifile;
    std::string line, name;
    
    std::vector<std::string> vnames;
    std::vector<std::string> mnames;
    
    std::vector< std::vector<std::string> > twoDmnames;
    
    ifile.open(file);
    if(!ifile.is_open()){
        std::cout << "Error opening " << file << std::endl;
        exit(1);
    }
    
    int counter = 0;
    
    while(!ifile.eof()){
        getline(ifile,line);
        std::stringstream linestream(line);
        linestream >> name;
        
        if(varmap.count(name)>0){
            vnames.push_back(name);
            
            while (!linestream.eof()) {
                linestream >> name;
                if(varmap2.count(name)>0){
                    mnames.push_back(name);
                }
            }
            if (mnames.size()>0)
                twoDmnames.push_back(mnames);
            else
                vnames.pop_back();
            
            mnames.clear();
        }
        counter++;
        
    }
    ifile.close();
    return make_tuple(vnames,twoDmnames);
};

//#############################################################
// Create vectors based on mixed list in file.  First row element is checked in varmap
// and gets stored in vector, rest of row used to create row in 2D vector.
// Returns vector with first column and rest of file in 2D vector.
//#############################################################
std::tuple< std::vector<std::string>, std::vector< std::vector<std::string> > > Protocol::parsemixedmap(std::map<std::string,double*> varmap, std::string file)
{
    std::ifstream ifile;
    std::string line, name;
    
    std::vector<std::string> cnames;
    std::vector<std::string> rnames;
    
    std::vector< std::vector<std::string> > twoDrnames;
    
    ifile.open(file);
    if(!ifile.is_open()){
        std::cout << "Error opening " << file << std::endl;
        exit(1);
    }
    
    int counter = 0;
    
    while(!ifile.eof()){
        getline(ifile,line);
        std::stringstream linestream(line);
        linestream >> name;
        
        if(varmap.count(name)>0){
            cnames.push_back(name);
            
            while (!linestream.eof()) {
                linestream >> name;
                rnames.push_back(name);
            }
            if (rnames.size()>0)
                twoDrnames.push_back(rnames);
            else
                cnames.pop_back();
            
            rnames.clear();
        }
        counter++;
        
    }
    ifile.close();
    return make_tuple(cnames,twoDrnames);
};

//#############################################################
// Returns copy of variable values (not pointers) in varmap.
//#############################################################
std::map<std::string,double> Protocol::copymapvals(std::map<std::string, double*> varmap)
{
    std::map<std::string, double> vars;
    std::map<std::string, double*>::iterator p;
    
    for(p=varmap.begin(); p!=varmap.end(); ++p)
        vars[p->first] = *p->second;
    
    return vars;
};

//#############################################################
// Write values of variables in varmap to screen.
//#############################################################
int Protocol::map2screen(std::map<std::string, double*> varmap)
{
    std::map<std::string, double*>::iterator p;
    
    for(p=varmap.begin(); p!=varmap.end(); ++p)
        std::cout << p->first << " = " << *p->second << "; ";
    
    std::cout << std::endl;
    
    return 1;
};

//#############################################################
// Write values of variables in varmap to screen.
//#############################################################
int Protocol::map2screen(std::map<std::string, double> varmap)
{
    std::map<std::string, double>::iterator p;
    
    for(p=varmap.begin(); p!=varmap.end(); ++p)
        std::cout << p->first << " = " << p->second << "; ";
    
    std::cout << std::endl;
    
    return 1;
};


//#############################################################
// Output class constructor and destructor
//#############################################################

Output::Output()
{
    counter = 0;
    interval = 1;
};

Output::~Output()
{
    if(ofile.is_open()){
        ofile.close();
        ofile.clear();
    }
};

//#############################################################
// Write values of variables in varmap to file. Output is
// organized with variable name in 1st column and values
// in 2nd column.
//#############################################################
int Output::writevals(std::map<std::string, double*> varmap, std::string file)
{
    std::map<std::string, double*>::iterator p;
    ofile.precision(10);
    if(!ofile.is_open())
        ofile.open(file);
    
    if(!ofile.is_open()){
        std::cout << "Error opening " << file << std::endl;
        exit(1);
    }
    
    for(p=varmap.begin(); p!=varmap.end(); ++p)
        ofile << p->first << "\t" << *p->second << "\n";
    
    ofile.close();
    return 1;
};

//#############################################################
// Write values of variables in varmap to file more than once.
// Output is organized in columns with variable names in first
// row.
//#############################################################
int Output::writevals(std::map<std::string, double*> varmap, std::string file, char type)
{
    std::map<std::string, double*>::iterator p;
    ofile.precision(10);
    
    if(counter==0){
        if(!ofile.is_open())
            ofile.open(file);
        if(!ofile.is_open()){
            std::cout << "Error opening " << file << std::endl;
            exit(1);
        }
        for(p=varmap.begin(); p!=varmap.end(); ++p)
            ofile << p->first << "\t";
        ofile << std::endl;
    }
    
    for(p=varmap.begin(); p!=varmap.end(); ++p)
        ofile << *p->second << "\t";
    
    ofile << std::endl;
    
    counter++;
    return 1;
};

//#############################################################
// Write values of variables in varmap to file. Output is
// organized with variable name in 1st column and values
// in 2nd column.
//#############################################################
int Output::writevals(std::map<std::string, double> varmap, std::string file)
{
    std::map<std::string, double>::iterator p;
    ofile.precision(10);
    if(!ofile.is_open())
        ofile.open(file);
    
    if(!ofile.is_open()){
        std::cout << "Error opening " << file << std::endl;
        exit(1);
    }
    
    for(p=varmap.begin(); p!=varmap.end(); ++p)
        ofile << p->first << "\t" << p->second << "\n";
    
    ofile.close();
    return 1;
};

//#############################################################
// Write values of variables in varmap to file more than once.
// Output is organized in columns with variable names in first
// row.
//#############################################################
int Output::writevals(std::map<std::string, double> varmap, std::string file, char type)
{
    std::map<std::string, double>::iterator p;
    ofile.precision(10);
    
    if(counter==0){
        if(!ofile.is_open())
            ofile.open(file);
        if(!ofile.is_open()){
            std::cout << "Error opening " << file << std::endl;
            exit(1);
        }
        for(p=varmap.begin(); p!=varmap.end(); ++p)
            ofile << p->first << "\t";
        ofile << std::endl;
    }
    
    for(p=varmap.begin(); p!=varmap.end(); ++p)
        ofile << p->second << "\t";
    
    ofile << std::endl;
    
    counter++;
    return 1;
};

//#############################################################
// Measure class constructor and destructor
//#############################################################


Measure::Measure()
{
    peak=-100.0;
    min=100.0;
    vartakeoff=-100.0;
    repol = -25.0;
    amp = 70.0;
    maxderiv=0.0;
    maxderiv2nd=0.0;
    cl=0.0;
    told = -10000.0;
    mint = 0.0;
    maxt = 0.0;
    varold = 100.0;
    derivold = 0.0;
    minflag = 0;
    maxflag = 0;
    ampflag = 0;
    ddrflag = 0;
    derivt2 = 0.0;
    derivt1 = 0.0;
    derivt = 0.0;
    deriv2ndt = 0.0;
    durflag = 0;
    percrepol = 50.0;
    returnflag = 0;
    
    varmap["cl"]=&cl;
    varmap["peak"]=&peak;
    varmap["min"]=&min;
    varmap["amp"]=&amp;
    varmap["ddr"]=&ddr;
    varmap["maxderiv"]=&maxderiv;
    varmap["dur"]=&dur;
    varmap["durtime1"]=&durtime1;
    varmap["vartakeoff"]=&vartakeoff;
    varmap["mint"]=&mint;
    varmap["derivt"]=&derivt;
    varmap["deriv2ndt"]=&deriv2ndt;
    
    datamap["peak"]=&peak;
    datamap["min"]=&min;
    datamap["maxderiv"]=&maxderiv;
    datamap["dur"]=&dur;
};

Measure::~Measure()
{
};

//################################################################
// Function to track properties (e.g. peak, min, duration) of
// specified state variable and return status flag to calling fxn.
//################################################################
int Measure::measure(double time, double var)
{
    double deriv,deriv2nd;
    
    returnflag = 0;  //default for return...set to 1 when props ready for output
    
    deriv=(var-varold)/(time-told);
    deriv2nd=(deriv-derivold)/(time-told);
    
    if(deriv>maxderiv){             // Track value and time of max 1st deriv
        maxderiv=deriv;
        derivt=time;
    }
    
    if(deriv2nd>.02&&var>(0.01*std::abs(min)+min)&&ddrflag==0){   // Track 2nd deriv for SAN ddr
        vartakeoff=var;
        deriv2ndt=time;
        ddr=(vartakeoff-min)/(time-mint);
        ddrflag=1;
    }
    
    if(minflag==1&&var>peak){          // Track value and time of peak
        peak=var;
        maxt=time;
    }
    else if((peak-min)>0.3*std::abs(min))    // Assumes true max is more than 30% greater than the min.
        maxflag=1;
    
    if(var<min){                        // Track value and time of min
        min=var;
        mint=time;
    }
    else
        minflag=1;
    
    if(var>repol&&durflag==0){          // t1 for dur calculation = first time var crosses repol.
        durtime1=time;            // will depend on percrepol - default is 50 but can be changed.
        durflag=1;
    }
    
    if(maxflag==1&&minflag==1&&ampflag==0){
        amp=peak-min;
        ampflag = 1;
        cl=derivt-derivt1;
        derivt2=derivt1;
        derivt1=derivt;
        repol = (1-percrepol*0.01)*amp+min;
    }
    
    if(durflag==1&&var<repol){
        dur=time-durtime1;
        durflag=0;
        returnflag = 1;  // lets calling fxn know that it is time to output and reset.
    }
    
    told=time;
    varold=var;
    derivold=deriv;
    
    return (returnflag);
};


void Measure::reset()
{
    peak=-100.0;
    min=100.0;
    maxderiv=0.0;
    maxderiv2nd=0.0;
    told = 0.0;
    minflag = 0;
    maxflag = 0;
    ampflag = 0;
    ddrflag = 0;
};



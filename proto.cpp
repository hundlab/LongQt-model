//####################################################
// This file contains general function definitions 
// for simulation class.
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015 by TJH
//####################################################

#include "proto.h"
#include <QDebug>
using namespace std;

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
          cout << "Stimulus to " << cell->type << " at t = " << cell->t << endl;
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



//#############################################################
// Resize varmap based on list in file.
//#############################################################
map<string, double*> Protocol::resizemap(map<string,double*> varmap, string file)
{
    ifstream ifile;
    map<string, double*> vars;
    string name;
    
    ifile.open(file);
    if(!ifile.is_open()){
        cout << "Error opening " << file << endl;
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
map<string, double*> Protocol::resizemap(map<string,double*> varmap, vector<string> names)
{
    unsigned int i;
    map<string, double*> vars;
    
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
tuple< vector<string>, vector< vector<string> > > Protocol::parse2Dmap(map<string,double*> varmap,map<string,double*> varmap2, string file)
{
    ifstream ifile;
    string line, name;
    
    vector<string> vnames;
    vector<string> mnames;
    
    vector< vector<string> > twoDmnames;
    
    ifile.open(file);
    if(!ifile.is_open()){
        cout << "Error opening " << file << endl;
        exit(1);
    }
    
    int counter = 0;
    
    while(!ifile.eof()){
        getline(ifile,line);
        stringstream linestream(line);
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
tuple< vector<string>, vector< vector<string> > > Protocol::parsemixedmap(map<string,double*> varmap, string file)
{
    ifstream ifile;
    string line, name;
    
    vector<string> cnames;
    vector<string> rnames;
    
    vector< vector<string> > twoDrnames;
    
    ifile.open(file);
    if(!ifile.is_open()){
        cout << "Error opening " << file << endl;
        exit(1);
    }
    
    int counter = 0;
    
    while(!ifile.eof()){
        getline(ifile,line);
        stringstream linestream(line);
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
map<string,double> Protocol::copymapvals(map<string, double*> varmap)
{
    map<string, double> vars;
    map<string, double*>::iterator p;
    
    for(p=varmap.begin(); p!=varmap.end(); ++p)
        vars[p->first] = *p->second;
    
    return vars;
};

//#############################################################
// Write values of variables in varmap to screen.
//#############################################################
int Protocol::map2screen(map<string, double*> varmap)
{
    map<string, double*>::iterator p;
    
    for(p=varmap.begin(); p!=varmap.end(); ++p)
        cout << p->first << " = " << *p->second << "; ";
    
    cout << endl;
    
    return 1;
};

//#############################################################
// Write values of variables in varmap to screen.
//#############################################################
int Protocol::map2screen(map<string, double> varmap)
{
    map<string, double>::iterator p;
    
    for(p=varmap.begin(); p!=varmap.end(); ++p)
        cout << p->first << " = " << p->second << "; ";
    
    cout << endl;
    
    return 1;
};

//#############################################################
// Set params for measuring state var props (dur, min, max, etc).
//#############################################################
int Protocol::initializeMeasure(int measureSize) {
    unsigned int i;

     measures = new Measure[measureSize];   // Array of measure streams.
    
    //Parse and resize 2D map based on measfile...move into measure?
    
    if (int(measflag)==1){
        parse2Dmap(cell->vars,measures[0].varmap, measfile, &mvnames, &mpnames);
        for (i=0; i<mvnames.size(); i++) {
            if (i<int(maxmeassize)) {
                measures[i].datamap = resizemap(measures[i].varmap,mpnames[i]);
                measures[i].varname = mvnames[i];
            }
        }
        tempvals = new map<string, double> [mvnames.size()];
    
    }

    return 0;

};

//############################################################
// Get the number of Doutput arrays to create
//############################################################
int Protocol::getNeededDOutputSize(){
    return int(mvnames.size()+1+numtrials*(mvnames.size()+1));
};

//############################################################
// Run the cell simulation
//############################################################
int Protocol::runSim() {
    unsigned int i,j;
    char writefile[50];     // Buffer for storing filenames

    //###############################################################
    // Loop over number of trials
    //###############################################################
    for(i=0;i<int(numtrials);i++)
    {
        assign_cell_pars(pnames,pvals,i);   // Assign cell pars before each trial.
        time = cell->t = 0.0;      // reset time
        doneflag=1;     // reset doneflag
  
//        if (int(readflag)==1)
//            readvals(cell->vars, readfile);  // Init SVs before each trial.
        
        //###############################################################
        // Every time step, currents, concentrations, and Vm are calculated.
        //###############################################################
        int pCount = 0;
        
        while(int(doneflag)&&(time<tMax)){
            
            time = cell->tstep(stimt);    // Update time
            cell->updateCurr();    // Update membrane currents

            if(int(paceflag)==1)  // Apply stimulus
                stim();

            cell->updateConc();   // Update ion concentrations
            vM=cell->updateV();     // Update transmembrane potential
            
            //##### Output select variables to file  ####################
            if(int(measflag)==1&&cell->t>meastime){
                for (j=0; j<mvnames.size(); j++) {
                    if(measures[j].measure(cell->t,*cell->vars[measures[j].varname])){
                        tempvals[j]=copymapvals(measures[j].datamap);
                        if (int(writeflag)==1) {
                            sprintf(writefile,"./data/dt%d_%s.dat",i,mvnames[j].c_str());
                            douts[j+(i+1)*(mvnames.size()+1)].writevals(measures[j].datamap,writefile,'a');
                        }
                        measures[j].reset();
                    }
                }
            }
            if (int(writeflag)==1&&time>writetime&&pCount%int(writeint)==0) {
                sprintf(writefile,"./data/dt%d_dvars.dat",i);
                douts[mvnames.size()+(i+1)*(mvnames.size()+1)].writevals(datamap,writefile,'a');
            }
            cell->setV(vM); //Overwrite vOld with new value
            pCount++;
        }
      
      map2screen(parmap);  // send final property values to console
      // Output final (ss) property values for each trial
      for (j=0; j<mvnames.size(); j++){
          map2screen(tempvals[j]);
          sprintf(writefile,"./data/dss_%s.dat",mvnames[j].c_str());
          douts[j].writevals(tempvals[j],writefile,'a');
          measures[j].reset();
      }
      
      // Output parameter values for each trial
      douts[mvnames.size()].writevals(parmap, "./data/dss_pvars.dat", 'a');
      
  }

};

//#############################################################
// Read values of variables in varmap from file.
// Format of file should be "variable name" tab "value"
//#############################################################

int Protocol::readvals(map<string, double*> varmap, string file)
{
    ifstream ifile;
    
    string name;
    double num;
    map<string, double*>::iterator p;
    
    if(!ifile.is_open())
        ifile.open(file);
    if(!ifile.is_open()){
        cout << "Error opening " << file << endl;
        return 1;
    }
    
    while(!ifile.eof()){
        ifile >> name;
        if(varmap.count(name)>0){
            ifile >> num;
            *varmap[name] = num;
        }
    }
    
    ifile.close();
    return 1;
  
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

//#############################################################
// Create vectors based on 2D list in file.  First row element is checked in varmap
// and gets stored in vector, rest of row checked in varmap2 and used to create row in 2D vector.
// Returns vector with first column and rest of file in 2D vector.
//#############################################################
int Protocol::parse2Dmap(map<string,double*> varmap,map<string,double*> varmap2, string file, vector<string>* vnames, vector< vector<string> >* twoDmnames)
{
    ifstream ifile;
    string line, name;
    
    vector<string> mnames;
    
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
            (*vnames).push_back(name);
            
            while (!linestream.eof()) {
                linestream >> name;
                if(varmap2.count(name)>0){
                    mnames.push_back(name);
                }
            }
            if (mnames.size()>0)
                (*twoDmnames).push_back(mnames);
            else
               (* vnames).pop_back();
            
            mnames.clear();
        }
        counter++;
        
    }
    ifile.close();
    return 0;
};

//#############################################################
// Resize varmap based on list in file.
//#############################################################
int Protocol::resizemap(map<string,double*> varmap, string file, map<string, double*>* vars)
{
    ifstream ifile;
    string name;
    
    ifile.open(file);
    if(!ifile.is_open()){
        cout << "Error opening " << file << endl;
        return 1;
    }
    
    while(!ifile.eof()){
        ifile >> name;
        if(varmap.count(name)>0)
            (*vars)[name] = varmap[name];
    }
    ifile.close();
    return 0;
};

//#############################################################
// Set params for reading/saving model params.
//#############################################################
int Protocol::read_model_params(){

    parsemixedmap(cell->pars, pvarfile ,&pnames, &pvals);
    parmap = resizemap(cell->pars,pnames);
    return 0;
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
int Output::writevals(map<string, double*> varmap, string file)
{
    map<string, double*>::iterator p;
    ofile.precision(10);
    if(!ofile.is_open())
        ofile.open(file);
    
    if(!ofile.is_open()){
        cout << "Error opening " << file << endl;
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
int Output::writevals(map<string, double*> varmap, string file, char type)
{
    map<string, double*>::iterator p;
    ofile.precision(10);
    
    if(counter==0){
        if(!ofile.is_open())
            ofile.open(file);
        if(!ofile.is_open()){
            cout << "Error opening " << file << endl;
            exit(1);
        }
        for(p=varmap.begin(); p!=varmap.end(); ++p)
            ofile << p->first << "\t";
        ofile << endl;
    }
    
    for(p=varmap.begin(); p!=varmap.end(); ++p)
        ofile << *p->second << "\t";
    
    ofile << endl;
    
    counter++;
    return 1;
};

//#############################################################
// Write values of variables in varmap to file. Output is
// organized with variable name in 1st column and values
// in 2nd column.
//#############################################################
int Output::writevals(map<string, double> varmap, string file)
{
    map<string, double>::iterator p;
    ofile.precision(10);
    if(!ofile.is_open())
        ofile.open(file);
    
    if(!ofile.is_open()){
        cout << "Error opening " << file << endl;
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
int Output::writevals(map<string, double> varmap, string file, char type)
{
    map<string, double>::iterator p;
    ofile.precision(10);
    
    if(counter==0){
        if(!ofile.is_open())
            ofile.open(file);
        if(!ofile.is_open()){
            cout << "Error opening " << file << endl;
            exit(1);
        }
        for(p=varmap.begin(); p!=varmap.end(); ++p)
            ofile << p->first << "\t";
        ofile << endl;
    }
    
    for(p=varmap.begin(); p!=varmap.end(); ++p)
        ofile << p->second << "\t";
    
    ofile << endl;
    
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
    
    if(deriv2nd>.02&&var>(0.01*abs(min)+min)&&ddrflag==0){   // Track 2nd deriv for SAN ddr
        vartakeoff=var;
        deriv2ndt=time;
        ddr=(vartakeoff-min)/(time-mint);
        ddrflag=1;
    }
    
    if(minflag==1&&var>peak){          // Track value and time of peak
        peak=var;
        maxt=time;
    }
    else if((peak-min)>0.3*abs(min))    // Assumes true max is more than 30% greater than the min.
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



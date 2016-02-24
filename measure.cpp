//################################################
// This code file
// defines Measure class used to track properties
// (e.g. peak, min, duration) of specified state variable.
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015
// Email thomas.hund@osumc.edu
//#################################################

#include "measure.h"

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
    minflag = false;
    maxflag = false;
    ampflag = false;
    ddrflag = false;
    derivt2 = 0.0;
    derivt1 = 0.0;
    derivt = 0.0;
    deriv2ndt = 0.0;
    durflag = false;
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
    
    selection.insert("peak");
    selection.insert("min");
    selection.insert("maxderiv");
    selection.insert("dur");
};

Measure::Measure(const Measure& toCopy) {
    this->copy(toCopy);
};

Measure::Measure( Measure&& toCopy) {
    this->copy(toCopy); 
};

Measure& Measure::operator=(const Measure& toCopy) {
    this->copy(toCopy);
    return *this;
};

Measure::~Measure()
{
};

bool Measure::setOutputfile(string filename) {
    ifstream test;
    bool exists;
    test.open(filename);
    exists = test.good();
    test.close();
    if(ofile.is_open()) {
        ofile.close();
    }
    ofile.precision(10);
    ofile.open(filename,ios_base::app);
    if(!ofile.good()) {
        cout << "Error Opening " << filename << endl;
    } else if(!exists) {
        for(set<string>::iterator it = selection.begin(); it != selection.end(); it++) {
            ofile << *it << "\t";
        }
        ofile << endl;
    }
    return true;
}

set<string> Measure::getSelection() {
    return selection;
}

bool Measure::setSelection(set<string> new_selection) {
    bool toReturn = true;
    set<string>::iterator set_it;
    map<string,double*>::iterator map_it;
    
    selection = set<string>();

    for(set_it = new_selection.begin(); set_it != new_selection.end(); set_it++) {
        map_it = varmap.find(*set_it);
        if(map_it != varmap.end()) {
            selection.insert(*set_it);
        } else {
            toReturn = false;
        }
    }
    if(!ofile.good()) {
        return toReturn;
    }
    ofile.seekp(0);
    for(set<string>::iterator it = selection.begin(); it != selection.end(); it++) {
        ofile << *it << "\t";
    }
    ofile << endl;
    
    return toReturn;
}

set<string> Measure::getVariables() {
    set<string> toReturn;
    map<string,double*>::iterator it;
    for(it = varmap.begin(); it != varmap.end(); it++) {
        toReturn.insert(it->first);
    }
    return toReturn;
}
 
map<string,double> Measure::getVariablesMap() {
    map<string,double> toReturn;
    for(map<string,double*>::iterator it = varmap.begin(); it != varmap.end(); it++) {
        toReturn.insert(std::pair<string,double>(it->first, *it->second));
    }
    return toReturn;
}

void Measure::copy(const Measure& toCopy) {
    std::map<string, double*>::iterator it;

    peak= toCopy.peak;
    min= toCopy.min;
    vartakeoff= toCopy.vartakeoff;
    repol = toCopy.repol;
    amp = toCopy.amp;
    maxderiv= toCopy.maxderiv;
    maxderiv2nd= toCopy.maxderiv2nd;
    cl= toCopy.cl;
    told = toCopy.told;
    mint = toCopy.mint;
    maxt = toCopy.maxt;
    varold = toCopy.varold;
    derivold = toCopy.derivold;
    minflag = toCopy.minflag;
    maxflag = toCopy.maxflag;
    ampflag = toCopy.ampflag;
    ddrflag = toCopy.ddrflag;
    derivt2 = toCopy.derivt2;
    derivt1 = toCopy.derivt1;
    derivt = toCopy.derivt;
    deriv2ndt = toCopy.deriv2ndt;
    durflag = toCopy.durflag;
    percrepol = toCopy.percrepol;
    returnflag = toCopy.returnflag;
    dur = toCopy.dur;
    varname = toCopy.varname;    

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

    selection = toCopy.selection;
};

bool Measure::write(bool useFlags, bool reset) {
    set<string>::iterator it;

    if(useFlags&&!returnflag) {
        return false;
    }

    if(!ofile.good()) {
        return false;
    }
    for(it = selection.begin(); it != selection.end(); it++) {
        ofile << *varmap[*it] << "\t";
    }
    ofile << endl;
    if(reset) {
        this->reset();
    }
    return true;
}

//################################################################
// Function to track properties (e.g. peak, min, duration) of
// specified state variable and return status flag to calling fxn.
//################################################################
bool Measure::measure(double time, double var)
{
    double deriv,deriv2nd;
    
    returnflag = false;  //default for return...set to 1 when props ready for output
    
    deriv=(var-varold)/(time-told);
    deriv2nd=(deriv-derivold)/(time-told);
    
    if(deriv>maxderiv){             // Track value and time of max 1st deriv
        maxderiv=deriv;
        derivt=time;
    }
    
    if(deriv2nd>.02&&var>(0.01*abs(min)+min)&&!ddrflag){   // Track 2nd deriv for SAN ddr
        vartakeoff=var;
        deriv2ndt=time;
        ddr=(vartakeoff-min)/(time-mint);
        ddrflag=true;
    }
    
    if(minflag&&var>peak){          // Track value and time of peak
        peak=var;
        maxt=time;
    }
    else if((peak-min)>0.3*abs(min))    // Assumes true max is more than 30% greater than the min.
        maxflag=true;
    
    if(var<min){                        // Track value and time of min
        min=var;
        mint=time;
    }
    else
        minflag=true;
    
    if(var>repol&&!durflag){          // t1 for dur calculation = first time var crosses repol.
        durtime1=time;            // will depend on percrepol - default is 50 but can be changed.
        durflag=true;
    }
    
    if(maxflag&&minflag&&!ampflag){
        amp=peak-min;
        ampflag = true;
        cl=derivt-derivt1;
        derivt2=derivt1;
        derivt1=derivt;
        repol = (1-percrepol*0.01)*amp+min;
    }
    
    if(durflag&&var<repol){
        dur=time-durtime1;
        durflag=false;
        returnflag = true;  // lets calling fxn know that it is time to output and reset.
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



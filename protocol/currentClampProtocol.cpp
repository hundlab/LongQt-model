#include "currentClampProtocol.h"
#include "cellutils.h"
#include "pvarscurrentclamp.h"

CurrentClamp::CurrentClamp()  : Protocol(){
    stimdur = 1.0;  // stim duration, ms
    stimt = 0.0;    // time of first stim, ms
    stimval = 0.0;  // stim current amplitude, uA/uF
    bcl = 1000;     // basic cycle length, ms
    numstims = 1;   // # of stimuli to apply
    paceflag = 0;   // 1 to pace cell.
    stimflag = 0;
    stimcounter = 0;

    GetSetRef toInsert;
    pars["stimdur"]= toInsert.Initialize("double", [this] () {return std::to_string(stimdur);}, [this] (const string& value) {stimdur = std::stod(value);});
    pars["stimt"]= toInsert.Initialize("double", [this] () {return std::to_string(stimt);}, [this] (const string& value) {stimt = std::stod(value);});
    pars["stimval"]= toInsert.Initialize("double", [this] () {return std::to_string(stimval);}, [this] (const string& value) {stimval = std::stod(value);});
    pars["bcl"] = toInsert.Initialize("double", [this] () {return std::to_string(bcl);}, [this] (const string& value) {bcl = std::stod(value);});
    pars["numstims"]= toInsert.Initialize("int", [this] () {return std::to_string(numstims);}, [this] (const string& value) {numstims = std::stoi(value);});
    pars["paceflag"]= toInsert.Initialize("bool", [this] () {return CellUtils::to_string(paceflag);}, [this] (const string& value) {paceflag = CellUtils::stob(value);});
    pars["numtrials"]= toInsert.Initialize("int", [this] () {return std::to_string(numtrials);},
            [this] (const string& value) {
                numtrials = std::stoi(value);
                this->pvars->calcIonChanParams();});
    type = "Current Clamp Protocol";
    this->pvars = new PvarsCurrentClamp(this);

    CellUtils::set_default_vals(this);
}
//overriden deep copy funtion
CurrentClamp* CurrentClamp::clone(){
    return new CurrentClamp(*this);
};
CurrentClamp::CurrentClamp(const CurrentClamp& toCopy) : Protocol(toCopy){
    this->CCcopy(toCopy);
}

CurrentClamp& CurrentClamp::operator=(const CurrentClamp& toCopy) {
    this->copy(toCopy);
    this->CCcopy(toCopy);
    return *this;
}

void CurrentClamp::CCcopy(const CurrentClamp& toCopy) {
    stimdur = toCopy.stimdur;  // stim duration, ms
    stimt = toCopy.stimt;    // time of first stim, ms
    stimval = toCopy.stimval;  // stim current amplitude, uA/uF
    bcl = toCopy.bcl;     // basic cycle length, ms
    numstims = toCopy.numstims;   // # of stimuli to apply
    stimflag = toCopy.stimflag;
    stimcounter = toCopy.stimcounter;
    paceflag = toCopy.paceflag;   // 1 to pace cell.
}

// External stimulus.
int CurrentClamp::stim()
{
    if(cell->t>=stimt&&cell->t<(stimt+stimdur)){
        if(stimflag==0){
            stimcounter++;
            stimflag=1;
            if(stimcounter>int(numstims)){
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

void CurrentClamp::setupTrial() {
    set<string> temp;
    for(auto& pvar: *pvars) {
        temp.insert(pvar.first);
    }
    cell->setConstantSelection(temp);
    temp.clear();
    time = cell->t = 0.0;      // reset time
    this->readInCellState(this->readCellState);
    this->pvars->setIonChanParams();
    doneflag=1;     // reset doneflag
    cell->setOuputfileVariables(
        CellUtils::strprintf((datadir+"/"+dvarsoutfile).c_str(),trial));
    this->measureMgr->setupMeasures(
        CellUtils::strprintf((datadir+"/"+propertyoutfile).c_str(),trial));
}

bool CurrentClamp::runTrial() {
    this->setupTrial();

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
            this->measureMgr->measure(time);
       }
        if (int(writeflag)==1&&time>writetime&&pCount%int(writeint)==0) {
            cell->writeVariables();
        }
        cell->setV(vM); //Overwrite vOld with new value
        pCount++;
    }

    // Output final (ss) property values for each trial
    this->measureMgr->writeLast(CellUtils::strprintf(
        (datadir+"/"+finalpropertyoutfile).c_str(),trial));

    // Output parameter values for each trial
    cell->setOutputfileConstants(CellUtils::strprintf(
        (datadir+"/"+finaldvarsoutfile).c_str(),trial));
    cell->writeConstants();
    this->measureMgr->close();
    cell->closeFiles();
    this->writeOutCellState(this->writeCellState);

    return true;
}

void CurrentClamp::readInCellState(bool read) {
    if(read) {
        cell->readCellState(cellStateDir+"/"+cellStateFile+std::to_string(trial)+".xml");
        this->stimt = cell->t;
        this->tMax += this->cell->t;
    }
}



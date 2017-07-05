#include "currentClampProtocol.h"
#include "cellutils.h"
#include "pvarscurrentclamp.h"

CurrentClamp::CurrentClamp()  : Protocol() {
    __measureMgr.reset(new MeasureManager(cell()));
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
                this->pvars().calcIonChanParams();});
    type = "Current Clamp Protocol";

    CellUtils::set_default_vals(*this);
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

CurrentClamp::~CurrentClamp() {}

Cell* CurrentClamp::cell() const
{
    return __cell.get();
}

void CurrentClamp::cell(Cell* cell) {
    if(__measureMgr) {
        __measureMgr->clear();
        __measureMgr->cell(cell);
    }
    if(__pvars)
        pvars().clear();
    this->__cell.reset(cell);
}

CellPvars& CurrentClamp::pvars() {
    return *this->__pvars;
}

MeasureManager &CurrentClamp::measureMgr() {
    return *this->__measureMgr;
}

void CurrentClamp::CCcopy(const CurrentClamp& toCopy) {
    __cell.reset(toCopy.cell()->clone());
    stimdur = toCopy.stimdur;  // stim duration, ms
    stimt = toCopy.stimt;    // time of first stim, ms
    stimval = toCopy.stimval;  // stim current amplitude, uA/uF
    bcl = toCopy.bcl;     // basic cycle length, ms
    numstims = toCopy.numstims;   // # of stimuli to apply
    stimflag = toCopy.stimflag;
    stimcounter = toCopy.stimcounter;
    paceflag = toCopy.paceflag;   // 1 to pace cell.
    __pvars = unique_ptr<PvarsCurrentClamp>(new PvarsCurrentClamp(*toCopy.__pvars));
    __pvars->protocol(this);
    __measureMgr.reset(toCopy.__measureMgr->clone());
    __measureMgr->cell(cell());
}

// External stimulus.
int CurrentClamp::stim()
{
    if(__cell->t>=stimt&&__cell->t<(stimt+stimdur)){
        if(stimflag==0){
            stimcounter++;
            stimflag=1;
            if(stimcounter>int(numstims)){
                doneflag = 0;
                return 0;
            }
        }
        __cell->externalStim(stimval);
    }
    else if(stimflag==1){     //trailing edge of stimulus
        stimt=stimt+bcl;
        stimflag=0;
        __cell->apTime = 0.0;
    }

    __cell->apTime = __cell->apTime+__cell->dt;

    doneflag = 1;
    return 1;
};

void CurrentClamp::setupTrial() {
    set<string> temp;
    for(auto& pvar: *__pvars) {
        temp.insert(pvar.first);
    }
    __cell->setConstantSelection(temp);
    temp.clear();
    time = __cell->t = 0.0;      // reset time
    this->readInCellState(this->readCellState);
    this->__pvars->setIonChanParams();
    doneflag=1;     // reset doneflag
    __cell->setOuputfileVariables(
        CellUtils::strprintf((datadir+"/"+dvarsoutfile).c_str(),__trial));
    this->__measureMgr->setupMeasures(
        CellUtils::strprintf((datadir+"/"+propertyoutfile).c_str(),__trial));
}

bool CurrentClamp::runTrial() {
    this->setupTrial();

    //###############################################################
    // Every time step, currents, concentrations, and Vm are calculated.
    //###############################################################
    int pCount = 0;

    while(int(doneflag)&&(time<tMax)){
        time = __cell->tstep(stimt);    // Update time
        __cell->updateCurr();    // Update membrane currents
        if(int(paceflag)==1)  // Apply stimulus
            stim();

        __cell->updateConc();   // Update ion concentrations
        vM=__cell->updateV();     // Update transmembrane potential

        //##### Output select variables to file  ####################
        if(int(measflag)==1&&__cell->t>meastime){
            this->__measureMgr->measure(time);
       }
        if (int(writeflag)==1&&time>writetime&&pCount%int(writeint)==0) {
            __cell->writeVariables();
        }
        __cell->setV(vM); //Overwrite vOld with new value
        pCount++;
    }

    // Output final (ss) property values for each trial
    this->__measureMgr->writeLast(CellUtils::strprintf(
        (datadir+"/"+finalpropertyoutfile).c_str(),__trial));

    // Output parameter values for each trial
    __cell->setOutputfileConstants(CellUtils::strprintf(
        (datadir+"/"+finaldvarsoutfile).c_str(),__trial));
    __cell->writeConstants();
    this->__measureMgr->close();
    __cell->closeFiles();
    this->writeOutCellState(this->writeCellState);

    return true;
}

void CurrentClamp::readInCellState(bool read) {
    if(read) {
        __cell->readCellState(cellStateDir+"/"+cellStateFile+std::to_string(__trial)+".xml");
        this->stimt = __cell->t;
        this->tMax += this->__cell->t;
    }
}



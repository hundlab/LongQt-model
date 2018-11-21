#include "currentClampProtocol.h"
#include "cellutils.h"
#include "pvarscurrentclamp.h"
using namespace LongQt;
using namespace std;

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
    stimbegin = 0;
    stimend = 0;

    this->mkmap();

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

shared_ptr<Cell> CurrentClamp::cell() const
{
    return __cell;
}

void CurrentClamp::cell(shared_ptr<Cell> cell) {
    __measureMgr.reset(new MeasureManager(cell));
    if(__pvars)
        pvars().clear();
    this->__cell = cell;
}

PvarsCell& CurrentClamp::pvars() {
    return *this->__pvars;
}

MeasureManager &CurrentClamp::measureMgr() {
    return *this->__measureMgr;
}

void CurrentClamp::CCcopy(const CurrentClamp& toCopy) {
    this->mkmap();
    __cell.reset(toCopy.cell()->clone());
    stimdur = toCopy.stimdur;  // stim duration, ms
    stimt = toCopy.stimt;    // time of first stim, ms
    stimval = toCopy.stimval;  // stim current amplitude, uA/uF
    bcl = toCopy.bcl;     // basic cycle length, ms
    numstims = toCopy.numstims;   // # of stimuli to apply
    stimflag = toCopy.stimflag;
    stimcounter = toCopy.stimcounter;
    paceflag = toCopy.paceflag;   // 1 to pace cell.
    __pvars.reset(new PvarsCurrentClamp(*toCopy.__pvars, this));
    __measureMgr.reset(new MeasureManager(*toCopy.__measureMgr, __cell));
}

// External stimulus.
int CurrentClamp::stim()
{
    if(stimcounter >= numstims) {
        return 0;
    }
    if(stimbegin <= __cell->t && __cell->t < stimend) {
        __cell->externalStim(stimval);
    }
    if(stimend < __cell->t) {
        stimbegin += bcl;
        stimend += bcl;
        __cell->apTime = 0.0;
        stimcounter++;
    }

    __cell->apTime = __cell->apTime+__cell->dt;
    return 1;
///////////////////////////////////////////test
//    if(__cell->t>=stimt&&__cell->t<(stimt+stimdur)){
//        if(stimflag==0){
//            stimcounter++;
//            stimflag=1;
//            if(stimcounter>int(numstims)){
//                doneflag = 0;
//                return 0;
//            }
//        }
//        __cell->externalStim(stimval);
//    }
//    else if(stimflag==1){     //trailing edge of stimulus
//        stimt=stimt+bcl;
//        stimflag=0;
//        __cell->apTime = 0.0;
//    }

//    __cell->apTime = __cell->apTime+__cell->dt;

//    doneflag = 1;
//    return 1;
};

void CurrentClamp::setupTrial() {
    this->Protocol::setupTrial();
    set<string> temp;
    for(auto& pvar: *__pvars) {
        temp.insert(pvar.first);
    }
    __cell->setConstantSelection(temp);
    temp.clear();
    __cell->t = 0.0;      // reset time
    stimbegin = stimt;
    stimend = stimt+stimdur;
//    stimt = 0;
    stimcounter = 0;
    this->readInCellState(this->readCellState);
    this->__pvars->setIonChanParams();
    runflag=true;     // reset doneflag
    __cell->setOuputfileVariables(
        CellUtils::strprintf(getDataDir()+"/"+dvarsoutfile,__trial));
    this->__measureMgr->setupMeasures(
        CellUtils::strprintf(getDataDir()+"/"+propertyoutfile,__trial));
}

bool CurrentClamp::runTrial() {
    this->setupTrial();
    this->runBefore(*this);

    //###############################################################
    // Every time step, currents, concentrations, and Vm are calculated.
    //###############################################################
    int pCount = 0;
    int numrunsLeft = this->numruns;
    const double& time = __cell->t;
    double nextRunT = this->firstRun + this->runEvery;

    while(runflag&&(time<tMax)){
        if(numrunsLeft > 0 && time >= nextRunT) {
            this->runDuring(*this);
            --numrunsLeft;
            nextRunT += this->runEvery;
        }
        __cell->tstep(stimt);    // Update time
        __cell->updateCurr();    // Update membrane currents
        if(int(paceflag)==1)  // Apply stimulus
            stim();

        __cell->updateConc();   // Update ion concentrations
        double vM=__cell->updateV();     // Update transmembrane potential

        //##### Output select variables to file  ####################
        if(measflag == 1 && __cell->t>meastime){
            this->__measureMgr->measure(time);
       }

        if (writeflag && time>writetime && (pCount%writeint==0)) {
            __cell->writeVariables();
        }
        __cell->setV(vM); //Overwrite vOld with new value
        pCount++;
    }

    // Output final (ss) property values for each trial
    this->__measureMgr->writeLast(CellUtils::strprintf(
        getDataDir()+"/"+finalpropertyoutfile,__trial));

    // Output parameter values for each trial
    __cell->setOutputfileConstants(CellUtils::strprintf(
        getDataDir()+"/"+finaldvarsoutfile,__trial));
    __cell->writeConstants();
    this->__measureMgr->close();
    __cell->closeFiles();
    this->writeOutCellState(this->writeCellState);

    this->runAfter(*this);
    return true;
}

void CurrentClamp::readInCellState(bool read) {
    if(read) {
        __cell->readCellState(cellStateDir.absolutePath().toStdString()+"/"+cellStateFile+std::to_string(__trial)+".xml");
        this->stimt = __cell->t;
        this->tMax += this->__cell->t;
    }
}

void CurrentClamp::mkmap() {
    GetSetRef toInsert;
    __pars["stimdur"]= toInsert.Initialize("double", [this] () {return std::to_string(stimdur);}, [this] (const string& value) {stimdur = std::stod(value);});
    __pars["stimt"]= toInsert.Initialize("double", [this] () {return std::to_string(stimt);}, [this] (const string& value) {stimt = std::stod(value);});
    __pars["stimval"]= toInsert.Initialize("double", [this] () {return std::to_string(stimval);}, [this] (const string& value) {stimval = std::stod(value);});
    __pars["bcl"] = toInsert.Initialize("double", [this] () {return std::to_string(bcl);}, [this] (const string& value) {bcl = std::stod(value);});
    __pars["numstims"]= toInsert.Initialize("int", [this] () {return std::to_string(numstims);}, [this] (const string& value) {numstims = std::stoi(value);});
    __pars["paceflag"]= toInsert.Initialize("bool", [this] () {return CellUtils::to_string(paceflag);}, [this] (const string& value) {paceflag = CellUtils::stob(value);});
    __pars["numtrials"]= toInsert.Initialize("int", [this] () {return std::to_string(numtrials);},
            [this] (const string& value) {
                auto temp = std::stoi(value);
                if(temp == numtrials) return;
                numtrials = temp;
                this->pvars().calcIonChanParams();});
}

const char* CurrentClamp::name = "Current Clamp Protocol";
const char* CurrentClamp::type() const {
    return name;
}

#include "voltageClampProtocol.h"
#include "pvarsvoltageclamp.h"

VoltageClamp::VoltageClamp()  : Protocol(){
    __measureMgr.reset(new MeasureManager(cell()));
    v1 = v2 = v3 = v4 = v5 = 0;
    t1 = t2 = t3 = t4 = t5 = 0;
    type = "Voltage Clamp Protocol";
    this->mkmap();

    CellUtils::set_default_vals(*this);
}
//overriden deep copy funtion
VoltageClamp* VoltageClamp::clone(){
    return new VoltageClamp(*this);
};

VoltageClamp::VoltageClamp(const VoltageClamp& toCopy) : Protocol(toCopy){
    this->CCcopy(toCopy);
}

VoltageClamp& VoltageClamp::operator=(const VoltageClamp& toCopy) {
    this->copy(toCopy);
    this->CCcopy(toCopy);
    return *this;
}
shared_ptr<Cell> VoltageClamp::cell() const
{
    return __cell;
}

void VoltageClamp::cell(shared_ptr<Cell> cell) {
    if(__measureMgr) {
        __measureMgr->clear();
        __measureMgr->cell(cell);
    }
    if(__pvars)
        pvars().clear();
    this->__cell = cell;
}

CellPvars &VoltageClamp::pvars() {
    return *this->__pvars;
}


void VoltageClamp::CCcopy(const VoltageClamp& toCopy) {
    this->mkmap();

    __cell.reset(toCopy.cell()->clone());
    v1 = toCopy.v1;
    v2= toCopy.v2;
    v3 = toCopy.v3;
    v4 = toCopy.v4;
    v5 = toCopy.v5;
    t1 = toCopy.t1;
    t2 = toCopy.t2;
    t3 = toCopy.t3;
    t4 = toCopy.t4;
    t5 = toCopy.t5;
    __pvars = unique_ptr<PvarsVoltageClamp>(new PvarsVoltageClamp(*toCopy.__pvars));
    __pvars->protocol(this);
    __measureMgr.reset(toCopy.__measureMgr->clone());
    __measureMgr->cell(cell());
}

// External stimulus.
int VoltageClamp::clamp()
{
    if(__cell->t >= t5 && t5 != 0) {
        vM = __cell->vOld = v5;
    } else if(__cell->t >= t4 && t4 != 0) {
        vM = __cell->vOld = v4;
    } else if(__cell->t >= t3 && t3 != 0) {
        vM = __cell->vOld = v3;
    } else if(__cell->t >= t2 && t2 != 0) {
        vM = __cell->vOld = v2;
    } else if(__cell->t >= t1) {
        vM = __cell->vOld = v1;
    }
    return 1;
};

void VoltageClamp::setupTrial() {
    this->Protocol::setupTrial();
    set<string> temp;
    for(auto& pvar: pvars()) {
        temp.insert(pvar.first);
    }
    __cell->setConstantSelection(temp);
    temp.clear();

    time = __cell->t = 0.0;      // reset time
    this->readInCellState(this->readCellState);
    this->pvars().setIonChanParams();
    doneflag=1;     // reset doneflag

    this->__measureMgr->setupMeasures(
        CellUtils::strprintf((getDataDir()+"/"+propertyoutfile).c_str(),__trial));
    __cell->setOuputfileVariables(
        CellUtils::strprintf((getDataDir()+"/"+dvarsoutfile).c_str(),__trial));
}

bool VoltageClamp::runTrial() {
    this->setupTrial();
    this->runBefore(*this);
    //        if (int(readflag)==1)
    //            readvals(cell->vars, readfile);  // Init SVs before each trial.
    //###############################################################
    // Every time step, currents, concentrations, and Vm are calculated.
    //###############################################################
    int pCount = 0;
    int numrunsLeft = this->numruns;
    double nextRunT = this->firstRun + this->runEvery;

    while(int(doneflag)&&(time<tMax)){
        if(numrunsLeft > 1 && time >= nextRunT) {
            this->runDuring(*this);
            --numrunsLeft;
            nextRunT += this->runEvery;
        }

        //what should stimt be made to be??
        time = __cell->tstep(0.0);    // Update time
        __cell->updateCurr();    // Update membrane currents

        __cell->updateConc();   // Update ion concentrations
        vM=__cell->updateV();     // Update transmembrane potential

        clamp();

        //##### Output select variables to file  ####################
        if(int(measflag)==1&&__cell->t>meastime){
            this->__measureMgr->measure(time);
        }
        if (int(writeflag)==1&&time>writetime&&pCount%int(writeint)==0) {
            __cell->writeVariables();
            //                douts[mvnames.size()+(trial+1)*(mvnames.size()+1)].writevals(datamap,writefile,'a');
        }
        __cell->setV(vM); //Overwrite vOld with new value
        pCount++;
    }

    // Output final (ss) property values for each trial
    this->__measureMgr->writeLast(CellUtils::strprintf(
        (getDataDir()+"/"+finalpropertyoutfile).c_str(),__trial));

    // Output parameter values for each trial
    __cell->setOutputfileConstants(CellUtils::strprintf(
        (getDataDir()+"/"+finaldvarsoutfile).c_str(),__trial));
    __cell->writeConstants();
    this->__measureMgr->close();
    __cell->closeFiles();
    this->writeOutCellState(this->writeCellState);

    this->runAfter(*this);
    return true;
}
void VoltageClamp::readInCellState(bool read) {
    if(read) {
        __cell->readCellState(cellStateDir.absolutePath().toStdString()+"/"+cellStateFile+".xml");
        this->tMax += this->__cell->t;
        this->t1 += this->__cell->t;
        this->t2 += this->__cell->t;
        this->t3 += this->__cell->t;
        this->t4 += this->__cell->t;
        this->t5 += this->__cell->t;
    }
}

MeasureManager &VoltageClamp::measureMgr()
{
    return *this->__measureMgr;
}
void VoltageClamp::mkmap() {
    pars.erase("numtrials");
    pars.erase("meastime");
    GetSetRef toInsert;
    pars["v1"] = toInsert.Initialize("double",[this] () {return std::to_string(v1);},[this] (const string& value) {v1 = std::stod(value);});
    pars["v2"] = toInsert.Initialize("double",[this] () {return std::to_string(v2);},[this] (const string& value) {v2 = std::stod(value);});
    pars["v3"] = toInsert.Initialize("double",[this] () {return std::to_string(v3);},[this] (const string& value) {v3 = std::stod(value);});
    pars["v4"] = toInsert.Initialize("double",[this] () {return std::to_string(v4);},[this] (const string& value) {v4 = std::stod(value);});
    pars["v5"] = toInsert.Initialize("double",[this] () {return std::to_string(v5);},[this] (const string& value) {v5 = std::stod(value);});
    pars["t1"] = toInsert.Initialize("double",[this] () {return std::to_string(t1);},[this] (const string& value) {t1 = std::stod(value);});
    pars["t2"] = toInsert.Initialize("double",[this] () {return std::to_string(t2);},[this] (const string& value) {t2 = std::stod(value);});
    pars["t3"] = toInsert.Initialize("double",[this] () {return std::to_string(t3);},[this] (const string& value) {t3 = std::stod(value);});
    pars["t4"] = toInsert.Initialize("double",[this] () {return std::to_string(t4);},[this] (const string& value) {t4 = std::stod(value);});
    pars["t5"] = toInsert.Initialize("double",[this] () {return std::to_string(t5);},[this] (const string& value) {t5 = std::stod(value);});


}

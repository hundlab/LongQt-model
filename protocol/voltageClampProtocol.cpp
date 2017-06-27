#include "voltageClampProtocol.h"
#include "pvarsvoltageclamp.h"

VoltageClamp::VoltageClamp()  : Protocol(){
    v1 = v2 = v3 = v4 = v5 = 0;
    t1 = t2 = t3 = t4 = t5 = 0;
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

    type = "Voltage Clamp Protocol";
    this->pvars.reset(new PvarsVoltageClamp(this));

    CellUtils::set_default_vals(this);
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

void VoltageClamp::CCcopy(const VoltageClamp& toCopy) {
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
    ((PvarsVoltageClamp*)pvars.get())->protocol(this);
}

// External stimulus.
int VoltageClamp::clamp()
{
    if(cell->t >= t5 && t5 != 0) {
        vM = cell->vOld = v5;
    } else if(cell->t >= t4 && t4 != 0) {
        vM = cell->vOld = v4;
    } else if(cell->t >= t3 && t3 != 0) {
        vM = cell->vOld = v3;
    } else if(cell->t >= t2 && t2 != 0) {
        vM = cell->vOld = v2;
    } else if(cell->t >= t1) {
        vM = cell->vOld = v1;
    }
    return 1;
};

void VoltageClamp::setupTrial() {
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

    this->measureMgr->setupMeasures(
        CellUtils::strprintf((datadir+"/"+propertyoutfile).c_str(),trial));
    cell->setOuputfileVariables(
        CellUtils::strprintf((datadir+"/"+dvarsoutfile).c_str(),trial));
}

bool VoltageClamp::runTrial() {
    this->setupTrial();
    //        if (int(readflag)==1)
    //            readvals(cell->vars, readfile);  // Init SVs before each trial.
    //###############################################################
    // Every time step, currents, concentrations, and Vm are calculated.
    //###############################################################
    int pCount = 0;

    while(int(doneflag)&&(time<tMax)){

        //what should stimt be made to be??
        time = cell->tstep(0.0);    // Update time
        cell->updateCurr();    // Update membrane currents

        cell->updateConc();   // Update ion concentrations
        vM=cell->updateV();     // Update transmembrane potential

        clamp();

        //##### Output select variables to file  ####################
        if(int(measflag)==1&&cell->t>meastime){
            this->measureMgr->measure(time);
        }
        if (int(writeflag)==1&&time>writetime&&pCount%int(writeint)==0) {
            cell->writeVariables();
            //                douts[mvnames.size()+(trial+1)*(mvnames.size()+1)].writevals(datamap,writefile,'a');
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
void VoltageClamp::readInCellState(bool read) {
    if(read) {
        cell->readCellState(cellStateDir+"/"+cellStateFile+".xml");
        this->tMax += this->cell->t;
        this->t1 += this->cell->t;
        this->t2 += this->cell->t;
        this->t3 += this->cell->t;
        this->t4 += this->cell->t;
        this->t5 += this->cell->t;
    }
}


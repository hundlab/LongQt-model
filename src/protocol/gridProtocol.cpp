// 5/10/2016
//################################################
//protocol for a grid off cells
//class definitions
//################################################

#include "gridProtocol.h"
#include "gridCell.h"
#include "cellutils.h"

#include <QFile>
#include <QDebug>
using namespace LongQt;
using namespace std;

GridProtocol::GridProtocol() : CurrentClamp() {
    __measureMgr.reset(new GridMeasureManager(this->__cell));
    grid = __cell->getGrid();
    this->__pvars.reset(new PvarsGrid(grid));
    this->mkmap();
    stimval2 = stimval;
    stimdur2 = stimdur;
    bcl2 = bcl;
    stimt2 = stimt;
    propertyoutfile = "cell_%%i_%%i_"+this->propertyoutfile;
    finalpropertyoutfile = "cell_%%i_%%i_"+this->finalpropertyoutfile;
    dvarsoutfile = "cell_%%i_%%i_"+this->dvarsoutfile;
    finaldvarsoutfile = "cell_%%i_%%i_"+this->finaldvarsoutfile;

    CellUtils::set_default_vals(*this);
}
//overriden deep copy funtion
GridProtocol* GridProtocol::clone(){
    return new GridProtocol(*this);
};
GridProtocol::GridProtocol(const GridProtocol& toCopy) : CurrentClamp(toCopy){
    this->CCcopy(toCopy);
}
void GridProtocol::CCcopy(const GridProtocol& toCopy) {
    this->mkmap();
    this->setStim2(toCopy.stim2);
    stimval2 = toCopy.stimval2;
    stimdur2 = toCopy.stimdur2;
    bcl2 = toCopy.bcl2;
    stimt2 = toCopy.stimt2;
    __cell.reset(dynamic_cast<GridCell*>(toCopy.cell()->clone()));
    this->stimNodes = toCopy.stimNodes;
    this->grid = this->__cell->getGrid();
    __pvars = unique_ptr<PvarsGrid>(new PvarsGrid(*toCopy.__pvars));
    __pvars->setGrid(this->grid);
    __measureMgr.reset(toCopy.__measureMgr->clone());
    __measureMgr->cell(this->__cell);
}
// External stimulus.
int GridProtocol::stim()
{
    for(auto n : __stimN) {
        auto cell = n->cell;
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
    }
    return 1;
};

void GridProtocol::setupTrial() {
    this->Protocol::setupTrial();
    for(auto& n: stimNodes) {
        auto n_ptr = (*grid)(n);
        if(n_ptr) {
            __stimN.insert(n_ptr);
        }
    }
    for(auto& n: stimNodes2) {
        auto n_ptr = (*grid)(n);
        if(n_ptr) {
            __stimN2.insert(n_ptr);
        }
    }
    set<string> temp;
    for(auto& pvar: pvars()) {
        temp.insert(pvar.first);
    }
    __cell->setConstantSelection(temp);
    temp.clear();
    this->__measureMgr->setupMeasures(getDataDir()+"/"+
        CellUtils::strprintf(propertyoutfile.c_str(),__trial));

    time = __cell->t = 0.0;      // reset time

    this->readInCellState(this->readCellState);

    this->pvars().setIonChanParams();
    doneflag=1;     // reset doneflag

    __cell->setOuputfileVariables(getDataDir()+"/"
        +CellUtils::strprintf(dvarsoutfile.c_str(),__trial));
}

bool GridProtocol::runTrial() {
    this->setupTrial();
    this->runBefore(*this);

    //###############################################################
    // Every time step, currents, concentrations, and Vm are calculated.
    //###############################################################
    int pCount = 0;
    bool stimSet = false;
    int numrunsLeft = this->numruns;
    double nextRunT = this->firstRun + this->runEvery;
    while(int(doneflag)&&(time<tMax)){
        if(numrunsLeft > 0 && time >= nextRunT) {
            this->runDuring(*this);
            --numrunsLeft;
            nextRunT += this->runEvery;
        }
        time = __cell->tstep(stimt);    // Update time
        __cell->updateCurr();    // Update membrane currents
        if(stim2&&(!stimSet)&&(__cell->t >= stimt2)) {
            this->swapStims();
            stimSet=true;
        }
        if(int(paceflag)==1) {  // Apply stimulus
            stim();
        }
        __cell->updateV();

        __cell->updateConc();   // Update ion concentrations

        //##### Output select variables to file  ####################
        if(int(measflag)==1&&__cell->t>meastime){
            this->__measureMgr->measure(time);
        }
        if (int(writeflag)==1&&time>writetime&&pCount%int(writeint)==0) {
            __cell->writeVariables();
        }
        pCount++;
    }

    // Output final (ss) property values for each trial
    this->__measureMgr->writeLast(CellUtils::strprintf(
        (getDataDir()+"/"+finalpropertyoutfile).c_str(),__trial));

    // Output parameter values for each trial
    __cell->setOutputfileConstants(getDataDir()+"/"+CellUtils::strprintf(
        finaldvarsoutfile.c_str(),__trial));
    __cell->writeConstants();
    this->__measureMgr->close();
    __cell->closeFiles();
    this->writeOutCellState(this->writeCellState);
    if(stimSet) {
        this->swapStims();
    }
    this->runAfter(*this);
    return true;
}
set<pair<int,int>>& GridProtocol::getStimNodes() {
    return stimNodes;
}
bool GridProtocol::writepars(QXmlStreamWriter& xml) {
    bool toReturn;
    toReturn = this->__cell->writeGridfile(xml);
    toReturn &= CurrentClamp::writepars(xml);
    return toReturn;
}
int GridProtocol::readpars(QXmlStreamReader& xml) {
    this->grid->reset();
    bool toReturn = this->__cell->readGridfile(xml);
    toReturn &= (bool)CurrentClamp::readpars(xml);
    return toReturn;
}
string GridProtocol::setToString(set<pair<int,int>>& nodes) {
    stringstream toReturn;
    for(auto node : nodes) {
        toReturn << node.first << " " << node.second << "\t";
    }
    return toReturn.str();
}
Grid& GridProtocol::getGrid() {
    return *this->grid;
}
GridMeasureManager& GridProtocol::gridMeasureMgr() {
    return *this->__measureMgr;
}
shared_ptr<Cell> GridProtocol::cell() const
{
    return __cell;
}

void GridProtocol::cell(shared_ptr<Cell> cell) {
    if(!cell || string(cell->type()) != "gridCell") {
        return;
    }
    if(__measureMgr) {
        __measureMgr->clear();
        __measureMgr->cell(dynamic_pointer_cast<GridCell>(cell));
    }
    if(__pvars)
        pvars().clear();
    this->__cell = dynamic_pointer_cast<GridCell>(cell);
}

bool GridProtocol::cell(const string&) {
    return false;
}

list<string> GridProtocol::cellOptions() {
    return {""};
}

PvarsCell& GridProtocol::pvars() {
    return *this->__pvars;
}

MeasureManager &GridProtocol::measureMgr() {
    return *this->__measureMgr;
}
set<pair<int,int>> GridProtocol::stringToSet(string nodesList) {
    set<pair<int,int>> toReturn;
    stringstream stream(nodesList);
    while(!stream.eof()) {
        pair<int,int> p(-1,-1);
        stream >> p.first >> p.second;
        shared_ptr<Node> n = (*grid)(p);
        if(n) {
            toReturn.insert(p);
        }
    }
//    cell->closeFiles();

    return toReturn;
}

void GridProtocol::swapStims() {
    double temp;
    temp = stimval;
    stimval = stimval2;
    stimval2 = temp;

    temp = stimdur;
    stimdur = stimdur2;
    stimdur2 = temp;

    temp = bcl;
    bcl = bcl2;
    bcl2 = temp;

    temp = stimt;
    stimt = stimt2;
    stimt2 = temp;

    auto temp2 = __stimN;
    __stimN = __stimN2;
    __stimN2 = temp2;
}

void GridProtocol::setStim2(bool enable) {
    if(enable == stim2) {
        return;
    }
    stim2 = !stim2;
    if(stim2) {
        GetSetRef toInsert;
        pars["stimt2"] = toInsert.Initialize("double", 
                [this] () {return std::to_string(this->stimt2);}, 
                [this] (const string& value) {this->stimt2 = stod(value);});
        pars["bcl2"] = toInsert.Initialize("double", 
                [this] () {return std::to_string(this->stim2);}, 
                [this] (const string& value) {this->stim2 = stod(value);});
        pars["stimdur2"] = toInsert.Initialize("double", 
                [this] () {return std::to_string(this->stim2);}, 
                [this] (const string& value) {this->stim2 = stod(value);});
        pars["stimval2"] = toInsert.Initialize("double", 
                [this] () {return std::to_string(this->stim2);}, 
                [this] (const string& value) {this->stim2 = stod(value);});
        pars["stimNodes2"]= toInsert.Initialize("set", 
                [this] () {return setToString(stimNodes2);}, 
                [this] (const string& value) {stimNodes2 = stringToSet(value);});
    } else {
        pars.erase("stimt2");
        pars.erase("bcl2");
        pars.erase("stimdur2");
        pars.erase("stimval2");
        pars.erase("stimNodes2");
    }
}
bool GridProtocol::getStim2() {
    return this->stim2;
}
void GridProtocol::mkmap() {
    GetSetRef toInsert;
    pars["gridFile"]= toInsert.Initialize("file",
            [this] () {return __cell->gridfile();},
            [this] (const string& value) {__cell->setGridfile(value);});
//    pars["measNodes"]= toInsert.Initialize("set",
//            [this] () {return setToString(dataNodes);},
//            [this] (const string& value) {dataNodes = stringToSet(value);});
    pars["stimNodes"]= toInsert.Initialize("set",
            [this] () {return setToString(stimNodes);},
            [this] (const string& value) {stimNodes = stringToSet(value);});
    pars["secondStim"]= toInsert.Initialize("bool",
            [this] () {return CellUtils::to_string(this->stim2);},
            [this] (const string& value) {this->setStim2(CellUtils::stob(value));});
    pars["celltype"]= toInsert.Initialize("cell",
            [this] () {return "";},
            [] (const string&) {});
    pars.erase("numtrials");
    pars["paceflag"].set("true");
    pars.erase("paceflag");
}

const char* GridProtocol::name = "Grid Protocol";
const char* GridProtocol::type() const {
    return name;
}

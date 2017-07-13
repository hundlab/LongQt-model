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

GridProtocol::GridProtocol() : CurrentClamp() {
    __measureMgr.reset(new GridMeasureManager(this->__cell.get()));
    grid = __cell->getGrid();
    this->__pvars.reset(new PvarsGrid(grid));
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
    type = "Grid Protocol";
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
    __cell.reset(dynamic_cast<GridCell*>(toCopy.cell()->clone()));
    this->stimNodes = toCopy.stimNodes;
    this->grid = this->__cell->getGrid();
    __pvars = unique_ptr<PvarsGrid>(new PvarsGrid(*toCopy.__pvars));
    __pvars->setGrid(this->grid);
    __measureMgr.reset(toCopy.__measureMgr->clone());
    __measureMgr->cell(this->__cell.get());
}
// External stimulus.
int GridProtocol::stim()
{
    for(auto it : stimNodes) {
        Cell* cell = (*grid)(it)->cell.get();
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
    set<string> temp;
    for(auto& pvar: pvars()) {
        temp.insert(pvar.first);
    }
    __cell->setConstantSelection(temp);
    temp.clear();
    this->__measureMgr->setupMeasures(datadir+"/"+
        CellUtils::strprintf(propertyoutfile.c_str(),__trial));

    time = __cell->t = 0.0;      // reset time

    this->readInCellState(this->readCellState);

    this->pvars().setIonChanParams();
    doneflag=1;     // reset doneflag

    __cell->setOuputfileVariables(datadir+"/"
        +CellUtils::strprintf(dvarsoutfile.c_str(),__trial));
}

bool GridProtocol::runTrial() {
    this->setupTrial();

    //###############################################################
    // Every time step, currents, concentrations, and Vm are calculated.
    //###############################################################
    int pCount = 0;

    while(int(doneflag)&&(time<tMax)){


        time = __cell->tstep(stimt);    // Update time
        __cell->updateCurr();    // Update membrane currents
        if(stim2 && stimt2 > __cell->t) {
            this->swapStims();
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
        (datadir+"/"+finalpropertyoutfile).c_str(),__trial));

    // Output parameter values for each trial
    __cell->setOutputfileConstants(datadir+"/"+CellUtils::strprintf(
        finaldvarsoutfile.c_str(),__trial));
    __cell->writeConstants();
    this->__measureMgr->close();
    __cell->closeFiles();
    this->writeOutCellState(this->writeCellState);
    if(stim2 && stimt2 > __cell->t) {
        this->swapStims();
    }
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
GridMeasureManager& GridProtocol::gridMeasureMgr() {
    return *this->__measureMgr;
}
Cell* GridProtocol::cell() const
{
    return __cell.get();
}

void GridProtocol::cell(Cell* cell) {
    if(__measureMgr) {
        __measureMgr->clear();
        __measureMgr->cell(dynamic_cast<GridCell*>(cell));
    }
    if(__pvars)
        pvars().clear();
    this->__cell.reset(dynamic_cast<GridCell*>(cell));
}

bool GridProtocol::cell(const string&) {
    return false;
}

list<string> GridProtocol::cellOptions() {
    return {""};
}

CellPvars& GridProtocol::pvars() {
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
        Node* n = (*grid)(p);
        if(n != NULL) {
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

    set<pair<int,int>> temp2 = stimNodes;
    stimNodes = stimNodes2;
    stimNodes2 = temp2;
}

void GridProtocol::setStim2(bool enable) {
    if(enable == stim2) {
        return;
    }
    stim2 = !stim2;
    if(stim2) {
        GetSetRef toInsert;
        pars["stim2"] = toInsert.Initialize("double", 
                [this] () {return std::to_string(this->stim2);}, 
                [this] (const string& value) {this->stim2 = stod(value);});
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
        pars.erase("stim2");
        pars.erase("bcl2");
        pars.erase("stimdur2");
        pars.erase("stimval2");
        pars.erase("stimNodes2");
    }
}

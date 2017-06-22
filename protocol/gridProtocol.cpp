// 5/10/2016
//################################################
//protocol for a grid off cells
//class definitions
//################################################

#include "gridProtocol.h"
#include "gridCell.h"
#include "cellutils.h"
#include "pvarsgrid.h"

#include <QFile>
#include <QDebug>

GridProtocol::GridProtocol() : CurrentClamp(){
    GridCell* temp = new GridCell();
    measureMgr = new GridMeasureManager(temp);
    cell = temp;
    grid = temp->getGrid();
    this->pvars = new PvarsGrid(grid);
    baseCellMap = cellMap;
    baseCellMap["Inexcitable Cell"] = [] () {return new Cell;};
    cellMap.clear();
    cellMap["gridCell"] = [] () {return (Cell*) new GridCell;};
    GetSetRef toInsert;
    pars["gridFile"]= toInsert.Initialize("file",
            [temp] () {return temp->gridfile();},
            [temp] (const string& value) {temp->setGridfile(value);});
//    pars["measNodes"]= toInsert.Initialize("set",
//            [this] () {return setToString(dataNodes);},
//            [this] (const string& value) {dataNodes = stringToSet(value);});
    pars["stimNodes"]= toInsert.Initialize("set",
            [this] () {return setToString(stimNodes);},
            [this] (const string& value) {stimNodes = stringToSet(value);});
    pars["secondStim"]= toInsert.Initialize("bool",
            [this] () {return CellUtils::to_string(this->stim2);},
            [this] (const string& value) {this->setStim2(CellUtils::stob(value));});
    pars.erase("numtrials");
    pars["paceflag"].set("true");
    pars.erase("paceflag");
    type = "Grid Protocol";
    stimval2 = stimval;
    stimdur2 = stimdur;
    bcl2 = bcl;
    stimt2 = stimt;

    CellUtils::set_default_vals(this);
}
//overriden deep copy funtion
GridProtocol* GridProtocol::clone(){
    return new GridProtocol(*this);
};
GridProtocol::GridProtocol(const GridProtocol& toCopy) : CurrentClamp(toCopy){
    this->CCcopy(toCopy);
}
void GridProtocol::CCcopy(const GridProtocol& toCopy) {
    this->stimNodes = toCopy.stimNodes;
    this->grid = ((GridCell*)this->cell)->getGrid();
    ((PvarsGrid*)pvars)->setGrid(this->grid);
}
// External stimulus.
int GridProtocol::stim()
{
    for(auto it : stimNodes) {
        Cell* cell = grid->findNode(it)->cell;
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
    for(auto& pvar: *pvars) {
        temp.insert(pvar.first);
    }
    cell->setConstantSelection(temp);
    temp.clear();
    this->measureMgr->setupMeasures(
        CellUtils::strprintf((datadir+"/"+propertyoutfile).c_str(),trial));

    time = cell->t = 0.0;      // reset time

    this->readInCellState(this->readCellState);

    this->pvars->setIonChanParams();
    doneflag=1;     // reset doneflag

    cell->setOuputfileVariables(datadir+"/"
        +"cell_%i_%i_"+CellUtils::strprintf(dvarsoutfile.c_str(),trial));

}

bool GridProtocol::runTrial() {
    this->setupTrial();

    //###############################################################
    // Every time step, currents, concentrations, and Vm are calculated.
    //###############################################################
    int pCount = 0;

    while(int(doneflag)&&(time<tMax)){


        time = cell->tstep(stimt);    // Update time
        cell->updateCurr();    // Update membrane currents
        if(stim2 && stimt2 > cell->t) {
            this->swapStims();
        }
        if(int(paceflag)==1) {  // Apply stimulus
            stim();
        }
        cell->updateV();

        cell->updateConc();   // Update ion concentrations

        //##### Output select variables to file  ####################
        if(int(measflag)==1&&cell->t>meastime){
            this->measureMgr->measure(time);
        }
        if (int(writeflag)==1&&time>writetime&&pCount%int(writeint)==0) {
            cell->writeVariables();
        }
        pCount++;
    }

    // Output final (ss) property values for each trial
    this->measureMgr->writeLast(CellUtils::strprintf(
        (datadir+"/"+finalpropertyoutfile).c_str(),trial));

    // Output parameter values for each trial
    cell->setOutputfileConstants(datadir+"/"+"cell_%i_%i_"+CellUtils::strprintf(
        finaldvarsoutfile.c_str(),trial));
    cell->writeConstants();
    this->measureMgr->close();
    cell->closeFiles();
    this->writeOutCellState(this->writeCellState);
    if(stim2 && stimt2 > cell->t) {
        this->swapStims();
    }
    return true;
}

map<string, CellUtils::CellInitializer>& GridProtocol::getCellMap() {
    return baseCellMap;
}
set<pair<int,int>>& GridProtocol::getStimNodes() {
    return stimNodes;
}
bool GridProtocol::writepars(QXmlStreamWriter& xml) {
    bool toReturn;
    toReturn = ((GridCell*)this->cell)->writeGridfile(xml);
    toReturn &= CurrentClamp::writepars(xml);
    return toReturn;
}
int GridProtocol::readpars(QXmlStreamReader& xml) {
    this->grid->reset();
    bool toReturn = ((GridCell*)this->cell)->readGridfile(xml);
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
GridMeasureManager* GridProtocol::gridMeasureMgr() {
    return (GridMeasureManager*)this->measureMgr;
}
set<pair<int,int>> GridProtocol::stringToSet(string nodesList) {
    set<pair<int,int>> toReturn;
    stringstream stream(nodesList);
    while(!stream.eof()) {
        pair<int,int> p(-1,-1);
        stream >> p.first >> p.second;
        Node* n = grid->findNode(p);
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

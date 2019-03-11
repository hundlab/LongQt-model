// 5/10/2016
//################################################
// protocol for a grid off cells
// class definitions
//################################################

#include "gridProtocol.h"
#include "cellutils.h"
#include "gridCell.h"

#include <QFile>
using namespace LongQt;
using namespace std;

GridProtocol::GridProtocol() : CurrentClamp() {
  __measureMgr.reset(new GridMeasureManager(this->__cell));
  grid = __cell->getGrid();
  this->__pvars.reset(new PvarsGrid(grid));
  this->mkmap();
  propertyoutfile = "cell_{{}}_{{}}_" + this->propertyoutfile;
  dvarsoutfile = "cell_{{}}_{{}}_" + this->dvarsoutfile;

  CellUtils::set_default_vals(*this);
}
// overriden deep copy funtion
GridProtocol* GridProtocol::clone() { return new GridProtocol(*this); }
GridProtocol::GridProtocol(const GridProtocol& toCopy) : CurrentClamp(toCopy) {
  this->CCcopy(toCopy);
}
void GridProtocol::CCcopy(const GridProtocol& toCopy) {
  this->mkmap();
  __cell.reset(dynamic_cast<GridCell*>(toCopy.cell()->clone()));
  this->stimNodes = toCopy.stimNodes;
  this->grid = this->__cell->getGrid();
  __pvars.reset(new PvarsGrid(*toCopy.__pvars, this->grid));
  __measureMgr.reset(
      new GridMeasureManager(*toCopy.__measureMgr, this->__cell));
}

void GridProtocol::setupTrial() {
  this->Protocol::setupTrial();
  __cell->setup(stimNodes, __measureMgr->dataNodes());

  stimbegin = stimt;
  stimend = stimt + stimdur;
  stimcounter = 0;

  set<string> temp;
  for (auto& pvar : pvars()) {
    temp.insert(pvar.first);
  }
  __cell->setConstantSelection(temp);
  temp.clear();
  this->__measureMgr->setupMeasures();

  __cell->t = 0.0;  // reset time

  this->readInCellState(this->readCellState);

  this->pvars().setIonChanParams();
  runflag = true;  // reset doneflag

  __cell->setOuputfileVariables(getDataDir() + "/" +
                                CellUtils::strprintf(dvarsoutfile, __trial));
}

bool GridProtocol::runTrial() {
  this->setupTrial();
  this->runBefore(*this);

  //###############################################################
  // Every time step, currents, concentrations, and Vm are calculated.
  //###############################################################
  const double& time = __cell->t;
  int pCount = 0;
  int numrunsLeft = this->numruns;
  double nextRunT = this->firstRun + this->runEvery;
  while (runflag && (time < tMax)) {
    if (numrunsLeft > 0 && time >= nextRunT) {
      this->runDuring(*this);
      --numrunsLeft;
      nextRunT += this->runEvery;
    }
    __cell->tstep(stimt);      // Update time
    __cell->updateCurr();      // Update membrane currents
    if (int(paceflag) == 1) {  // Apply stimulus
      stim();
    }
    __cell->updateV();

    __cell->updateConc();  // Update ion concentrations

    //##### Output select variables to file  ####################
    if (int(measflag) == 1 && __cell->t > meastime) {
      this->__measureMgr->measure(time);
    }
    if (writeflag && time > writetime && pCount % writeint == 0) {
      __cell->writeVariables();
    }
    __cell->setV();
    pCount++;
  }

  this->__measureMgr->write(getDataDir() + "/" +
                            CellUtils::strprintf(propertyoutfile, __trial));
  __cell->closeFiles();
  this->writeOutCellState(this->writeCellState);
  this->runAfter(*this);
  return true;
}
set<pair<int, int>>& GridProtocol::getStimNodes() { return stimNodes; }
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
string GridProtocol::setToString(set<pair<int, int>>& nodes) {
  stringstream toReturn;
  for (auto node : nodes) {
    toReturn << node.first << " " << node.second << "\t";
  }
  return toReturn.str();
}
Grid& GridProtocol::getGrid() { return *this->grid; }
GridMeasureManager& GridProtocol::gridMeasureMgr() {
  return *this->__measureMgr;
}
shared_ptr<Cell> GridProtocol::cell() const { return __cell; }

std::shared_ptr<GridCell> GridProtocol::gridCell() const { return __cell; }

void GridProtocol::cell(shared_ptr<Cell> cell) {
  if (!cell || string(cell->type()) != "gridCell") {
    return;
  }
  auto new_cell = dynamic_pointer_cast<GridCell>(cell);
  __measureMgr.reset(new GridMeasureManager(new_cell));
  if (__pvars) pvars().clear();
  this->__cell = new_cell;
}

bool GridProtocol::cell(const string&) { return false; }

list<string> GridProtocol::cellOptions() { return {""}; }

PvarsCell& GridProtocol::pvars() { return *this->__pvars; }

MeasureManager& GridProtocol::measureMgr() { return *this->__measureMgr; }

set<pair<int, int>> GridProtocol::stringToSet(string nodesList) {
  set<pair<int, int>> toReturn;
  stringstream stream(nodesList);
  pair<int, int> p(-1, -1);
  while (!stream.eof()) {
    stream >> p.first >> p.second;
    toReturn.insert(p);
  }
  //    cell->closeFiles();

  return toReturn;
}

void GridProtocol::mkmap() {
  GetSetRef toInsert;
  __pars["gridFile"] = toInsert.Initialize(
      "file", [this]() { return __cell->gridfile(); },
      [this](const string& value) { __cell->setGridfile(value); });
  //    pars["measNodes"]= toInsert.Initialize("set",
  //            [this] () {return setToString(dataNodes);},
  //            [this] (const string& value) {dataNodes = stringToSet(value);});
  __pars["stimNodes"] = toInsert.Initialize(
      "set", [this]() { return setToString(stimNodes); },
      [this](const string& value) { stimNodes = stringToSet(value); });
  __pars["celltype"] = toInsert.Initialize("cell", [this]() { return ""; },
                                           [](const string&) {});
  __pars.erase("numtrials");
  __pars["paceflag"].set("true");
  __pars.erase("paceflag");
}

const char* GridProtocol::name = "Grid Protocol";
const char* GridProtocol::type() const { return name; }

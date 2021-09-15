#include "voltageClampProtocol.h"
#include <algorithm>
#include "pvarsvoltageclamp.h"
using namespace LongQt;
using namespace std;

VoltageClamp::VoltageClamp() : Protocol() {
  __measureMgr.reset(new MeasureManager(cell()));
  __measureMgr->determineWriteTime = false;
  clampsHint = 0;
  this->mkmap();
  CellUtils::set_default_vals(*this);
}
// overriden deep copy funtion
VoltageClamp* VoltageClamp::clone() { return new VoltageClamp(*this); }

VoltageClamp::VoltageClamp(const VoltageClamp& toCopy) : Protocol(toCopy) {
  this->CCcopy(toCopy);
}

VoltageClamp& VoltageClamp::operator=(const VoltageClamp& toCopy) {
  this->copy(toCopy);
  this->CCcopy(toCopy);
  return *this;
}

bool VoltageClamp::writepars(QXmlStreamWriter& xml) {
  bool toReturn = true;
  xml.writeStartElement("clamps");
  for (auto clamp : __clamps) {
    xml.writeStartElement("clamp");
    xml.writeAttribute("time", QString::number(clamp.first));
    xml.writeCharacters(QString::number(clamp.second));
    xml.writeEndElement();
  }
  xml.writeEndElement();
  toReturn &= Protocol::writepars(xml);
  return toReturn;
}

int VoltageClamp::readpars(QXmlStreamReader& xml) {
  if (!CellUtils::readNext(xml, "clamps")) return 1;
  vector<pair<double, double>> clamps;
  while (!xml.atEnd() && xml.readNextStartElement()) {
    double time = xml.attributes().value("time").toDouble();
    xml.readNext();
    double voltage = xml.text().toDouble();
    clamps.push_back({time, voltage});
    xml.readNext();
  }
  this->Protocol::readpars(xml);
  this->clamps(clamps);
}
shared_ptr<Cell> VoltageClamp::cell() const { return __cell; }

void VoltageClamp::cell(shared_ptr<Cell> cell) {
  __measureMgr.reset(new MeasureManager(cell));
  __measureMgr->determineWriteTime = false;
  if (__pvars) pvars().clear();
  this->__cell = cell;
}

PvarsCell& VoltageClamp::pvars() { return *this->__pvars; }

void VoltageClamp::CCcopy(const VoltageClamp& toCopy) {
  this->mkmap();

  clampsHint = 0;
  __cell.reset(toCopy.cell()->clone());
  __clamps = toCopy.__clamps;
  __pvars.reset(new PvarsVoltageClamp(*toCopy.__pvars, this));
  __measureMgr.reset(new MeasureManager(*toCopy.__measureMgr, cell()));
  __measureMgr->determineWriteTime = false;
}

// External stimulus.
bool VoltageClamp::clamp(double& vM) {
  double t = __cell->t;
  if (__clamps.empty() || t < __clamps[0].first) return false;
  int initialClamp = clampsHint;
  int pos = clampsHint;
  while (pos < __clamps.size() && __clamps[pos].first <= t) {
    clampsHint = pos;
    pos += 1;
  }
  vM = __cell->vOld = __clamps[clampsHint].second;
  return bool(clampsHint - initialClamp);
}

void VoltageClamp::setupTrial() {
  this->Protocol::setupTrial();
  set<string> temp;
  for (auto& pvar : pvars()) {
    temp.insert(pvar.first);
  }
  __cell->setConstantSelection(temp);
  temp.clear();

  __cell->setup();
  this->readInCellState(this->readCellState);
  this->pvars().setIonChanParams();
  runflag = true;  // reset doneflag

  this->__measureMgr->setupMeasures();
  __cell->setOuputfileVariables(
      CellUtils::strprintf(getDataDir() + "/" + dvarsoutfile, __trial));
}

bool VoltageClamp::runTrial() {
  this->setupTrial();
  this->runBefore(*this);
  //        if (int(readflag)==1)
  //            readvals(cell->vars, readfile);  // Init SVs before each trial.
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

    // what should stimt be made to be??
    __cell->tstep(0.0);    // Update time
    __cell->updateCurr();  // Update membrane currents

    __cell->updateConc();           // Update ion concentrations
    double vM = __cell->updateV();  // Update transmembrane potential

    bool clampChanged = clamp(vM);

    //##### Output select variables to file  ####################
    if (measflag && __cell->t > meastime) {
      this->__measureMgr->measure(time, clampChanged);
    }
    if (writeflag && time > writetime && (pCount % writeint == 0)) {
      __cell->writeVariables();
    }
    __cell->setV(vM);  // Overwrite vOld with new value
    pCount++;
  }

  this->__measureMgr->saveCurrent();
  this->__measureMgr->write(
      CellUtils::strprintf(getDataDir() + "/" + propertyoutfile, __trial));
  this->writeOutCellState(this->writeCellState);
  __cell->closeFiles();

  this->runAfter(*this);
  return true;
}
void VoltageClamp::readInCellState(bool read) {
  // BROKEN
  if (read) {
    __cell->readCellState(cellStateDir.absolutePath().toStdString() + "/" +
                          cellStateFile + ".xml");
    this->tMax += this->__cell->t;
  }
}

MeasureManager& VoltageClamp::measureMgr() { return *this->__measureMgr; }

int VoltageClamp::insertClamp(double time, double voltage) {
  pair<double, double> p = {time, voltage};
  auto pos =
      std::lower_bound(__clamps.begin(), __clamps.end(), p,
                       [](pair<double, double> f, pair<double, double> s) {
                         return f.first < s.first;
                       });
  int posInt = pos - __clamps.begin();
  __clamps.insert(pos, p);
  return posInt;
}

void VoltageClamp::changeClampVoltage(int pos, double voltage) {
  if (pos >= __clamps.size() || pos < 0) return;
  __clamps[pos] = {__clamps[pos].first, voltage};
}
void VoltageClamp::removeClamp(int pos) {
  if (pos >= __clamps.size() || pos < 0) return;
  __clamps.erase(__clamps.begin() + pos);
}

const std::vector<pair<double, double>>& VoltageClamp::clamps() {
  return __clamps;
}

void VoltageClamp::clamps(vector<pair<double, double>> clamps) {
  __clamps = clamps;
  std::sort(__clamps.begin(), __clamps.end(),
            [](pair<double, double> f, pair<double, double> s) {
              return f.first < s.first;
            });
}
void VoltageClamp::mkmap() {
  __pars.erase("numtrials");
  __pars.erase("meastime");
  GetSetRef toInsert;
}
const char* VoltageClamp::name = "Voltage Clamp Protocol";
const char* VoltageClamp::type() const { return name; }

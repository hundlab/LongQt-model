#include "measuremanager.h"
#include "cellutils.h"
#include "logger.h"
#include "measuredefault.h"
using namespace LongQt;
using namespace std;

MeasureManager::MeasureManager(shared_ptr<Cell> cell) : __cell(cell) {}

MeasureManager::~MeasureManager() {}

MeasureManager::MeasureManager(const MeasureManager& o,
                               std::shared_ptr<Cell> cell)
    : __cell(cell), measMaker(o.measMaker) {
    variableSelection = o.variableSelection;
}

map<string, set<string>> MeasureManager::selection() {
  return this->variableSelection;
}

void MeasureManager::selection(map<string, set<string>> sel) {
  this->variableSelection = sel;
}

void MeasureManager::addMeasure(string var, set<string> selection) {
  variableSelection.insert({var, selection});
}

void MeasureManager::removeMeasure(string var) { measures.erase(var); }

void MeasureManager::setupMeasures(string filename) {
  this->measures.clear();
  if (variableSelection.count("vOld") == 0) {
    variableSelection.insert({"vOld", {}});
  }
  this->removeBad();
  this->ofile.setFileName(filename);
  string nameLine = "";
  for (auto& item1 : variableSelection) {
    auto& name = item1.first;
    auto& sel = item1.second;
    auto it =
        measures.insert({name, this->measMaker.buildMeasure(name, sel)}).first;
    string nameStr = it->second->getNameString(name);
    nameLine += nameStr;
  }
  nameLine += "\n";
  ofile.write(nameLine);
}

void MeasureManager::measure(double time) {
  bool write = false;
  for (auto& m : this->measures) {
    bool varWrite = m.second->measure(time, __cell->var(m.first));
    if (m.first == "vOld" && varWrite) {
      write = true;
    }
  }
  if (write) {
    this->write();
    this->resetMeasures();
  }
}

void MeasureManager::write() {
  std::string text = "";
  for (auto& meas : measures) {
    text += meas.second->getValueString();
  }
  text += "\n";
  ofile.write(text);
}

void MeasureManager::close() { ofile.close(); }

void MeasureManager::resetMeasures() {
  for (auto& meas : this->measures) {
    meas.second->reset();
  }
}

//#############################################################
// read and mvars style file and use it to set measures list
// where varname is the first word on the line
// and all sequential words are properties to measure (the selection)
//#############################################################
bool MeasureManager::readMvarsFile(QXmlStreamReader& xml) {
  variableSelection.clear();
  set<string> possible_vars = __cell->vars();
  if (!CellUtils::readNext(xml, "mvars")) return false;
  if (xml.readNextStartElement() && xml.name() == "percrepol") {
    xml.readNext();
    measMaker.percrepol(xml.text().toDouble());
    xml.skipCurrentElement();
  } else {
    return true;
  }
  while (!xml.atEnd() && xml.readNextStartElement() && xml.name() == "mvar") {
    string varname = xml.attributes().value("name").toString().toStdString();
    set<string> selection;
    while (!xml.atEnd() && xml.readNextStartElement() &&
           xml.name() == "property") {
      xml.readNext();
      string propName = xml.text().toString().toStdString();
      selection.insert(propName);
      xml.skipCurrentElement();
    }
    if (possible_vars.count(varname) == 1) {
      variableSelection.insert({varname, selection});
    }
  }
  return true;
};

bool MeasureManager::writeMVarsFile(QXmlStreamWriter& xml) {
  xml.writeStartElement("mvars");
  xml.writeTextElement("percrepol", QString::number(measMaker.percrepol()));
  for (auto& sel : this->variableSelection) {
    xml.writeStartElement("mvar");
    xml.writeAttribute("name", sel.first.c_str());
    for (auto& prop : sel.second) {
      xml.writeTextElement("property", prop.c_str());
    }
    xml.writeEndElement();
  }
  xml.writeEndElement();
  return 0;
}

void MeasureManager::removeBad() {
  list<map<string, set<string>>::iterator> bad;
  auto vars = __cell->vars();
  for (auto it = this->variableSelection.begin(); it != variableSelection.end();
       ++it) {
    if (vars.count(it->first) != 1) {
      bad.push_back(it);
    }
  }
  for (auto& b : bad) {
    variableSelection.erase(b);
  }
}

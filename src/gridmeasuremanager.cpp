#include "gridmeasuremanager.h"
#include "logger.h"
using namespace LongQt;
using namespace std;

GridMeasureManager::GridMeasureManager(shared_ptr<GridCell> cell)
    : MeasureManager(cell) {
  this->grid = cell->getGrid();
}

GridMeasureManager::GridMeasureManager(const GridMeasureManager& o,
                                       shared_ptr<GridCell> cell)
    : MeasureManager(o, cell), grid(cell->getGrid()) {
  __dataNodes = o.__dataNodes;
}

void GridMeasureManager::dataNodes(set<pair<int, int>> nodes) {
  this->__dataNodes = nodes;
}

set<pair<int, int>> GridMeasureManager::dataNodes() {
  return this->__dataNodes;
}

bool GridMeasureManager::writeMVarsFile(QXmlStreamWriter& xml) {
  xml.writeStartElement("gridMvars");
  MeasureManager::writeMVarsFile(xml);
  xml.writeStartElement("dataNodes");
  for (auto& node : this->__dataNodes) {
    xml.writeStartElement("node");
    xml.writeAttribute("row", QString::number(node.first));
    xml.writeAttribute("col", QString::number(node.second));
    xml.writeEndElement();
  }
  xml.writeEndElement();
  xml.writeEndElement();
  return true;
}

bool GridMeasureManager::readMvarsFile(QXmlStreamReader& xml) {
  if (!CellUtils::readNext(xml, "gridMvars")) return false;
  MeasureManager::readMvarsFile(xml);
  if (xml.readNextStartElement() && xml.name() == "dataNodes") {
    while (!xml.atEnd() && xml.readNextStartElement() && xml.name() == "node") {
      auto attrib = xml.attributes();
      int row = attrib.value("row").toInt();
      int col = attrib.value("col").toInt();
      this->__dataNodes.insert({row, col});
      xml.skipCurrentElement();
    }
  }
  return true;
}

void GridMeasureManager::setupMeasures() {
  this->measures.clear();
  if (variableSelection.count("vOld") == 0) {
    variableSelection.insert({"vOld", {}});
  }
  std::string header;
  bool first = true;
  for (auto& node : this->__dataNodes) {
    int nodeSelectedCount = 0;
    auto pos = measures.insert({node, {}}).first;
    for (auto& items1 : variableSelection) {
      auto& varName = items1.first;
      auto& sel = items1.second;
      auto gridNode = (*this->grid)(node);
      if (gridNode && gridNode->cell->hasVar(varName)) {
        pos->second.insert(
            {varName, this->measMaker.buildMeasure(varName, sel)});
        nodeSelectedCount += sel.size();
      }
      this->numSelected.insert({node, nodeSelectedCount});
    }
    if (first) {
      first = false;
    } else {
      header += '\t';
    }
    header += this->nameString(node);
  }
  header += '\n';
}

std::string GridMeasureManager::nameString(pair<int, int> node) const {
  string nameStr = "";
  for (auto& meas : this->measures.at(node)) {
    nameStr +=
        meas.second->getNameString("cell" + to_string(node.first) + "_" +
                                   to_string(node.second) + "/" + meas.first);
  }
  return nameStr;
}

void GridMeasureManager::measure(double time) {
  for (auto& pos : this->measures) {
    bool writeCell = false;
    for (auto& meas : pos.second) {
      double val = (*this->grid)(pos.first)->cell->var(meas.first);
      if (meas.second->measure(time, val) && meas.first == "vOld") {
        writeCell = true;
      }
    }
    if (writeCell) {
      this->saveSingleCell(pos.first);
      this->resetMeasures(pos.first);
    }
  }
}
void GridMeasureManager::write(std::string filename) {
  std::ofstream ofile;
  ofile.open(filename, std::ios_base::app);
  if (!ofile.good()) {
    ofile.close();
    Logger::getInstance()->write<std::runtime_error>(
        "GridMeasureManager: Error Opening \'{}\'", filename);
  }
  ofile << header;
  ofile << std::scientific;
  bool first = true;
  for (auto node : this->__dataNodes) {
    for (auto& row : this->data[node]) {
      for (double val : row) {
        if (first) {
          first = false;
        } else {
          ofile << '\t';
        }
        ofile << val;
      }
    }
    ofile << '\n';
  }
  ofile.flush();
  ofile.close();
}

void GridMeasureManager::saveSingleCell(pair<int, int> node) {
  std::vector<double> dat(this->numSelected[node]);
  auto pos = dat.begin();
  for (auto& meas : measures[node]) {
    auto vals = meas.second->getValues();
    pos = std::copy(vals.begin(), vals.end(), pos);
  }
  this->data[node].push_back(dat);
}

void GridMeasureManager::resetMeasures(pair<int, int> node) {
  for (auto& meas : this->measures[node]) {
    meas.second->reset();
  }
}

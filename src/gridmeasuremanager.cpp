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

void GridMeasureManager::setupMeasures(string filenameTemplate) {
  this->measures.clear();
  if (variableSelection.count("vOld") == 0) {
    variableSelection.insert({"vOld", {}});
  }
  for (auto& node : this->__dataNodes) {
    auto pos = measures.insert({node, {}}).first;
    for (auto& sel : variableSelection) {
      auto gridNode = (*this->grid)(node);
      if (gridNode && gridNode->cell->hasVar(sel.first)) {
        pos->second.insert(
            {sel.first, this->measMaker.buildMeasure(sel.first, sel.second)});
      }
    }
    std::string filename =
        CellUtils::strprintf(filenameTemplate, node.first, node.second);
    this->ofiles.emplace(node, filename);
    auto& ofile = this->ofiles[node];
    ofile.write(this->nameString(node) + "\n");
  }
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
      this->writeSingleCell(pos.first, this->ofiles[pos.first]);
      this->resetMeasures(pos.first);
    }
  }
}
void GridMeasureManager::write() {
  for (auto& pos : measures) {
    FileOutputHandler& ofile = ofiles[pos.first];
    for (auto& meas : pos.second) {
      string valStr = meas.second->getValueString();
      ofile.write(valStr);
    }
    ofile.write("\n");
  }
}

void GridMeasureManager::writeSingleCell(pair<int, int> node,
                                         FileOutputHandler& file) {
  std::string text = "";
  for (auto& meas : measures[node]) {
    string valStr = meas.second->getValueString();
    text += valStr;
  }
  text += "\n";
  file.write(text);
}

void GridMeasureManager::resetMeasures(pair<int, int> node) {
  for (auto& meas : this->measures[node]) {
    meas.second->reset();
  }
}

void GridMeasureManager::close() {
  for (auto& ofile : this->ofiles) {
    ofile.second.close();
  }
}

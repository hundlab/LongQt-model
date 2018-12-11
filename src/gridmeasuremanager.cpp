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
      if ((*this->grid)(node) != NULL &&
          (*this->grid)(node)->cell->hasVar(sel.first)) {
        pos->second.insert(
            {sel.first, this->getMeasure(sel.first, sel.second)});
      }
    }
    QString filename =
        CellUtils::strprintf(filenameTemplate, node.first, node.second).c_str();
    this->ofiles[node].reset(new QFile(filename));
    auto ofile = this->ofiles[node];
    if (!ofile->open(QIODevice::WriteOnly | QIODevice::Text)) {
      Logger::getInstance()->write<std::runtime_error>(
          "MeasureManager: File could not be opened for writing");
    }
    if (ofile->write((this->nameString(node) + "\n").c_str()) == -1) {
      Logger::getInstance()->write<std::runtime_error>(
          "MeasureManager: File cound not be written to");
    }
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
      this->writeSingleCell(pos.first);
      this->resetMeasures(pos.first);
    }
  }
}
void GridMeasureManager::write(QFile* file) {
  if (!file->isOpen()) {
    Logger::getInstance()->write<std::runtime_error>(
        "MeasureManager: Measure file must be open to be written");
    return;
  }
  for (auto& pos : measures) {
    QFile& ofile = file ? *file : *ofiles[pos.first];
    this->lasts[pos.first] = "";
    for (auto& meas : pos.second) {
      string valStr = meas.second->getValueString();
      this->lasts[pos.first] += valStr;
      if (ofile.write(valStr.c_str()) == -1) {
        Logger::getInstance()->write<std::runtime_error>(
            "MeasureManager: File cound not be written to");
      }
    }
    lasts[pos.first] += "\n";
    if (ofile.write("\n") == -1) {
      Logger::getInstance()->write<std::runtime_error>(
          "MeasureManager: File cound not be written to");
    }
  }
}

void GridMeasureManager::writeSingleCell(pair<int, int> node, QFile* file) {
  QFile& ofile = file ? *file : *this->ofiles[node];
  if (!ofile.isOpen()) {
    Logger::getInstance()->write<std::runtime_error>(
        "MeasureManager: Measure file must be open to be written");
    return;
  }
  auto& last = this->lasts[node];
  last = "";
  for (auto& meas : measures[node]) {
    string valStr = meas.second->getValueString();
    last += valStr;
    if (ofiles[node]->write(valStr.c_str()) == -1) {
      Logger::getInstance()->write<std::runtime_error>(
          "MeasureManager: File cound not be written to");
    }
  }
  last += "\n";
  if (ofiles[node]->write("\n") == -1) {
    Logger::getInstance()->write<std::runtime_error>(
        "MeasureManager: File cound not be written to");
  }
}

void GridMeasureManager::writeLast(string filename) {
  for (auto& pos : this->measures) {
    QFile lastFile(
        CellUtils::strprintf(filename, pos.first.first, pos.first.second)
            .c_str());
    if (!lastFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
      Logger::getInstance()->write<std::runtime_error>(
          "MeasureManager: File could not be opened for writing.");
    }
    if (lastFile.write((this->nameString(pos.first) + "\n").c_str()) == -1) {
      Logger::getInstance()->write<std::runtime_error>(
          "MeasureManager: Last file cound not be written to");
    }
    if (this->lasts[pos.first] == "") {
      this->writeSingleCell(pos.first, &lastFile);
    } else {
      if (lastFile.write(lasts[pos.first].c_str()) == -1) {
        Logger::getInstance()->write<std::runtime_error>(
            "MeasureManager: Last file cound not be written to");
      }
    }
    lastFile.close();
  }
}

void GridMeasureManager::resetMeasures(pair<int, int> node) {
  for (auto& meas : this->measures[node]) {
    meas.second->reset();
  }
}

void GridMeasureManager::close() {
  for (auto& ofile : this->ofiles) {
    if (ofile.second->isOpen()) {
      ofile.second->close();
    }
  }
}
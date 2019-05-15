//####################################################
// This file contains general function definitions
// for simulation of excitable cell activity read and
// write functions.
//
// Copyright (C) 2011 Thomas J. Hund.
//####################################################

#include "cell.h"
#include <QFile>
#include <sstream>
#include "cellutils.h"
#include "logger.h"
using namespace LongQt;
using namespace std;

void Cell::setOuputfileVariables(string filename) {
  varsofile.open(filename, std::ios_base::app);
  varsofile << std::scientific;
  if (!varsofile.good()) {
    varsofile.close();
    Logger::getInstance()->write<std::runtime_error>(
        "Cell: Error Opening \'{}\'", filename);
  }
  bool first = true;
  for (auto& sel : this->getVariableSelection()) {
    if (first) {
      first = false;
    } else {
      varsofile << '\t';
    }
    varsofile << sel;
  }
  varsofile << '\n';
};

void Cell::setOutputfileConstants(string filename) {
  parsofile.open(filename, std::ios_base::app);
  parsofile << std::scientific;
  if (!parsofile.good()) {
    parsofile.close();
    Logger::getInstance()->write<std::runtime_error>(
        "Cell: Error Opening \'{}\'", filename);
  }
};

void Cell::writeVariables() {
  bool first = true;
  for (auto& sel : this->varsSelection) {
    if (!first) {
      varsofile << '\t';
    } else {
      first = false;
    }
    varsofile << this->var(sel);
  }
  varsofile << '\n';
}

std::vector<double> Cell::getVariablesVals() {
  std::vector<double> res(this->varsSelection.size());
  int i = 0;
  for (auto& sel : this->varsSelection) {
    res[i] = this->var(sel);
    ++i;
  }
  return res;
}

void Cell::writeConstants() {
  bool first = true;
  for (auto& sel : this->parsSelection) {
    if (!first) {
      parsofile << "\t";
    } else {
      first = false;
    }
    parsofile << this->par(sel);
  }
  parsofile << '\n';
}

void Cell::setConstantSelection(set<string> selection) {
  parsSelection.clear();
  for (auto& sel : selection) {
    if (this->hasPar(sel)) {
      parsSelection.insert(sel);
    } else {
      Logger::getInstance()->write("Cell: {} is not a var in cell", sel);
    }
  }
};

void Cell::setVariableSelection(set<string> selection) {
  varsSelection.clear();
  for (auto& sel : selection) {
    if (this->hasVar(sel)) {
      varsSelection.insert(sel);
    } else {
      Logger::getInstance()->write("Cell: {} is not a par in cell", sel);
    }
  }
};

set<string> Cell::getConstantSelection() { return parsSelection; }

set<string> Cell::getVariableSelection() { return varsSelection; }

bool Cell::writeCellState(string file) {
  QFile ofile(file.c_str());
  string name;

  if (!ofile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    Logger::getInstance()->write<std::runtime_error>("Cell: Error opening {}",
                                                     file);
    return 1;
  }
  QXmlStreamWriter xml(&ofile);
  xml.setAutoFormatting(true);
  xml.writeStartDocument();
  bool success = this->writeCellState(xml);
  xml.writeEndDocument();
  ofile.close();
  return success;
}
bool Cell::writeCellState(QXmlStreamWriter& xml) {
  xml.writeStartElement("cell");
  xml.writeAttribute("type", this->type());

  xml.writeStartElement("pars");
  for (auto& par : this->__pars) {
    xml.writeStartElement("par");
    xml.writeAttribute("name", par.first.c_str());
    xml.writeCharacters(QString::number(*par.second));
    xml.writeEndElement();
  }
  xml.writeEndElement();

  xml.writeStartElement("vars");
  for (auto& var : this->__vars) {
    xml.writeStartElement("var");
    xml.writeAttribute("name", var.first.c_str());
    xml.writeCharacters(QString::number(*var.second));
    xml.writeEndElement();
  }
  xml.writeEndElement();

  xml.writeEndElement();
  return true;
}
bool Cell::readCellState(string file) {
  QFile ifile(file.c_str());

  if (!ifile.open(QIODevice::ReadOnly)) {
    Logger::getInstance()->write<std::runtime_error>("Cell: Error opening {}",
                                                     file);
    return false;
  }
  QXmlStreamReader xml(&ifile);
  this->readCellState(xml);
  ifile.close();
  return true;
}
bool Cell::readCellState(QXmlStreamReader& xml) {
  string name = "";
  if (!CellUtils::readNext(xml, "cell")) return false;
  QString type = xml.attributes().value("type").toString();
  if (type != this->type()) {
    Logger::getInstance()->write<std::logic_error>(
        "Cell: cell state file is for {} but cell is of type {}",
        type.toStdString(), this->type());
  }
  if (!CellUtils::readNext(xml, "pars")) return false;
  while (!xml.atEnd() && xml.readNextStartElement()) {
    name = xml.attributes().value("name").toString().toStdString();
    xml.readNext();
    bool ok = true;
    double val = xml.text().toDouble(&ok);
    if (ok) {
      try {
        *(this->__pars.at(name)) = val;
      } catch (const std::out_of_range&) {
        Logger::getInstance()->write("Cell: {} not in cell pars", name);
      }
    }
    xml.readNext();
  }
  if (!CellUtils::readNext(xml, "vars")) return false;
  while (!xml.atEnd() && xml.readNextStartElement()) {
    name = xml.attributes().value("name").toString().toStdString();
    xml.readNext();
    bool ok = true;
    double val = xml.text().toDouble(&ok);
    if (ok) {
      try {
        *(this->__vars.at(name)) = val;
      } catch (const std::out_of_range&) {
        Logger::getInstance()->write("Cell: {} not in cell vars", name);
      }
    }
    xml.readNext();
  }
  return true;
}

void Cell::closeFiles() {
  this->parsofile.close();
  this->varsofile.close();
}

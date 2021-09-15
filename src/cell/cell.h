//################################################
// This header file contains class definition for
// simulation of excitable cell activity, with
// functions for input and output.
//
// Copyright (C) 2011 Thomas J. Hund.
// Email thomas.hund@osumc.edu
//#################################################

#ifndef MODEL
#define MODEL

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "cell_kernel.h"

namespace LongQt {

//######################################################
// Define class for parent cell.
//######################################################
class Cell : public CellKernel {
 public:
  Cell();
  Cell(const Cell& toCopy);
  virtual ~Cell() {}
  virtual Cell* clone() = 0;

  virtual void setOutputfileConstants(std::string filename);
  virtual void setOuputfileVariables(std::string filename);
  virtual void setConstantSelection(std::set<std::string> new_selection);
  virtual void setVariableSelection(std::set<std::string> new_selection);
  virtual std::set<std::string> getConstantSelection();
  virtual std::set<std::string> getVariableSelection();
  virtual void writeConstants();
  virtual void writeVariables();
  virtual std::vector<double> getVariablesVals();
  virtual bool writeCellState(std::string file);
  virtual bool writeCellState(QXmlStreamWriter& xml);
  virtual bool readCellState(std::string file);
  virtual bool readCellState(QXmlStreamReader& xml);
  virtual void closeFiles();

 protected:
  std::ofstream parsofile;
  std::ofstream varsofile;
  std::set<std::string> parsSelection;
  std::set<std::string> varsSelection;
};
}  // namespace LongQt

#endif

//################################################
// This code file
// defines Measure class used to track properties
// (e.g. peak, min, duration) of specified state variable.
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015
// Email thomas.hund@osumc.edu
//#################################################

#include "measure.h"
#include <iomanip>
#include <sstream>

using namespace LongQt;
using namespace std;

Measure::Measure(std::map<string, double* const> varmap, set<string> selected) {
  this->varmap = varmap;
  this->selection(selected);
};

Measure::Measure(const Measure& toCopy) : __selection(toCopy.__selection) {
  for (auto& var : toCopy.varmap) {
    *this->varmap[var.first] = *var.second;
  }
}

Measure::Measure(Measure&& toCopy) : __selection(toCopy.__selection) {
  for (auto& var : toCopy.varmap) {
    *this->varmap[var.first] = *var.second;
  }
};

void Measure::beforeOutput() {}

set<string> Measure::variables() {
  set<string> toReturn;
  for (auto& var : varmap) {
    toReturn.insert(var.first);
  }
  return toReturn;
}

map<string, double> Measure::variablesMap() {
  this->beforeOutput();
  map<string, double> toReturn;
  for (auto& var : varmap) {
    toReturn.insert(std::pair<string, double>(var.first, *var.second));
  }
  return toReturn;
}

string Measure::getNameString(string name) const {
  string nameStr = "";
  for (auto& sel : this->__selection) {
    nameStr += name + "/" + sel + "\t";
  }
  return nameStr;
}
string Measure::getValueString() {
  this->beforeOutput();
  std::ostringstream valStr;
  for (auto& sel : this->__selection) {
    valStr << std::scientific << *this->varmap.at(sel) << "\t";
  }
  return valStr.str();
}

set<string> Measure::selection() { return this->__selection; }
void Measure::selection(set<string> new_selection) {
  this->__selection.clear();
  for (auto& select : new_selection) {
    if (varmap.count(select) == 1) {
      __selection.insert(select);
    }
  }
}

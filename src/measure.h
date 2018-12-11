//################################################
// This header file
// declares Measure class used to track properties
// (e.g. peak, min, duration) of specified state variable.
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015
// Email thomas.hund@osumc.edu
//#################################################

#ifndef MEASURE_H
#define MEASURE_H

#include <cmath>
#include <limits>
#include <map>
#include <set>
#include <string>

#define INF std::numeric_limits<double>::infinity()
#define Q_NAN std::numeric_limits<double>::quiet_NaN()

namespace LongQt {

class Measure {
 public:
  Measure(std::map<std::string, double* const> varmap,
          std::set<std::string> selected = {});
  Measure(const Measure&);
  Measure(Measure&&);

  virtual bool measure(double time,
                       double var) = 0;  // measures props related to var;
                                         // returns 1 when ready for output.
  virtual void reset() = 0;  // resets params to init vals

  virtual std::set<std::string> variables();
  virtual std::map<std::string, double> variablesMap();

  virtual std::set<std::string> selection();
  virtual void selection(std::set<std::string> new_selection);
  //		void restoreLast();

  virtual std::string getNameString(std::string name) const;
  virtual std::string getValueString();

 protected:
  virtual void beforeOutput();

  std::set<std::string>
      __selection;  // map for refing properties that will be output.
  std::map<std::string, double* const> varmap;
};
}  // namespace LongQt
#undef INF
#undef Q_NAN
#endif

//################################################
// This header file
// declares Measure class used to track properties
// (e.g. peak, min, duration) of specified state variable.
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015
// Email thomas.hund@osumc.edu
//#################################################

#ifndef MEASUREWAVE_H
#define MEASUREWAVE_H

#include <limits>
#include <map>
#include <set>
#include <string>
#include "measure.h"
#define INF std::numeric_limits<double>::infinity()
#define Q_NAN std::numeric_limits<double>::quiet_NaN()

namespace LongQt {

class MeasureVoltage : public Measure {
 public:
  MeasureVoltage(std::set<std::string> selected = {}, double percrepol = 50);
  MeasureVoltage(const MeasureVoltage& toCopy);
  MeasureVoltage(MeasureVoltage&& toCopy);
  virtual ~MeasureVoltage() = default;

  MeasureVoltage& operator=(const MeasureVoltage& toCopy) = delete;

  virtual bool measure(double time, double var);
  virtual void reset();  // resets params to init vals

  //		string varname;

  void percrepol(double val);
  double percrepol() const;

 protected:
  virtual void beforeOutput();

 private:
  virtual void calcMeasure(double time,
                           double var);  // measures props related to var;
                                         // returns 1 when ready for output.
  virtual void updateOld(double time, double var);

  double varold = Q_NAN;
  double told = Q_NAN;
  double derivold = Q_NAN;  // dv/dt from prev. time step

  double deriv = 0;
  std::pair<double, double> maxderiv = {Q_NAN, -INF};  // time of max deriv.
  std::pair<double, double> max = {Q_NAN, -INF};       // max var value
  std::pair<double, double> min = {Q_NAN, INF};        // min var value
  double maxderiv_prev = Q_NAN;  // time of prev. cycle max deriv.
  std::pair<double, double> maxderiv2d = {Q_NAN, -INF};

  double vartakeoff = Q_NAN;  // var value at point of max deflection.
  double durtime1 = Q_NAN;
  double amp = Q_NAN;
  double ddr = Q_NAN;  // diastolic depol. rate

  double cl = 0;
  double dur = Q_NAN;       // duration
  double __percrepol = 50;  // specify percent repolarization
  double repol = -25;       // repol var val for duration measure.
  bool minfound = false;
  bool maxfound = false;
  bool inAP = false;  // 1 while measuring duration.
  bool ampfound = false;
  bool ddrfound = false;
  bool resetflag = false;
};
}  // namespace LongQt
#undef INF
#undef Q_NAN
#endif

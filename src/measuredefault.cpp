#include "measuredefault.h"
#include <cmath>
#include <iomanip>
#include <sstream>

#define INF std::numeric_limits<double>::infinity()
#define Q_NAN std::numeric_limits<double>::quiet_NaN()

using namespace std;
using namespace LongQt;

MeasureDefault::MeasureDefault(set<string> selected)
    : Measure({{"peak", &max.second},
               {"min", &min.second},
               {"maxderiv", &maxderiv.second},
               {"mint", &min.first},
               {"derivt", &maxderiv.first},
               {"maxt", &max.first},
               {"amp", &amp},
               {"avg", &avg},
               {"stdev", &sd}},
              selected) {}

MeasureDefault::MeasureDefault(const MeasureDefault& toCopy) : Measure(toCopy) {
  varold = toCopy.varold;
  told = toCopy.told;
  derivold = toCopy.derivold;

  deriv = toCopy.deriv;
  count = toCopy.count;
  avg_2 = toCopy.avg_2;
}

MeasureDefault::MeasureDefault(MeasureDefault&& toCopy) : Measure(toCopy) {
  varold = toCopy.varold;
  told = toCopy.told;
  derivold = toCopy.derivold;

  deriv = toCopy.deriv;
  count = toCopy.count;
  avg_2 = toCopy.avg_2;
}

//################################################################
// Function to track properties (e.g. peak, min, duration) of
// specified state variable and return status flag to calling fxn.
//################################################################
bool MeasureDefault::measure(double time, double var) {
  this->calcMeasure(time, var);
  this->updateOld(time, var);
  return false;
}

void MeasureDefault::calcMeasure(double time, double var) {
  avg = (var + count * avg) / (count + 1);
  avg_2 = (var * var + count * avg_2) / (count + 1);

  if (var > max.second) {
    max = {time, var};
    amp = max.second - min.second;
  }

  if (var < min.second) {  // Track value and time of min
    min = {time, var};
    amp = max.second - min.second;
  }
  if (std::isnan(told)) {
    return;
  }

  deriv = (var - varold) / (time - told);

  if (deriv > maxderiv.second) {  // Track value and time of max 1st deriv
    maxderiv = {time, deriv};
  }
  count++;
};

void MeasureDefault::updateOld(double time, double var) {
  told = time;
  varold = var;
  derivold = deriv;
}

void MeasureDefault::reset() {
  max.second = -INF;
  min.second = INF;
  maxderiv.second = -INF;
  avg = 0;
  count = 0;
};

void MeasureDefault::beforeOutput() { sd = std::sqrt(avg_2 - avg * avg); }
#undef INF
#undef Q_NAN

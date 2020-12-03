#include "pvarscell.h"
#include "logger.h"

using namespace LongQt;
using namespace std;

PvarsCell::~PvarsCell() {
  //    for(auto pvar : pvars) {
  //        delete pvar.second;
  //    }
}

void PvarsCell::erase(string elem) { this->pvars.erase(elem); }

PvarsCell::IonChanParam* PvarsCell::at(string elem) {
  return this->pvars.at(elem);
}
PvarsCell::const_iterator PvarsCell::begin() const {

    return this->pvars.begin();
}

PvarsCell::const_iterator PvarsCell::end() const { return this->pvars.end(); }

int PvarsCell::numtrials() const {
    return 1;
}

int PvarsCell::size() const { return this->pvars.size(); }

void PvarsCell::clear() { this->pvars.clear(); }

string PvarsCell::IonChanParam::str(string name) {
  string info = name;
  string var1Text;
  string var2Text;
  switch (this->dist) {
    case Distribution::none:
      var1Text = "\tIncrementing\tInitial Value: ";
      var2Text = "\tIncrement Amount: ";
      break;
    case Distribution::normal:
      var1Text = "\tNormal Distribution\tMean: ";
      var2Text = "\tStandard Deviation: ";
      break;
    case Distribution::lognormal:
      var1Text = "\tLognormal Distribution\tMean: ";
      var2Text = "\tStandard Deviation: ";
      break;
  }
  info += var1Text + std::to_string(this->val[0]) + var2Text +
          std::to_string(this->val[1]);
  return info;
}

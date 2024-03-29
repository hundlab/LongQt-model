#include "measurefactory.h"

#include "measuredefault.h"
#include "measurevoltage.h"

using namespace LongQt;

const std::map<std::string, std::string> MeasureFactory::varsMeas = {
    {"vOld", MeasureVoltage().type()},
};

const std::string MeasureFactory::defaultMeasure = MeasureDefault().type();

MeasureFactory::MeasureFactory()
    : varMeasCreator({{MeasureVoltage().type(),
                       [this](std::set<std::string> sel) {
                         return std::make_shared<MeasureVoltage>(
                             sel, this->__percrepol);
                       }},
                      {defaultMeasure,
                       [](std::set<std::string> sel) {
                         return std::make_shared<MeasureDefault>(sel);
                       }

                      }}) {}

MeasureFactory::MeasureFactory(const MeasureFactory& o) : MeasureFactory() {
  this->__percrepol = o.__percrepol;
}

std::shared_ptr<Measure> MeasureFactory::buildMeasure(
    std::string varname, std::set<std::string> selection) {
  std::string measName = this->measureType(varname);
  return this->buildMeasureFromType(measName, selection);
}

std::string MeasureFactory::measureType(std::string varname) {
  if (varsMeas.count(varname) > 0) {
    return varsMeas.at(varname);
  }
  return defaultMeasure;
}

std::set<std::string> MeasureFactory::measureOptions(std::string measType) {
  if (measOpts.count(measType) > 0) {
    return measOpts.at(measType);
  } else {
    auto meas = this->buildMeasureFromType(measType);
    std::set<std::string> opts = meas->variables();
    measOpts.insert({measType, opts});
    return opts;
  }
}

std::pair<std::set<std::string>, std::set<std::string> >
    MeasureFactory::checkSelection(std::string varname, std::set<std::string> selection)
{
    std::set<std::string> good;
    std::set<std::string> bad;
    auto measType = this->measureType(varname);
    auto measOpts = this->measureOptions(measType);
    for(auto& sel: selection) {
        if(measOpts.count(sel) == 1) {
            good.insert(sel);
        } else {
            bad.insert(sel);
        }
    }
    return std::make_pair(good, bad);
}

std::shared_ptr<Measure> MeasureFactory::buildMeasureFromType(
    std::string measType, std::set<std::string> selection) {
  if (varMeasCreator.count(measType) > 0) {
    return varMeasCreator.at(measType)(selection);
  }
  return varMeasCreator.at(defaultMeasure)(selection);
}

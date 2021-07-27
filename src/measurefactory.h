#ifndef MEASUREFACTORY_H
#define MEASUREFACTORY_H

#include <memory>
#include <set>
#include <string>
#include <functional>
#include "measure.h"

namespace LongQt {
class MeasureFactory {
 public:
  MeasureFactory();
  MeasureFactory(const MeasureFactory&);
  std::shared_ptr<Measure> buildMeasure(std::string varname,
                                        std::set<std::string> selection = {});
  std::string measureType(std::string varname);
  std::set<std::string> measureOptions(std::string measType);
  std::pair<std::set<std::string>, std::set<std::string>>
    checkSelection(std::string varname, std::set<std::string> selection);

  double percrepol() { return __percrepol; }
  void percrepol(double percrepol) {
    if (0 <= percrepol && percrepol <= 100) this->__percrepol = percrepol;
  }

 private:
  typedef std::function<std::shared_ptr<Measure>(
      std::set<std::string> selection)>
      MeasBuilder;
  double __percrepol = 50;

  std::shared_ptr<Measure> buildMeasureFromType(
      std::string measType, std::set<std::string> selection = {});

  static const std::map<std::string, std::string> varsMeas;
  const std::map<std::string, MeasBuilder> varMeasCreator;
  static const std::string defaultMeasure;

  std::map<std::string, std::set<std::string>> measOpts;
};
}  // namespace LongQt

#endif  // MEASUREFACTORY_H

#ifndef MEASUREMANAGER_H
#define MEASUREMANAGER_H

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include "cell.h"
#include "fileoutputgenerator.h"
#include "measure.h"
#include "measurevoltage.h"
namespace LongQt {

class MeasureManager {
 public:
  MeasureManager(std::shared_ptr<Cell> cell);
  MeasureManager(const MeasureManager&, std::shared_ptr<Cell> cell);
  virtual ~MeasureManager();

  virtual bool writeMVarsFile(QXmlStreamWriter& xml);
  virtual bool readMvarsFile(QXmlStreamReader& xml);

  std::map<std::string, std::set<std::string>> selection();
  void selection(std::map<std::string, std::set<std::string>> sel);
  double percrepol();
  void percrepol(double percrepol);
  std::shared_ptr<Measure> getMeasure(std::string varname,
                                      std::set<std::string> selection);

  virtual void addMeasure(std::string var,
                          std::set<std::string> selection = {});
  virtual void removeMeasure(std::string var);
  virtual void setupMeasures(std::string filename);
  virtual void measure(double time);
  virtual void write();
  virtual void writeLast(std::string filename);
  virtual void close();
  virtual void resetMeasures();

  const std::map<std::string, std::string> varsMeas = {
      {"vOld", "MeasureVoltage"},
  };
  const std::map<std::string,
                 std::function<Measure*(std::set<std::string> selection)>>
      varMeasCreator = {
          {"MeasureVoltage", [this](std::set<std::string> selection) {
             return (Measure*)new MeasureVoltage(selection, this->__percrepol);
           }}};

 protected:
  std::map<std::string, std::set<std::string>> variableSelection;

 private:
  void removeBad();
  void copy(const MeasureManager& other);

  std::shared_ptr<Cell> __cell = 0;
  std::string last = "";
  double __percrepol = 50;
  FileOutputHandler ofile;
  std::map<std::string, std::shared_ptr<Measure>> measures;
};
}  // namespace LongQt

#endif

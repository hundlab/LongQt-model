#ifndef MEASUREMANAGER_H
#define MEASUREMANAGER_H

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include "cell.h"
#include "measure.h"
#include "measurefactory.h"
#include "measurevoltage.h"
namespace LongQt {

class MeasureManager {
 public:
  MeasureManager(std::shared_ptr<Cell> cell);
  MeasureManager(const MeasureManager&, std::shared_ptr<Cell> cell);
  virtual ~MeasureManager();

  virtual bool writeMVarsFile(QXmlStreamWriter& xml);
  virtual bool readMvarsFile(QXmlStreamReader& xml);

  double percrepol() { return measMaker.percrepol(); }
  void percrepol(double percrepol) { measMaker.percrepol(percrepol); }

  std::map<std::string, std::set<std::string>> selection();
  void selection(std::map<std::string, std::set<std::string>> sel);

  std::shared_ptr<Cell> cell() { return this->__cell; }

  virtual void addMeasure(std::string var,
                          std::set<std::string> selection = {});
  virtual void removeMeasure(std::string var);
  virtual void setupMeasures();
  virtual void measure(double time, bool write = false);
  virtual void write(std::string filename);
  virtual void saveCurrent();

  MeasureFactory measMaker;
  bool determineWriteTime = true;

 protected:
  std::map<std::string, std::set<std::string>> variableSelection;
  virtual void resetMeasures();

 private:
  void save();
  void removeBad();

  std::shared_ptr<Cell> __cell = 0;
  std::map<std::string, std::shared_ptr<Measure>> measures;

  int numSelected = 0;
  std::string header = "";
  std::vector<std::vector<double>> data;
};
}  // namespace LongQt

#endif

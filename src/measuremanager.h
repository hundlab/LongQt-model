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

  virtual void addMeasure(std::string var,
                          std::set<std::string> selection = {});
  virtual void removeMeasure(std::string var);
  virtual void setupMeasures(std::string filename);
  virtual void measure(double time);
  virtual void write();
  virtual void close();
  virtual void resetMeasures();

  MeasureFactory measMaker;

 protected:
  std::map<std::string, std::set<std::string>> variableSelection;

 private:
  void removeBad();

  std::shared_ptr<Cell> __cell = 0;
  FileOutputHandler ofile;
  std::map<std::string, std::shared_ptr<Measure>> measures;
};
}  // namespace LongQt

#endif

#ifndef GRIDMEASUREMANAGER_H
#define GRIDMEASUREMANAGER_H

#include <QFile>
#include "grid.h"
#include "gridCell.h"
#include "measure.h"
#include "measuremanager.h"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
namespace LongQt {

class GridMeasureManager : public MeasureManager {
 public:
  GridMeasureManager(std::shared_ptr<GridCell> cell);
  GridMeasureManager(const GridMeasureManager&, std::shared_ptr<GridCell> cell);
  virtual ~GridMeasureManager() = default;

  void dataNodes(std::set<std::pair<int, int>> nodes);
  std::set<std::pair<int, int>> dataNodes();

  virtual bool writeMVarsFile(QXmlStreamWriter& xml);
  virtual bool readMvarsFile(QXmlStreamReader& xml);

  virtual void setupMeasures(std::string filename);
  virtual void measure(double time);
  virtual void write(QFile* file = 0);
  virtual void writeSingleCell(std::pair<int, int> node, QFile* file = 0);
  virtual void writeLast(std::string filename);
  virtual std::string nameString(std::pair<int, int> node) const;
  virtual void close();
  virtual void resetMeasures(std::pair<int, int> node);

 private:
  std::set<std::pair<int, int>> __dataNodes;
  std::map<std::pair<int, int>, std::shared_ptr<QFile>> ofiles;
  std::map<std::pair<int, int>, std::string> lasts;
  std::map<std::pair<int, int>, std::map<std::string, std::shared_ptr<Measure>>>
      measures;
  Grid* grid = 0;
};
}  // namespace LongQt

#endif
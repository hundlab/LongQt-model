#ifndef GRIDMEASUREMANAGER_H
#define GRIDMEASUREMANAGER_H

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

  bool writeMVarsFile(QXmlStreamWriter& xml);
  bool readMvarsFile(QXmlStreamReader& xml);

  void setupMeasures();
  void measure(double time, bool write = false) override;
  void write(std::string filename);
  std::string nameString(std::pair<int, int> node) const;
  void resetMeasures(std::pair<int, int> node);

 private:
  void saveSingleCell(std::pair<int, int> node);

  std::set<std::pair<int, int>> __dataNodes;
  std::map<std::pair<int, int>, std::map<std::string, std::shared_ptr<Measure>>>
      measures;
  std::string header = "";
  std::map<std::pair<int, int>, std::vector<std::vector<double>>> data;
  std::map<std::pair<int, int>, int> numSelected;
  Grid* grid = 0;
};
}  // namespace LongQt

#endif

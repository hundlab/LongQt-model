// 5/10/2016
//################################################
// protocol for a grid off cells class declarations
//################################################

#ifndef GRIDPROTOCOL_H
#define GRIDPROTOCOL_H

#include <set>
#include "currentClampProtocol.h"
#include "grid.h"
#include "gridmeasuremanager.h"
#include "pvarsgrid.h"
#include "structure/node.h"
namespace LongQt {

class GridProtocol : public CurrentClamp {
 public:
  GridProtocol();
  GridProtocol(const GridProtocol& toCopy);
  GridProtocol* clone();
  GridProtocol& operator=(const GridProtocol& toCopy);

  virtual bool runTrial() override;
  std::map<std::string, CellUtils::CellInitializer>& getCellMap();
  std::set<std::pair<int, int>>& getStimNodes();
  virtual bool writepars(QXmlStreamWriter& xml);
  virtual int readpars(QXmlStreamReader& xml);

  virtual std::shared_ptr<Cell> cell() const override;
  std::shared_ptr<GridCell> gridCell() const;
  virtual void cell(std::shared_ptr<Cell> cell) override;
  virtual bool cell(const std::string& type) override;
  virtual std::list<std::string> cellOptions() override;

  Grid& getGrid();

  GridMeasureManager& gridMeasureMgr();

  virtual PvarsCell& pvars() override;

  virtual MeasureManager& measureMgr() override;

  std::set<std::pair<int, int>> stimNodes;

  const static char* name;
  const char* type() const override;

 protected:
  virtual void setupTrial() override;
  virtual int stim() override;

 private:
  void CCcopy(const GridProtocol& toCopy);
  void mkmap();
  std::string setToString(std::set<std::pair<int, int>>& nodes);
  std::set<std::pair<int, int>> stringToSet(std::string nodesList);
  Grid* grid;
  std::shared_ptr<GridCell> __cell =
      std::make_shared<GridCell>();  // pointer to cell class
  std::unique_ptr<PvarsGrid> __pvars;
  std::unique_ptr<GridMeasureManager>
      __measureMgr;  // set of measure class for measuring SV props.
};
}  // namespace LongQt

#endif

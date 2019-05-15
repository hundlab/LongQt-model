// 5/10/2016
//################################################
// cell type for a grid, used by grid protocol to
// run a simulaiton on a grid of cells
// class declarations
//################################################

#ifndef GRIDCELL_H
#define GRIDCELL_H

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "cell.h"
#include "grid.h"

#include <threadpool.h>
#include <list>
#include <set>
namespace LongQt {

class GridCell : public Cell {
 public:
  GridCell();
  GridCell(GridCell& toCopy);
  ~GridCell();

  GridCell* clone() override;
  Grid* getGrid();

  void updateConc();
  void updateCurr();
  double updateV();
  void externalStim(double stimval);
  double tstep(double stimt);
  void setV(double v = dlim::quiet_NaN());
  std::set<std::string> vars();
  std::set<std::string> pars();
  void setup() override;
  void setup(std::set<std::pair<int, int>> stimNodes,
                     std::set<std::pair<int, int>> traceNodes);

  // cell io functions
  bool writeGridfile(QXmlStreamWriter& xml);
  bool writeGridfile(std::string fileName = "");
  bool readGridfile(QXmlStreamReader& xml);
  bool readGridfile(std::string filename);
  void setOutputfileConstants(std::string filename);
  void setOuputfileVariables(std::string filename);
  void setConstantSelection(std::set<std::string> new_selection);
  void setVariableSelection(std::set<std::string> new_selection);
  void writeConstants();
  void writeVariables();
  bool readCellState(std::string filename);
  bool writeCellState(std::string filename);
  void closeFiles();
  const char* type() const;

 private:
  bool handleNode(QXmlStreamReader& xml, int row);
  bool handleRow(QXmlStreamReader& xml);
  bool handleGrid(QXmlStreamReader& xml);

  Grid grid;
  int tcount = 0;

  ThreadPool pool;

  void makeMap();

 private:
  void Initialize();

  std::set<std::shared_ptr<Node>> __stimN;
  std::set<std::shared_ptr<Node>> __traceN;
};
}  // namespace LongQt

#endif

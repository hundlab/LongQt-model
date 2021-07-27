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
 using dlim = std::numeric_limits<double>;
 public:
  GridCell();
  GridCell(GridCell& toCopy);
  ~GridCell();

  GridCell* clone() override;
  Grid* getGrid();

  void updateConc() override;
  void updateCurr() override;
  double updateV() override;
  void externalStim(double stimval) override;
  double tstep(double stimt) override;
  void setV(double v = dlim::quiet_NaN()) override;
  std::set<std::string> vars() override;
  std::set<std::string> pars() override;
  void setup() override;
  void setup(std::set<std::pair<int, int>> stimNodes,
                     std::set<std::pair<int, int>> traceNodes);

  // cell io functions
  bool writeGridfile(QXmlStreamWriter& xml);
  bool writeGridfile(std::string fileName = "");
  bool readGridfile(QXmlStreamReader& xml);
  bool readGridfile(std::string filename);
  void setOutputfileConstants(std::string filename) override;
  void setOuputfileVariables(std::string filename) override;
  void setConstantSelection(std::set<std::string> new_selection) override;
  void setVariableSelection(std::set<std::string> new_selection) override;
  void writeConstants() override;
  void writeVariables() override;
  bool readCellState(std::string filename) override;
  bool writeCellState(std::string filename) override;
  void closeFiles() override;
  const char* type() const override;
  const char* citation() const override;

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

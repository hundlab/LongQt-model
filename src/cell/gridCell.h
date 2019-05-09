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

  virtual void updateConc();
  virtual void updateCurr();
  virtual double updateV();
  virtual void externalStim(double stimval);
  virtual double tstep(double stimt);
  virtual void setV(double v = dlim::quiet_NaN());
  virtual std::set<std::string> vars();
  virtual std::set<std::string> pars();
  virtual void setup() override;
  void setup(std::set<std::pair<int, int>> stimNodes,
                     std::set<std::pair<int, int>> traceNodes);

  // cell io functions
  virtual bool writeGridfile(QXmlStreamWriter& xml);
  virtual bool writeGridfile(std::string fileName = "");
  virtual bool readGridfile(QXmlStreamReader& xml);
  virtual bool readGridfile(std::string filename);
  virtual void setOutputfileConstants(std::string filename);
  virtual void setOuputfileVariables(std::string filename);
  virtual void setConstantSelection(std::set<std::string> new_selection);
  virtual void setVariableSelection(std::set<std::string> new_selection);
  virtual void writeConstants();
  virtual void writeVariables();
  virtual bool readCellState(std::string filename);
  virtual bool writeCellState(std::string filename);
  virtual void closeFiles();
  virtual const char* type() const;

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

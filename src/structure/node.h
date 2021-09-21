/*
 * A container for cells that also keeps some of the information about the cells
 */
#ifndef NODE_H
#define NODE_H

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <array>
#include <atomic>
#include <memory>
#include "cell.h"
#include "cellutils.h"
#include "inexcitablecell.h"
namespace LongQt {

class Grid;

struct Node : public std::enable_shared_from_this<Node> {
  Node() {}
  Node(const Node& other);
  ~Node() {}

  void resetCondConst(CellUtils::Side s = (CellUtils::Side)-1);
  void setCondConst(CellUtils::Side s, bool perc = true, double val = 1);
  //	void updateV(double dt);
  bool cell(const std::string& type);
  void cell(std::shared_ptr<Cell> cell);
  std::shared_ptr<Cell> cell() const;
  std::list<std::string> cellOptions();

  double rd = 1.5;  // gap junctional disk resistance.
  double getCondConst(CellUtils::Side s);
  //  double setFiberB();
  //## default value cannot be deterimined by constructor
  std::array<std::atomic<bool>, 2> lock{0, 0};
  void waitUnlock(int which);
  double dIax = 0;
  // can't change atm
  //    double x = 0;
  //    double y = 0;
  //  std::string nodeType = "";
  int row = -1;
  int col = -1;
  void setParent(Grid* par, int row = -1, int col = -1);
  void setPosition(int row, int col);
  Grid* getParent();

  bool writeNode(QXmlStreamWriter& xml);
  bool readNode(QXmlStreamReader& xml);

 private:
  std::shared_ptr<Cell> __cell = std::make_shared<InexcitableCell>();
  //  void setCondConstDirect(CellUtils::Side s, double val);
  std::array<double, 4> c = {{NAN, NAN, NAN, NAN}};
  Grid* parent = 0;
  std::pair<int, int> calcNeighborPos(CellUtils::Side s);
  double calcOurCondConst(CellUtils::Side s, double val);
  const std::string inexName = __cell->type();
};
}  // namespace LongQt

#endif

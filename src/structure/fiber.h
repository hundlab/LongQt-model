/*
 * Fiber is a 1D line of cells stored with other information in Nodes
 */
#ifndef FIBER_H
#define FIBER_H

#include <memory>
#include "cellutils_core.h"
#include "structure/node.h"
namespace LongQt {

class Fiber {
 public:
  Fiber(int size, LongQt::CellUtils::Side dir);
  Fiber(const Fiber&);
  ~Fiber();

  typedef std::vector<std::shared_ptr<Node>>::const_iterator const_iterator;

  virtual void updateVm(const double& dt);
  virtual std::shared_ptr<Node> operator[](int pos);
  virtual std::shared_ptr<Node> at(int pos);
  virtual int size() const;
  virtual void setup();

  virtual const_iterator begin() const;
  virtual const_iterator end() const;

  std::vector<std::shared_ptr<Node>> nodes;

 private:
  void setOrderedSides(LongQt::CellUtils::Side s);
  std::array<LongQt::CellUtils::Side, 2> directions;
  std::vector<double> B;
  std::vector<double> gam;
  std::vector<double> d1;// off-diagonal for tridag solver
  std::vector<double> d2;// diagonal elements for tridag solver
  std::vector<double> d3;// off-diagonal for tridag solver
  std::vector<double> r;// right side of eqn for tridag solver
  void tridag();
};
}  // namespace LongQt

#endif

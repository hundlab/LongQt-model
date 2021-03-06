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

  class FiberIterator {
    using value_type = std::shared_ptr<Node>;
    using reference = value_type&;
    using pointer = value_type*;
    using difference_type = std::pair<int, int>;
    using iterator_category = std::bidirectional_iterator_tag;

    int pos = 0;
    Fiber* parent = 0;

    FiberIterator(Fiber *parent);

    friend class Fiber;

   public:
    FiberIterator(const FiberIterator& o)
        : pos(o.pos), parent(o.parent) {}

    FiberIterator& operator++();
    FiberIterator operator++(int);
    FiberIterator& operator--();
    FiberIterator operator--(int);
    FiberIterator& operator+=(int i);
    FiberIterator& operator-=(int i);
    FiberIterator operator+(int i);
    FiberIterator operator-(int i);

    bool operator==(const FiberIterator& rhs) const;
    bool operator!=(const FiberIterator& rhs) const;
    std::shared_ptr<Node> operator*();
  };

  virtual void updateVm(const double& dt);
  virtual std::shared_ptr<Node> operator[](int pos);
  virtual std::shared_ptr<Node> at(int pos);
  virtual int size() const;
  virtual void setup();

  virtual FiberIterator begin();
  virtual FiberIterator end();

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

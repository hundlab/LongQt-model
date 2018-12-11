/*
 * Fiber is a 1D line of cells stored with other information in Nodes
 */
#ifndef FIBER_H
#define FIBER_H

#include <memory>
#include "structure/node.h"
namespace LongQt {

class Fiber {
 public:
  Fiber(int size);
  Fiber(const Fiber&);
  ~Fiber();

  typedef std::vector<std::shared_ptr<Node>>::const_iterator const_iterator;

  virtual void updateVm(const double& dt);
  virtual std::shared_ptr<Node> operator[](int pos);
  virtual std::shared_ptr<Node> at(int pos);
  virtual int size() const;
  virtual inline void diffuse(int node);
  virtual inline void diffuseBottom(int node);
  virtual inline void diffuseTop(int node);

  virtual const_iterator begin() const;
  virtual const_iterator end() const;

  std::vector<std::shared_ptr<Node>> nodes;
  std::vector<double> B;  // coefficients for tridag solver.
};
}  // namespace LongQt

#endif

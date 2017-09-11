/*
 * Fiber is a 1D line of cells stored with other information in Nodes
 */
#ifndef FIBER_H
#define FIBER_H

#include "node.h"
#include <memory>

class Fiber {
  public:
    Fiber(int size);
    Fiber(const Fiber&);
    ~Fiber();

    typedef std::vector<shared_ptr<Node>>::const_iterator const_iterator;

    virtual void updateVm(double& dt);
    virtual shared_ptr<Node> operator[](int pos);
    virtual int size() const;
	virtual inline void diffuse(int node);
	virtual inline void diffuseBottom(int node);
	virtual inline void diffuseTop(int node);

    virtual const_iterator begin() const;
    virtual const_iterator end() const;

    vector<shared_ptr<Node>> nodes;
    vector<double> B; //coefficients for tridag solver.
};

#endif

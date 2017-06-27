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
    ~Fiber();
    
    virtual void updateVm(double& dt);
	virtual inline void diffuse(int node);
	virtual inline void diffuseBottom(int node);
	virtual inline void diffuseTop(int node);

    vector<shared_ptr<Node>> nodes;
    vector<double> B; //coefficients for tridag solver.
};

#endif

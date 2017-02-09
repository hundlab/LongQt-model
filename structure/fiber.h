/*
 * Fiber is a 1D line of cells stored with other information in Nodes
 */
#ifndef FIBER_H
#define FIBER_H

#include "node.h"

class Fiber {
  public:
    Fiber(int size);
    ~Fiber();
    
    virtual void updateVm(double& dt);

    vector<Node*> nodes;
    vector<double> B; //coefficients for tridag solver.
};

#endif

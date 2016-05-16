#ifndef NODE_H
#define NODE_H

#include "cell.h"

struct Node {
    Node() {
//## default values
        rd = 1.5;
        Rmyo = 150;
//## default value cannot be deterimined by constructor
        dIax = 0;
        x = 0;
        d1 = 0;
        d2 = 0;
        d3 = 0;
        r = 0;
        B = 0;
        vNew = 0;
    }
    ~Node() {};
    Cell* cell;
    double rd; // gap junctional disk resistance.
    double Rmyo; //Myoplasmic resistivity.
    double dIax;
    double x;
    double y;

    double d1; //off-diagonal for tridag solver
    double d2; //diagonal elements for tridag solver
    double d3; //off-diagonal for tridag solver
    double r; //right side of eqn for tridag solver
    double B; //coefficients for tridag solver.
    double vNew; //vOld(t+1) for tridag solver
    string nodeType;

};
#endif

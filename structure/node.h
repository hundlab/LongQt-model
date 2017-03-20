/*
 * A container for cells that also keeps some of the information about the cells
 */
#ifndef NODE_H
#define NODE_H

#include "cell.h"

struct Node {
    Node() {};
	Node(const Node& other);
	~Node() {};
//	void updateV(double dt);
    Cell* cell = new Cell();
    double rd = 1.5; // gap junctional disk resistance.
    double Rmyo = 150; //Myoplasmic resistivity.

//## default value cannot be deterimined by constructor
    double dIax = 0;
    double x = 0;
    double y = 0;

    double d1 = 0; //off-diagonal for tridag solver
    double d2 = 0; //diagonal elements for tridag solver
    double d3 = 0; //off-diagonal for tridag solver
    double r = 0; //right side of eqn for tridag solver
    double vNew = 0; //vOld(t+1) for tridag solver
    string nodeType;

};
#endif

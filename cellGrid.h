// 5/10/2016
//################################################
// a 2D grid of cells of various types
// class declaration
//################################################

#ifndef CELLGRID_H
#define CELLGRID_H

#include <tuple>
#include <cell.h>
#include <set>
#include <string>

enum Direction {UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3};
Direction flip(Direction dir) {
    switch(dir) {
        case UP: return DOWN;
        case DOWN: return UP;
        case RIGHT: return LEFT;
        case LEFT: return RIGHT;
        case default: return UP; //included to avoid compiler warning
    }
}

struct cellInfo {
    tuple<int,int> pos;
    Node n;
}

struct Node {
    Cell* cell;
    double rd; // gap junctional disk resistance.
    double Rmyo; //Myoplasmic resistivity.
    double dIax;
    double x;

    double d1; //off-diagonal for tridag solver
    double d2; //diagonal elements for tridag solver
    double d3; //off-diagonal for tridag solver
    double r; //right side of eqn for tridag solver
    double B; //coefficients for tridag solver.
    double vNew; //vOld(t+1) for tridag solver
    int numcells; 
    string nodeType;

};

class Fiber {
  public:
    Fiber();
    ~Fiber();
    
    virtual void updateVm();
    virtual void tstep();

    vector<Node*> nodes;
}

class cellGrid {
  public:
    cellGrid();
    ~cellGrid();

    virtual void addRow(int pos); //create new row at 0 <= pos < len of empty cells
    virtual void addColumn(int pos); //same but for culumns
    virtual void setCellTypes(set<cellInfo> cells); 
    virtual void updateVm();
    virtual int tstep();

  private:
    vector<Fiber> fiber;
    vector<Fiber> fibery; 
};
#endif

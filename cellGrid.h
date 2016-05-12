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

class Node {
  public:
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

    vector<Node*> neighbors; 

    Node() {
        neighbors = vector<Node*>(4);
    }

    void setNeighbors(Node* above, Node* below, Node* left, Node* right) {
        neighbors[DOWN] = below;
        neighbors[LEFT] = left;
        neighbors[RIGHT] = right;
        neighbors[UP] = above;
    }

    void updateVm(double dt, Direction dir) {
        bool edge = false;
        Node* next = neighbors[dir];
        Node* prev = neighbors[flip(dir)];
        if(next == NULL) {
            r = -B*dt*prev->cell->vOld+(B*dt-2)*cell->vOld+dt/cell->Cm*(cell->iTotold+cell->iTot);
            edge = true;
        }
        d1 = B*dt;
        d2 = -(B*dt+next->B*dt+2);
        d3 = next->B*dt;
        if(prev == NULL) {
            r = (next->B*dt-2)*cell->vOld-next->B*dt*next->cell->vOld+dt/cell->Cm*(cell->iTotold+cell->iTot);
            edge = true;
        }
        if(!edge) {
            r = -B*dt*prev->cell->vOld+(B*dt+next->B*dt-2)*cell->vOld-next->B*dt*next->cell->vOld+dt/cell->Cm*(cell->iTotold+cell->iTot);
        }


        cell->iTotold=cell->iTot;
        cell->dVdt=(vNew[i]-cell->vOld)/dt;
        //##### Conservation for multicellular fiber ############
        dIax[i]=-(cell->dVdt+cell->iTot);
        cell->iKt=cell->iKt+dIax[i];
        cell->setV(vNew[i]);

    }

    ~Node() {}
};

class cellGrid {
  public:
    cellGrid();
    ~cellGrid();

    virtual void addRow(int pos); //create new row at 0 <= pos < len of empty cells
    virtual void addColumn(int pos); //same but for culumns
    virtual void setCellTypes(set<cellInfo> cells); 
    virtual void updateVm();
    virtual int tstep();

    
};
#endif

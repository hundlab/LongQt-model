// 5/10/2016
//################################################
// a 2D grid of cells of various types
// class declaration
//################################################

#ifndef CELLGRID_H
#define CELLGRID_H

#include <set>
#include <string>

#include "fiber.h"

struct cellInfo {
    int X;
    int Y;
    double dx;
    double dy;
    int np;
    Node* n;
};

class Grid {
  public:
    Grid();
    ~Grid();

    virtual void addRow(int pos); //create new row at 0 <= pos < len of empty cells
    virtual void addColumn(int pos); //same but for culumns
    virtual void removeRow(int pos);
    virtual void removeColumn(int pos);
    virtual void setCellTypes(set<cellInfo> cells); 
    virtual int rowCount();
    virtual int columnCount();

    vector<Fiber> fiber;
    vector<Fiber> fibery; 
};
#endif

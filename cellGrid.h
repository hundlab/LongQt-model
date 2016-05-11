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

struct cellInfo {
    tuple<int,int> pos;
    Cell* cellpntr;
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
};
#endif

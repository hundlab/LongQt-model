// 5/10/2016
//################################################
//cell type for a grid, used by grid protocol to
// run a simulaiton on a grid of cells
// class declarations
//################################################

#ifndef GRIDCELL_H
#define GRIDCELL_H

#include <cellGrid.h>

class gridCell: public Cell {
  public:
    gridCell();
    gridCell(GridCell& toCopy);
    ~gridCell();

    void Initialize();
    gridCell* clone();

    void updateConc();
    void updateCurr();
    int externalStim(double stimval);
    virtual void makemap();
  private:
    cellGrid grid;
};
#endif

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
    Cell* cell;
};

class Grid {
  public:
    Grid();
    Grid(const Grid& other);
    ~Grid();

    virtual void addRow(int pos); //create new row at 0 <= pos < len of empty cells
    virtual void addRows(unsigned int num, int position = 0);
    virtual void addColumn(int pos); //same but for culumns
    virtual void addColumns(unsigned int num, int position = 0);
    virtual void removeRow(int pos);
    virtual void removeRows(unsigned int num, int position = 0);
    virtual void removeColumn(int pos);
    virtual void removeColumns(unsigned int num, int position = 0);
    virtual void setCellTypes(set<cellInfo*>& cells); 
    virtual void setCellTypes(const cellInfo& singleCell);
    virtual int rowCount();
    virtual int columnCount();
    virtual void addBuffer();
    virtual pair<int,int> findNode(const Node* node);
    virtual Node* findNode(const pair<int,int>& p);
	virtual void reset();

    vector<Fiber> fiber;
    vector<Fiber> fibery; 
};
#endif

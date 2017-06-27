// 5/10/2016
//################################################
// a 2D grid of cells of various types
// class declaration
//################################################

#ifndef CELLGRID_H
#define CELLGRID_H

#include <set>
#include <string>
#include <cmath>

#include "fiber.h"
#include "cellutils.h"

struct CellInfo {
	//necessary
    int X = -1;
    int Y = -1;
    double dx = 0.01;
    double dy = 0.01;
    int np = 1;
	//if cell == NULL then cell will not be changed
    Cell* cell = 0;
	double c[4] = {NAN,NAN,NAN,NAN};
	bool c_perc = false;
};

class Grid {
  public:

    Grid();
    Grid(const Grid& other);
    ~Grid();

//	inline virtual edge(int x, int y, CellUtils::Side s);
    virtual void addRow(int pos); //create new row at 0 <= pos < len of empty cells
    virtual void addRows(unsigned int num, int position = 0);
    virtual void addColumn(int pos); //same but for culumns
    virtual void addColumns(unsigned int num, int position = 0);
    virtual void removeRow(int pos);
    virtual void removeRows(unsigned int num, int position = 0);
    virtual void removeColumn(int pos);
    virtual void removeColumns(unsigned int num, int position = 0);
    virtual void setCellTypes(set<CellInfo*>& cells);
    virtual void setCellTypes(const CellInfo& singleCell);
    virtual int rowCount();
    virtual int columnCount();
    virtual pair<int,int> findNode(const Node* node);
    virtual Node* operator()(const pair<int,int>& p);
    virtual Node* operator()(const int x, const int y);
	virtual void reset();
	virtual void updateB(CellInfo node, CellUtils::Side s);

    vector<Fiber> fiber;
    vector<Fiber> fibery; 
};
#endif

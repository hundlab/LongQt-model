// 5/10/2016
//################################################
// a 2D grid of cells of various types
// class declaration
//################################################

#ifndef CELLGRID_H
#define CELLGRID_H

#include <set>
#include <list>
#include <array>
#include <string>
#include <cmath>
#include <memory>

#include "fiber.h"
#include "cellutils.h"
namespace LongQt {

struct CellInfo {
	//necessary
    CellInfo(int row=-1, int col=-1, double dx=0.01, double dy=0.01, int np=1,
        std::shared_ptr<Cell> cell=0, std::array<double,4> c={NAN,NAN,NAN,NAN},
        bool c_perc=false) {
        this->row = row;
        this->col = col;
        this->dx = dx;
        this->dy = dy;
        this->np = np;
        this->cell = cell;
        this->c = c;
        this->c_perc = c_perc;
    }
    ~CellInfo() {}
    int row = -1;
    int col = -1;
    double dx = 0.01;
    double dy = 0.01;
    int np = 1;
	//if cell == NULL then cell will not be changed
    std::shared_ptr<Cell> cell = 0;
    std::array<double,4> c = {{NAN,NAN,NAN,NAN}};
	bool c_perc = false;
};

class Grid {
  public:

    Grid();
    Grid(const Grid& other);
    ~Grid();

    typedef std::vector<Fiber>::const_iterator const_iterator;
    typedef std::vector<Fiber>::iterator iterator;

    Grid& operator=(const Grid&) = delete;

//	inline virtual edge(int x, int y, CellUtils::Side s);
    virtual void addRow();
    virtual void addRows(unsigned int num);
    virtual void addColumn();
    virtual void addColumns(unsigned int num);
    virtual void removeRow(int pos);
    virtual void removeRows(unsigned int num, int position = 0);
    virtual void removeColumn(int pos);
    virtual void removeColumns(unsigned int num, int position = 0);
    virtual void setCellTypes(std::list<CellInfo>& cells);
    virtual void setCellTypes(const CellInfo& singleCell);
    virtual int rowCount() const;
    virtual int columnCount() const;
    virtual std::pair<int,int> findNode(const std::shared_ptr<Node> toFind);
    virtual std::shared_ptr<Node> operator()(const std::pair<int,int>& p);
    virtual std::shared_ptr<Node> operator()(const int row, const int col);
	virtual void reset();
	virtual void updateB(CellInfo node, CellUtils::Side s);
    void updateNodePositions();

    virtual const_iterator begin() const;
    virtual const_iterator end() const;
    virtual iterator begin();
    virtual iterator end();

    std::vector<Fiber> rows;
    std::vector<Fiber> columns;
};
} //LongQt

#endif

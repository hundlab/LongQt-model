/*
 * A container for cells that also keeps some of the information about the cells
 */
#ifndef NODE_H
#define NODE_H

#include "cell.h"
#include "cellutils.h"
#include "inexcitablecell.h"
#include <memory>
#include <array>
namespace LongQt {

class Grid;

struct Node : public std::enable_shared_from_this<Node> {
    Node() {};
	Node(const Node& other);
	~Node() {};

    void setCondConst(double dx, CellUtils::Side s, bool perc = true, double val = 1);
//	void updateV(double dt);
    std::shared_ptr<Cell> cell = std::make_shared<InexcitableCell>();
    double rd = 1.5; // gap junctional disk resistance.
    double getCondConst(CellUtils::Side s);
//## default value cannot be deterimined by constructor
    double dIax = 0;
	int np = 1; //number of cells in each node
	//can't change atm
//    double x = 0;
//    double y = 0;
    double d1 = 0; //off-diagonal for tridag solver
    double d2 = 0; //diagonal elements for tridag solver
    double d3 = 0; //off-diagonal for tridag solver
    double r = 0; //right side of eqn for tridag solver
    double vNew = 0; //vOld(t+1) for tridag solver
    std::string nodeType ="";
    int row = -1;
    int col = -1;
    void setParent(Grid* par,int row = -1, int col = -1);
    void setPosition(int row, int col);
    Grid* getParent();

private:
    void setCondConstDirect(CellUtils::Side s, double val);
    Grid* parent;
    std::pair<int, int> calcNeighborPos(CellUtils::Side s);
    double calcCondConst(double dx, CellUtils::Side s, double val);
};
} //LongQt

#endif

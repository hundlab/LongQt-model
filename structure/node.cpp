#include "node.h"
#include "grid.h"

Node::Node(const Node& other) : std::enable_shared_from_this<Node>(other) {
	rd = other.rd;
	dIax = other.dIax;
//	x = other.x;
//	y = other.y;
	d1 = other.d1;
	d2 = other.d2;
	d3 = other.d3;
	r = other.r;
	vNew = other.vNew;
	nodeType = other.nodeType;
    cell.reset(other.cell->clone());
    row = other.row;
    col = other.col;
}

/*double Node::calPerc(int X, double dx, double val) {
	if(val == 0) {
		return 0;
	}
	if((np==1)||((X%np)==0)) {
		return ((1000*cell->cellRadius)/(val*2*cell->Rcg*cell->Cm*dx)-cell->Rmyo*dx)/rd;
	} else {
		return 1;
	}
}*/

void Node::setCondConst(CellUtils::Side s, bool perc, double val) {
    auto nei = this->calcNeighborPos(s);
    auto neiNode = (*parent)(nei);
    if(!neiNode) {
        return;
    }
    if(cell->type() == string("Inexcitable Cell")
            || neiNode->cell->type() == string("Inexcitable Cell")) {
        setCondConstDirect(s,0.0);
		return;
    }
    if(isnan(val)) {
        double ourCond = this->calcCondConst(s,1);
        double theirCond = neiNode->calcCondConst(CellUtils::flipSide(s),1);
        setCondConstDirect(s,(ourCond+theirCond)/2);
        return;
    }
	if(!perc) {
        setCondConstDirect(s,val);
		return;
    } else {
        setCondConstDirect(s,this->calcCondConst(s,val));
    }
}
pair<int,int> Node::calcNeighborPos(CellUtils::Side s) {
    pair<int,int> nei;
    switch(s) {
    case CellUtils::Side::left:
        nei = pair<int,int>(row,col-1);
        break;
    case CellUtils::Side::right:
        nei = pair<int,int>(row,col+1);
        break;
    case CellUtils::Side::top:
        nei = pair<int,int>(row-1,col);
        break;
    case CellUtils::Side::bottom:
        nei = pair<int,int>(row+1,col);
        break;
    }
    return nei;
}

double Node::calcCondConst(CellUtils::Side s, double val) {
    if(val==0) {
        return 0;
    }
    int X = (s==CellUtils::Side::right||s==CellUtils::Side::left) ? row : col;
    double dx = (s==CellUtils::Side::right||s==CellUtils::Side::left) ? parent->dx : parent->dy;
    if((np==1)||((X%np)==0)) {
        return 1000*cell->cellRadius/(2*cell->Rcg*(cell->Rmyo*dx+rd*val)*cell->Cm*dx);
    } else {
        return 1001*cell->cellRadius/(2*cell->Rcg*cell->Rmyo*cell->Cm*dx*dx);
    }
}

void Node::setCondConstDirect(CellUtils::Side s, double val) {
    switch(s) {
    case CellUtils::Side::left:
        parent->rows.at(row).B.at(col) = val;
        break;
    case CellUtils::Side::right:
        parent->rows.at(row).B.at(col+1) = val;
        break;
    case CellUtils::Side::top:
        parent->columns.at(col).B.at(row) = val;
        break;
    case CellUtils::Side::bottom:
        parent->columns.at(col).B.at(row+1) = val;
        break;
    }
}

double Node::getCondConst(CellUtils::Side s)
{
    double val = -1;
    switch(s) {
    case CellUtils::Side::left:
        val = parent->rows.at(row).B[col];
        break;
    case CellUtils::Side::right:
        val = parent->rows.at(row).B[col+1];
        break;
    case CellUtils::Side::top:
        val = parent->columns.at(col).B[row];
        break;
    case CellUtils::Side::bottom:
        val = parent->columns.at(col).B[row+1];
        break;
    }

    return val;
}

void Node::setParent(Grid *par,int row, int col)
{
    this->parent = par;
    if(row==-1||col==-1) {
        auto temp = parent->findNode(this->shared_from_this());
        this->row = temp.first;
        this->col = temp.second;
    } else {
        this->row = row;
        this->col = col;
    }
}

void Node::setPosition(int row, int col)
{
    this->row = row;
    this->col = col;
}

Grid *Node::getParent()
{
    return this->parent;
}
/*
void Node::updateV(double dt) {
	this->cell->iTotold=this->cell->iTot;
	this->cell->dVdt=(this->vNew-this->cell->vOld)/dt;
	//##### Conservation for multicellular fiber ############
	this->dIax=-(this->cell->dVdt+this->cell->iTot);
	this->cell->iKt=this->cell->iKt+this->dIax;
	this->cell->setV(this->vNew);
}*/

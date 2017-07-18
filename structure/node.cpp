#include "node.h"

Node::Node(const Node& other) {
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

void Node::setCondConst(int X, double dx, CellUtils::Side s, bool perc, double val) {
    if(cell->type() == string("Inexcitable Cell")) {
		condConst[s] = 0.0;
		return;
	}
	if(!perc) {
		condConst[s] = val;
		return;
	}
	if((np==1)||((X%np)==0)) {
		condConst[s] = 1000*cell->cellRadius/(2*cell->Rcg*(cell->Rmyo*dx+rd*val)*cell->Cm*dx);
	} else {
		condConst[s] = 1001*cell->cellRadius/(2*cell->Rcg*cell->Rmyo*cell->Cm*dx*dx);
	}
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

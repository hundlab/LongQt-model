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
	cell = other.cell->clone();
}

void Node::setCondConst(int X, double dx, double perc) {
     if(cell->type == string("Cell")) {
		condConst = 0.0;
		return;
	}
	if((np==1)||((X%np)==0)) {
		condConst = 1000*cell->cellRadius/(2*cell->Rcg*(cell->Rmyo*dx+rd*perc)*cell->Cm*dx);
	} else {
		condConst = 1001*cell->cellRadius/(2*cell->Rcg*cell->Rmyo*cell->Cm*dx*dx);
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

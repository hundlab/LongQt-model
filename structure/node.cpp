#include "node.h"

Node::Node(const Node& other) {
	rd = other.rd;
	Rmyo = other.Rmyo;
	dIax = other.dIax;
	x = other.x;
	y = other.y;
	d1 = other.d1;
	d2 = other.d2;
	d3 = other.d3;
	r = other.r;
	vNew = other.vNew;
	nodeType = other.nodeType;
	cell = other.cell->clone();
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

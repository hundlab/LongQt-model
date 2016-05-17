#include "grid.h"

Grid::Grid(){

}

Grid::~Grid(){}

void Grid::addRow(int pos) {
    fiber.insert(fiber.begin() +pos, Fiber(fibery.size()));
    {unsigned int i = 0;
    for(auto it = fibery.begin(); it != fibery.end()&&i<fiber[pos].nodes.size(); it++,i++) {
        it->nodes.insert(it->nodes.begin() + pos, fiber[pos].nodes[i]);
    }
    }
}
void Grid::addColumn(int pos) {
    fibery.insert(fibery.begin() +pos, Fiber(fiber.size()));
    {unsigned int i = 0;
    for(auto it = fiber.begin(); it != fiber.end()&&i<fibery[pos].nodes.size(); it++,i++) {
        it->nodes.insert(it->nodes.begin() + pos, fibery[pos].nodes[i]);
    }
    }
}
void Grid::removeRow(int pos) {
    fiber.erase(fiber.begin()+pos);
    for(auto it = fibery.begin(); it!=fibery.end() ; it++) {
        it->nodes.erase(it->nodes.begin() +pos);
    }
}
void Grid::removeColumn(int pos) {
    fibery.erase(fibery.begin()+pos);
    for(auto it = fiber.begin(); it!=fiber.end() ; it++) {
        it->nodes.erase(it->nodes.begin() +pos);
    }
}
void Grid::setCellTypes(set<cellInfo> cells) {
    {int i = 0;
    for(auto it = cells.begin(); it != cells.end(); it++, i++) {
        fiber[it->X].nodes[it->Y] = it->n;
        fibery[it->Y].nodes[it->X] = it->n;
        it->n->x = it->X*it->dx;
        it->n->y = it->Y*it->dy;
        if((it->np==1)||((i%it->np)==0)) { //may be different in x vrs y
            it->n->B = 1000*it->n->cell->cellRadius/(2*it->n->cell->Rcg*(it->n->Rmyo*it->dx+it->n->rd)*it->n->cell->Cm*it->dx);
        } else {
            it->n->B = 1000*it->n->cell->cellRadius/(2*it->n->cell->Rcg*it->n->Rmyo*it->n->cell->Cm*it->dx*it->dx);
        }
        if(it->n->cell->type == string("Cell")) {
            it->n->B = 0.0;
        }
    }
    }
} 
int Grid::rowCount() {
    return fiber.size();
}
int Grid::columnCount() {
    return fibery.size();
}

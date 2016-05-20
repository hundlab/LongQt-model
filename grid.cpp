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
        Node* n = fiber[it->X].nodes[it->Y];
        n->x = it->X*it->dx;
        n->y = it->Y*it->dy;
        if((it->np==1)||((i%it->np)==0)) {
            n->B = 1000*n->cell->cellRadius/(2*n->cell->Rcg*(n->Rmyo*it->dx+n->rd)*n->cell->Cm*it->dx);
        } else {
            n->B = 1000*n->cell->cellRadius/(2*n->cell->Rcg*n->Rmyo*n->cell->Cm*it->dx*it->dx);
        }
        if(it->n->cell->type == string("Cell")) {
            n->B = 0.0;
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
void Grid::addBuffer() {
    addRow(fiber.size());
    addColumn(fibery.size());
    for(auto it = fiber[fiber.size()-1].nodes.begin();it != fiber[fiber.size()-1].nodes.end(); it++) {
        (*it)->B = 0.0;
    }
    for(auto it = fiber[0].nodes.begin();it != fiber[0].nodes.end(); it++) {
        (*it)->B = 0.0;
    }
    for(auto it = fibery[fibery.size()-1].nodes.begin();it != fibery[fibery.size()-1].nodes.end(); it++) {
        (*it)->B = 0.0;
    }
    for(auto it = fibery[0].nodes.begin();it != fibery[0].nodes.end(); it++) {
        (*it)->B = 0.0;
    }
}

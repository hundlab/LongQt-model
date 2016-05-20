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
    for(auto it = cells.begin(); it != cells.end(); it++) {
        setCellTypes(*it);
    }
} 
void Grid::setCellTypes(const cellInfo& singleCell) {
        Node* n = fiber[singleCell.X].nodes[singleCell.Y];
        n->cell = singleCell.cell;
        n->x = singleCell.X*singleCell.dx;
        n->y = singleCell.Y*singleCell.dy;
        if((singleCell.np==1)||((singleCell.X%singleCell.np)==0)) {
            n->B = 1000*n->cell->cellRadius/(2*n->cell->Rcg*(n->Rmyo*singleCell.dx+n->rd)*n->cell->Cm*singleCell.dx);
        } else {
            n->B = 1000*n->cell->cellRadius/(2*n->cell->Rcg*n->Rmyo*n->cell->Cm*singleCell.dx*singleCell.dx);
        }
        if(singleCell.cell->type == string("Cell")) {
            n->B = 0.0;
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

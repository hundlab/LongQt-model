#include "grid.h"

Grid::Grid(){

}

Grid::~Grid(){}

void Grid::addRow(int pos) {
    fiber.insert(fiber.begin() +pos, Fiber(fibery.size()));
    {unsigned int i = 0;
    for(auto it = fibery.begin(); it != fibery.end()&&i<fiber[pos].nodes.size(); it++,i++) {
        it->nodes.insert(it->nodes.begin() + pos, fiber[pos].nodes[i]);
        it->B.insert(it->B.begin() + pos, 0);
    }
    }
}
void Grid::addRows(unsigned int num) {
    for(unsigned int i = 0; i < num; i++) {
        this->addRow(0);
    }
}
void Grid::addColumn(int pos) {
    fibery.insert(fibery.begin() +pos, Fiber(fiber.size()));
    {unsigned int i = 0;
    for(auto it = fiber.begin(); it != fiber.end()&&i<fibery[pos].nodes.size(); it++,i++) {
        it->nodes.insert(it->nodes.begin() + pos, fibery[pos].nodes[i]);
        it->B.insert(it->B.begin() + pos, 0);
    }
    }
}
void Grid::addColumns(unsigned int num) {
    for(unsigned int i = 0; i < num; i++) {
        this->addColumn(0);
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
void Grid::setCellTypes(set<cellInfo*>& cells) {
    for(auto it : cells) {
        setCellTypes(*it);
    }
}
void Grid::setCellTypes(const cellInfo& singleCell) {
    try {
        Node* n = fiber.at(singleCell.X).nodes.at(singleCell.Y);
        n->cell = singleCell.cell;
        n->x = singleCell.X*singleCell.dx;
        n->y = singleCell.Y*singleCell.dy;
        if((singleCell.np==1)||((singleCell.X%singleCell.np)==0)) {
            fiber.at(singleCell.X).B.at(singleCell.Y) = fibery.at(singleCell.Y).B.at(singleCell.X) = 1000*n->cell->cellRadius/(2*n->cell->Rcg*(n->Rmyo*singleCell.dx+n->rd)*n->cell->Cm*singleCell.dx);
        } else {
            fiber.at(singleCell.X).B.at(singleCell.Y) = fibery.at(singleCell.Y).B.at(singleCell.X) = 1000*n->cell->cellRadius/(2*n->cell->Rcg*n->Rmyo*n->cell->Cm*singleCell.dx*singleCell.dx);
        }
        if(singleCell.cell->type == string("Cell")) {
            fiber.at(singleCell.X).B.at(singleCell.Y) = fibery.at(singleCell.Y).B.at(singleCell.X) = 0.0;
        }
        if(singleCell.X == 0 ||singleCell.X == fiber.size()) {
            fibery.at(singleCell.Y).B.at(singleCell.X) = 0.0;
        }
        if(singleCell.Y == fibery.size() || singleCell.Y == 0) {
            fiber.at(singleCell.X).B.at(singleCell.Y) = 0.0;
        }
    } catch(const std::out_of_range& oor) {
        return;
    }
}
int Grid::rowCount() {
    return fiber.size();
}
int Grid::columnCount() {
    return fibery.size();
}
void Grid::addBuffer() {
/*    addRow(fiber.size());
    addColumn(fibery.size());
    for(auto it : fiber[fiber.size()-1].nodes) {
        it->B = 0.0;
    }
    for(auto it : fiber[0].nodes) {
        it->B = 0.0;
    }
    for(auto it : fibery[fibery.size()-1].nodes) {
        it->B = 0.0;
    }
    for(auto it : fibery[0].nodes) {
        it->B = 0.0;
    }*/
}
pair<int,int> Grid::findNode(const Node* node) {
    pair<int,int> p(-1,-1);
    int i,j;
    i = j = 0;
    for(auto it : fiber) {
        for(auto iv : it.nodes) {
            if(iv == node) {
                p = make_pair(i,j);
                return p;
            }
            i++;
        }
        i=0;
        j++;
    }
    return p;
}
Node* Grid::findNode(const pair<int,int>& p) {
    try {
        return fiber.at(p.second).nodes.at(p.first);
    } catch(const std::out_of_range& oor) {
        return NULL;
    }
}

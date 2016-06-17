#include "grid.h"

Grid::Grid(){}
Grid::Grid(const Grid& other) {
    fiber = vector<Fiber>(other.fiber.size(), Fiber(other.fibery.size()));
    fibery = vector<Fiber>(other.fibery.size(), Fiber(other.fiber.size()));
    for(unsigned int i = 0; i < fiber.size();i++) {
        fiber[i].B = other.fiber[i].B;
        for(unsigned int j = 0; j < fibery.size(); j++) {
            Node* n = new Node(*other.fiber[i].nodes[j]);
            fiber[i].nodes[j] = n;
            fibery[j].nodes[i] = n;
        }
    }
    for(unsigned int i = 0; i < fibery.size(); i++) {
        fibery[i].B = other.fibery[i].B;
    }
}
Grid::~Grid(){}

void Grid::addRow(int pos) {
    fiber.insert(fiber.begin() +pos, Fiber(static_cast<int>(fibery.size())));
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
    fibery.insert(fibery.begin() +pos, Fiber(static_cast<int>(fiber.size())));
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
        it->B.erase(it->B.begin() +pos);
    }
}
void Grid::removeColumn(int pos) {
    fibery.erase(fibery.begin()+pos);
    for(auto it = fiber.begin(); it!=fiber.end() ; it++) {
        it->nodes.erase(it->nodes.begin() +pos);
        it->B.erase(it->B.begin() +pos);
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
        if(singleCell.X == 0 ||static_cast<unsigned int>(singleCell.X) == fiber.size()) {
            fibery.at(singleCell.Y).B.at(singleCell.X) = 0.0;
        }
        if(static_cast<unsigned int>(singleCell.Y) == fibery.size() || singleCell.Y == 0) {
            fiber.at(singleCell.X).B.at(singleCell.Y) = 0.0;
        }
    } catch(const std::out_of_range& oor) {
        cerr << oor.what() << ": " << "new cell was not in range of grid" << endl;
        return;
    }
}
int Grid::rowCount() {
    return static_cast<int>(fiber.size());
}
int Grid::columnCount() {
    return static_cast<int>(fibery.size());
}
void Grid::addBuffer() {
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
            j++;
        }
        j=0;
        i++;
    }
    return p;
}
Node* Grid::findNode(const pair<int,int>& p) {
    try {
        return fiber.at(p.first).nodes.at(p.second);
    } catch(const std::out_of_range&) {
        return NULL;
    }
}

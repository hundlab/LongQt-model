#include "grid.h"

Grid::Grid(){}
Grid::Grid(const Grid& other) {
    fiber = vector<Fiber>(other.fiber.size(), Fiber(other.fibery.size()));
    fibery = vector<Fiber>(other.fibery.size(), Fiber(other.fiber.size()));
    for(unsigned int i = 0; i < fiber.size();i++) {
        fiber[i].B = other.fiber[i].B;
        for(unsigned int j = 0; j < fibery.size(); j++) {
            Node* n = new Node(*other.fiber[i].nodes[j]);
            fiber[i].nodes[j].reset(n);
            fibery[j].nodes[i].reset(n);
        }
    }
    for(unsigned int i = 0; i < fibery.size(); i++) {
        fibery[i].B = other.fibery[i].B;
    }
}
Grid::~Grid(){}

void Grid::addRow(int pos) {
    if(pos < 0) {
        pos = fiber.size();
    }
    fiber.insert(fiber.begin() +pos, Fiber(static_cast<int>(fibery.size())));
    {unsigned int i = 0;
    for(auto it = fibery.begin(); it != fibery.end()&&i<fiber[pos].nodes.size(); it++,i++) {
        it->nodes.insert(it->nodes.begin() + pos, fiber[pos].nodes[i]);
        it->B.insert(it->B.begin() + pos, 0);
    }
    }
}
void Grid::addRows(unsigned int num, int position) {
    for(unsigned int i = 0; i < num; i++) {
        this->addRow(position);
    }
}
void Grid::addColumn(int pos) {
    if(pos < 0) {
        pos = fibery.size();
    }
    fibery.insert(fibery.begin() +pos, Fiber(static_cast<int>(fiber.size())));
    {unsigned int i = 0;
    for(auto it = fiber.begin(); it != fiber.end()&&i<fibery[pos].nodes.size(); it++,i++) {
        it->nodes.insert(it->nodes.begin() + pos, fibery[pos].nodes[i]);
        it->B.insert(it->B.begin() + pos, 0);
    }
    }
}
void Grid::addColumns(unsigned int num, int position) {
    for(unsigned int i = 0; i < num; i++) {
        this->addColumn(position);
    }
}
void Grid::removeRows(unsigned int num, int position) {
    for(unsigned int i = 0; i < num; i++) {
        this->removeRow(position);
    }
}
void Grid::removeRow(int pos) {
    if(pos < 0) {
        pos = fiber.size()-1;
    }
	if(pos < 0 || (unsigned int)pos >= fiber.size()) {
		return;
	}
    fiber.erase(fiber.begin()+pos);
    for(auto it = fibery.begin(); it!=fibery.end() ; it++) {
        it->nodes.erase(it->nodes.begin() +pos);
        it->B.erase(it->B.begin() +pos);
    }
}
void Grid::removeColumns(unsigned int num, int position) {
    for(unsigned int i = 0; i < num; i++) {
        this->removeColumn(position);
    }
}
void Grid::removeColumn(int pos) {
    if(pos < 0) {
        pos = fibery.size()-1;
    }
	if(pos < 0 || (unsigned int)pos >= fibery.size()) {
		return;
	}
    fibery.erase(fibery.begin()+pos);
    for(auto it = fiber.begin(); it!=fiber.end() ; it++) {
        it->nodes.erase(it->nodes.begin() +pos);
        it->B.erase(it->B.begin() +pos);
    }
}
void Grid::setCellTypes(set<CellInfo*>& cells) {
    for(auto it : cells) {
        setCellTypes(*it);
    }
}
void Grid::setCellTypes(const CellInfo& singleCell) {
    try {
        Node* n = (*this)(singleCell.X,singleCell.Y);
		if(singleCell.cell) {
            n->cell.reset(singleCell.cell);
		}
		n->np = singleCell.np;
//        n->x = singleCell.X*singleCell.dx;
//        n->y = singleCell.Y*singleCell.dy;

		bool update = !(isnan(singleCell.c[0])&&isnan(singleCell.c[1])
				&&isnan(singleCell.c[2])&&isnan(singleCell.c[3]));
		if(!(update&&isnan(singleCell.c[CellUtils::top]))) {
			this->updateB(singleCell, CellUtils::top);
		}
		if(!(update&&isnan(singleCell.c[CellUtils::bottom]))) {
			this->updateB(singleCell, CellUtils::bottom);
		}
		if(!(update&&isnan(singleCell.c[CellUtils::left]))) {
			this->updateB(singleCell, CellUtils::left);
		}
		if(!(update&&isnan(singleCell.c[CellUtils::right]))) {
			this->updateB(singleCell, CellUtils::right);
		}
        if(singleCell.X == 0 ||
				static_cast<unsigned int>(singleCell.X) == fiber.size()) {
//            fibery.at(singleCell.Y).B.at(singleCell.X) = 0.0;
        }
		if(static_cast<unsigned int>(singleCell.Y) == fibery.size() ||
				singleCell.Y == 0) {
//			fiber.at(singleCell.X).B.at(singleCell.Y) = 0.0;
		}
    } catch(const std::out_of_range& oor) {
        throw new std::out_of_range(string(oor.what())+string(": new cell was not in range of grid"));
    }
}
int Grid::rowCount() {
    return static_cast<int>(fiber.size());
}
int Grid::columnCount() {
    return static_cast<int>(fibery.size());
}
pair<int,int> Grid::findNode(const Node* node) {
    pair<int,int> p(-1,-1);
    int i,j;
    i = j = 0;
    for(auto it : fiber) {
        for(auto iv : it.nodes) {
            if(iv.get() == node) {
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
Node* Grid::operator()(const pair<int,int>& p) {
    return (*this)(p.first,p.second);
}

Node* Grid::operator()(const int x, const int y) {
    try {
        return fiber.at(x).nodes.at(y).get();
    } catch(const std::out_of_range&) {
        qDebug("Grid: (%i,%i) not in grid", x,y);
        return NULL;
    }
}

void Grid::reset() {
	this->fiber.clear();
	this->fibery.clear();
}

void Grid::updateB(CellInfo node, CellUtils::Side s) {
	int x = node.X;
	int y = node.Y;
    Node* nc = (*this)(x,y);
	double condOther;
	double xo = x;
	double yo = y;
	double bpx = x;
	double bpy = y;
	bool lr = true;
	Node* n = NULL;
	CellUtils::Side so;

	if(!isnan(node.c[s]))
		nc->setCondConst(x, node.dx, s, node.c_perc, node.c[s]);
	else
		nc->setCondConst(x, node.dx, s);

	double cond = nc->condConst[s];

	switch(s) {
	case CellUtils::top:
		xo = x-1;
		lr = false;
		so = CellUtils::bottom;
		break;
	case CellUtils::bottom:
		xo = x+1;
		bpx = x+1;
		lr = false;
		so = CellUtils::top;
		break;
	case CellUtils::right:
		yo = y+1;
		bpy = y+1;
		so = CellUtils::left;
		break;
	case CellUtils::left:
		yo = y-1;
		so = CellUtils::right;
		break;
	}
    n = (*this)(xo,yo);
	if(n == NULL) {
		return;
	}
	if(!isnan(node.c[s]))
		n->setCondConst(xo, node.dx, so, node.c_perc, node.c[s]);
	condOther = n->condConst[so];
	try {
		if(lr) {
			fiber.at(bpx).B.at(bpy) = (cond+condOther)/2;
		} else {
			fibery.at(bpy).B.at(bpx) = (cond+condOther)/2;
		}
	} catch(const std::out_of_range& oor) {
        throw new std::out_of_range(string(oor.what())+string(": new cell was not in range of grid"));
    }
}

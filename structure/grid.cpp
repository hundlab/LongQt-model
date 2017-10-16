#include "grid.h"

Grid::Grid(){}
Grid::Grid(const Grid& other) {
    rows.resize(other.rowCount(), Fiber(other.columnCount()));
    columns.resize(other.columnCount(), Fiber(other.rowCount()));
    for(unsigned int rn = 0; rn < rows.size();rn++) {
        rows[rn].B = other.rows[rn].B;
        for(unsigned int cn = 0; cn < columns.size(); cn++) {
            rows[rn].nodes[cn] = make_shared<Node>(*other.rows[rn].nodes[cn]);
            columns[cn].nodes[rn] = rows[rn].nodes[cn];
        }
    }
    for(unsigned int cn = 0; cn < columns.size(); cn++) {
        columns[cn].B = other.columns[cn].B;
    }
}
Grid::~Grid(){}

void Grid::addRow(int pos) {
    if(pos < 0|| pos>rows.size()) {
        pos = rows.size();
    }
    rows.insert(rows.begin() +pos, Fiber(static_cast<int>(columns.size())));
    {unsigned int i = 0;
    for(auto it = columns.begin(); it != columns.end()&&i<rows[pos].nodes.size(); it++,i++) {
        it->nodes.insert(it->nodes.begin() + pos, rows[pos].nodes[i]);
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
    if(pos < 0 || pos>columns.size()) {
        pos = columns.size();
    }
    columns.insert(columns.begin() +pos, Fiber(static_cast<int>(rows.size())));
    {unsigned int i = 0;
    for(auto it = rows.begin(); it != rows.end()&&i<columns[pos].nodes.size(); it++,i++) {
        it->nodes.insert(it->nodes.begin() + pos, columns[pos].nodes[i]);
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
        pos = rows.size()-1;
    }
	if(pos < 0 || (unsigned int)pos >= rows.size()) {
		return;
	}
    rows.erase(rows.begin()+pos);
    for(auto it = columns.begin(); it!=columns.end() ; it++) {
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
        pos = columns.size()-1;
    }
	if(pos < 0 || (unsigned int)pos >= columns.size()) {
		return;
	}
    columns.erase(columns.begin()+pos);
    for(auto it = rows.begin(); it!=rows.end() ; it++) {
        it->nodes.erase(it->nodes.begin() +pos);
        it->B.erase(it->B.begin() +pos);
    }
}
void Grid::setCellTypes(list<CellInfo>& cells) {
    for(auto& c : cells) {
        setCellTypes(c);
    }
}
void Grid::setCellTypes(const CellInfo& singleCell) {
    try {
        shared_ptr<Node> n = (*this)(singleCell.row,singleCell.col);
        if(!n) return;
		if(singleCell.cell) {
            n->cell = singleCell.cell;
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
        if(singleCell.row == 0 ||
				static_cast<unsigned int>(singleCell.row) == rows.size()) {
//            fibery.at(singleCell.Y).B.at(singleCell.X) = 0.0;
        }
		if(static_cast<unsigned int>(singleCell.col) == columns.size() ||
				singleCell.col == 0) {
//			fiber.at(singleCell.X).B.at(singleCell.Y) = 0.0;
		}
    } catch(const std::out_of_range& oor) {
        throw new std::out_of_range(string(oor.what())+string(": new cell was not in range of grid"));
    }
}
int Grid::rowCount() const {
    return static_cast<int>(rows.size());
}
int Grid::columnCount() const {
    return static_cast<int>(columns.size());
}
pair<int,int> Grid::findNode(const shared_ptr<Node> toFind) {
    pair<int,int> p(-1,-1);
    int rn,cn;
    rn = cn = 0;
    for(auto row : rows) {
        for(auto node : row) {
            if(node == toFind) {
                p = make_pair(rn,cn);
                return p;
            }
            cn++;
        }
        cn=0;
        rn++;
    }
    return p;
}
shared_ptr<Node> Grid::operator()(const pair<int,int>& p) {
    return (*this)(p.first,p.second);
}

shared_ptr<Node> Grid::operator()(const int row, const int col) {
    try {
        return rows.at(row).at(col);
    } catch(const std::out_of_range&) {
        qDebug("Grid: (%i,%i) not in grid", row,col);
        return shared_ptr<Node>();
    }
}

void Grid::reset() {
	this->rows.clear();
	this->columns.clear();
}

void Grid::updateB(CellInfo node, CellUtils::Side s) {
    int row = node.row;
    int col = node.col;
    shared_ptr<Node> nc = (*this)(row,col);
	double condOther;
    double rowo = row;
    double colo = col;
    double bpx = row;
    double bpy = col;
	bool lr = true;
    shared_ptr<Node> n;
	CellUtils::Side so;

	if(!isnan(node.c[s]))
        nc->setCondConst(row, node.dx, s, node.c_perc, node.c[s]);
	else
        nc->setCondConst(row, node.dx, s);

	double cond = nc->condConst[s];

	switch(s) {
	case CellUtils::top:
        rowo = row-1;
		lr = false;
		so = CellUtils::bottom;
		break;
	case CellUtils::bottom:
        rowo = row+1;
        bpx = row+1;
		lr = false;
		so = CellUtils::top;
		break;
	case CellUtils::right:
        colo = col+1;
        bpy = col+1;
		so = CellUtils::left;
		break;
	case CellUtils::left:
        colo = col-1;
		so = CellUtils::right;
		break;
	}
    n = (*this)(rowo,colo);
    if(!n) {
		return;
	}
	if(!isnan(node.c[s]))
        n->setCondConst(rowo, node.dx, so, node.c_perc, node.c[s]);
	condOther = n->condConst[so];
	try {
		if(lr) {
			rows.at(bpx).B.at(bpy) = (cond+condOther)/2;
		} else {
			columns.at(bpy).B.at(bpx) = (cond+condOther)/2;
		}
	} catch(const std::out_of_range& oor) {
        throw new std::out_of_range(string(oor.what())+string(": new cell was not in range of grid"));
    }
}

void Grid::updateConnectivities()
{
    for(int row = 0; row < this->rowCount(); ++row) {
        for(int col = 0; col < this->columnCount(); ++col) {
            if(row == 0) {
                (*this)(row,col)->condConst[CellUtils::Side::top] = 0;
            }
            if(row == this->rowCount()-1) {
                (*this)(row,col)->condConst[CellUtils::Side::bottom] = 0;
            }
            if(col == 0) {
                (*this)(row,col)->condConst[CellUtils::Side::left] = 0;
            }
            if(col == this->columnCount()-1) {
                (*this)(row,col)->condConst[CellUtils::Side::right] = 0;
            }
            this->rows.at(row).B[col] = (*this)(row,col)->condConst[CellUtils::Side::left];
            this->rows.at(row).B[col+1] = (*this)(row,col)->condConst[CellUtils::Side::right];
            this->columns.at(col).B[row] = (*this)(row,col)->condConst[CellUtils::Side::top];
            this->columns.at(col).B[row+1] = (*this)(row,col)->condConst[CellUtils::Side::bottom];
        }
    }
}

Grid::const_iterator Grid::begin() const {
    return this->rows.begin();
}

Grid::const_iterator Grid::end() const {
    return this->rows.end();
}

Grid::iterator Grid::begin() {
    return this->rows.begin();
}

Grid::iterator Grid::end() {
    return this->rows.end();
}

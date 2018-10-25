#include "grid.h"
using namespace LongQt;
using namespace std;

Grid::Grid(){}
Grid::Grid(const Grid& other) {
    rows.resize(other.rowCount(), Fiber(other.columnCount()));
    columns.resize(other.columnCount(), Fiber(other.rowCount()));
    for(unsigned int rn = 0; rn < rows.size();rn++) {
        rows[rn].B = other.rows[rn].B;
        for(unsigned int cn = 0; cn < columns.size(); cn++) {
            rows[rn].nodes[cn] = make_shared<Node>(*other.rows[rn].nodes[cn]);
            columns[cn].nodes[rn] = rows[rn].nodes[cn];
            rows[rn].nodes[cn]->setParent(this,rn,cn);
        }
    }
    for(unsigned int cn = 0; cn < columns.size(); cn++) {
        columns[cn].B = other.columns[cn].B;
    }
    this->updateNodePositions();
}
Grid::~Grid(){}

void Grid::addRow() {
    rows.push_back(Fiber(static_cast<int>(columns.size())));
    int cc = 0;
    int rc = rows.size()-1;
    for(auto& column: columns) {
        column.nodes.push_back(rows.at(rc)[cc]);
        column.B.push_back(0.0);
        rows[rc][cc]->setParent(this,rc,cc);
        ++cc;
    }
}
void Grid::addRows(unsigned int num) {
    for(unsigned int i = 0; i < num; i++) {
        this->addRow();
    }
}
void Grid::addColumn() {
    columns.push_back(Fiber(static_cast<int>(rows.size())));
    int rc = 0;
    int cc = columns.size()-1;
    for(auto& row: rows) {
        row.nodes.push_back(columns[cc][rc]);
        row.B.push_back(0.0);
        columns[cc][rc]->setParent(this,rc,cc);
        ++rc;
    }
}
void Grid::addColumns(unsigned int num) {
    for(unsigned int i = 0; i < num; i++) {
        this->addColumn();
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
        for(int i = 0; i < 4; ++i) {
            if(!(update&&isnan(singleCell.c[i]))) {
                 n->setCondConst(singleCell.dx,CellUtils::Side(i),
                     singleCell.c_perc,singleCell.c[i]);
            }
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

/*    int row = node.row;
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
        nc->setCondConst(node.dx, s, node.c_perc, node.c[s]);
	else
        nc->setCondConst(node.dx, s);

    double cond = nc->getCondConst(s);

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
        n->setCondConst(node.dx, so, node.c_perc, node.c[s]);
        condOther = n->getCondConst(so);
	try {
		if(lr) {
			rows.at(bpx).B.at(bpy) = (cond+condOther)/2;
		} else {
			columns.at(bpy).B.at(bpx) = (cond+condOther)/2;
		}
	} catch(const std::out_of_range& oor) {
        throw new std::out_of_range(string(oor.what())+string(": new cell was not in range of grid"));
    }
    */
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

void Grid::updateNodePositions()
{
    for(int rc = 0; rc < rowCount(); ++rc) {
        for(int cc = 0; cc < columnCount(); ++cc) {
            (*this)(rc,cc)->setPosition(rc,cc);
        }
    }
}

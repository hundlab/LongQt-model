#include "grid.h"
#include "logger.h"
using namespace LongQt;
using namespace std;

Grid::Grid() {}
Grid::Grid(const Grid& other) {
  using namespace CellUtils;
  rows.resize(other.rowCount(), Fiber(other.columnCount(), Side::right));
  columns.resize(other.columnCount(), Fiber(other.rowCount(), Side::bottom));
  for (unsigned int rn = 0; rn < rows.size(); rn++) {
    for (unsigned int cn = 0; cn < columns.size(); cn++) {
      rows[rn].nodes[cn] = make_shared<Node>(*other.rows[rn].nodes[cn]);
      columns[cn].nodes[rn] = rows[rn].nodes[cn];
      rows[rn].nodes[cn]->setParent(this, rn, cn);
    }
  }
  //  for (unsigned int cn = 0; cn < columns.size(); cn++) {
  //    columns[cn].B = other.columns[cn].B;
  //  }
  dx = other.dx;
  dy = other.dy;
  np = other.np;
  this->updateNodePositions();
}
Grid::~Grid() {}

void Grid::addRow() {
  rows.push_back(
      Fiber(static_cast<int>(columns.size()), CellUtils::Side::left));
  int cc = 0;
  int rc = rows.size() - 1;
  for (auto& column : columns) {
    column.nodes.push_back(rows.at(rc)[cc]);
    //    column.B.push_back(0.0);
    rows[rc][cc]->setParent(this, rc, cc);
    ++cc;
  }
}
void Grid::addRows(unsigned int num) {
  for (unsigned int i = 0; i < num; i++) {
    this->addRow();
  }
}
void Grid::addColumn() {
  columns.push_back(Fiber(static_cast<int>(rows.size()), CellUtils::Side::top));
  int rc = 0;
  int cc = columns.size() - 1;
  for (auto& row : rows) {
    row.nodes.push_back(columns[cc][rc]);
    //    row.B.push_back(0.0);
    columns[cc][rc]->setParent(this, rc, cc);
    ++rc;
  }
}
void Grid::addColumns(unsigned int num) {
  for (unsigned int i = 0; i < num; i++) {
    this->addColumn();
  }
}
void Grid::removeRows(unsigned int num, int position) {
  for (unsigned int i = 0; i < num; i++) {
    this->removeRow(position);
  }
}
void Grid::removeRow(int pos) {
  if (pos < 0) {
    pos = rows.size() - 1;
  }
  if (pos < 0 || (unsigned int)pos >= rows.size()) {
    return;
  }
  rows.erase(rows.begin() + pos);
  for (auto it = columns.begin(); it != columns.end(); it++) {
    it->nodes.erase(it->nodes.begin() + pos);
    //    it->B.erase(it->B.begin() + pos);
  }
}
void Grid::removeColumns(unsigned int num, int position) {
  for (unsigned int i = 0; i < num; i++) {
    this->removeColumn(position);
  }
}
void Grid::removeColumn(int pos) {
  if (pos < 0) {
    pos = columns.size() - 1;
  }
  if (pos < 0 || (unsigned int)pos >= columns.size()) {
    return;
  }
  columns.erase(columns.begin() + pos);
  for (auto it = rows.begin(); it != rows.end(); it++) {
    it->nodes.erase(it->nodes.begin() + pos);
    //    it->B.erase(it->B.begin() + pos);
  }
}
// void Grid::setCellTypes(list<CellInfo>& cells) {
//  for (auto& c : cells) {
//    setCellTypes(c);
//  }
//}
// void Grid::setCellTypes(const CellInfo& singleCell) {
//  try {
//    shared_ptr<Node> n = (*this)(singleCell.row, singleCell.col);
//    if (!n) return;
//    if (singleCell.cell) {
//      n->cell = singleCell.cell;
//    }
//    //        n->x = singleCell.X*singleCell.dx;
//    //        n->y = singleCell.Y*singleCell.dy;

//    for (int i = 0; i < 4; ++i) {
//      n->setCondConst(CellUtils::Side(i), singleCell.c_perc, singleCell.c[i]);
//    }
//  } catch (const std::out_of_range& oor) {
//    Logger::getInstance()->write<std::out_of_range>(
//        "{} : new cell was not in range of grid", oor.what());
//  }
//}
int Grid::rowCount() const { return static_cast<int>(rows.size()); }
int Grid::columnCount() const { return static_cast<int>(columns.size()); }
pair<int, int> Grid::findNode(const shared_ptr<Node> toFind) {
  pair<int, int> p(-1, -1);
  int rn, cn;
  rn = cn = 0;
  for (auto row : rows) {
    for (auto node : row) {
      if (node == toFind) {
        p = make_pair(rn, cn);
        return p;
      }
      cn++;
    }
    cn = 0;
    rn++;
  }
  return p;
}
shared_ptr<Node> Grid::operator()(const pair<int, int>& p) {
  return (*this)(p.first, p.second);
}

shared_ptr<Node> Grid::operator()(const int row, const int col) {
  try {
    return rows.at(row).at(col);
  } catch (const std::out_of_range&) {
    // no log cuz it happens just too much
    // Logger::getInstance()->write<std::out_of_range>(
    //    "Grid: ({}, {}) not in grid", row, col);
    return shared_ptr<Node>();
  }
}

void Grid::reset() {
  this->rows.clear();
  this->columns.clear();
}

// void Grid::updateB(CellInfo node, CellUtils::Side s) {
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
        throw new std::out_of_range(string(oor.what())+string(": new cell was
   not in range of grid"));
    }
    */
//}

// Grid::const_iterator Grid::begin() const { return this->rows.begin(); }

// Grid::const_iterator Grid::end() const { return this->rows.end(); }

Grid::GridIterator Grid::begin() {
  return GridIterator(this, this->iterRowsFirst);
}

Grid::GridIterator Grid::end() { return GridIterator(0, this->iterRowsFirst); }

Grid::GridIterator Grid::beginRowsFirst() { return GridIterator(this, true); }

Grid::GridIterator Grid::beginColumnsFirst() {
  return GridIterator(this, false);
}

void Grid::updateNodePositions() {
  for (int rc = 0; rc < rowCount(); ++rc) {
    for (int cc = 0; cc < columnCount(); ++cc) {
      (*this)(rc, cc)->setPosition(rc, cc);
    }
  }
}

void Grid::setup() {
  for (auto& row : rows) {
    row.setup();
    for (auto node : row) {
      node->cell->setup();
    }
  }
  for (auto& col : columns) {
    col.setup();
  }
}

Grid::GridIterator::GridIterator(Grid* parent, bool rowsFirst)
    : parent(parent), rowsFirst(rowsFirst) {
  if (!parent || parent->rowCount() == 0 || parent->columnCount() == 0) {
    row = -1;
    col = -1;
  }
}

Grid::GridIterator Grid::GridIterator::operator++(int) {
  GridIterator tmp(*this);
  operator++();
  return tmp;
}

Grid::GridIterator& Grid::GridIterator::operator--() {
  if (row < 0 || col < 0) return *this;

  int& fst = this->rowsFirst ? row : col;
  int& snd = this->rowsFirst ? col : row;
  int lenFst = this->rowsFirst ? parent->rowCount() : parent->columnCount();

  --fst;
  if (fst < 0) {
    fst = lenFst - 1;
    --snd;
    if (snd < 0) {
      fst = snd = -1;
    }
  }

  return *this;
}

Grid::GridIterator Grid::GridIterator::operator--(int) {
  GridIterator tmp(*this);
  operator--();
  return tmp;
}

Grid::GridIterator& Grid::GridIterator::operator+=(int i) {
  if (row < 0 || col < 0) return *this;

  int& fst = this->rowsFirst ? row : col;
  int& snd = this->rowsFirst ? col : row;
  int lenFst = this->rowsFirst ? parent->rowCount() : parent->columnCount();
  int lenSnd = this->rowsFirst ? parent->columnCount() : parent->rowCount();

  fst = (fst + i) % lenFst;
  snd = (int)((i + fst + (snd * lenFst)) / lenFst);
  if (snd < 0 || lenSnd <= snd) {
    fst = snd = -1;
  }
  return *this;
}

Grid::GridIterator& Grid::GridIterator::operator-=(int i) {
  (*this) += -i;
  return *this;
}

Grid::GridIterator Grid::GridIterator::operator+(int i) {
  GridIterator tmp(*this);
  tmp += i;
  return tmp;
}

Grid::GridIterator Grid::GridIterator::operator-(int i) {
  GridIterator tmp(*this);
  tmp -= i;
  return tmp;
}

bool Grid::GridIterator::operator==(const Grid::GridIterator& rhs) const {
  return ((row == rhs.row) && (col == rhs.col));
}

bool Grid::GridIterator::operator!=(const Grid::GridIterator& rhs) const {
  return !operator==(rhs);
}

std::shared_ptr<Node> Grid::GridIterator::operator*() {
  return (*parent)(row, col);
}

Grid::GridIterator& Grid::GridIterator::operator++() {
  if (row < 0 || col < 0) return *this;

  int& fst = this->rowsFirst ? row : col;
  int& snd = this->rowsFirst ? col : row;
  int lenFst = this->rowsFirst ? parent->rowCount() : parent->columnCount();
  int lenSnd = this->rowsFirst ? parent->columnCount() : parent->rowCount();

  ++fst;
  if (fst >= lenFst) {
    fst = 0;
    ++snd;
    if (snd >= lenSnd) {
      fst = snd = -1;
    }
  }

  return *this;
}

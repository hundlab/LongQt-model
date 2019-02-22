#include "node.h"
#include <algorithm>
#include <thread>
#include "grid.h"
using namespace LongQt;
using namespace std;

Node::Node(const Node &o) : std::enable_shared_from_this<Node>(o) {
  rd = o.rd;
  dIax = o.dIax;
  //	x = other.x;
  //	y = other.y;
  d1 = o.d1;
  d2 = o.d2;
  d3 = o.d3;
  r = o.r;
  vNew = o.vNew;
  //  nodeType = other.nodeType;
  cell.reset(o.cell->clone());
  row = o.row;
  col = o.col;
  c = o.c;
}

/*double Node::calPerc(int X, double dx, double val) {
        if(val == 0) {
                return 0;
        }
        if((np==1)||((X%np)==0)) {
                return
((1000*cell->cellRadius)/(val*2*cell->Rcg*cell->Cm*dx)-cell->Rmyo*dx)/rd; } else
{ return 1;
        }
}*/

void Node::setCondConst(CellUtils::Side s, bool perc, double val) {
  //  auto nei = this->calcNeighborPos(s);
  //  std:shared_ptr<Node> neiNode;
  //  try {
  //    neiNode = (*parent)(nei);
  //  } catch(std::out_of_range) {
  //      return;
  //  }
  //  if (!neiNode) {
  //    return;
  //  }

  if (perc) {
    if (isnan(val)) {
      val = 1;
    }
    this->c[s] = this->calcOurCondConst(s, val);
  } else {
    this->c[s] = val;
  }
}
pair<int, int> Node::calcNeighborPos(CellUtils::Side s) {
  pair<int, int> nei;
  switch (s) {
    case CellUtils::Side::left:
      nei = pair<int, int>(row, col - 1);
      break;
    case CellUtils::Side::right:
      nei = pair<int, int>(row, col + 1);
      break;
    case CellUtils::Side::top:
      nei = pair<int, int>(row - 1, col);
      break;
    case CellUtils::Side::bottom:
      nei = pair<int, int>(row + 1, col);
      break;
  }
  return nei;
}

double Node::calcOurCondConst(CellUtils::Side s, double val) {
  if (val == 0 || cell->type() == inexName) {
    return 0;
  }
  bool isRow = (s == CellUtils::Side::right || s == CellUtils::Side::left);
  int reduction = isRow ? 1 : 2;
  double dx = isRow ? parent->dx : parent->dy;
  int X = isRow ? row : col;
  if ((parent->np == 1) || ((X % parent->np) == 0)) {
    return (1000 / reduction) * cell->cellRadius /
           (2 * cell->Rcg * (cell->Rmyo * dx + rd * val) * cell->Cm * dx);
  } else {
    return (1001 / reduction) * cell->cellRadius /
           (2 * cell->Rcg * cell->Rmyo * cell->Cm * dx * dx);
  }
}

// void Node::setCondConstDirect(CellUtils::Side s, double val) {
//  switch (s) {
//    case CellUtils::Side::left:
//      parent->rows.at(row).B.at(col) = val;
//      break;
//    case CellUtils::Side::right:
//      parent->rows.at(row).B.at(col + 1) = val;
//      break;
//    case CellUtils::Side::top:
//      parent->columns.at(col).B.at(row) = val;
//      break;
//    case CellUtils::Side::bottom:
//      parent->columns.at(col).B.at(row + 1) = val;
//      break;
//  }
//}

double Node::getCondConst(CellUtils::Side s) {
  auto nei = this->calcNeighborPos(s);
  CellUtils::Side fs = CellUtils::flipSide(s);
  std::shared_ptr<Node> neiNode;
  try {
    neiNode = (*parent)(nei);
  } catch (std::out_of_range) {
    return 0;
  }
  if (!neiNode) {
    return 0;
  }
  if (isnan(this->c[s])) {
    this->c[s] = this->calcOurCondConst(s, 1);
  }
  if (isnan(neiNode->c[fs])) {
    neiNode->c[fs] = neiNode->calcOurCondConst(fs, 1);
  }
  return std::min(neiNode->c[fs], this->c[s]);
  //  double val = -1;
  //  switch (s) {
  //    case CellUtils::Side::left:
  //      val = parent->rows.at(row).B[col];
  //      break;
  //    case CellUtils::Side::right:
  //      val = parent->rows.at(row).B[col + 1];
  //      break;
  //    case CellUtils::Side::top:
  //      val = parent->columns.at(col).B[row];
  //      break;
  //    case CellUtils::Side::bottom:
  //      val = parent->columns.at(col).B[row + 1];
  //      break;
  //  }

  //  return val;
}

void Node::waitUnlock(int which) {
  for (int i = which; i >= 0; --i) {
    bool first = true;
    while (this->lock[i]) {
      if (first) {
        Logger::getInstance()->write("Locked Node encountered");
        first = false;
      }
      std::this_thread::yield();
    }
  }
}

// double Node::setFiberB() {
//  parent->rows.at(row).B.at(col) = this->getCondConst(CellUtils::Side::left);
//  parent->rows.at(row).B.at(col + 1) =
//      this->getCondConst(CellUtils::Side::right);
//  parent->columns.at(col).B.at(row) =
//  this->getCondConst(CellUtils::Side::top); parent->columns.at(col).B.at(row +
//  1) =
//      this->getCondConst(CellUtils::Side::bottom);
//}

void Node::setParent(Grid *par, int row, int col) {
  this->parent = par;
  if (!this->parent) return;
  if (row == -1 || col == -1) {
    auto temp = parent->findNode(this->shared_from_this());
    this->row = temp.first;
    this->col = temp.second;
  } else {
    this->row = row;
    this->col = col;
  }
}

void Node::setPosition(int row, int col) {
  this->row = row;
  this->col = col;
}

Grid *Node::getParent() { return this->parent; }
/*
void Node::updateV(double dt) {
        this->cell->iTotold=this->cell->iTot;
        this->cell->dVdt=(this->vNew-this->cell->vOld)/dt;
        //##### Conservation for multicellular fiber ############
        this->dIax=-(this->cell->dVdt+this->cell->iTot);
        this->cell->iKt=this->cell->iKt+this->dIax;
        this->cell->setV(this->vNew);
}*/

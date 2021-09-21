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
  //  nodeType = other.nodeType;
  __cell.reset(o.__cell->clone());
  row = o.row;
  col = o.col;
  c = o.c;
}

void Node::resetCondConst(CellUtils::Side s) {
  if (s == -1) {
    this->c = {{NAN, NAN, NAN, NAN}};
  } else {
    this->c[s] = NAN;
  }
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

bool Node::cell(const std::string &type)
{
    if (cell() != NULL && type == cell()->type()) {
      return false;
    }
    if(type == InexcitableCell().type()) {
        this->cell(make_shared<InexcitableCell>());
        return true;
    }
    try {
      this->cell((CellUtils::cellMap.at(type))());
      return true;
    } catch (const std::out_of_range&) {
      Logger::getInstance()->write<std::out_of_range>(
          "Protocol: {} is not a valid cell type", type);
      return false;
    }
}

void Node::cell(std::shared_ptr<Cell> cell) {
  this->__cell = cell;
  this->resetCondConst();
}

std::shared_ptr<Cell> Node::cell() const { return this->__cell; }

std::list<std::string> Node::cellOptions()
{
    list<string> options;
    options.push_back(InexcitableCell().type());
    for (auto& cellOpt : CellUtils::cellMap) {
      options.push_back(cellOpt.first);
    }
    return options;
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
  if (val == 0 || __cell->type() == inexName) {
    return 0;
  }
  bool isRow = (s == CellUtils::Side::right || s == CellUtils::Side::left);
  int reduction = isRow ? 1 : 2;
  double dx = isRow ? parent->dx : parent->dy;
  int X = isRow ? row : col;
  if ((parent->np == 1) || ((X % parent->np) == 0)) {
    return (1000 / reduction) * __cell->cellRadius /
           (2 * __cell->Rcg * (__cell->Rmyo * dx + rd * val) * __cell->Cm * dx);
  } else {
    return (1001 / reduction) * __cell->cellRadius /
           (2 * __cell->Rcg * __cell->Rmyo * __cell->Cm * dx * dx);
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

bool Node::writeNode(QXmlStreamWriter &xml) {
  xml.writeTextElement("type", __cell->type());
  xml.writeStartElement("conductance");
  xml.writeTextElement("left", QString::number(this->c[CellUtils::Side::left]));
  xml.writeTextElement("right",
                       QString::number(this->c[CellUtils::Side::right]));
  xml.writeTextElement("top", QString::number(this->c[CellUtils::Side::top]));
  xml.writeTextElement("bottom",
                       QString::number(this->c[CellUtils::Side::bottom]));
  xml.writeEndElement();
  return true;
}

bool Node::readNode(QXmlStreamReader &xml) {
  if (xml.atEnd()) return false;
  auto cellMap = CellUtils::cellMap;
  auto inexcitable = InexcitableCell().type();
  cellMap[inexcitable] = []() { return make_shared<InexcitableCell>(); };
  map<QString, function<bool(QXmlStreamReader &)>> handlers;
  handlers["type"] = [cellMap, inexcitable, this](QXmlStreamReader &xml) {
    bool success = true;
    string cell_type;
    try {
      xml.readNext();
      cell_type = xml.text().toString().toStdString();
      this->cell(cellMap.at(cell_type)());
    } catch (const std::out_of_range &) {
      success = false;
      Logger::getInstance()->write("{} not a valid cell type", cell_type);
      this->cell(cellMap.at(inexcitable)());
    }
    xml.skipCurrentElement();
    return success;
  };
  handlers["conductance"] = [this](QXmlStreamReader &xml) {
    while (xml.readNextStartElement()) {
      if (xml.name() == "top") {
        xml.readNext();
        this->c[CellUtils::top] = xml.text().toDouble();
        xml.skipCurrentElement();
      } else if (xml.name() == "bottom") {
        xml.readNext();
        this->c[CellUtils::bottom] = xml.text().toDouble();
        xml.skipCurrentElement();
      } else if (xml.name() == "right") {
        xml.readNext();
        this->c[CellUtils::right] = xml.text().toDouble();
        xml.skipCurrentElement();
      } else if (xml.name() == "left") {
        xml.readNext();
        this->c[CellUtils::left] = xml.text().toDouble();
        xml.skipCurrentElement();
      }
    }
    return true;
  };

  bool success = true;
  while (xml.readNextStartElement()) {
    try {
      success &= handlers.at(xml.name().toString())(xml);
    } catch (const std::out_of_range &) {
      Logger::getInstance()->write("Node: xml type {} not recognized",
                                   qUtf8Printable(xml.name().toString()));
    }
  }
  return success;
}
/*
void Node::updateV(double dt) {
        this->cell->iTotold=this->cell->iTot;
        this->cell->dVdt=(this->vNew-this->cell->vOld)/dt;
        //##### Conservation for multicellular fiber ############
        this->dIax=-(this->cell->dVdt+this->cell->iTot);
        this->cell->iKt=this->cell->iKt+this->dIax;
        this->cell->setV(this->vNew);
}*/

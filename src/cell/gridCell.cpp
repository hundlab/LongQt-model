// 5/10/2016
//################################################
// cell type for a grid, used by grid protocol to
// run a simulaiton on a grid of cells
// class definitions
//################################################

#include <logger.h>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <functional>
#include <iostream>
#include <set>
#include <sstream>
//#include <QtConcurrent>

#include "cellutils.h"
#include "gridCell.h"
#include "inexcitablecell.h"
using namespace LongQt;
using namespace std;

GridCell::GridCell() : Cell() { this->Initialize(); }
void GridCell::Initialize() {
  makeMap();
}
GridCell::GridCell(GridCell& toCopy) : Cell(toCopy), grid(toCopy.grid) {
  this->Initialize();
  this->gridfileName = toCopy.gridfileName;
}
GridCell* GridCell::clone() { return new GridCell(*this); }
GridCell::~GridCell() {}
Grid* GridCell::getGrid() { return &grid; }
bool GridCell::setOutputfileConstants(string filename) {
  int rc = 0;
  bool toReturn = true;
  for (auto& row : grid.rows) {
    int cc = 0;
    for (auto& column : row) {
      toReturn &= column->cell->setOutputfileConstants(
          CellUtils::strprintf(filename, rc, cc));
      cc++;
    }
    rc++;
  }
  return toReturn;
}
bool GridCell::setOuputfileVariables(string filename) {
  int rc = 0;
  bool toReturn = true;
  for (auto& row : grid.rows) {
    int cc = 0;
    for (auto& column : row) {
      toReturn &= column->cell->setOuputfileVariables(
          CellUtils::strprintf(filename, rc, cc));
      ++cc;
    }
    ++rc;
  }
  return toReturn;
}
set<string> GridCell::vars() {
  set<string> toReturn = Cell::vars();
  for (auto& row : grid.rows) {
    for (auto& column : row) {
      set<string> ivSet = column->cell->vars();
      toReturn.insert(ivSet.begin(), ivSet.end());
    }
  }
  return toReturn;
}
set<string> GridCell::pars() {
  set<string> toReturn = Cell::pars();
  for (auto& row : grid.rows) {
    for (auto& column : row) {
      set<string> ivSet = column->cell->pars();
      toReturn.insert(ivSet.begin(), ivSet.end());
    }
  }
  return toReturn;
}

void GridCell::setup() {
  dtmin = std::numeric_limits<double>::infinity();
  dtmed = std::numeric_limits<double>::infinity();
  dtmax = std::numeric_limits<double>::infinity();
  for (auto& it : grid.rows) {
    for (auto& iv : it.nodes) {
      if (iv->cell->dtmin < dtmin) {
        dtmin = iv->cell->dtmin;
      }
      if (iv->cell->dtmed < dtmed) {
        dtmin = iv->cell->dtmin;
      }
      if (iv->cell->dtmax < dtmax) {
        dtmin = iv->cell->dtmin;
      }
    }
  }
  dt = dtmin;
}

bool GridCell::setConstantSelection(set<string> new_selection) {
  bool toReturn = true;
  parsSelection = new_selection;
  for (auto& it : grid.rows) {
    for (auto& iv : it.nodes) {
      toReturn &= iv->cell->setConstantSelection(new_selection);
    }
  }
  return toReturn;
}
bool GridCell::setVariableSelection(set<string> new_selection) {
  bool toReturn = true;
  varsSelection = new_selection;
  for (auto& it : grid.rows) {
    for (auto& iv : it) {
      toReturn &= iv->cell->setVariableSelection(new_selection);
    }
  }
  return toReturn;
}
void GridCell::writeConstants() {
  for (auto& it : grid.rows) {
    for (auto& iv : it) {
      iv->cell->writeConstants();
    }
  }
}
void GridCell::writeVariables() {
  for (auto& it : grid.rows) {
    for (auto& iv : it) {
      iv->cell->writeVariables();
    }
  }
}
void GridCell::closeFiles() {
  for (auto& it : grid.rows) {
    for (auto& iv : it) {
      iv->cell->closeFiles();
    }
  }
}
double GridCell::updateV() {
  double dt = this->dt;
  if (tcount == 0 && grid.columnCount() > 1) {
    //        QtConcurrent::blockingMap(grid.rows,[dt] (Fiber& f) {
    //            f.updateVm(dt);
    //        });
    //    pool.pushAllpnt([dt] (Fiber& f) {f.updateVm(dt);}, grid.rows.begin(),
    //    grid.rows.end());
    for (auto& row : grid.rows) {
      row.updateVm(dt);
    }
  } else {
    //        QtConcurrent::blockingMap(grid.columns,[dt] (Fiber& f) {
    //            f.updateVm(dt);
    //        });
    //    pool.pushAllpnt([dt] (Fiber& f) {f.updateVm(dt);}, grid.rows.begin(),
    //    grid.rows.end());
    for (auto& column : grid.columns) {
      column.updateVm(dt);
    }
  }
  //  pool.wait();
  return 0.0;
}
void GridCell::updateConc() {
  for (auto& row : grid.rows) {
    for (auto& node : row) {
      node->cell->updateConc();
    }
  }
}
void GridCell::updateCurr() {
  for (auto& row : grid.rows) {
    for (auto& node : row) {
      node->cell->updateCurr();
    }
  }
}
void GridCell::externalStim(double stimval) {
  for (auto& row : grid.rows) {
    for (auto& node : row) {
      node->cell->externalStim(stimval);
    }
  }
}
double GridCell::tstep(double stimt) {
  unsigned int i, j;
  int vmflag = 0;

  tcount = ((++tcount) % 2);
  if (tcount == 1 && (grid.rowCount() == 1 || grid.columnCount() == 1)) {
    tcount = 0;
  }
  t = t + dt;
  for (i = 0; i < grid.rows.size(); i++) {
    for (j = 0; j < grid.columns.size(); j++) {
      grid.rows[i][j]->cell->t = t;
      grid.rows[i][j]->cell->dt = dt;
      if (tcount == 0) {  // used to prevent time step change in middle of ADI
        if (grid.rows[i][j]->cell->dVdt > grid.rows[i][j]->cell->dvcut ||
            (t > (stimt - 2.0) && t < stimt + 10) || (apTime < 5.0))
          vmflag = 2;
        else if ((apTime < 40) && vmflag != 2)
          vmflag = 1;
      }
    }
  }
  if (tcount == 0) {
    if (vmflag == 2)
      dt = dtmin;
    else if (vmflag == 1)
      dt = dtmed;
    else
      dt = dtmax;
  }

  return t;
}
void GridCell::makeMap() {  // only aply to cells added after the change?
  __pars["dx"] = &grid.dx;
  __pars["dy"] = &grid.dy;
//  __pars["np"] = &grid.np;
}

const char* GridCell::type() const { return "gridCell"; }
void GridCell::setGridfile(string name) { gridfileName = name; }
string GridCell::gridfile() { return gridfileName; }
bool GridCell::writeGridfile(QXmlStreamWriter& xml) {
  int i = 0;
  int j = 0;
  xml.writeStartElement("grid");
  xml.writeAttribute("rows", QString::number(grid.rowCount()));
  xml.writeAttribute("columns", QString::number(grid.columnCount()));
  xml.writeAttribute("np", QString::number(grid.np));
  xml.writeAttribute("dx", QString::number(grid.dx));
  xml.writeAttribute("dy", QString::number(grid.dy));

  for (auto& row : grid.rows) {
    xml.writeStartElement("row");
    xml.writeAttribute("pos", QString::number(j));
    for (auto& node : row) {
      xml.writeStartElement("node");
      xml.writeAttribute("pos", QString::number(i));
      xml.writeTextElement("type", node->cell->type());
      xml.writeStartElement("conductance");
      xml.writeTextElement("left", QString::number(row.B[i]));
      xml.writeTextElement("right", QString::number(row.B[i + 1]));
      xml.writeTextElement("top", QString::number(grid.columns[i].B[j]));
      xml.writeTextElement("bottom", QString::number(grid.columns[i].B[j + 1]));
      xml.writeEndElement();
      xml.writeEndElement();
      i++;
    }
    xml.writeEndElement();
    i = 0;
    j++;
  }

  xml.writeEndElement();
  return true;
}
bool GridCell::writeGridfile(string fileName) {
  if (fileName == "") {
    fileName = this->gridfileName;
  }
  QFile ofile(fileName.c_str());

  if (!ofile.open(QIODevice::Append)) {
    Logger::getInstance()->write<std::runtime_error>(
        "GridCell: Error opening {}", fileName);
    return false;
  }

  QXmlStreamWriter xml(&ofile);
  xml.setAutoFormatting(true);
  if (ofile.exists()) {
    xml.writeStartDocument();
  }
  bool success = this->writeGridfile(xml);
  xml.writeEndElement();
  return success;
}
bool GridCell::readGridfile(QXmlStreamReader& xml) {
  xml.readNext();
  while (!xml.atEnd() && xml.name() != "grid") {
    xml.readNext();
  }
  return this->handleGrid(xml);
}
bool GridCell::handleGrid(QXmlStreamReader& xml) {
  if (xml.atEnd()) return false;
  bool success = true;
  list<CellInfo> cells;

  grid.addRows(xml.attributes().value("rows").toInt());
  grid.addColumns(xml.attributes().value("columns").toInt());
  grid.np = xml.attributes().value("np").toDouble();
  grid.dx = xml.attributes().value("dx").toDouble();
  grid.dy = xml.attributes().value("dy").toDouble();

  CellInfo info;

  while (xml.readNextStartElement() && xml.name() == "row") {
    success = this->handleRow(xml, cells, info);
  }

  grid.setCellTypes(cells);
  return success;
}
bool GridCell::handleRow(QXmlStreamReader& xml, list<CellInfo>& cells,
                         CellInfo& info) {
  if (xml.atEnd()) return false;
  bool success = true;
  info.row = xml.attributes().value("pos").toInt();
  while (xml.readNextStartElement() && xml.name() == "node") {
    success &= this->handleNode(xml, cells, info);
  }
  return success;
}
bool GridCell::handleNode(QXmlStreamReader& xml, list<CellInfo>& cells,
                          CellInfo& info) {
  if (xml.atEnd()) return false;
  auto cellMap = CellUtils::cellMap;
  auto inexcitable = InexcitableCell().type();
  cellMap[inexcitable] = []() { return make_shared<InexcitableCell>(); };
  map<QString, function<bool(QXmlStreamReader & xml)>> handlers;
  handlers["type"] = [&info, cellMap, inexcitable](QXmlStreamReader& xml) {
    bool success = true;
    string cell_type;
    try {
      xml.readNext();
      cell_type = xml.text().toString().toStdString();
      info.cell = cellMap.at(cell_type)();
    } catch (const std::out_of_range&) {
      success = false;
      Logger::getInstance()->write("{} not a valid cell type", cell_type);
      info.cell = cellMap.at(inexcitable)();
    }
    xml.skipCurrentElement();
    return success;
  };
  handlers["conductance"] = [&info, this](QXmlStreamReader& xml) {
    while (xml.readNextStartElement()) {
      if (xml.name() == "top") {
        xml.readNext();
        info.c[CellUtils::top] = xml.text().toDouble();
        xml.skipCurrentElement();
      } else if (xml.name() == "bottom") {
        xml.readNext();
        info.c[CellUtils::bottom] = xml.text().toDouble();
        xml.skipCurrentElement();
      } else if (xml.name() == "right") {
        xml.readNext();
        info.c[CellUtils::right] = xml.text().toDouble();
        xml.skipCurrentElement();
      } else if (xml.name() == "left") {
        xml.readNext();
        info.c[CellUtils::left] = xml.text().toDouble();
        xml.skipCurrentElement();
      }
    }
    return true;
  };

  bool success = true;
  info.col = xml.attributes().value("pos").toInt();
  while (xml.readNextStartElement()) {
    try {
      success &= handlers.at(xml.name().toString())(xml);
    } catch (const std::out_of_range&) {
      Logger::getInstance()->write("GridCell: xml type {} not recognized",
                                   qUtf8Printable(xml.name().toString()));
    }
  }
  cells.push_back(info);
  return success;
}
bool GridCell::readGridfile(string filename) {
  QFile ifile(filename.c_str());
  if (!ifile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    Logger::getInstance()->write<std::runtime_error>(
        "GridCell: Error opening {}", filename);
    return false;
  }
  QXmlStreamReader xml(&ifile);
  return this->readGridfile(xml);
}
bool GridCell::readCellState(string file) {
  QFile ifile(file.c_str());

  if (!ifile.open(QIODevice::ReadOnly)) {
    Logger::getInstance()->write<std::runtime_error>("Error opening {}", file);
    return false;
  }
  QXmlStreamReader xml(&ifile);
  if (!CellUtils::readNext(xml, "grid")) return false;
  Cell::readCellState(xml);
  while (!xml.atEnd() && xml.readNextStartElement()) {
    int row = xml.attributes().value("row").toInt();
    int col = xml.attributes().value("col").toInt();
    shared_ptr<Node> n = this->grid(row, col);
    if (!n) {
      Logger::getInstance()->write("Warning: ({},{}) not a cell in grid", col,
                                   row);
      xml.skipCurrentElement();
      continue;
    }
    n->cell->readCellState(xml);
    xml.readNext();
  }
  ifile.close();
  return true;
}
bool GridCell::writeCellState(string file) {
  QFile ofile(file.c_str());
  string name;

  if (!ofile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    Logger::getInstance()->write<std::runtime_error>("Error opening {}", file);
    return false;
  }
  QXmlStreamWriter xml(&ofile);
  xml.setAutoFormatting(true);
  xml.writeStartDocument();
  xml.writeStartElement("grid");
  bool success = true;
  success &= Cell::writeCellState(xml);
  int rn = 0;
  int cn = 0;
  for (auto& row : grid.rows) {
    for (auto& node : row) {
      xml.writeStartElement("pos");
      xml.writeAttribute("row", QString::number(rn));
      xml.writeAttribute("col", QString::number(cn));
      success &= node->cell->writeCellState(xml);
      xml.writeEndElement();
      cn++;
    }
    rn++;
    cn = 0;
  }
  xml.writeEndElement();
  xml.writeEndDocument();
  ofile.close();
  return success;
}

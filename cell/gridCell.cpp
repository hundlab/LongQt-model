// 5/10/2016
//################################################
//cell type for a grid, used by grid protocol to
// run a simulaiton on a grid of cells
// class definitions
//################################################

#include <iostream>
#include <sstream>
#include <set>
#include <functional>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>

#include "gridCell.h"
#include "cellutils.h"
#include "inexcitablecell.h"

GridCell::GridCell() {
    this->Initialize();
}
void GridCell::Initialize() {
    dx = 0.01;
    dy = 0.01;
    np = 1;
    gridfileName = "grid.txt";
    tcount = 0;
    makeMap();
}
GridCell::GridCell(GridCell& toCopy) : Cell(toCopy),grid(toCopy.grid) {
    this->Initialize();
    this->gridfileName = toCopy.gridfileName;
}
GridCell *GridCell::clone() {
    return new GridCell(*this);
}
GridCell::~GridCell() {}
Grid* GridCell::getGrid() {
    return &grid;
}
bool GridCell::setOutputfileConstants(string filename) {
    int rc = 0;
    bool toReturn = true;
    for(auto& row : grid.rows) {
        int cc = 0;
        for(auto& column : row) {
            toReturn &= column->cell->setOutputfileConstants(CellUtils::strprintf(filename.c_str(),rc,cc));
            cc++;
        }
        rc++;
    }
    return toReturn;
}
bool GridCell::setOuputfileVariables(string filename) {
    int rc = 0;
    bool toReturn = true;
    for(auto& row : grid.rows) {
        int cc = 0;
        for(auto& column : row) {
            toReturn &= column->cell->setOuputfileVariables(CellUtils::strprintf(filename.c_str(),rc,cc));
            ++cc;
        }
        ++rc;
    }
    return toReturn;
}
set<string> GridCell::getVariables() {
    set<string> toReturn = Cell::getVariables();
    for(auto& row : grid.rows) {
        for(auto& column : row) {
            set<string> ivSet = column->cell->getVariables();
            toReturn.insert(ivSet.begin(), ivSet.end());
        }
    }
    return toReturn;
}
set<string> GridCell::getConstants() {
    set<string> toReturn = Cell::getConstants();
    for(auto& row : grid.rows) {
        for(auto& column : row) {
            set<string> ivSet = column->cell->getConstants();
            toReturn.insert(ivSet.begin(), ivSet.end());
        }
    }
    return toReturn;
}

bool GridCell::setConstantSelection(set<string> new_selection) {
    bool toReturn = true;
    parsSelection = new_selection;
    for(auto& it :grid.rows) {
        for(auto& iv : it.nodes) {
            toReturn &=iv->cell->setConstantSelection(new_selection);
        }
    }
    return toReturn;
}
bool GridCell::setVariableSelection(set<string> new_selection) {
    bool toReturn = true;
    varsSelection = new_selection;
    for(auto& it : grid.rows) {
        for(auto& iv : it) {
            toReturn &= iv->cell->setVariableSelection(new_selection);
        }
    }
    return toReturn;
}
void GridCell::writeConstants() {
    for(auto& it : grid.rows) {
        for(auto& iv : it) {
            iv->cell->writeConstants();
        }
    }
}
void GridCell::writeVariables() {
    for(auto& it : grid.rows) {
        for(auto& iv : it) {
            iv->cell->writeVariables();
        }
    }
}
void GridCell::closeFiles() {
    for(auto& it : grid.rows) {
        for(auto& iv : it) {
            iv->cell->closeFiles();
        }
    }
}
double GridCell::updateV() {
    if(tcount==0&&grid.columnCount()>1){
        for(auto& row: grid.rows){
            row.updateVm(dt);
        }
    } else {
        for(auto& column: grid.columns){
            column.updateVm(dt);
        }
    }
    return 0.0;
}
void GridCell::updateConc() {
    for(auto& row: grid.rows) {
        for(auto& node: row) {
            node->cell->updateConc();
        }
    }
}
void GridCell::updateCurr() {
    for(auto& row: grid.rows) {
        for(auto& node: row) {
            node->cell->updateCurr();
        }
    }
}
int GridCell::externalStim(double stimval) {
    for(auto& row: grid.rows) {
        for(auto& node: row) {
            node->cell->externalStim(stimval);
        }
    }
    return 1;
}
double GridCell::tstep(double stimt)
{
    unsigned int i,j;
    int vmflag=0;

    tcount = ((++tcount)%2);
    if(tcount==1&&(grid.rowCount()==1||grid.columnCount()==1)) {
        tcount = 0;
    }
    t=t+dt;
    for(i=0;i<grid.rows.size();i++){
        for(j=0;j<grid.columns.size();j++){
            grid.rows[i][j]->cell->t=t;
            grid.rows[i][j]->cell->dt = dt;
            if(tcount==0){  // used to prevent time step change in middle of ADI
                if(grid.rows[i][j]->cell->dVdt>grid.rows[i][j]->cell->dvcut||(t>(stimt-2.0)&&t<stimt+10)||(apTime<5.0))
                    vmflag = 2;
                else if((apTime<40)&&vmflag!=2)
                    vmflag = 1;
            }
        }
    }
    if(tcount==0){
        if(vmflag==2)
            dt = dtmin;
        else if(vmflag==1)
            dt = dtmed;
        else  
            dt = dtmax;
    }

    return t;
}
void GridCell::makeMap() {//only aply to cells added after the change?
    pars["dx"] = &dx;
    pars["dy"] = &dy;
    pars["np"] = &np;
}

const char *GridCell::type() const
{
    return "gridCell";
}
void GridCell::setGridfile(string name) {
    gridfileName = name;
}
string GridCell::gridfile() {
    return gridfileName;
}
bool GridCell::writeGridfile(QXmlStreamWriter& xml) {
    int i = 0;
    int j = 0;
    xml.writeStartElement("grid");
    xml.writeAttribute("rows", QString::number(grid.rowCount()));
    xml.writeAttribute("columns", QString::number(grid.columnCount()));
    xml.writeAttribute("np", QString::number(np));
    xml.writeAttribute("dx", QString::number(dx));
    xml.writeAttribute("dy", QString::number(dy));

    for(auto& row : grid.rows) {
        xml.writeStartElement("row");
        xml.writeAttribute("pos",QString::number(j));
        for(auto& node : row) {
            xml.writeStartElement("node");
            xml.writeAttribute("pos",QString::number(i));
            xml.writeTextElement("type",node->cell->type());
            xml.writeStartElement("conductance");
            xml.writeTextElement("left", QString::number(row.B[i]));
            xml.writeTextElement("right", QString::number(row.B[i+1]));
            xml.writeTextElement("top", QString::number(grid.columns[i].B[j]));
            xml.writeTextElement("bottom", QString::number(grid.columns[i].B[j+1]));
            xml.writeEndElement();
            xml.writeEndElement();
            i++;
        }
        xml.writeEndElement();
        i=0;
        j++;
    }

    xml.writeEndElement();
    return true;
}
bool GridCell::writeGridfile(string fileName) {
    if(fileName == "") {
        fileName = this->gridfileName;
    }
    QFile ofile(fileName.c_str());

    if(!ofile.open(QIODevice::Append)){
        qCritical() << "Error opening " << fileName.c_str();
        return false;
    }

    QXmlStreamWriter xml(&ofile);
    xml.setAutoFormatting(true);
    if(ofile.exists()){
        xml.writeStartDocument();
    }
    bool success = this->writeGridfile(xml);
    xml.writeEndElement();
    return success;
}
bool GridCell::readGridfile(QXmlStreamReader& xml) {
    xml.readNext();
    while(!xml.atEnd() && xml.name() != "grid") {
        xml.readNext();
    }
    return this->handleGrid(xml);
}
bool GridCell::handleGrid(QXmlStreamReader& xml) {
    if(xml.atEnd()) return false;
    bool success = true;
    list<CellInfo> cells;

    grid.addRows(xml.attributes().value("rows").toInt());
    grid.addColumns(xml.attributes().value("columns").toInt());
    this->np =  xml.attributes().value("np").toDouble();
    this->dx =  xml.attributes().value("dx").toDouble();
    this->dy =  xml.attributes().value("dy").toDouble();

    CellInfo info;
    info.dx = dx;
    info.dy = dy;
    info.np = np;

    while(xml.readNextStartElement() && xml.name()=="row"){
        success = this->handleRow(xml, cells, info);
    }

    grid.setCellTypes(cells);
    return success;
}
bool GridCell::handleRow(QXmlStreamReader& xml, list<CellInfo>& cells, CellInfo& info) {
    if(xml.atEnd()) return false;
    bool success = true;
    info.row = xml.attributes().value("pos").toInt();
    while(xml.readNextStartElement() && xml.name() == "node") {
        success &= this->handleNode(xml, cells, info);
    }
    return success;
}
bool GridCell::handleNode(QXmlStreamReader& xml, list<CellInfo>& cells, CellInfo& info) {
    if(xml.atEnd()) return false;
    auto cellMap = CellUtils::cellMap;
    auto inexcitable = InexcitableCell().type();
    cellMap[inexcitable] = [] () {return make_shared<InexcitableCell>();};
    map<QString,function<bool(QXmlStreamReader& xml)>> handlers; 
    handlers["type"] = [&info,cellMap,inexcitable] (QXmlStreamReader& xml) {
        bool success = true;
        string cell_type;
        try {
            xml.readNext();
            cell_type = xml.text().toString().toStdString();
            info.cell = cellMap.at(cell_type)();
        } catch(const std::out_of_range&) {
            success = false;
            qWarning("%s not a valid cell type", cell_type.c_str());
            info.cell = cellMap.at(inexcitable)();
        }
        xml.skipCurrentElement();
        return success;
    };
    handlers["conductance"] = [&info,this] (QXmlStreamReader& xml) {
        while(xml.readNextStartElement()) {
            if(xml.name() == "top") {
                xml.readNext();
                info.c[CellUtils::top] = xml.text().toDouble();
                xml.skipCurrentElement();
            } else if(xml.name() == "bottom") {
                xml.readNext();
                info.c[CellUtils::bottom] = xml.text().toDouble();
                xml.skipCurrentElement();
            } else if(xml.name() == "right") {
                xml.readNext();
                info.c[CellUtils::right] = xml.text().toDouble();
                xml.skipCurrentElement();
            } else if(xml.name() == "left") {
                xml.readNext();
                info.c[CellUtils::left] = xml.text().toDouble();
                xml.skipCurrentElement();
            }
        }
        return true;
    };

    bool success = true;
    info.col = xml.attributes().value("pos").toInt();
    while(xml.readNextStartElement()) {
        try {
            success &= handlers.at(xml.name().toString())(xml);
        } catch(const std::out_of_range&) {
            qWarning("GridCell: xml type %s not recognized",qUtf8Printable(xml.name().toString()));
        }
    }
    cells.push_back(info);
    return success;
}
bool GridCell::readGridfile(string filename) {
    QFile ifile(filename.c_str());
    if(!ifile.open(QIODevice::ReadOnly|QIODevice::Text)){
        qCritical() << "Error opening " << filename.c_str();
        return false;
    }
    QXmlStreamReader xml(&ifile);
    return this->readGridfile(xml);
}
bool GridCell::readCellState(string file) {
    QFile ifile(file.c_str());

    if(!ifile.open(QIODevice::ReadOnly)){
        qCritical() << "Error opening " << file.c_str();
        return false;
    }
    QXmlStreamReader xml(&ifile);
    if(!CellUtils::readNext(xml, "grid")) return false;
    Cell::readCellState(xml);
    while(!xml.atEnd() && xml.readNextStartElement()){
        int row = xml.attributes().value("row").toInt();
        int col = xml.attributes().value("col").toInt();
        shared_ptr<Node> n = this->grid(row,col);
        if(!n) {
            qWarning("Warning: (%i,%i) not a cell in grid",col,row);	
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

    if(!ofile.open(QIODevice::WriteOnly|QIODevice::Text)){
        qCritical() << "Error opening " << file.c_str();
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
    for(auto& row : grid.rows) {
        for(auto& node : row) {
            xml.writeStartElement("pos");
            xml.writeAttribute("row",QString::number(rn));
            xml.writeAttribute("col",QString::number(cn));
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

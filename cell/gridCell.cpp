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
#include "cellUtils.h"

GridCell::GridCell() {
	this->Initialize();
}
void GridCell::Initialize() {
	dx = 0.01;
	dy = 0.01;
	np = 1;
	type = "gridCell";
	gridfileName = "grid.txt";
	tcount = 0;
	makeMap();
}
GridCell::GridCell(GridCell& toCopy) : Cell(toCopy) {
	this->Initialize(); 
	this->gridfileName = toCopy.gridfileName;
	this->grid = Grid(toCopy.grid);
}
GridCell* GridCell::clone() {
	return new GridCell(*this);
}
GridCell::~GridCell() {}
Grid* GridCell::getGrid() {
	return &grid;
}
bool GridCell::setOutputfileConstants(string filename) {
	int i = 0;
	bool toReturn = true;
	for(auto it = grid.fiber.begin(); it != grid.fiber.end(); it++, i++) {
		int j = 0;
		for(auto iv = it->nodes.begin(); iv != it->nodes.end(); iv++, j++) {
			sprintf(buffer,filename.c_str(),j,i);
			toReturn &= (*iv)->cell->setOutputfileConstants(string(buffer));
		}
	}
	return toReturn;
}
bool GridCell::setOuputfileVariables(string filename) {
	int i = 0;
	bool toReturn = true;
	for(auto it = grid.fiber.begin(); it != grid.fiber.end(); it++, i++) {
		int j = 0;
		for(auto iv = it->nodes.begin(); iv != it->nodes.end(); iv++, j++) {
			sprintf(buffer,filename.c_str(),j,i);
			toReturn &= (*iv)->cell->setOuputfileVariables(string(buffer));
		}
	}
	return toReturn;
}
set<string> GridCell::getVariables() {
	set<string> toReturn = Cell::getVariables();
	for(auto it = grid.fiber.begin(); it != grid.fiber.end(); it++) {
		for(auto iv = it->nodes.begin(); iv != it->nodes.end(); iv++) {
			set<string> ivSet = (*iv)->cell->getVariables();
			toReturn.insert(ivSet.begin(), ivSet.end());
		}
	}
	return toReturn;
}
set<string> GridCell::getConstants() {
	set<string> toReturn = Cell::getConstants();
	for(auto it = grid.fiber.begin(); it != grid.fiber.end(); it++) {
		for(auto iv = it->nodes.begin(); iv != it->nodes.end(); iv++) {
			set<string> ivSet = (*iv)->cell->getConstants();
			toReturn.insert(ivSet.begin(), ivSet.end());
		}
	}
	return toReturn;
}

bool GridCell::setConstantSelection(set<string> new_selection) {
	bool toReturn = true;
	parsSelection = new_selection;
	for(auto it = grid.fiber.begin(); it != grid.fiber.end(); it++) {
		for(auto iv = it->nodes.begin(); iv != it->nodes.end(); iv++) {
			toReturn &=(*iv)->cell->setConstantSelection(new_selection);
		}
	}
	return toReturn;
}
bool GridCell::setVariableSelection(set<string> new_selection) {
	bool toReturn = true;
	varsSelection = new_selection;
	for(auto it = grid.fiber.begin(); it != grid.fiber.end(); it++) {
		for(auto iv = it->nodes.begin(); iv != it->nodes.end(); iv++) {
			toReturn &= (*iv)->cell->setVariableSelection(new_selection);
		}
	}
	return toReturn;
}
void GridCell::writeConstants() {
	for(auto it : grid.fiber) {
		for(auto iv : it.nodes) {
			iv->cell->writeConstants();
		}
	}
}
void GridCell::writeVariables() {
	for(auto it : grid.fiber) {
		for(auto iv : it.nodes) {
			iv->cell->writeVariables();
		}
	}
}
void GridCell::closeFiles() {
	for(auto& it : grid.fiber) {
		for(auto& iv : it.nodes) {
			iv->cell->closeFiles();
		}
	}   
}
double GridCell::updateV() {
	int i,j;
	int xLen = static_cast<int>(grid.fibery.size());
	int yLen = static_cast<int>(grid.fiber.size());
	if((tcount%2==0)){
		for(i=0;i<yLen;i++){
			grid.fiber[i].updateVm(dt);
		}
	} else {
		for(i=0;i<xLen;i++){
			grid.fibery[i].updateVm(dt);
		}
	}
	return 0.0;
}
void GridCell::updateConc() {
	for(auto it = grid.fiber.begin(); it != grid.fiber.end(); it++) {
		for(auto iv = it->nodes.begin(); iv != it->nodes.end(); iv++) {
			(*iv)->cell->updateConc();
		}
	}
}
void GridCell::updateCurr() {
	for(auto it = grid.fiber.begin(); it != grid.fiber.end(); it++) {
		for(auto iv = it->nodes.begin(); iv != it->nodes.end(); iv++) {
			(*iv)->cell->updateCurr();
		}
	}
}
int GridCell::externalStim(double stimval) {
	for(auto it = grid.fiber.begin(); it != grid.fiber.end(); it++) {
		for(auto iv = it->nodes.begin(); iv != it->nodes.end(); iv++) {
			(*iv)->cell->externalStim(stimval);
		}
	}
	return 1;
}
double GridCell::tstep(double stimt)
{
	unsigned int i,j;
	int vmflag=0;

	tcount++;
	t=t+dt;
	for(i=0;i<grid.fiber.size();i++){
		for(j=0;j<grid.fibery.size();j++){
			grid.fiber[i].nodes[j]->cell->t=t;
			grid.fiber[i].nodes[j]->cell->dt = dt;
			if(tcount%2==0){  // used to prevent time step change in middle of ADI	
				if(grid.fiber[i].nodes[j]->cell->dVdt>grid.fiber[i].nodes[j]->cell->dvcut||(t>(stimt-2.0)&&t<stimt+10)||(apTime<5.0))
					vmflag = 2;
				else if((apTime<40)&&vmflag!=2)
					vmflag = 1;
			}
		}
	}
	if(tcount%2==0){
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

	for(auto& it : grid.fiber) {
		xml.writeStartElement("row");
		xml.writeAttribute("pos",QString::number(j));
		for(auto& iv : it.nodes) {
			xml.writeStartElement("node");
			xml.writeAttribute("pos",QString::number(i));
			xml.writeTextElement("type",iv->cell->type);
			xml.writeStartElement("conductance");
			xml.writeTextElement("left", QString::number(it.B[i]));
			xml.writeTextElement("right", QString::number(it.B[i+1]));
			xml.writeTextElement("top", QString::number(grid.fibery[i].B[j]));
			xml.writeTextElement("bottom", QString::number(grid.fibery[i].B[j+1]));
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
		cout << "Error opening " << fileName << endl;
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
	while(!xml.atEnd() && xml.name() != "file") {
		xml.readNext();
	}
	xml.readNext();
	while(!xml.atEnd() && xml.name() != "grid") {
		xml.readNext();
	}
	return this->handleGrid(xml);
}
bool GridCell::handleGrid(QXmlStreamReader& xml) {
	if(xml.atEnd()) return false;
	bool success = true;
	set<CellInfo*> cells;

	grid.addRows(xml.attributes().value("rows").toInt());
	grid.addColumns(xml.attributes().value("columns").toInt());
	this->np =  xml.attributes().value("np").toDouble();
	this->dx =  xml.attributes().value("dx").toDouble();
	this->dy =  xml.attributes().value("dy").toDouble();

	CellInfo* info = new CellInfo;
	info->dx = dx;
	info->dy = dy;
	info->np = np;

	while(xml.readNextStartElement() && xml.name()=="row"){
		success = this->handleRow(xml, cells, info);
	}

	grid.setCellTypes(cells);
	return success;
}
bool GridCell::handleRow(QXmlStreamReader& xml, set<CellInfo*>& cells, CellInfo* info) {
	if(xml.atEnd()) return false;
	bool success = true;
	info->X = xml.attributes().value("pos").toInt();
	while(xml.readNextStartElement() && xml.name() == "node") {
		success &= this->handleNode(xml, cells, info);
	}
	return success;
}
bool GridCell::handleNode(QXmlStreamReader& xml, set<CellInfo*>& cells, CellInfo* info) {
	if(xml.atEnd()) return false;
    auto cellMap = CellUtils::cellMap;
	cellMap[Cell().type] = [] () {return (Cell*) new Cell;};
	map<QString,function<bool(QXmlStreamReader& xml)>> handlers; 
	handlers["type"] = [&info,cellMap] (QXmlStreamReader& xml) {
		bool success = true;
		string cell_type;
		try {
			xml.readNext();
			cell_type = xml.text().toString().toStdString();
			info->cell = cellMap.at(cell_type)();
		} catch(const std::out_of_range&) {
			success = false;
			info->cell = new Cell();
		}
		xml.skipCurrentElement();
		return success;
	};
	handlers["conductance"] = [&info,this] (QXmlStreamReader& xml) {
		while(xml.readNextStartElement()) {
			if(xml.name() == "top") {
				xml.readNext();
				info->c[CellUtils::top] = xml.text().toDouble();
				xml.skipCurrentElement();
			} else if(xml.name() == "bottom") {
				xml.readNext();
				info->c[CellUtils::bottom] = xml.text().toDouble();
				xml.skipCurrentElement();
			} else if(xml.name() == "right") {
				xml.readNext();
				info->c[CellUtils::right] = xml.text().toDouble();
				xml.skipCurrentElement();
			} else if(xml.name() == "left") {
				xml.readNext();
				info->c[CellUtils::left] = xml.text().toDouble();
				xml.skipCurrentElement();
			}
		}
		return true;
	};

	bool success = true;
	info = new CellInfo(*info);
	info->Y = xml.attributes().value("pos").toInt();
	while(xml.readNextStartElement()) {
		try {
			success &= handlers.at(xml.name().toString())(xml);
		} catch(const std::out_of_range&) {}
	}
	cells.insert(info);
	return success;
}
bool GridCell::readGridfile(string filename) {
	QFile ifile(filename.c_str());
	if(!ifile.open(QIODevice::ReadOnly|QIODevice::Text)){
		cout << "Error opening " << filename << endl;
		return false;
	}
	QXmlStreamReader xml(&ifile);
	return this->readGridfile(xml);
}
bool GridCell:: readCellState(string file) {
	QFile ifile(file.c_str());

	if(!ifile.open(QIODevice::ReadOnly)){
		cout << "Error opening " << file << endl;
		return false;
	}
	QXmlStreamReader xml(&ifile);
	if(!CellUtils::readNext(xml, "grid")) return false;
	while(!xml.atEnd() && xml.readNextStartElement()){
		int row = xml.attributes().value("row").toInt();
		int col = xml.attributes().value("col").toInt();
		Node* n = this->grid.findNode({row,col});
		if(n == NULL) {
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
		cout << "Error opening " << file << endl;
		return false;
	}
	QXmlStreamWriter xml(&ofile);
	xml.setAutoFormatting(true);
	xml.writeStartDocument();
	xml.writeStartElement("grid");
	bool success = true;
	int row = 0;
	int col = 0;
	for(auto& fiber : grid.fiber) {
		row++;
		for(auto& node : fiber.nodes) {
			col++;
			xml.writeStartElement("pos");
			xml.writeAttribute("row",QString::number(row));
			xml.writeAttribute("col",QString::number(col));
	 		success &= node->cell->writeCellState(xml);
			xml.writeEndElement();
		}
		col = 0;
	}
	xml.writeEndElement();
	xml.writeEndDocument();
  ofile.close();
  return success;
}

// 5/10/2016
//################################################
//cell type for a grid, used by grid protocol to
// run a simulaiton on a grid of cells
// class definitions
//################################################

#include <iostream>
#include <sstream>
#include <set>

#include "gridCell.h"
#include "cellUtils.h"

gridCell::gridCell() {
    this->Initialize();
}
void gridCell::Initialize() {
     dx = 0.01;
    dy = 0.01;
    np = 1;
    type = "gridCell";
    gridfileName = "grid.txt";
    tcount = 0;
    makeMap();
}
gridCell::gridCell(gridCell& toCopy) : Cell(toCopy) {
    this->Initialize(); 
    this->gridfileName = toCopy.gridfileName;
    this->grid = Grid(toCopy.grid);
}
gridCell* gridCell::clone() {
    return new gridCell(*this);
}
gridCell::~gridCell() {}
Grid* gridCell::getGrid() {
    return &grid;
}
void gridCell::addBuffer() {
    grid.addBuffer();
}
bool gridCell::setOutputfileConstants(string filename) {
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
bool gridCell::setOuputfileVariables(string filename) {
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
set<string> gridCell::getVariables() {
    set<string> toReturn = Cell::getVariables();
    for(auto it = grid.fiber.begin(); it != grid.fiber.end(); it++) {
        for(auto iv = it->nodes.begin(); iv != it->nodes.end(); iv++) {
            set<string> ivSet = (*iv)->cell->getVariables();
            toReturn.insert(ivSet.begin(), ivSet.end());
        }
    }
    return toReturn;
}
set<string> gridCell::getConstants() {
    set<string> toReturn = Cell::getConstants();
    for(auto it = grid.fiber.begin(); it != grid.fiber.end(); it++) {
        for(auto iv = it->nodes.begin(); iv != it->nodes.end(); iv++) {
            set<string> ivSet = (*iv)->cell->getConstants();
            toReturn.insert(ivSet.begin(), ivSet.end());
        }
    }
    return toReturn;
}

bool gridCell::setConstantSelection(set<string> new_selection) {
    bool toReturn = true;
    parsSelection = new_selection;
    for(auto it = grid.fiber.begin(); it != grid.fiber.end(); it++) {
        for(auto iv = it->nodes.begin(); iv != it->nodes.end(); iv++) {
            toReturn &=(*iv)->cell->setConstantSelection(new_selection);
        }
    }
    return toReturn;
}
bool gridCell::setVariableSelection(set<string> new_selection) {
    bool toReturn = true;
    varsSelection = new_selection;
    for(auto it = grid.fiber.begin(); it != grid.fiber.end(); it++) {
        for(auto iv = it->nodes.begin(); iv != it->nodes.end(); iv++) {
            toReturn &= (*iv)->cell->setVariableSelection(new_selection);
        }
    }
    return toReturn;
}
void gridCell::writeConstants() {
    for(auto it : grid.fiber) {
        for(auto iv : it.nodes) {
            iv->cell->writeConstants();
        }
    }
}
void gridCell::writeVariables() {
    for(auto it : grid.fiber) {
        for(auto iv : it.nodes) {
            iv->cell->writeVariables();
        }
    }
}
void gridCell::closeFiles() {
     for(auto it : grid.fiber) {
        for(auto iv : it.nodes) {
            iv->cell->closeFiles();
        }
    }   
}
double gridCell::updateV() {
    int i,j;
    int xLen = static_cast<int>(grid.fibery.size());
    int yLen = static_cast<int>(grid.fiber.size());
    if((tcount%2==0)){
        for(i=0;i<yLen;i++){
            for(j=0;j<xLen&&yLen>1;j++){
                if(i>0&&i<(yLen-1))
                    grid.fiber[i].nodes[j]->cell->iTot-=grid.fibery[j].B[i]*(grid.fiber[i-1].nodes[j]->cell->vOld-grid.fiber[i].nodes[j]->cell->vOld)-grid.fibery[j].B[i+1]*(grid.fiber[i].nodes[j]->cell->vOld-grid.fiber[i+1].nodes[j]->cell->vOld);
                else if(i==0)
                    grid.fiber[i].nodes[j]->cell->iTot-=-grid.fibery[j].B[i+1]*(grid.fiber[i].nodes[j]->cell->vOld-grid.fiber[i+1].nodes[j]->cell->vOld);
                else if(i==yLen-1)
                    grid.fiber[i].nodes[j]->cell->iTot-=grid.fibery[j].B[i]*(grid.fiber[i-1].nodes[j]->cell->vOld-grid.fiber[i].nodes[j]->cell->vOld);
            }
            grid.fiber[i].updateVm(dt);
        }
    }
    else{
        for(i=0;i<xLen;i++){
            for(j=0;j<yLen&&xLen>1;j++){
                if(i>0&&i<(xLen-1))
                    grid.fibery[i].nodes[j]->cell->iTot-=grid.fiber[j].B[i]*(grid.fibery[i-1].nodes[j]->cell->vOld-grid.fibery[i].nodes[j]->cell->vOld)-grid.fiber[j].B[i+1]*(grid.fibery[i].nodes[j]->cell->vOld-grid.fibery[i+1].nodes[j]->cell->vOld);
                else if(i==0)
                    grid.fibery[i].nodes[j]->cell->iTot-=-grid.fiber[j].B[i+1]*(grid.fibery[i].nodes[j]->cell->vOld-grid.fibery[i+1].nodes[j]->cell->vOld);
                else if(i==xLen-1)
                    grid.fibery[i].nodes[j]->cell->iTot-=grid.fiber[j].B[i]*(grid.fibery[i-1].nodes[j]->cell->vOld-grid.fibery[i].nodes[j]->cell->vOld);
            }
            grid.fibery[i].updateVm(dt);
        }
    }
    return 0.0;
}
void gridCell::updateConc() {
    for(auto it = grid.fiber.begin(); it != grid.fiber.end(); it++) {
        for(auto iv = it->nodes.begin(); iv != it->nodes.end(); iv++) {
            (*iv)->cell->updateConc();
        }
    }
}
void gridCell::updateCurr() {
    for(auto it = grid.fiber.begin(); it != grid.fiber.end(); it++) {
        for(auto iv = it->nodes.begin(); iv != it->nodes.end(); iv++) {
            (*iv)->cell->updateCurr();
        }
    }
}
int gridCell::externalStim(double stimval) {
    for(auto it = grid.fiber.begin(); it != grid.fiber.end(); it++) {
        for(auto iv = it->nodes.begin(); iv != it->nodes.end(); iv++) {
            (*iv)->cell->externalStim(stimval);
        }
    }
    return 1;
}
double gridCell::tstep(double stimt)
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
void gridCell::makeMap() {//only aply to cells added after the change?
    pars["dx"] = &dx;
    pars["dy"] = &dy;
    pars["np"] = &np;
}
void gridCell::setGridfile(string name) {
    gridfileName = name;
}
string gridCell::gridfile() {
    return gridfileName;
}
bool gridCell::writeGridfile(string fileName) {
    bool succes = true;
    ofstream ofile;
    int i = 0;
    int j = 0;
    if(fileName == "") {
        fileName = this->gridfileName;
    }

    ofile.open(fileName);
    succes = ofile.good();
    if(!succes) {
        return succes;
    }
    ofile << "##BEGIN GRID\n";
    ofile << grid.rowCount() << " " << grid.columnCount() << endl;
    ofile << np << " " << dx << " " << dy << endl;
    for(auto it : grid.fiber) {
        for(auto iv : it.nodes) {
            ofile <<"\t"<<iv->cell->type<<"| "<<i<<" "<<j;
            i++;
        }
        ofile << endl;
        i=0;
        j++;
    }
    ofile << "##END GRID\n";
    return succes;    
}
bool gridCell::readGridfile(string filename) {
    bool succes = true;
    ifstream ifile;
    cellInfo* info;
    double dx, dy;
    int np;
    set<cellInfo*> cells;
    string type;
    stringstream line;
    string temp;
    
    ifile.open(filename);
    if(!(succes = ifile.good())) return succes;
    while(!ifile.eof() && line.str() != "##BEGIN GRID") {
        getline(ifile,temp);
        line.clear();
        line.str(temp);
    }
    int rows, columns;
    ifile >> rows >> columns;
    grid.addRows(rows);
    grid.addColumns(columns);
    ifile >> np >> dx >> dy;
    if(ifile.eof()) {
        return succes = false;
    }
 
    getline(ifile,temp);
    getline(ifile,temp);
    line.clear();
    line.str(temp);
    while(!ifile.eof() && line.str() != "##END GRID"){
        while(!line.eof()) {
            info = new cellInfo;
            info->dx = dx;
            info->dy = dy;
            info->np = np;
            char nextChar;
            bool firstCharFound =false;
            line.get(nextChar);
            type = "";
            while(!line.eof() && nextChar != '|') {
                if(firstCharFound||!isspace(nextChar)) {
                    type = type + nextChar;
                    firstCharFound = true;
                }
                line.get(nextChar);
            }
            line >> info->Y >> info->X;
            try {
                info->cell = cellUtils().cellMap.at(type)();
            } catch(const std::out_of_range&) {
                succes = false;
                info->cell = new Cell();
            }
            cells.insert(info);
        }
        getline(ifile,temp);
        line.clear();
        line.str(temp);
    }
    grid.setCellTypes(cells);
    return succes;
}
bool gridCell:: readCellState(string filename) {
    ifstream ifile;
    ifile.open(filename);
    if(!ifile.good()) {
        cout << "Error Opening " << filename << "\n";
        return false;
    }
    bool succes = true;
    succes &= Cell::readCellState(ifile);
    for(auto fiber : grid.fiber) {
        for(auto node : fiber.nodes) {
            succes &= node->cell->readCellState(ifile);
        }
    }
    ifile.close();
    return succes;
}
bool gridCell::writeCellState(string filename) {
    ofstream ofile;
    ofile.open(filename,ios_base::app);
    if(!ofile.good()) {
        cout << "Error Opening " << filename << "\n";
        return false;
    }
    bool succes = true;
    succes &= Cell::writeCellState(ofile);
    for(auto fiber : grid.fiber) {
        for(auto node : fiber.nodes) {
            succes &= node->cell->writeCellState(ofile);
        }
    }
    return succes;
}

// 5/10/2016
//################################################
//protocol for a grid off cells
//class definitions
//################################################

#include "gridProtocol.h"
#include "gridCell.h"

gridProtocol::gridProtocol()  : CurrentClamp(){
	gridCell* temp = new gridCell();
	cell = temp;
	grid = temp->getGrid();
	baseCellMap = cellMap;
	baseCellMap["Inexcitable Cell"] = [] () {return new Cell;};
	cellMap.clear();
	cellMap["gridCell"] = [] () {return (Cell*) new gridCell;};
	GetSetRef toInsert;
	pars["gridFile"]= toInsert.Initialize("file", [temp] () {return temp->gridfile();}, [temp] (const string& value) {temp->setGridfile(value);});
	pars["measNodes"]= toInsert.Initialize("set", [this] () {return setToString(dataNodes);}, [this] (const string& value) {dataNodes = stringToSet(value);});
	pars["stimNodes"]= toInsert.Initialize("set", [this] () {return setToString(stimNodes);}, [this] (const string& value) {stimNodes = stringToSet(value);});
	pars.erase("numtrials");
	pars["paceflag"].set("true");
	pars.erase("paceflag");
}
//overriden deep copy funtion
gridProtocol* gridProtocol::clone(){
	return new gridProtocol(*this);
};
gridProtocol::gridProtocol(const gridProtocol& toCopy) : CurrentClamp(toCopy){
	this->CCcopy(toCopy);
}
void gridProtocol::CCcopy(const gridProtocol& toCopy) {
	this->dataNodes = toCopy.dataNodes;
	this->stimNodes = toCopy.stimNodes;
	this->grid = ((gridCell*)this->cell)->getGrid();
}
// External stimulus.
int gridProtocol::stim()
{
	for(auto it : stimNodes) {
		Cell* cell = grid->findNode(it)->cell;
		if(cell->t>=stimt&&cell->t<(stimt+stimdur)){
			if(stimflag==0){
				stimcounter++;
				stimflag=1;
				if(stimcounter>int(numstims)){
					doneflag = 0;
					return 0;
				}
			}
			cell->externalStim(stimval);
		}
		else if(stimflag==1){     //trailing edge of stimulus
			stimt=stimt+bcl;
			stimflag=0;
			cell->apTime = 0.0;
		}

		cell->apTime = cell->apTime+cell->dt;

		doneflag = 1;
	}
	return 1;
};

bool gridProtocol::runTrial() {
	char writefile[150];     // Buffer for storing filenames

	((gridCell*)cell)->addBuffer();
	//to be moved to a better location
	set<string> temp;
	temp.insert(pnames.begin(),pnames.end());
	cell->setConstantSelection(temp);
	temp.clear();
	for(auto& node: this->dataNodes) {
		for(auto& meas : measures) {
			if(this->grid->findNode(node)!=NULL&&this->grid->findNode(node)->cell->vars.find(meas.first) != cell->vars.end()) {
				this->realMeasures[node].insert(pair<string,Measure>(meas.first,meas.second));
			}
		}
	}

	//should not be here

	time = cell->t = 0.0;      // reset time
	doneflag=1;     // reset doneflag

	if(readCellState) {
		sprintf(writefile,cellStateFile.c_str());
		cell->readCellState(writefile);
		this->stimt = cell->t;
		this->tMax += this->cell->t;
	}

	//###############################################################
	// Every time step, currents, concentrations, and Vm are calculated.
	//###############################################################
	int pCount = 0;
	//open i/o streams
	for(auto& it : realMeasures) {
		for(auto& iv : it.second) {
			sprintf(writefile,propertyoutfile.c_str(),trial,iv.first.c_str());
			sprintf(writefile, (datadir + "/" + "cell_%i_%i_" + string(writefile)).c_str(),  it.first.second, it.first.first, iv.first.c_str());
			iv.second.setOutputfile(writefile);
		}
	}

	sprintf(writefile,dvarsoutfile.c_str(),trial);
	sprintf(writefile, (datadir + "/" + "cell_%%i_%%i_" + string(writefile)).c_str());
	cell->setOuputfileVariables(writefile);

	while(int(doneflag)&&(time<tMax)){

		time = cell->tstep(stimt);    // Update time
		cell->updateCurr();    // Update membrane currents
		if(int(paceflag)==1) {  // Apply stimulus
			stim();
		}
		cell->updateV();

		cell->updateConc();   // Update ion concentrations

		//##### Output select variables to file  ####################
		if(int(measflag)==1&&cell->t>meastime){
			for(auto& iv : dataNodes) {
				for (auto& it : realMeasures[iv]) {
					if(it.second.measure(grid->findNode(iv)->cell->t,*grid->findNode(iv)->cell->vars[it.second.varname])&&(int(writeflag)==1)) {
						it.second.write();
						it.second.reset();
					}
				}
			}
		}
		if (int(writeflag)==1&&time>writetime&&pCount%int(writeint)==0) {
			cell->writeVariables();
		}
		pCount++;
	}

	// Output final (ss) property values for each trial
	for(auto& it : realMeasures) {
		for(auto& iv : it.second) {
			sprintf(writefile,finalpropertyoutfile.c_str(),trial,iv.second.varname.c_str());
			sprintf(writefile, (datadir + "/" + "cell_%i_%i_" + string(writefile)).c_str(),it.first.second, it.first.first);
			iv.second.setOutputfile(writefile);
			iv.second.restoreLast();
			iv.second.write();
			iv.second.reset();
		}
	}

	// Output parameter values for each trial
	sprintf(writefile,finaldvarsoutfile.c_str(),trial);
	sprintf(writefile, (datadir + "/" + "cell_%%i_%%i_" + string(writefile)).c_str());
	cell->setOutputfileConstants(writefile);
	cell->writeConstants();
	for(map<string,Measure>::iterator it = measures.begin(); it != measures.end(); it++) {
		it->second.closeFiles();
	}
	cell->closeFiles();
	if(writeCellState) {
		sprintf(writefile,(datadir + "/" + simvarfile).c_str(),trial);
		cell->writeCellState(writefile);
	}

	return true; 
}

map<string, CellInitializer>& gridProtocol::getCellMap() {
	return baseCellMap;
}
set<pair<int,int>>& gridProtocol::getStimNodes() {
	return stimNodes;
}
set<pair<int,int>>& gridProtocol::getDataNodes() {
	return dataNodes;
}
bool gridProtocol::writepars(string file) {
	bool toReturn = ((gridCell*)this->cell)->writeGridfile(file);
	toReturn &= CurrentClamp::writepars(file);
	return toReturn;
}
int gridProtocol::readpars(string file) {
	this->grid->reset();
	bool toReturn = ((gridCell*)this->cell)->readGridfile(file);
	toReturn &= (bool)CurrentClamp::readpars(file);
	return (int)toReturn;
}
string gridProtocol::setToString(set<pair<int,int>>& nodes) {
	stringstream toReturn;
	for(auto node : nodes) {
		toReturn << node.first << " " << node.second << "\t";
	}
	return toReturn.str();
}
set<pair<int,int>> gridProtocol::stringToSet(string nodesList) {
	set<pair<int,int>> toReturn;
	stringstream stream(nodesList);
	while(!stream.eof()) {
		pair<int,int> p(-1,-1);
		stream >> p.first >> p.second;
		Node* n = grid->findNode(p);
		if(n != NULL) {
			toReturn.insert(p);
		}
	}
	for(map<string,Measure>::iterator it = measures.begin(); it != measures.end(); it++) {
		it->second.closeFiles();
	}
	cell->closeFiles();

	return toReturn;
}

//bool gridProtocol::addMeasure(Measure toInsert)

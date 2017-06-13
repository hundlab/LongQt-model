// 5/10/2016
//################################################
//protocol for a grid off cells
//class definitions
//################################################

#include "gridProtocol.h"
#include "gridCell.h"
#include "cellutils.h"
#include "pvarsgrid.h"

#include <QFile>
#include <QDebug>

GridProtocol::GridProtocol() : CurrentClamp(){
	GridCell* temp = new GridCell();
	cell = temp;
	grid = temp->getGrid();
	this->pvars = new PvarsGrid(grid);
	baseCellMap = cellMap;
	baseCellMap["Inexcitable Cell"] = [] () {return new Cell;};
	cellMap.clear();
	cellMap["gridCell"] = [] () {return (Cell*) new GridCell;};
	GetSetRef toInsert;
	pars["gridFile"]= toInsert.Initialize("file", 
			[temp] () {return temp->gridfile();}, 
			[temp] (const string& value) {temp->setGridfile(value);});
	pars["measNodes"]= toInsert.Initialize("set", 
			[this] () {return setToString(dataNodes);}, 
			[this] (const string& value) {dataNodes = stringToSet(value);});
	pars["stimNodes"]= toInsert.Initialize("set", 
			[this] () {return setToString(stimNodes);}, 
			[this] (const string& value) {stimNodes = stringToSet(value);});
	pars["secondStim"]= toInsert.Initialize("bool", 
			[this] () {return CellUtils::to_string(this->stim2);},
			[this] (const string& value) {this->setStim2(CellUtils::stob(value));});
	pars.erase("numtrials");
	pars["paceflag"].set("true");
	pars.erase("paceflag");
	type = "Grid Protocol";
	stimval2 = stimval;
	stimdur2 = stimdur;
	bcl2 = bcl;
	stimt2 = stimt;

	CellUtils::set_default_vals(this);
}
//overriden deep copy funtion
GridProtocol* GridProtocol::clone(){
	return new GridProtocol(*this);
};
GridProtocol::GridProtocol(const GridProtocol& toCopy) : CurrentClamp(toCopy){
	this->CCcopy(toCopy);
}
void GridProtocol::CCcopy(const GridProtocol& toCopy) {
	this->dataNodes = toCopy.dataNodes;
	this->stimNodes = toCopy.stimNodes;
	this->grid = ((GridCell*)this->cell)->getGrid();
}
// External stimulus.
int GridProtocol::stim()
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

void GridProtocol::setupTrial() {
	set<string> temp;
	for(auto& pvar: *pvars) {
		temp.insert(pvar.first);
	}
	cell->setConstantSelection(temp);
	temp.clear();
	for(auto& node: this->dataNodes) {
		for(auto& meas : measures) {
			if(this->grid->findNode(node)!=NULL&&this->grid->findNode(node)->cell->vars.find(meas.first) != cell->vars.end()) {
				this->realMeasures[node].insert(pair<string,Measure>(meas.first,meas.second));
			}
		}
	}
	time = cell->t = 0.0;      // reset time

	this->readInCellState(this->readCellState);

	this->pvars->setIonChanParams();
	doneflag=1;     // reset doneflag
}

bool GridProtocol::runTrial() {
	char writefile[150];     // Buffer for storing filenames

	this->setupTrial();
	
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
		if(stim2 && stimt2 > cell->t) {
			this->swapStims();
		}
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
	this->writeOutCellState(this->writeCellState);
	if(stim2 && stimt2 > cell->t) {
		this->swapStims();
	}

	return true; 
}

map<string, CellUtils::CellInitializer>& GridProtocol::getCellMap() {
	return baseCellMap;
}
set<pair<int,int>>& GridProtocol::getStimNodes() {
	return stimNodes;
}
set<pair<int,int>>& GridProtocol::getDataNodes() {
	return dataNodes;
}
bool GridProtocol::writepars(QXmlStreamWriter& xml) {
	bool toReturn;
	toReturn = ((GridCell*)this->cell)->writeGridfile(xml);
	toReturn &= CurrentClamp::writepars(xml);
	return toReturn;
}
int GridProtocol::readpars(string file) {
	QFile ifile(file.c_str());
	if(!ifile.open(QIODevice::ReadOnly|QIODevice::Text)){
        qCritical() << "Error opening " << file.c_str();
		return false;
	}
	QXmlStreamReader xml(&ifile);
	bool toReturn = this->readpars(xml);
	return (int)toReturn;
}
int GridProtocol::readpars(QXmlStreamReader& xml) {
	this->grid->reset();
	bool toReturn = ((GridCell*)this->cell)->readGridfile(xml);
	toReturn &= (bool)CurrentClamp::readpars(xml);
	return toReturn;
}
string GridProtocol::setToString(set<pair<int,int>>& nodes) {
	stringstream toReturn;
	for(auto node : nodes) {
		toReturn << node.first << " " << node.second << "\t";
	}
	return toReturn.str();
}
set<pair<int,int>> GridProtocol::stringToSet(string nodesList) {
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
void GridProtocol::swapStims() {
	double temp;
	temp = stimval;
	stimval = stimval2;
	stimval2 = temp;

	temp = stimdur;
	stimdur = stimdur2;
	stimdur2 = temp;

	temp = bcl;
	bcl = bcl2;
	bcl2 = temp;

	temp = stimt;
	stimt = stimt2;
	stimt2 = temp;

	set<pair<int,int>> temp2 = stimNodes;
	stimNodes = stimNodes2;
	stimNodes2 = temp2;
}

void GridProtocol::setStim2(bool enable) {
	if(enable == stim2) {
		return;
	}
	stim2 = !stim2;
	if(stim2) {
		GetSetRef toInsert;
		pars["stim2"] = toInsert.Initialize("double", 
				[this] () {return std::to_string(this->stim2);}, 
				[this] (const string& value) {this->stim2 = stod(value);});
		pars["bcl2"] = toInsert.Initialize("double", 
				[this] () {return std::to_string(this->stim2);}, 
				[this] (const string& value) {this->stim2 = stod(value);});
		pars["stimdur2"] = toInsert.Initialize("double", 
				[this] () {return std::to_string(this->stim2);}, 
                [this] (const string& value) {this->stim2 = stod(value);});
		pars["stimval2"] = toInsert.Initialize("double", 
				[this] () {return std::to_string(this->stim2);}, 
				[this] (const string& value) {this->stim2 = stod(value);});
		pars["stimNodes2"]= toInsert.Initialize("set", 
				[this] () {return setToString(stimNodes2);}, 
				[this] (const string& value) {stimNodes2 = stringToSet(value);});
	} else {
		pars.erase("stim2");
		pars.erase("bcl2");
		pars.erase("stimdur2");
		pars.erase("stimval2");
		pars.erase("stimNodes2");
	}
}

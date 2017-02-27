// 5/10/2016
//################################################
//protocol for a grid off cells
//class definitions
//################################################

#include "gridProtocol.h"
#include "gridCell.h"
#include <QFile>

GridProtocol::GridProtocol() : CurrentClamp(){
	GridCell* temp = new GridCell();
	cell = temp;
	grid = temp->getGrid();
	baseCellMap = cellMap;
	baseCellMap["Inexcitable Cell"] = [] () {return new Cell;};
	cellMap.clear();
	cellMap["gridCell"] = [] () {return (Cell*) new GridCell;};
	GetSetRef toInsert;
	pars["gridFile"]= toInsert.Initialize("file", [temp] () {return temp->gridfile();}, [temp] (const string& value) {temp->setGridfile(value);});
	pars["measNodes"]= toInsert.Initialize("set", [this] () {return setToString(dataNodes);}, [this] (const string& value) {dataNodes = stringToSet(value);});
	pars["stimNodes"]= toInsert.Initialize("set", [this] () {return setToString(stimNodes);}, [this] (const string& value) {stimNodes = stringToSet(value);});
	pars.erase("numtrials");
	pars["paceflag"].set("true");
	pars.erase("paceflag");
	type = "Grid Protocol";
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

bool GridProtocol::runTrial() {
	char writefile[150];     // Buffer for storing filenames

	((GridCell*)cell)->addBuffer();
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

map<string, CellUtils::CellInitializer>& GridProtocol::getCellMap() {
	return baseCellMap;
}
set<pair<int,int>>& GridProtocol::getStimNodes() {
	return stimNodes;
}
set<pair<int,int>>& GridProtocol::getDataNodes() {
	return dataNodes;
}
bool GridProtocol::writepars(string file) {
	QFile ofile(file.c_str());
	string name;
	bool toReturn;

	bool exists = ofile.exists();
	if(!ofile.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate)){
		cout << "Error opening " << file << endl;
		return 1;
	}
	QXmlStreamWriter xml(&ofile);
	xml.setAutoFormatting(true);
	xml.writeStartDocument();
	xml.writeStartElement("file");

	toReturn = ((GridCell*)this->cell)->writeGridfile(xml);
	toReturn &= CurrentClamp::writepars(xml);
	this->writePvars(xml);
	xml.writeEndElement();

	return toReturn;
}
int GridProtocol::readpars(string file, set<string> varnames) {
	QFile ifile(file.c_str());
	if(!ifile.open(QIODevice::ReadOnly|QIODevice::Text)){
		cout << "Error opening " << file << endl;
		return false;
	}
	QXmlStreamReader xml(&ifile);
	this->grid->reset();
	bool toReturn = ((GridCell*)this->cell)->readGridfile(xml);
	toReturn &= (bool)CurrentClamp::readpars(xml);
	this->readPvars(xml);
	return (int)toReturn;
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
void GridProtocol::setIonChanParams() {
	Cell* cell = 0;
	for(auto& pvar : this->new_pvars) {
		for(auto& oneCell : pvar.second.cells) {
			cell = this->grid->findNode(oneCell.first)->cell;
			*cell->pars.at(pvar.first) = oneCell.second;
		}
	}
}

void GridProtocol::writePvars(QXmlStreamWriter& xml) {
	xml.writeStartElement("pvars");
	for(auto& pvar : this->new_pvars) {
		xml.writeStartElement("pvar");
		xml.writeAttribute("name", pvar.first.c_str());
		xml.writeTextElement("distribution_type", QString::number(pvar.second.dist));
		xml.writeTextElement("value0", QString::number(pvar.second.val[0]));
		xml.writeTextElement("value1", QString::number(pvar.second.val[1]));
		xml.writeStartElement("cells");
		for(auto& cell : pvar.second.cells) {
			xml.writeStartElement("cell");
			xml.writeAttribute("x", QString::number(cell.first.first));
			xml.writeAttribute("y", QString::number(cell.first.second));
			xml.writeCharacters(QString::number(cell.second));
			xml.writeEndElement();
		}
		xml.writeEndElement();
		xml.writeEndElement();
	}
	xml.writeEndElement();
}

void GridProtocol::readPvars(QXmlStreamReader& xml) {
	//	while(!xml.atEnd() && xml.name() != "file") {
	//		xml.readNext();
	//	}
	//	xml.readNext();
	while(!xml.atEnd() && xml.name() != "pvars") {
		xml.readNext();
	}
	this->handlePvars(xml);
}

void GridProtocol::handlePvars(QXmlStreamReader& xml) {
	if(xml.atEnd()) return;
	while(xml.readNextStartElement() && xml.name()=="pvar"){
		this->handlePvar(xml);
	}
	xml.skipCurrentElement();
}

void GridProtocol::handlePvar(QXmlStreamReader& xml) {
	if(xml.atEnd()) return;
	pair<string,MIonChanParam> pvar;
	pvar.first = xml.attributes().value("name").toString().toStdString();
	while(xml.readNextStartElement()) {
		if(xml.name()=="distribution_type") {
			xml.readNext();
			pvar.second.dist = static_cast<Distribution>(xml.text().toInt());
			xml.skipCurrentElement();
		} else if(xml.name()=="value0") {
			xml.readNext();
			pvar.second.val[0] = xml.text().toDouble();
			xml.skipCurrentElement();
		} else if(xml.name()=="value1") {
			xml.readNext();
			pvar.second.val[1] = xml.text().toDouble();
			xml.skipCurrentElement();
		} else if(xml.name()=="cells") {
			while(xml.readNextStartElement() && xml.name()=="cell"){
				pair<pair<int,int>,double> p = this->handleCell(xml);
				pvar.second.cells[p.first] = p.second;
			}
		}
		else {
			xml.skipCurrentElement();
		}
	}
	this->new_pvars[pvar.first] = pvar.second;
}

pair<pair<int,int>,double> GridProtocol::handleCell(QXmlStreamReader& xml) {
	pair<pair<int,int>,double> c;
	if(xml.atEnd()) return c;
	c.first.first = xml.attributes().value("x").toInt();
	c.first.second = xml.attributes().value("y").toInt();
	xml.readNext();
	c.second = xml.text().toDouble();
	xml.skipCurrentElement();
	return c;
}

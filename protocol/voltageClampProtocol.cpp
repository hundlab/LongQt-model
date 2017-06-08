#include "voltageClampProtocol.h"

VoltageClamp::VoltageClamp()  : Protocol(){
    v1 = v2 = v3 = v4 = v5 = 0;
    t1 = t2 = t3 = t4 = t5 = 0;
    pars.erase("numtrials");
    pars.erase("meastime");
    GetSetRef toInsert;
    pars["v1"] = toInsert.Initialize("double",[this] () {return std::to_string(v1);},[this] (const string& value) {v1 = std::stod(value);});
    pars["v2"] = toInsert.Initialize("double",[this] () {return std::to_string(v2);},[this] (const string& value) {v2 = std::stod(value);});
    pars["v3"] = toInsert.Initialize("double",[this] () {return std::to_string(v3);},[this] (const string& value) {v3 = std::stod(value);});
    pars["v4"] = toInsert.Initialize("double",[this] () {return std::to_string(v4);},[this] (const string& value) {v4 = std::stod(value);});
    pars["v5"] = toInsert.Initialize("double",[this] () {return std::to_string(v5);},[this] (const string& value) {v5 = std::stod(value);});
    pars["t1"] = toInsert.Initialize("double",[this] () {return std::to_string(t1);},[this] (const string& value) {t1 = std::stod(value);});
    pars["t2"] = toInsert.Initialize("double",[this] () {return std::to_string(t2);},[this] (const string& value) {t2 = std::stod(value);});
    pars["t3"] = toInsert.Initialize("double",[this] () {return std::to_string(t3);},[this] (const string& value) {t3 = std::stod(value);});
    pars["t4"] = toInsert.Initialize("double",[this] () {return std::to_string(t4);},[this] (const string& value) {t4 = std::stod(value);});
    pars["t5"] = toInsert.Initialize("double",[this] () {return std::to_string(t5);},[this] (const string& value) {t5 = std::stod(value);});

	type = "Voltage Clamp Protocol";

}
//overriden deep copy funtion
VoltageClamp* VoltageClamp::clone(){
    return new VoltageClamp(*this);
};

VoltageClamp::VoltageClamp(const VoltageClamp& toCopy) : Protocol(toCopy){
    this->CCcopy(toCopy);
}

VoltageClamp& VoltageClamp::operator=(const VoltageClamp& toCopy) {
    this->copy(toCopy);
    this->CCcopy(toCopy);
    return *this;
}

void VoltageClamp::CCcopy(const VoltageClamp& toCopy) {
    v1 = toCopy.v1;
    v2= toCopy.v2;
    v3 = toCopy.v3;
    v4 = toCopy.v4;
    v5 = toCopy.v5;
    t1 = toCopy.t1;
    t2 = toCopy.t2;
    t3 = toCopy.t3;
    t4 = toCopy.t4;
    t5 = toCopy.t5;
}

// External stimulus.
int VoltageClamp::clamp()
{
    if(cell->t >= t5 && t5 != 0) {
        vM = cell->vOld = v5;
    } else if(cell->t >= t4 && t4 != 0) {
        vM = cell->vOld = v4;
    } else if(cell->t >= t3 && t3 != 0) {
        vM = cell->vOld = v3;
    } else if(cell->t >= t2 && t2 != 0) {
        vM = cell->vOld = v2;
    } else if(cell->t >= t1) {
        vM = cell->vOld = v1;
    }
    return 1;
};

bool VoltageClamp::runTrial() {
        char writefile[150];     // Buffer for storing filenames
//to be moved to a better location
set<string> temp;
temp.insert(pnames.begin(),pnames.end());
cell->setConstantSelection(temp);
temp.clear();

//should not be here

        time = cell->t = 0.0;      // reset time
		this->readInCellState(this->readCellState);
        doneflag=1;     // reset doneflag
  
//        if (int(readflag)==1)
//            readvals(cell->vars, readfile);  // Init SVs before each trial.
        
        //###############################################################
        // Every time step, currents, concentrations, and Vm are calculated.
        //###############################################################
        int pCount = 0;
        //open i/o streams
        for(map<string,Measure>::iterator it = measures.begin(); it != measures.end(); it++) {
            sprintf(writefile,(datadir + "/" + propertyoutfile).c_str(),trial,it->second.varname.c_str());
            it->second.setOutputfile(writefile);
        }

        sprintf(writefile,(datadir + "/" + dvarsoutfile).c_str(),trial);
        cell->setOuputfileVariables(writefile);

        while(int(doneflag)&&(time<tMax)){

//what should stimt be made to be??            
            time = cell->tstep(0.0);    // Update time
            cell->updateCurr();    // Update membrane currents

            cell->updateConc();   // Update ion concentrations
            vM=cell->updateV();     // Update transmembrane potential

            clamp();

            //##### Output select variables to file  ####################
            if(int(measflag)==1&&cell->t>meastime){
                for (map<string,Measure>::iterator it = measures.begin(); it!=measures.end(); it++) {
                    if(it->second.measure(cell->t,*cell->vars[it->second.varname])&&(int(writeflag)==1)) {
                        it->second.write();
						it->second.reset();
                    }
                }
            }
            if (int(writeflag)==1&&time>writetime&&pCount%int(writeint)==0) {
                cell->writeVariables();
//                douts[mvnames.size()+(trial+1)*(mvnames.size()+1)].writevals(datamap,writefile,'a');
            }
            cell->setV(vM); //Overwrite vOld with new value
            pCount++;
        }
    
      // Output final (ss) property values for each trial
      for (map<string,Measure>::iterator it = measures.begin(); it != measures.end(); it++){
          sprintf(writefile,(datadir + "/" + finalpropertyoutfile).c_str(), trial, it->second.varname.c_str());
          it->second.setOutputfile(writefile);
		  it->second.restoreLast();
          it->second.write();
          it->second.reset();
      } 
      
      // Output parameter values for each trial
      sprintf(writefile,(datadir + "/" + finaldvarsoutfile).c_str(), trial);
      cell->setOutputfileConstants(writefile);
      cell->writeConstants();
      for(map<string,Measure>::iterator it = measures.begin(); it != measures.end(); it++) {
          it->second.closeFiles();
      }
      cell->closeFiles();
	  this->writeOutCellState(this->writeCellState);

      return true; 
}
void VoltageClamp::readInCellState(bool read) {
	if(read) {
		cell->readCellState(cellStateDir+"/"+cellStateFile+".xml");
		this->tMax += this->cell->t;
		this->t1 += this->cell->t;
		this->t2 += this->cell->t;
		this->t3 += this->cell->t;
		this->t4 += this->cell->t;
		this->t5 += this->cell->t;
	}
}
void VoltageClamp::setIonChanParams() {
	for(auto& pvar : this->pvars) {
		*cell->pars.at(pvar.first) = pvar.second.paramVal;
	}
}
void VoltageClamp:calcIonChanParams() {
	for(auto& pvar : this->pvars) {
		double val = 0;
		switch(pvar.second.dist) {
			case Distribution::none:
				val = pvar.second.val[0];
				break;
			case Distribution::normal:
				{
					normal_distribution<double> distribution(pvar.second.val[0]
							,pvar.second.val[1]);
					val = distribution(this->generator);
					break;
				}
			case Distribution::lognormal:
				{
					lognormal_distribution<double> logdistribution(
							pvar.second.val[0], pvar.second.val[1]);
					val = logdistribution(this->generator);
					break;
				}
		}
		pvar.second.paramVal = val;
	}
}
void VoltageClamp::writePvars(QXmlStreamWriter& xml) {
	xml.writeStartElement("pvars");
	for(auto& pvar : this->pvars) {
		xml.writeStartElement("pvar");
		xml.writeAttribute("name", pvar.first.c_str());
		xml.writeTextElement("distribution_type", QString::number(pvar.second.dist));
		xml.writeTextElement("value0", QString::number(pvar.second.val[0]));
		xml.writeTextElement("value1", QString::number(pvar.second.val[1]));
		xml.writeTextElement("cell",QString::number(pvar.second.paramVal));
		xml.writeEndElement();
	}
	xml.writeEndElement();
}

void VoltageClamp::readPvars(QXmlStreamReader& xml) {
	while(!xml.atEnd() && xml.name() != "pvars") {
		xml.readNext();
	}
	this->handlePvars(xml);
}

void VoltageClamp::handlePvars(QXmlStreamReader& xml) {
	if(xml.atEnd()) return;
	while(xml.readNextStartElement() && xml.name()=="pvar"){
		this->handlePvar(xml);
	}
	xml.skipCurrentElement();
}

void VoltageClamp::handlePvar(QXmlStreamReader& xml) {
	if(xml.atEnd()) return;
	pair<string,SIonChanParam> pvar;
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
		} else if(xml.name()=="cell") {
			xml.readNext();
			pvar.second.paramVal = xml.text().toDouble();
		}
		else {
			xml.skipCurrentElement();
		}
	}
	this->pvars[pvar.first] = pvar.second;
}

// 5/10/2016
//################################################
//protocol for a grid off cells
//class definitions
//################################################

#include "gridProtocol.h"
#include "gridCell.h"

gridProtocol::gridProtocol()  : CurrentClamp(){
    cell = new gridCell();
    baseCellMap = cellMap;
    cellMap.clear();
    cellMap["gridCell"] = [] () {return (Cell*) new gridCell;};
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
}
// External stimulus.
int gridProtocol::stim()
{
    for(auto it = stimNodes.begin(); it != stimNodes.end(); it++) {
        Cell* cell = (*it)->cell;
        if(cell->t>=stimt&&cell->t<(stimt+stimdur)){
            if(stimflag==0){
                stimcounter++;
                stimflag=1;
                if(stimcounter>=int(numstims)){
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

//should not be here

        time = cell->t = 0.0;      // reset time
        doneflag=1;     // reset doneflag
  
        //###############################################################
        // Every time step, currents, concentrations, and Vm are calculated.
        //###############################################################
        int pCount = 0;
        //open i/o streams
        for(map<string,Measure>::iterator it = measures.begin(); it != measures.end(); it++) {
            sprintf(writefile,propertyoutfile.c_str(),trial,it->second.varname.c_str());
            sprintf(writefile, (datadir + "/" + "cell_%%i_%%i_" + string(writefile)).c_str());
            it->second.setOutputfile(writefile);
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
                for (map<string,Measure>::iterator it = measures.begin(); it!=measures.end(); it++) {
                    for(auto iv = dataNodes.begin(); iv != dataNodes.end(); iv++) {
                        it->second.measure((*iv)->cell->t,*(*iv)->cell->vars[it->second.varname]);
                        if(int(writeflag)==1) {
                            it->second.write(true, !(int(doneflag)&&(time<tMax)));
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
      for (map<string,Measure>::iterator it = measures.begin(); it != measures.end(); it++){
        sprintf(writefile,finalpropertyoutfile.c_str(),trial,it->second.varname.c_str());
        sprintf(writefile, (datadir + "/" + "cell_%%i_%%i_" + string(writefile)).c_str());
        it->second.setOutputfile(writefile);
        it->second.write(false, true);
        it->second.reset();
      } 
      
      // Output parameter values for each trial
      sprintf(writefile,finaldvarsoutfile.c_str(),trial);
      sprintf(writefile, (datadir + "/" + "cell_%%i_%%i_" + string(writefile)).c_str());
      cell->setOutputfileConstants(writefile);
      cell->writeConstants();

      return true; 
}
map<string, CellInitializer>& gridProtocol::getCellMap() {
    return baseCellMap;
}
set<Node*>& gridProtocol::getStimNodes() {
    return stimNodes;
}
set<Node*>& gridProtocol::getDataNodes() {
    return dataNodes;
}

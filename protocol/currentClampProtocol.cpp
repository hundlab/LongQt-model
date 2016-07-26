#include "currentClampProtocol.h"

CurrentClamp::CurrentClamp()  : Protocol(){
    stimdur = 1.0;  // stim duration, ms
    stimt = 0.0;    // time of first stim, ms
    stimval = 0.0;  // stim current amplitude, uA/uF
    bcl = 1000;     // basic cycle length, ms
    numstims = 1;   // # of stimuli to apply
    paceflag = 0;   // 1 to pace cell.
    stimflag = 0;
    stimcounter = 0;

    GetSetRef toInsert;
    pars["stimdur"]= toInsert.Initialize("double", [this] () {return std::to_string(stimdur);}, [this] (const string& value) {stimdur = std::stod(value);});
    pars["stimt"]= toInsert.Initialize("double", [this] () {return std::to_string(stimt);}, [this] (const string& value) {stimt = std::stod(value);});
    pars["stimval"]= toInsert.Initialize("double", [this] () {return std::to_string(stimval);}, [this] (const string& value) {stimval = std::stod(value);});
    pars["bcl"] = toInsert.Initialize("double", [this] () {return std::to_string(bcl);}, [this] (const string& value) {bcl = std::stod(value);});
    pars["numstims"]= toInsert.Initialize("int", [this] () {return std::to_string(numstims);}, [this] (const string& value) {numstims = std::stoi(value);});
    pars["paceflag"]= toInsert.Initialize("bool", [this] () {return to_string(paceflag);}, [this] (const string& value) {paceflag = stob(value);});
}
//overriden deep copy funtion
CurrentClamp* CurrentClamp::clone(){
    return new CurrentClamp(*this);
};
CurrentClamp::CurrentClamp(const CurrentClamp& toCopy) : Protocol(toCopy){
    this->CCcopy(toCopy);
}

CurrentClamp& CurrentClamp::operator=(const CurrentClamp& toCopy) {
    this->copy(toCopy);
    this->CCcopy(toCopy);
    return *this;
}

void CurrentClamp::CCcopy(const CurrentClamp& toCopy) {
    stimdur = toCopy.stimdur;  // stim duration, ms
    stimt = toCopy.stimt;    // time of first stim, ms
    stimval = toCopy.stimval;  // stim current amplitude, uA/uF
    bcl = toCopy.bcl;     // basic cycle length, ms
    numstims = toCopy.numstims;   // # of stimuli to apply
    stimflag = toCopy.stimflag;
    stimcounter = toCopy.stimcounter;
    paceflag = toCopy.paceflag;   // 1 to pace cell.
}

// External stimulus.
int CurrentClamp::stim()
{
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
    return 1;
};

bool CurrentClamp::runTrial() {
        char writefile[150];     // Buffer for storing filenames

//to be moved to a better location
set<string> temp;
temp.insert(pnames.begin(),pnames.end());
cell->setConstantSelection(temp);
temp.clear();

//should not be here

        time = cell->t = 0.0;      // reset time
        doneflag=1;     // reset doneflag
        if(readCellState) {
            sprintf(writefile,(cellStateDir + "/" + cellStateFile).c_str(),trial);
            cell->readCellState(writefile);
        }
 
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
            
            time = cell->tstep(stimt);    // Update time
            cell->updateCurr();    // Update membrane currents
            if(int(paceflag)==1)  // Apply stimulus
                stim();

            cell->updateConc();   // Update ion concentrations
            vM=cell->updateV();     // Update transmembrane potential

            //##### Output select variables to file  ####################
            if(int(measflag)==1&&cell->t>meastime){
                for (map<string,Measure>::iterator it = measures.begin(); it!=measures.end(); it++) {
                    it->second.measure(cell->t,*cell->vars[it->second.varname]);
                    if(int(writeflag)==1) {
                        it->second.write(true, !(int(doneflag)&&(time<tMax)));
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
          it->second.write(false, true);
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
      if(writeCellState) {
          sprintf(writefile,(datadir + "/" + cellStateFile).c_str(),trial);
          cell->writeCellState(writefile);
      }

      return true; 
}

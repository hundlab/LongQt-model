#include "voltageClampProtocol.h"

voltageClamp::voltageClamp()  : Protocol(){
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

}
//overriden deep copy funtion
voltageClamp* voltageClamp::clone(){
    return new voltageClamp(*this);
};

voltageClamp::voltageClamp(const voltageClamp& toCopy) : Protocol(toCopy){
    this->CCcopy(toCopy);
}

voltageClamp& voltageClamp::operator=(const voltageClamp& toCopy) {
    this->copy(toCopy);
    this->CCcopy(toCopy);
    return *this;
}

void voltageClamp::CCcopy(const voltageClamp& toCopy) {
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
int voltageClamp::clamp()
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

bool voltageClamp::runTrial() {
        char writefile[150];     // Buffer for storing filenames

//to be moved to a better location
set<string> temp;
temp.insert(pnames.begin(),pnames.end());
cell->setConstantSelection(temp);
temp.clear();

//should not be here

        time = cell->t = 0.0;      // reset time
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
      for(auto measure : measures) {
          measure.second.closeFiles();
      }
      cell->closeFiles();

      return true; 
}

#ifndef CELLUTILS_H
#define CELLUTILS_H

#include "gpbatrial.h"
#include "gridCell.h"
#include "hrd09.h"
#include "tnnp04.h"
#include "gpbatrialRyr.h"
#include "kurata08.h"

#include <map>
#include <list>
#include <utility>
#include <string>

typedef Cell* (*CellInitializer)(void);

class cellUtils {
  public:
    map<string, CellInitializer> cellMap;
    map<string, list<pair<string,string>>> protocolCellDefaults;
    cellUtils() {
        cellMap[ControlSa().type] = [] () {return (Cell*) new ControlSa;};
        cellMap[GpbAtrial().type] = [] () {return (Cell*) new GpbAtrial;};
        cellMap[HRD09Control().type] = [] () {return (Cell*) new HRD09Control;};
        cellMap[HRD09BorderZone().type] = [] () {return (Cell*) new HRD09BorderZone;};
        cellMap[TNNP04Control().type] = [] () {return (Cell*) new TNNP04Control;};
        cellMap[gpbatrialRyr().type] = [] () {return (Cell*) new gpbatrialRyr;};
    //defualt vals for a simulation with cell types
        protocolCellDefaults[ControlSa().type] = {{"paceflag","true"},{"stimval","-60"},{"stimdur","1"},{"tMax","500000"},{"writetime","495000"},{"bcl","1000"},{"numstims","500"}};
        protocolCellDefaults[GpbAtrial().type] = {{"paceflag","true"},{"stimval","-12.5"},{"stimdur","5"},{"tMax","500000"},{"writetime","495000"},{"bcl","1000"},{"numstims","500"}};
        protocolCellDefaults[HRD09Control().type] = {{"paceflag","true"},{"stimval","-80"},{"stimdur","0.5"},{"tMax","500000"},{"writetime","495000"},{"bcl","1000"},{"numstims","500"}};
        protocolCellDefaults[HRD09BorderZone().type] = {{"paceflag","true"},{"stimval","-80"},{"stimdur","0.5"},{"tMax","500000"},{"writetime","495000"},{"bcl","1000"},{"numstims","500"}};
        protocolCellDefaults[TNNP04Control().type] = {{"paceflag","true"},{"stimval","-60"},{"stimdur","1"},{"tMax","500000"},{"writetime","495000"},{"bcl","1000"},{"numstims","500"}};

 
   }
};
#endif
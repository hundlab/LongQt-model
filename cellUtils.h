#ifndef CELLUTILS_H
#define CELLUTILS_H

#include "gpbatrial.h"
#include "gridCell.h"
#include "hrd09.h"
#include "tnnp04.h"
#include "gpbatrialRyr.h"
#include "kurata08.h"


typedef Cell* (*CellInitializer)(void);

class cellUtils {
  public:
    map<string, CellInitializer> cellMap;
    cellUtils() {
        cellMap[ControlSa().type] = [] () {return (Cell*) new ControlSa;};
        cellMap[GpbAtrial().type] = [] () {return (Cell*) new GpbAtrial;};
        cellMap[HRD09Control().type] = [] () {return (Cell*) new HRD09Control;};
        cellMap[HRD09BorderZone().type] = [] () {return (Cell*) new HRD09BorderZone;};
        cellMap[TNNP04Control().type] = [] () {return (Cell*) new TNNP04Control;};
        cellMap[gpbatrialRyr().type] = [] () {return (Cell*) new gpbatrialRyr;};
   }
};
#endif

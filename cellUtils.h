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
        cellMap["Cell"] = [] () {return new Cell;};
        cellMap["ControlSa"] = [] () {return (Cell*) new ControlSa;};
        cellMap["GpbAtrial"] = [] () {return (Cell*) new GpbAtrial;};
        cellMap["HRD09Control"] = [] () {return (Cell*) new HRD09Control;};
        cellMap["HRD09BorderZone"] = [] () {return (Cell*) new HRD09BorderZone;};
        cellMap["TNNP04Control"] = [] () {return (Cell*) new TNNP04Control;};
        cellMap["gpbatrialRyr"] = [] () {return (Cell*) new gpbatrialRyr;};
    }
};
#endif

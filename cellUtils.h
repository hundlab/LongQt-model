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
        cellMap["Rabbit Sinus Node (Kurata)"] = [] () {return (Cell*) new ControlSa;};
        cellMap["Human Atrial (Grandi)"] = [] () {return (Cell*) new GpbAtrial;};
        cellMap["Canine Ventricular (Hund-Rudy)"] = [] () {return (Cell*) new HRD09Control;};
        cellMap["Canine Ventricular Border Zone (Hund-Rudy)"] = [] () {return (Cell*) new HRD09BorderZone;};
        cellMap["Human Ventricular (Ten Tusscher)"] = [] () {return (Cell*) new TNNP04Control;};
        cellMap["gpbatrialRyr"] = [] () {return (Cell*) new gpbatrialRyr;};
   }
};
#endif

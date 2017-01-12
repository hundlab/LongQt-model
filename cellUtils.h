#ifndef CELLUTILS_H
#define CELLUTILS_H

#include "gpbatrial.h"
#include "gridCell.h"
#include "hrd09.h"
#include "tnnp04.h"
#include "gpbatrialRyr.h"
#include "kurata08.h"
#include "gridCell.h"
#include "gpbhuman.h"
#include "ksan.h"
#include "gpbatrialWT.h"
#include "gpbatrialSE.h"
#include "atrial.h"
#include "br04.h"

#include <map>
#include <list>
#include <utility>
#include <string>
#include <QXmlStreamReader>

typedef Cell* (*CellInitializer)(void);

inline bool readNext(QXmlStreamReader& xml, QString name) {
	if(xml.name() == name && xml.tokenType() == QXmlStreamReader::StartElement) {
		return true;
	}
	while(!xml.atEnd()) {
		QXmlStreamReader::TokenType t = xml.readNext();
		if(xml.name() == name && t == QXmlStreamReader::StartElement) {
			return true;
		} else if(xml.name() == name && t == QXmlStreamReader::EndElement) {
			return false;
		}
	}
	return false;
}
inline bool readUpLevel(QXmlStreamReader& xml) {
	int depth = 1;
	while(!xml.atEnd()) {
		QXmlStreamReader::TokenType t = xml.readNext();
		if(t == QXmlStreamReader::StartElement) {
			++depth;
		} else if(t == QXmlStreamReader::EndElement) {
			--depth;
		}
		if(depth < 1) {
			return true;
		}
	}
	return false;
}
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
			//        cellMap[GpbVent().type] = [] () {return (Cell*) new GpbVent;};
			//        cellMap[Br04().type] = [] () {return (Cell*) new Br04;};
			//        cellMap[Ksan().type] = [] () {return (Cell*) new Ksan;};
			//        cellMap[Courtemanche98().type] = [] () {return (Cell*) new Courtemanche98;};
			//        cellMap[GpbAtrialWT().type] = [] () {return (Cell*) new GpbAtrialWT;};
			//        cellMap[GpbAtrialSE().type] = [] () {return (Cell*) new GpbAtrialSE;};



			//        cellMap[gpbatrialRyr().type] = [] () {return (Cell*) new gpbatrialRyr;};
			//defualt vals for a simulation with cell types
			protocolCellDefaults[ControlSa().type] = {{"paceflag","true"},{"stimval","-60"},{"stimdur","1"},{"tMax","500000"},{"writetime","495000"},{"bcl","1000"},{"numstims","500"}};
			protocolCellDefaults[HRD09Control().type] = {{"paceflag","true"},{"stimval","-80"},{"stimdur","0.5"},{"tMax","500000"},{"writetime","495000"},{"bcl","1000"},{"numstims","500"}};
			protocolCellDefaults[GpbAtrial().type] = {{"paceflag","true"},{"stimval","-12.5"},{"stimdur","5"},{"tMax","500000"},{"writetime","495000"},{"bcl","1000"},{"numstims","500"}};
			protocolCellDefaults[gridCell().type] = {{"paceflag","true"},{"stimval","-12.5"},{"stimdur","5"},{"tMax","500000"},{"writetime","495000"},{"bcl","1000"},{"numstims","500"}};
			protocolCellDefaults[HRD09BorderZone().type] = {{"paceflag","true"},{"stimval","-80"},{"stimdur","0.5"},{"tMax","500000"},{"writetime","495000"},{"bcl","1000"},{"numstims","500"}};
			protocolCellDefaults[TNNP04Control().type] = {{"paceflag","true"},{"stimval","-60"},{"stimdur","1"},{"tMax","500000"},{"writetime","495000"},{"bcl","1000"},{"numstims","500"}};


		}
};
#endif

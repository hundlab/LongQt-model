/*
 * general purpose functions used by mostly by protocols 
 * This is the place to add a new cell type
 */
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

#include "side.h" //more CellUtils broken up to avoid cyclic deps

namespace CellUtils {
	typedef Cell* (*CellInitializer)(void);

	static map<string, CellInitializer> cellMap = {
		{ ControlSa().type, [] () {return (Cell*) new ControlSa; }},
		{ GpbAtrial().type, [] () {return (Cell*) new GpbAtrial;}},
		{ HRD09Control().type, [] () {return (Cell*) new HRD09Control;}},
		{ HRD09BorderZone().type, [] () {return (Cell*) new HRD09BorderZone;}},
		{ TNNP04Control().type, [] () {return (Cell*) new TNNP04Control;}}

		/*		{ GpbVent().type, [] () {return (Cell*) new GpbVent;}},
				{ Br04().type, [] () {return (Cell*) new Br04;}},
				{ Ksan().type, [] () {return (Cell*) new Ksan;}},
				{ Courtemanche98().type, [] () {return (Cell*) new Courtemanche98;}},
				{ GpbAtrialWT().type, [] () {return (Cell*) new GpbAtrialWT;}},
				{ GpbAtrialSE().type, [] () {return (Cell*) new GpbAtrialSE;}}*/
	};
	static map<string, list<pair<string,string>>> protocolCellDefaults = {
		{ ControlSa().type, {{"paceflag","true"},{"stimval","-60"},{"stimdur","1"},
			{"tMax","500000"},{"writetime","495000"},{"bcl","1000"},
			{"numstims","500"}}},
		{ HRD09Control().type, {{"paceflag","true"},{"stimval","-80"},
			{"stimdur","0.5"},{"tMax","500000"},{"writetime","495000"},
			{"bcl","1000"},{"numstims","500"}}},
		{ GpbAtrial().type, {{"paceflag","true"},{"stimval","-12.5"},{"stimdur","5"},
			{"tMax","500000"},{"writetime","495000"},{"bcl","1000"},
			{"numstims","500"}}},
		{ GridCell().type, {{"paceflag","true"},{"stimval","-12.5"},{"stimdur","5"},
		   {"tMax","500000"},{"writetime","495000"},{"bcl","1000"},
		   {"numstims","500"}}},
		{ HRD09BorderZone().type, {{"paceflag","true"},{"stimval","-80"},
			{"stimdur","0.5"},{"tMax","500000"},{"writetime","495000"},
			{"bcl","1000"},{"numstims","500"}}},
		{ TNNP04Control().type, {{"paceflag","true"},{"stimval","-60"},
			{"stimdur","1"},{"tMax","500000"},{"writetime","495000"},{"bcl","1000"},
			{"numstims","500"}}}
	};



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
	//helper functions
	inline string trim(string str)
	{
		string toFind = " \t\n\v\f\r";
		str.erase(0, str.find_first_not_of(toFind));
		str.erase(str.find_last_not_of(toFind)+1);
		return str;
	}
	inline string to_string(const bool& b) {
		return b ? "true" : "false";
	}

	inline bool stob(const string& s) {
		return (strcasecmp("true",trim(s).c_str()) == 0);
	}

}
#endif

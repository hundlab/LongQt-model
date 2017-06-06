/*
 * general purpose functions used by mostly by protocols 
 * This is the place to add a new cell type
 */
#ifndef CELLUTILS_H
#define CELLUTILS_H

#include <map>
#include <list>
#include <utility>
#include <string>
#include <QXmlStreamReader>

#include "cell.h"

using namespace std;
class Protocol;
/*
 * This is equivalent to a simple static class which holds functions
 * and variables which are not class specific
 */
namespace CellUtils {
	//declare CellInitializer function type
	typedef Cell* (*CellInitializer)(void);

	/*
	 * cell map this is how new instances of cells are created
	 * if you are adding a new cell to longqt add it here
	 * and also add it to the protocolCellDefualts map below
	 */
	extern const map<string, CellInitializer> cellMap;

	/*
	 * this map is used to setup default simulations in longqt add your new cell to 
	 * this map to give it a meaningful default simulation. We typically pace to 
	 * study-state ~500,000 ms and output values for the last 5,000 ms
	 */
	extern map<string, list<pair<string,string>>> protocolCellDefaults;


	//declare the ProtocolIntializer type
	typedef Protocol* (*ProtocolInitializer) (void);

	/*
	 * map of known protocols used to create new instances of protocols in
	 * longqt
	 */
	extern const map<string, ProtocolInitializer> protoMap;

	/*
	 * Side provides a consistent system for numbering sides in 2D grids
	 */
	enum Side {
		top = 0,
		right = 1,
		bottom = 2,
		left = 3
	};

	/*reads in until the next StartElement with name name
	 *returns:
	 *	True if it is found
	 *	False if eof or error
	 */
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
	/*
	 * reads until xml tree is one level higher
	 * returns:
	 *	True if it is found
	 *	False if eof or error
	 */
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

	//trim whitespace from beginning and end of a string
	inline string trim(string str)
	{
		string toFind = " \t\n\v\f\r";
		str.erase(0, str.find_first_not_of(toFind));
		str.erase(str.find_last_not_of(toFind)+1);
		return str;
	}
	/* maps a bool to string
	 * b:true -> "true"
	 * b:false-> "false"
	 */
	inline string to_string(const bool& b) {
		return b ? "true" : "false";
	}
	
	/* maps a string to a bool
	 * s:"true" -> true
	 * s:"false" -> false
	 */
	inline bool stob(const string& s) {
		return (strcasecmp("true",trim(s).c_str()) == 0);
	}

}
#endif

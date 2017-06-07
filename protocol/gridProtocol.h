// 5/10/2016
//################################################
//protocol for a grid off cells class declarations
//################################################

#ifndef GRIDPROTOCOL_H
#define GRIDPROTOCOL_H

#include <set>
#include "currentClampProtocol.h"
#include "node.h"
#include "grid.h"

class GridProtocol : public CurrentClamp {
	public:
		struct MIonChanParam : IonChanParam {
			map<pair<int,int>,double> cells; //map from x,y pos -> value
		};

	public:
		GridProtocol();
		GridProtocol(const GridProtocol& toCopy);
		GridProtocol* clone();
		GridProtocol& operator=(const GridProtocol& toCopy);

		bool runTrial() override;
		int stim();
        map<string, CellUtils::CellInitializer>& getCellMap();
		set<pair<int,int>>& getDataNodes();
		set<pair<int,int>>& getStimNodes();
		virtual bool writepars(string file);
		virtual bool writepars(QXmlStreamWriter& xml);
		virtual int readpars(string file, set<string> varnames = {});
		virtual int readpars(QXmlStreamReader& xml, set<string> varnames = {});
		virtual void setIonChanParams();
		virtual void writePvars(QXmlStreamWriter& xml);
		virtual void readPvars(QXmlStreamReader& xml);
		void handlePvars(QXmlStreamReader& xml);
		void handlePvar(QXmlStreamReader& xml);
		pair<pair<int,int>,double> handleCell(QXmlStreamReader& xml);

		map<string,MIonChanParam> new_pvars;
		//    virtual bool addMeasure(Measure toInsert);

		void setStim2(bool enable);
	private:
        map<string, CellUtils::CellInitializer> baseCellMap;
		void CCcopy(const GridProtocol& toCopy);
		set<pair<int,int>> dataNodes;
		set<pair<int,int>> stimNodes;
		set<pair<int,int>> stimNodes2;
		double stimval2, stimdur2, bcl2, stimt2;
		bool stim2 = false;
		string setToString(set<pair<int,int>>& nodes);
		set<pair<int,int>> stringToSet(string nodesList);
		Grid* grid;
		map<pair<int,int>,map<string,Measure>> realMeasures;
		void swapStims();
};
#endif

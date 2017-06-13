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
	public:
		GridProtocol();
		GridProtocol(const GridProtocol& toCopy);
		GridProtocol* clone();
		GridProtocol& operator=(const GridProtocol& toCopy);

		virtual void setupTrial() override;
		virtual bool runTrial() override;
		int stim();
        map<string, CellUtils::CellInitializer>& getCellMap();
		set<pair<int,int>>& getDataNodes();
		set<pair<int,int>>& getStimNodes();
		virtual bool writepars(QXmlStreamWriter& xml);
		virtual int readpars(string file);
		virtual int readpars(QXmlStreamReader& xml);
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

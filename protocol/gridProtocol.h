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
#include "gridmeasuremanager.h"
#include "pvarsgrid.h"

class GridProtocol : public CurrentClamp {
    public:
        GridProtocol();
        GridProtocol(const GridProtocol& toCopy);
        GridProtocol* clone();
        GridProtocol& operator=(const GridProtocol& toCopy);

        virtual void setupTrial() override;
        virtual bool runTrial() override;
        int stim();
        map<string, CellUtils::CellInitializer>& getCellMap();
        set<pair<int,int>>& getStimNodes();
        virtual bool writepars(QXmlStreamWriter& xml);
        virtual int readpars(QXmlStreamReader& xml);

        virtual Cell* cell() const override;
        virtual void cell(Cell* cell) override;
        virtual bool cell(const string& type) override;
        virtual list<string> cellOptions() override;

        GridMeasureManager& gridMeasureMgr();

        virtual CellPvars& pvars() override;

        virtual MeasureManager& measureMgr() override;

        void setStim2(bool enable);
    private:
        void CCcopy(const GridProtocol& toCopy);
        set<pair<int,int>> stimNodes;
        set<pair<int,int>> stimNodes2;
        double stimval2, stimdur2, bcl2, stimt2;
        bool stim2 = false;
        string setToString(set<pair<int,int>>& nodes);
        set<pair<int,int>> stringToSet(string nodesList);
        Grid* grid;
        void swapStims();
        unique_ptr<GridCell> __cell = unique_ptr<GridCell>(new GridCell());        // pointer to cell class
        unique_ptr<PvarsGrid> __pvars;
        unique_ptr<GridMeasureManager> __measureMgr; // set of measure class for measuring SV props.

};
#endif

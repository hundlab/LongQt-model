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

        virtual shared_ptr<Cell> cell() const override;
        virtual void cell(shared_ptr<Cell> cell) override;
        virtual bool cell(const string& type) override;
        virtual list<string> cellOptions() override;

        Grid& getGrid();

        GridMeasureManager& gridMeasureMgr();

        virtual CellPvars& pvars() override;

        virtual MeasureManager& measureMgr() override;

        void setStim2(bool enable);
        bool getStim2();

        set<pair<int,int>> stimNodes;
        set<pair<int,int>> stimNodes2;
        double stimval2, stimdur2, bcl2, stimt2;

        const char* type() const override;

    private:
        void CCcopy(const GridProtocol& toCopy);
        void mkmap();
        bool stim2 = false;
        string setToString(set<pair<int,int>>& nodes);
        set<pair<int,int>> stringToSet(string nodesList);
        Grid* grid;
        void swapStims();
        set<shared_ptr<Node>> __stimN;
        set<shared_ptr<Node>> __stimN2;
        shared_ptr<GridCell> __cell = make_shared<GridCell>();        // pointer to cell class
        unique_ptr<PvarsGrid> __pvars;
        unique_ptr<GridMeasureManager> __measureMgr; // set of measure class for measuring SV props.

};
#endif

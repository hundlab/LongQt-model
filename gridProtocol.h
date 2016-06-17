// 5/10/2016
//################################################
//protocol for a grid off cells
//class declarations
//################################################

#ifndef GRIDPROTOCOL_H
#define GRIDPROTOCOL_H

#include <set>
#include "currentClampProtocol.h"
#include "node.h"
#include "grid.h"

class gridProtocol : public CurrentClamp {
  public:
    gridProtocol();
    gridProtocol(const gridProtocol& toCopy);
    gridProtocol* clone();
    gridProtocol& operator=(const gridProtocol& toCopy);

    bool runTrial() override;
    int stim();
    map<string, CellInitializer>& getCellMap();
    set<pair<int,int>>& getDataNodes();
    set<pair<int,int>>& getStimNodes();
    virtual bool writepars(string file);
    virtual int readpars(string file);

  private:
    map<string, CellInitializer> baseCellMap;
    void CCcopy(const gridProtocol& toCopy);
    set<pair<int,int>> dataNodes;
    set<pair<int,int>> stimNodes;
    string setToString(set<pair<int,int>>& nodes);
    set<pair<int,int>> stringToSet(string nodesList);
    Grid* grid;
};
#endif

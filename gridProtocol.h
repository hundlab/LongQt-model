// 5/10/2016
//################################################
//protocol for a grid off cells
//class declarations
//################################################

#ifndef GRIDPROTOCOL_H
#define GRIDPROTOCOL_H

#include <set>
#include "cellGrid.h"
#include "currentClampProtocol.h"

class gridProtocol : public CurrentClamp {
  public:
    gridProtocol();
    gridProtocol(const gridProtocol& toCopy);
    gridProtocol* clone();
    gridProtocol& operator=(const gridProtocol& toCopy);

    bool runTrial() override;
    int stim();
  private:
    void CCcopy(const gridProtocol& toCopy);
    Grid* grid;
    set<Node*> dataNodes;
    set<Node*> stimNodes;
};
#endif

//################################################
// This header file contains protocol class
// declaration for simulation of excitable cell activity
// with a current clamp
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015
// Email thomas.hund@osumc.edu
//#################################################

#ifndef CurrentClampProtocol_H
#define CurrentClampProtocol_H

#include "protocol.h"
#include <vector>
#include "pvarscurrentclamp.h"
#include "hrd09.h"

class CurrentClamp : public Protocol {
  public:
    CurrentClamp();
    CurrentClamp(const CurrentClamp& toCopy);
    CurrentClamp* clone();
    CurrentClamp& operator=(const CurrentClamp& toCopy);
    virtual ~CurrentClamp();

    virtual Cell* cell() const override;
    virtual void cell(Cell* cell) override;

    virtual CellPvars& pvars() override;

    virtual MeasureManager& measureMgr() override;

    virtual void setupTrial() override;
    virtual bool runTrial() override;
    void readInCellState(bool read) override;

  protected:
    double bcl,stimval,stimdur,stimt;
    int numstims;   //variables for pacing.
    bool stimflag,paceflag;
    double stimcounter;

    virtual int stim();
  private:
    void CCcopy(const CurrentClamp& toCopy);

    unique_ptr<Cell> __cell = unique_ptr<Cell>(new HRD09Control);        // pointer to cell class
    unique_ptr<PvarsCurrentClamp> __pvars
        = unique_ptr<PvarsCurrentClamp>(new PvarsCurrentClamp(this));
    unique_ptr<MeasureManager> __measureMgr; // set of measure class for measuring SV props.


};
#endif

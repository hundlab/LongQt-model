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
namespace LongQt {

class CurrentClamp : public Protocol {
  public:
    CurrentClamp();
    CurrentClamp(const CurrentClamp& toCopy);
    CurrentClamp* clone();
    CurrentClamp& operator=(const CurrentClamp& toCopy);
    virtual ~CurrentClamp();

    virtual std::shared_ptr<Cell> cell() const override;
    virtual void cell(std::shared_ptr<Cell> cell) override;

    virtual PvarsCell& pvars() override;

    virtual MeasureManager& measureMgr() override;

    virtual void setupTrial() override;
    virtual bool runTrial() override;
    void readInCellState(bool read) override;

    double bcl,stimval,stimdur,stimt,stimbegin,stimend;
    int numstims;   //variables for pacing.
    bool stimflag,paceflag;

    const static char* name;
    const char* type() const override;

  protected:
    virtual int stim();
    double stimcounter;
  private:
    void CCcopy(const CurrentClamp& toCopy);
    void mkmap();

    std::shared_ptr<Cell> __cell = std::make_shared<HRD09Control>();        // pointer to cell class
    std::unique_ptr<PvarsCurrentClamp> __pvars
        = std::unique_ptr<PvarsCurrentClamp>(new PvarsCurrentClamp(this));
    std::unique_ptr<MeasureManager> __measureMgr; // set of measure class for measuring SV props.


};
} //LongQt

#endif

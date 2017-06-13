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

class CurrentClamp : public Protocol {
  public:
    CurrentClamp();
    CurrentClamp(const CurrentClamp& toCopy);
    CurrentClamp* clone();
    CurrentClamp& operator=(const CurrentClamp& toCopy);

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
};
#endif

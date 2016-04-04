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

class CurrentClamp : public Protocol {
  public:
    CurrentClamp();
    CurrentClamp(const CurrentClamp& toCopy);
    CurrentClamp* clone();
    CurrentClamp& operator=(const CurrentClamp& toCopy);

    bool runTrial() override;
  private:
    double bcl,stimval,stimdur,stimt;
    int numstims;   //variables for pacing.
    bool stimflag;
    double stimcounter;

    int stim();
    void CCcopy(const CurrentClamp& toCopy);
};
#endif

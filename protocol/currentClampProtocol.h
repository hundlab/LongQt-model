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
#include <list>

class CurrentClamp : public Protocol {
  public:
	struct TIonChanParam : IonChanParam {
		list<double> trials; //index -> value
	};
  public:
    CurrentClamp();
    CurrentClamp(const CurrentClamp& toCopy);
    CurrentClamp* clone();
    CurrentClamp& operator=(const CurrentClamp& toCopy);

    bool runTrial() override;
	void readInCellState(bool read) override;

	virtual void setIonChanParams();
	virtual void calcIonChanParams();
	map<string,TIonChanParam> pvars;
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

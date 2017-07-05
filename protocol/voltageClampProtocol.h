//################################################
// This header file contains protocol class
// declaration for simulation of excitable cell activity
// with a voltage clamp
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015
// Email thomas.hund@osumc.edu
//#################################################

#ifndef voltageClampProtocol_H
#define voltageClampProtocol_H

#include "hrd09.h"
#include "protocol.h"
#include "pvarsvoltageclamp.h"

class VoltageClamp : public Protocol {
  public:
    VoltageClamp();
    VoltageClamp(const VoltageClamp& toCopy);
    VoltageClamp* clone();
    VoltageClamp& operator=(const VoltageClamp& toCopy);

    virtual Cell* cell() const override;
    virtual void cell(Cell* cell) override;

    virtual CellPvars& pvars() override;

	virtual void setupTrial() override;
    virtual bool runTrial() override;
	void readInCellState(bool read) override;

    virtual MeasureManager& measureMgr() override;

  private:
	int clamp();
    void CCcopy(const VoltageClamp& toCopy);

    double v1, v2, v3, v4, v5;
    double t1, t2, t3, t4, t5;
    unique_ptr<Cell> __cell = unique_ptr<Cell>(new HRD09Control);        // pointer to cell class
    unique_ptr<PvarsVoltageClamp> __pvars
        = unique_ptr<PvarsVoltageClamp>(new PvarsVoltageClamp(this));
    unique_ptr<MeasureManager> __measureMgr; // set of measure class for measuring SV props.


};
#endif

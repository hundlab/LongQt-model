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

#include <vector>
#include "hrd09.h"
#include "protocol.h"
#include "pvarscurrentclamp.h"
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

  virtual bool runTrial() override;

  double bcl, stimval, stimdur, stimt, stimbegin, stimend;
  int numstims;  // variables for pacing.
  bool stimflag, paceflag;

  const static char* name;
  const char* type() const override;

 protected:
  virtual int stim();
  virtual void readInCellState(bool read) override;
  virtual void setupTrial() override;

  double stimcounter;

 private:
  void CCcopy(const CurrentClamp& toCopy);
  void mkmap();

  std::shared_ptr<Cell> __cell =
      std::make_shared<HRD09Control>();  // pointer to cell class
  std::unique_ptr<PvarsCurrentClamp> __pvars =
      std::unique_ptr<PvarsCurrentClamp>(new PvarsCurrentClamp(this));
  std::unique_ptr<MeasureManager>
      __measureMgr;  // set of measure class for measuring SV props.
};
}  // namespace LongQt

#endif

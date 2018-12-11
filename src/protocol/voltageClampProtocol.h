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
namespace LongQt {

class VoltageClamp : public Protocol {
 public:
  VoltageClamp();
  VoltageClamp(const VoltageClamp& toCopy);
  VoltageClamp* clone();
  VoltageClamp& operator=(const VoltageClamp& toCopy);

  virtual bool writepars(QXmlStreamWriter& xml);
  virtual int readpars(QXmlStreamReader& xml);

  virtual std::shared_ptr<Cell> cell() const override;
  virtual void cell(std::shared_ptr<Cell> cell) override;

  virtual PvarsCell& pvars() override;

  virtual bool runTrial() override;
  void readInCellState(bool read) override;

  virtual MeasureManager& measureMgr() override;

  int insertClamp(double time, double voltage);
  void changeClampVoltage(int pos, double voltage);
  void removeClamp(int pos);
  const std::vector<std::pair<double, double>>& clamps();
  void clamps(std::vector<std::pair<double, double>> clamps);

  const static char* name;
  const char* type() const override;

 protected:
  virtual void setupTrial() override;

 private:
  int clampsHint;
  std::vector<std::pair<double, double>> __clamps;
  void clamp(double& vM);
  void CCcopy(const VoltageClamp& toCopy);
  void mkmap();

  std::shared_ptr<Cell> __cell =
      std::make_shared<HRD09Control>();  // pointer to cell class
  std::unique_ptr<PvarsVoltageClamp> __pvars =
      std::unique_ptr<PvarsVoltageClamp>(new PvarsVoltageClamp(this));
  std::unique_ptr<MeasureManager>
      __measureMgr;  // set of measure class for measuring SV props.
};
}  // namespace LongQt

#endif

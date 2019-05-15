#ifndef COUPLEDINEXCITABLECELL_H
#define COUPLEDINEXCITABLECELL_H

#include "cell.h"
namespace LongQt {

class CoupledInexcitableCell : public Cell {
 public:
  CoupledInexcitableCell();
  CoupledInexcitableCell(const CoupledInexcitableCell& toCopy);
  ~CoupledInexcitableCell();

  CoupledInexcitableCell* clone() override;  // public copy function

  void updateIcab();
  void updateInab();
  void updateIkb();

  double caI = 9.22820165e-05;
  double caO = 1.8;
  double naI = 9.136;
  double naO = 140.0;
  double kI = 120.0;
  double kO = 5.4;

  double iCab = 0;
  double iNab = 0;
  double iKb = 0;

  double Icabfactor = 1;
  double Inabfactor = 1;
  double Ikbfactor = 1;

  void updateCurr() override;
  void updateConc() override;
  void makemap();

  const char* type() const override;

 private:
  void Initialize();
};
}  // namespace LongQt

#endif  // COUPLEDINEXCITABLECELL_H

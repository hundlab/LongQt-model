#include "coupledinexcitablecell.h"

using namespace LongQt;

CoupledInexcitableCell::CoupledInexcitableCell() : Cell() {
  this->Initialize();
}

CoupledInexcitableCell::CoupledInexcitableCell(
    const CoupledInexcitableCell &toCopy)
    : Cell(toCopy) {
  this->Initialize();
  CellKernel::copyVarPar(toCopy);
}

CoupledInexcitableCell::~CoupledInexcitableCell() {}

CoupledInexcitableCell *CoupledInexcitableCell::clone() {
  return new CoupledInexcitableCell(*this);
}

// Background Ca current
void CoupledInexcitableCell::updateIcab() {
  //  double maxicab;  // Ecan,
  //  double gamcai = 1.0;
  //  double gamcao = 0.341;
  //  double gcab = 1.995084E-7;  // 2.25084E-7; different from HRd08
  //  const double frt = FDAY / (RGAS * TEMP);

  //  //	Ecan=(RGAS*TEMP/(2*FDAY))*log(caO/caI);

  //  maxicab = 4 * (vOld) * (FDAY * frt) *
  //            (gamcai * caI * exp(2 * (vOld)*frt) - gamcao * caO) /
  //            (exp(2 * (vOld)*frt) - 1.0);

  //  iCab = Icabfactor * gcab * maxicab;

  double gcab = 1e-5;
  //  double gcab = 6.0643E-4;  // A/F
  double Eca = RGAS * TEMP / (2 * FDAY) * log(caO / caI);

  iCab = Icabfactor * (gcab * (vOld - Eca));
};

void CoupledInexcitableCell::updateInab() {
  double gnab = 0.597E-3;  // mS/uF

  double Ena = RGAS * TEMP / FDAY * log(naO / naI);

  iNab = Inabfactor * gnab * (vOld - Ena);
}

void CoupledInexcitableCell::updateIkb() {
  double gkb = 1.62e-3;  // mS/uF

  double Ek = RGAS * TEMP / FDAY * log(kO / kI);

  iKb = Ikbfactor * gkb * (vOld - Ek);
}

void CoupledInexcitableCell::updateCurr() {
  updateInab();
  updateIcab();
  updateIkb();
  iCat = iCab;
  iNat = iNab;
  iKt = iKb;

  iTotold = iTot;
  iTot = iNat + iCat + iKt;
}

void CoupledInexcitableCell::updateConc() {}

const char *CoupledInexcitableCell::type() const {
  return "Coupled Inexcitable Cell";
}

void CoupledInexcitableCell::Initialize() {
  Rcg = 2;
  makemap();
}

void CoupledInexcitableCell::makemap() {
  __vars["caI"] = &caI;
  __vars["naI"] = &naI;
  __vars["kI"] = &kI;
  __vars["iCab"] = &iCab;
  __vars["iNab"] = &iNab;
  __vars["iKb"] = &iKb;

  __pars["InabFactor"] = &Inabfactor;
  __pars["IkbFactor"] = &Ikbfactor;
  __pars["IcabFactor"] = &Icabfactor;
}

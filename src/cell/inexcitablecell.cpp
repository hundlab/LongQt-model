#include "inexcitablecell.h"
using namespace LongQt;
InexcitableCell::InexcitableCell() { this->Initialize(); }

InexcitableCell::InexcitableCell(const InexcitableCell &toCopy) : Cell(toCopy) {
  this->Initialize();
  CellKernel::copyVarPar(toCopy);
}

InexcitableCell::~InexcitableCell() {}

InexcitableCell *InexcitableCell::clone() { return new InexcitableCell(*this); }

void InexcitableCell::updateCurr() {}

void InexcitableCell::updateConc() {}

const char *InexcitableCell::type() const { return "Inexcitable Cell"; }

void InexcitableCell::Initialize() {}

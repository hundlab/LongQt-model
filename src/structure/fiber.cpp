#include "fiber.h"
using namespace LongQt;
using namespace std;

Fiber::Fiber(int size, CellUtils::Side dir) {
  this->setOrderedSides(dir);
  unsigned int i = static_cast<int>(nodes.size());
  nodes.resize(size, NULL);
  for (; i < nodes.size(); i++) {
    nodes[i] = make_shared<Node>();
  }
}
Fiber::Fiber(const Fiber &o) {
  this->nodes = o.nodes;
  this->directions = o.directions;
  /*
  this->nodes.resize(o.nodes.size());
  for(unsigned int i = 0; i < o.nodes.size(); ++i) {
      this->nodes[i] = make_shared<Node>(*o.nodes[i]);
  }*/
}
Fiber::~Fiber() {}

//#############################################################
// Solve PDE for Vm along fiber using tridiagonal solver.
//#############################################################
void Fiber::updateVm(const double &dt) {
  int i;
  int nn = static_cast<int>(nodes.size());
  /*  for(i = 0; i < nn; ++i) {
        nodes[i]->waitUnlock(0);
    }*/
  if (nn <= 1) {
    return;
  }

  for (i = 0; i < nn; i++) {
    double vOldp = i - 1 >= 0 ? nodes[i - 1]->cell->vOld : 0;
    double vOldc = nodes[i]->cell->vOld;
    double vOldn = i + 1 < nn ? nodes[i + 1]->cell->vOld : 0;
    d1[i] = B[i] * dt;
    d2[i] = -(B[i] * dt + B[i + 1] * dt + 2);
    d3[i] = B[i + 1] * dt;
    nodes[i]->cell->iTot -= B[i] * (vOldp - vOldc) - B[i + 1] * (vOldc - vOldn);
    r[i] = dt * (nodes[i]->cell->iTotold + nodes[i]->cell->iTot) /
               nodes[i]->cell->Cm -
           (d1[i] * vOldp + (d2[i] + 4) * vOldc + d3[i] * vOldn);
  }

  this->tridag();

  for (i = 0; i < nn; i++) {
    nodes[i]->cell->iTotold = nodes[i]->cell->iTot;
    nodes[i]->cell->dVdt = (nodes[i]->vNew - nodes[i]->cell->vOld) / dt;
    //##### Conservation for multicellular fiber ############
    nodes[i]->dIax = -(nodes[i]->cell->dVdt + nodes[i]->cell->iTot);
    nodes[i]->cell->iKt = nodes[i]->cell->iKt + nodes[i]->dIax;
    nodes[i]->cell->setV(nodes[i]->vNew);
    nodes[i]->lock[1] = false;
  }
}
shared_ptr<Node> Fiber::operator[](int pos) {
  if (0 <= pos && pos < nodes.size()) {
    return this->nodes[pos];
  } else {
    return shared_ptr<Node>();
  }
}
shared_ptr<Node> Fiber::at(int pos) { return this->nodes.at(pos); }

int Fiber::size() const { return nodes.size(); }

void Fiber::setup() {
  int nn = static_cast<int>(nodes.size());
  gam.resize(nn);
  d1.resize(nn);
  d2.resize(nn);
  d3.resize(nn);
  r.resize(nn);

  B.resize(nn + 1);
  for (unsigned int i = 0; i < nodes.size(); ++i) {
    B[i] = nodes[i]->getCondConst(directions[0]);
    B[i + 1] = nodes[i]->getCondConst(directions[1]);
  }
}

/*
//#############################################################
// Dynamic time step for one-dimensional fiber.
//#############################################################
int Fiber::tstep()
{
  int i,j;
  int vmflag=0;

  for(i=0;i<nn;i++){
     nodes[i]->cell->t=nodes[i]->cell->t+dt;
     nodes[i]->cell->dt=dt;
     if(nodes[i]->cell->dVdt>1.0||(nodes[i]->cell->t>(nodes[i]->cell->stimt-1.0)&&nodes[i]->cell->t<nodes[i]->cell->stimt))
        vmflag=2;
     else if(nodes[i]->cell->vOld>-50&&vmflag!=2)
        vmflag=1;
  }

  if(vmflag==2)
      dt=dtmin;
  else if(vmflag==1)
      dt=dtmed;
  else
      dt=dtmax;

  if(nodes[0]->cell->t>tMax)
     return 0;
  else
     return 1;
}*/

Fiber::FiberIterator Fiber::begin() const { return FiberIterator(this); }

Fiber::FiberIterator Fiber::end() const { return FiberIterator(0); }

void Fiber::setOrderedSides(CellUtils::Side s) {
  using namespace CellUtils;
  if (s == Side::top || s == Side::bottom) {
    this->directions = {Side::top, Side::bottom};
  } else {  // if(s == Side::left || s == Side::right) {
    this->directions = {Side::left, Side::right};
  }
}

void Fiber::tridag() {
  int j;
  double bet;

  int nn = static_cast<int>(nodes.size());
  if (d2[0] == 0.0) {
    Logger::getInstance()->write("Error 1 in tridag");
  }

  nodes[0]->vNew = r[0] / (bet = d2[0]);
  for (j = 1; j < nn; j++) {
    gam[j] = d3[j - 1] / bet;
    bet = d2[j] - d1[j] * gam[j];
    if (bet == 0.0) {
      Logger::getInstance()->write("Error 2 in tridag");
    }
    nodes[j]->vNew = (r[j] - d1[j] * nodes[j - 1]->vNew) / bet;
  }
  for (j = (nn - 2); j >= 0; j--) {
    nodes[j]->vNew -= gam[j + 1] * nodes[j + 1]->vNew;
  }
}

Fiber::FiberIterator::FiberIterator(const Fiber *parent) {
  if (!parent || parent->size() == 0) {
    pos = -1;
  }
}

Fiber::FiberIterator &Fiber::FiberIterator::operator++() {
  if (pos < 0) return *this;
  ++pos;
  if (pos >= parent->size()) {
    pos = -1;
  }
  return *this;
}

Fiber::FiberIterator Fiber::FiberIterator::operator++(int) {
  FiberIterator tmp(*this);
  operator++();
  return tmp;
}

Fiber::FiberIterator &Fiber::FiberIterator::operator--() {
  if (pos < 0) return *this;
  --pos;
  if (pos < 0) {
    pos = -1;
  }
  return *this;
}

Fiber::FiberIterator Fiber::FiberIterator::operator--(int) {
  FiberIterator tmp(*this);
  operator--();
  return tmp;
}

Fiber::FiberIterator &Fiber::FiberIterator::operator+=(int i) {
  if (pos < 0) return *this;
  pos += i;
  if (pos < 0 || pos >= parent->size()) {
    pos = -1;
  }
  return *this;
}

Fiber::FiberIterator &Fiber::FiberIterator::operator-=(int i) {
  if (pos < 0) return *this;
  pos -= i;
  if (pos < 0 || pos >= parent->size()) {
    pos = -1;
  }
  return *this;
}

Fiber::FiberIterator Fiber::FiberIterator::operator+(int i) {
  FiberIterator tmp(*this);
  tmp += i;
  return tmp;
}

Fiber::FiberIterator Fiber::FiberIterator::operator-(int i) {
  FiberIterator tmp(*this);
  tmp -= i;
  return tmp;
}

bool Fiber::FiberIterator::operator==(const Fiber::FiberIterator &rhs) const {
  return (pos == rhs.pos);
}

bool Fiber::FiberIterator::operator!=(const Fiber::FiberIterator &rhs) const {
  return !operator==(rhs);
}

std::shared_ptr<Node> Fiber::FiberIterator::operator*() {
  return (*parent)[pos];
}

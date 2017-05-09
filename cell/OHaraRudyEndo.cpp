#include "OHaraRudyEndo.h"

OHaraRudyEndo::OHaraRudyEndo() {
	this->Initialize();
};

OHaraRudyEndo::~OHaraRudyEndo() {};

OHaraRudyEndo::OHaraRudyEndo(OHaraRudyEndo& toCopy) : OHaraRudy(toCopy) {
    this->Initialize(); 
    CellKernel::copyVarPar(toCopy);
}

void OHaraRudyEndo::Initialize() {
	type = "Human Ventricular Endo (O'Hara-Rudy 2011)";
	celltype = OHaraRudy::endo;
	this->makemap();
}

OHaraRudyEndo* OHaraRudyEndo::clone() {
    return new OHaraRudyEndo(*this);
}

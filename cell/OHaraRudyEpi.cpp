#include "OHaraRudyEpi.h"

OHaraRudyEpi::OHaraRudyEpi() {
	this->Initialize();
};

OHaraRudyEpi::~OHaraRudyEpi() {};

OHaraRudyEpi::OHaraRudyEpi(OHaraRudyEpi& toCopy) : OHaraRudy(toCopy) {
    this->Initialize(); 
    CellKernel::copyVarPar(toCopy);
}

void OHaraRudyEpi::Initialize() {
	type = "Human Ventricular Epi (O'Hara-Rudy 2011)";
	celltype = OHaraRudy::epi;
	this->makemap();
}

OHaraRudyEpi* OHaraRudyEpi::clone() {
    return new OHaraRudyEpi(*this);
}

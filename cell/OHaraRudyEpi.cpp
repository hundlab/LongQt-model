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
	celltype = OHaraRudy::epi;
	this->makemap();
}

OHaraRudyEpi *OHaraRudyEpi::clone() {
    return new OHaraRudyEpi(*this);
}

const char *OHaraRudyEpi::type() const
{
    return "Human Ventricular Epicardium (O'Hara-Rudy 2011)";
}

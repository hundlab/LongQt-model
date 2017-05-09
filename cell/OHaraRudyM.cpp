#include "OHaraRudyM.h"

OHaraRudyM::OHaraRudyM() {
	this->Initialize();
};

OHaraRudyM::~OHaraRudyM() {};

OHaraRudyM::OHaraRudyM(OHaraRudyM& toCopy) : OHaraRudy(toCopy) {
    this->Initialize(); 
    CellKernel::copyVarPar(toCopy);
}

void OHaraRudyM::Initialize() {
	type = "Human Ventricular M (O'Hara-Rudy 2011)";
	celltype = OHaraRudy::M;
	this->makemap();
}

OHaraRudyM* OHaraRudyM::clone() {
    return new OHaraRudyM(*this);
}

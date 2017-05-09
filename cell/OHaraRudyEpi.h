#ifndef MODEL_OHARARUDY_EPI_H
#define MODEL_OHARARUDY_EPI_H

#include "OHaraRudy.h"  // Parent class declaration

class OHaraRudyEpi : public OHaraRudy
{
	public:
		OHaraRudyEpi();
		OHaraRudyEpi(OHaraRudyEpi& toCopy);
		~OHaraRudyEpi();

		void Initialize();
		virtual OHaraRudyEpi* clone();
};
#endif

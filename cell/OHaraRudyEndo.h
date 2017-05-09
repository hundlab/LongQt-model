#ifndef MODEL_OHARARUDY_ENDO_H
#define MODEL_OHARARUDY_ENDO_H

#include "OHaraRudy.h"  // Parent class declaration

class OHaraRudyEndo : public OHaraRudy
{
	public:
		OHaraRudyEndo();
		OHaraRudyEndo(OHaraRudyEndo& toCopy);
		~OHaraRudyEndo();

		void Initialize();
		virtual OHaraRudyEndo* clone();
};
#endif

#ifndef MODEL_OHARARUDY_M_H
#define MODEL_OHARARUDY_M_H

#include "OHaraRudy.h"  // Parent class declaration

class OHaraRudyM : public OHaraRudy
{
	public:
		OHaraRudyM();
		OHaraRudyM(OHaraRudyM& toCopy);
		~OHaraRudyM();

		void Initialize();
		virtual OHaraRudyM* clone();
};
#endif

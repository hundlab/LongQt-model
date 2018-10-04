#ifndef MODEL_OHARARUDY_ENDO_H
#define MODEL_OHARARUDY_ENDO_H

#include "OHaraRudy.h"  // Parent class declaration

class OHaraRudyEndo : public OHaraRudy
{
	public:
		OHaraRudyEndo();
		OHaraRudyEndo(OHaraRudyEndo& toCopy);
		~OHaraRudyEndo();

        virtual OHaraRudyEndo* clone() override;
        virtual const char* type() const;

    protected:
		virtual void Initialize();
};
#endif

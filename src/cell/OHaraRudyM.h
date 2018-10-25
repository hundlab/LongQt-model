#ifndef MODEL_OHARARUDY_M_H
#define MODEL_OHARARUDY_M_H

#include "OHaraRudy.h"  // Parent class declaration
namespace LongQt {

class OHaraRudyM : public OHaraRudy
{
	public:
		OHaraRudyM();
		OHaraRudyM(OHaraRudyM& toCopy);
		~OHaraRudyM();

        virtual OHaraRudyM* clone() override;
        virtual const char* type() const;

    protected:
		virtual void Initialize();
};
} //LongQt

#endif

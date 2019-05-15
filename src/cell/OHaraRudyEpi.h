#ifndef MODEL_OHARARUDY_EPI_H
#define MODEL_OHARARUDY_EPI_H

#include "OHaraRudy.h"  // Parent class declaration
namespace LongQt {

class OHaraRudyEpi : public OHaraRudy {
 public:
  OHaraRudyEpi();
  OHaraRudyEpi(OHaraRudyEpi& toCopy);
  ~OHaraRudyEpi();

  OHaraRudyEpi* clone() override;
  const char* type() const override;

 private:
  void Initialize();
};
}  // namespace LongQt

#endif

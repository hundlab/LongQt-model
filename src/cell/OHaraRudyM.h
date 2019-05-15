#ifndef MODEL_OHARARUDY_M_H
#define MODEL_OHARARUDY_M_H

#include "OHaraRudy.h"  // Parent class declaration
namespace LongQt {

class OHaraRudyM : public OHaraRudy {
 public:
  OHaraRudyM();
  OHaraRudyM(OHaraRudyM& toCopy);
  ~OHaraRudyM();

  OHaraRudyM* clone() override;
  const char* type() const;

 private:
  void Initialize();
};
}  // namespace LongQt

#endif

#ifndef MODEL_OHARARUDY_ENDO_H
#define MODEL_OHARARUDY_ENDO_H

#include "OHaraRudy.h"  // Parent class declaration
namespace LongQt {

class OHaraRudyEndo : public OHaraRudy {
 public:
  OHaraRudyEndo();
  OHaraRudyEndo(OHaraRudyEndo& toCopy);
  ~OHaraRudyEndo();

  OHaraRudyEndo* clone() override;
  const char* type() const;

 private:
  void Initialize();
};
}  // namespace LongQt

#endif

#ifndef INEXCITABLECELL_H
#define INEXCITABLECELL_H

#include "cell.h"
namespace LongQt {

class InexcitableCell : public Cell {
 public:
  InexcitableCell();
  InexcitableCell(const InexcitableCell& toCopy);
  ~InexcitableCell();

  InexcitableCell* clone() override;  // public copy function

  void updateCurr();
  void updateConc();

  const char* type() const;

 private:
  void Initialize();
};
}  // namespace LongQt

#endif  // INEXCITABLECELL_H

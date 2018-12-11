#ifndef PVARSGRID_H
#define PVARSGRID_H

#include <map>
#include <set>
#include "grid.h"
#include "pvarscell.h"
namespace LongQt {

class PvarsGrid : public PvarsCell {
 public:
  // Types
  struct MIonChanParam : PvarsCell::IonChanParam {
    int maxDist = 0;
    double maxVal = 100;
    std::set<std::pair<int, int>> startCells;     // row,col of start cells
    std::map<std::pair<int, int>, double> cells;  // map from row,col -> value

    virtual std::string str(std::string name) override;
    MIonChanParam() = default;
    MIonChanParam(const IonChanParam& other) : IonChanParam(other){};
    virtual ~MIonChanParam() = default;
  };

  // Functions
  PvarsGrid(Grid* grid);
  PvarsGrid(const PvarsGrid&, Grid* grid);
  virtual ~PvarsGrid() = default;

  virtual void setIonChanParams();
  virtual void calcIonChanParams();

  virtual void writePvars(QXmlStreamWriter& xml);
  virtual void readPvars(QXmlStreamReader& xml);
  void handlePvars(QXmlStreamReader& xml);
  void handlePvar(QXmlStreamReader& xml);
  std::pair<std::pair<int, int>, double> handleCell(QXmlStreamReader& xml,
                                                    bool& start);

  virtual void insert(std::string, IonChanParam);
  void setMaxDistAndVal(std::string varname, int maxDist, double maxVal);
  void setStartCells(std::string varname,
                     std::set<std::pair<int, int>> startCells);

 private:
  std::map<std::string, MIonChanParam*>* __pvars =
      (reinterpret_cast<std::map<std::string, MIonChanParam*>*>(&this->pvars));
  Grid* grid;

  void calcIonChanParam(MIonChanParam* param);
  void getNext();
  void add(std::pair<int, int> e, std::set<std::pair<int, int>>& next);

  std::set<std::pair<int, int>> current;
  std::set<std::pair<int, int>> visited;
};
}  // namespace LongQt

#endif

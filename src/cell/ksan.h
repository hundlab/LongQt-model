//#######################################################
// This header file contains class definition for
// Kharche San

// Kharche, Sanjay, et al. “A Mathematical Model of Action
// Potentials of Mouse Sinoatrial Node Cells with Molecular Bases.”
// American Journal of Physiology-Heart and Circulatory Physiology,
// vol. 301, no. 3, Sept. 2011, pp. H945–63,
// doi:10.1152/ajpheart.00143.2010.
//#######################################################

#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include "cell.h"  // Parent class declaration

#ifndef KMODEL_H
#define KMODEL_H

namespace LongQt {

//######################################################
// Define class for normal canine ventricular cell.
//######################################################
class Ksan : public Cell {
 public:
  Ksan();
  Ksan(Ksan& toCopy);
  ~Ksan();

  Ksan* clone() override;

  /*########################*/
  /*    DEFINE STRUCTS	  */
  /*########################*/

  struct GateVariable {  // Ion channel gates.
    double dst;
    double fst;
    double dt;
    double ft;
    double ikr_act;
    double ikr_inact;
    double ikr_inact2;
    double iks_act;
    double fl12;
    double dl12;
    double fl13;
    double dl13;
    double fca;
    double r;
    double q;
    double y_1_2;
    double m_ttxr;
    double h_ttxr;
    double j_ttxr;
    double m_ttxs;
    double h_ttxs;
    double j_ttxs;
    double open;
    double resting;
    double inactivated;
    double resting_inactivated;
  };

  //##################################################
  // Declare functions/variables unique to Control
  // class. All functions/variables declared in parent
  // class (Cell) are inherited by Control class.
  //##################################################
  // Declare class functions
  //##################################################
  void updateIst();
  void updateIb();
  void updateIK1();
  void updateICaT();
  void updateIKr();
  void updateIKs();
  void updateICaL();
  void updateINa();
  void updateIf();
  void updateIto();
  void updateIsus();
  void updateINaK();
  void updateiNaCa();
  void updateCurr() override;
  void updateConc() override;

  void makemap();
  const char* type() const override;

  //##### Declare class variables ##############

  double naI, naO, kI, kO, caI, caO;
  double caSub;
  double caJsr, caNsr;
  double mgI;
  double ist;
  double ibca, ibna, ibk;
  double ik1;
  double icat;

  double ikr;
  double iks;
  double ical12;
  double ical13;
  double ina_ttxr, ina_ttxs;
  double ih, ihk, ihna;
  double inak;
  double inaca;
  double isus;
  double ito;
  double Jrel, Jup, Jtr, Jcadif;
  double Ftc, Ftmc, Ftmm, Fcms, Fcmi, Fcq;

  double Vnsr;
  double Vjsr;
  double Vss;

  double TotCap;
  struct GateVariable Gate;

  double Istfactor;
  double Ibnafactor;
  double Ibcafactor;
  double Ibkfactor;
  double IK1factor;
  double ICaTfactor;
  double IKrfactor;
  double IKsfactor;
  double ICaL12factor;
  double ICaL13factor;
  double INafactorxr;
  double INafactorxs;
  double Iffactor;
  double Itofactor;
  double Isusfactor;
  double INaKfactor;
  double iNaCafactor;

 private:
  void Initialize();
};
}  // namespace LongQt

#endif

//#######################################################
// This header file contains class definition for
// Ten Tusscher Human Ventricular model

// ten Tusscher, KHWJ H. W. J., et al. “A Model for Human
// Ventricular Tissue.” American Journal of Physiology.
// Heart and Circulatory Physiology, vol. 286, no. 4,
// 2004, pp. H1573-89, doi:10.1152/ajpheart.00794.2003.
//#######################################################

#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include "cell.h"  // Parent class declaration

#ifndef MODEL_TNNP
#define MODEL_TNNP

namespace LongQt {

class TNNP04Control : public Cell {
 public:
  TNNP04Control();
  TNNP04Control(const TNNP04Control& toCopy);
  ~TNNP04Control();

  TNNP04Control* clone() override;
  void setup() override;

  /*########################*/
  /*    DEFINE STRUCTS	  */
  /*########################*/
  struct GateVariable {
    double g;
    double d;
    double f;
    double fca;
    double r;
    double s;
    double xs;
    double xr1;
    double xr2;
    double m;
    double h;
    double j;
  };

  //##################################################
  // Declare class functions
  //##################################################
  //###Concentration updating functions ######
  void updatecaI();
  void updatecaSr();
  void updatekI();
  void updatenaI();
  //####Current updating functions #######
  void updateSRcurrents();
  void updateIcal();
  void updateIcab();
  void updateIpca();
  void updateIto();
  void updateIks();
  void updateIkr();
  void updateIk1();
  void updateIpk();
  void updateInaca();
  void updateInak();
  void updateInab();
  void updateIna();
  void updateCurr() override;
  void updateConc() override;
  void externalStim(double stimval) override;
  void makemap();
  const char* type() const override;
  const char* citation() const override;

  //##### Declare class variables ##############
  double Vc;
  double Vsr;

  double tRel;
  bool sponRelflag;

  //###### Concentrations #########
  double naI;
  double kI;
  double caI;
  double naO;
  double kO;
  double caO;
  double caSr;
  double caSrbufc;
  double caIbufc;
  double caItot;
  //###### Currents #######
  double iLeak;
  double iUp;
  double iRel;
  double iCal;
  double iCab;
  double iPca;
  double iTo;
  double iKs;
  double iKr;
  double iK1;
  double ipK;
  double iNaca;
  double iNak;
  double iNab;
  double iNa;

  double iRelfactor;
  double ileakfactor;
  double Icalfactor;
  double Icabfactor;
  double Ipcafactor;
  double Itofactor;
  double Iksfactor;
  double Ikrfactor;
  double Ik1factor;
  double Ipkfactor;
  double Inacafactor;
  double Inakfactor;
  double Inabfactor;
  double Inafactor;

  bool isoflag = false;

  struct GateVariable Gate;

 private:
  void Initialize();
};
}  // namespace LongQt

#endif

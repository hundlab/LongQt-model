//###################################################
// This header file contains class definition for
// Hund-Rudy dynamic models of control and border
// zone canine epicardial ventricular myocytes as
// published in Christensen et al. PLoS. 2009;5:
// e1000583.

// Christensen, Matthew D., et al. “Oxidized Calmodulin
// Kinase II Regulates Conduction Following Myocardial
// Infarction: A Computational Analysis.” PLoS
// Computational Biology, vol. 5, no. 12, 2009, p.
// 1000583, doi:10.1371/journal.pcbi.1000583.

// Copyright (C) 2011 Thomas J. Hund.
//###################################################
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include "cell.h"  // Parent class declaration

#ifndef MODEL_H
#define MODEL_H
namespace LongQt {

//######################################################
// Define class for normal canine ventricular cell.
//######################################################
class HRD09Control : public Cell {
 public:
  HRD09Control();
  HRD09Control(const HRD09Control& toCopy);
  ~HRD09Control();
  HRD09Control* clone() override;
  void setup() override;

  /*########################*/
  /*    DEFINE STRUCTS	  */
  /*########################*/

  struct {        // Ion channel gates.
    double m;     // INa activation
    double h;     // INa inactivation
    double j;     // INa slow inactivation
    double ml;    // late INa activation
    double hl;    // late INa inactivation
    double d;     // ICa activation
    double f;     // ICa volt-dependent inactivation
    double fca;   // ICa Ca-dependent inactivation
    double fca2;  // ICa slow Ca-dependent inactivation
    double xr;    // IKr activation
    double r;     // IKr inactivation
    double xs;    // IKs activation
    double xs2;   // IKs slow activation
    double k1;    // IK1 activation
    double kp;    // IKp activation
    double a;     // Ito activation
    double i;     // Ito inactivation
    double i2;    // Ito slow inactivation
    double aa;    // Ito2 activation
  } Gate;

  struct {  // Rate constants for ion channel gates.
    double am;
    double bm;
    double aml;
    double bml;
    double ah;
    double bh;
    double aj;
    double bj;
    double ak1;
    double bk1;

  } Rate;

  //##################################################
  // Declare functions/variables unique to Control
  // class. All functions/variables declared in parent
  // class (Cell) are inherited by Control class.
  //##################################################
  // Declare class functions
  //##################################################
  void updateIna();
  void updateInal();
  void updateInak();
  void updateInaca();
  void updateIcab();
  void updateIclb();
  void updateInacl();
  void updateIlca();
  void updateIpca();
  void updateIks();
  void updateIkr();
  void updateIkcl();
  void updateIk1();
  void updateIkp();
  void updateIto();
  void updateIto2();
  void updateSrFlux();
  void updateIrel();
  void updateCaSub();
  void updateNai();
  void updateKi();
  void updateSr();
  void updateCli();
  void updateCai();
  void updateCamk();
  void updateItrek();
  void updateCurr() override;
  void updateConc() override;
  void externalStim(double stimval) override;
  void makemap();
  const char* type() const override;
  //##### Declare class variables ##############
  double naI, naO, kI, kO, caI, caO, clI, clO;  // Ion concentrations
  double iNa;                                   // Fast inward Na current
  double iCa;                                   // L-type Ca current
  double iNal;                                  // late Na current
  double iNak;                                  // Na-K pump
  double iNaca, iNacar;  // Na-Ca exchanger (20% located in subspace)
  double iNacl;          // Na-Cl cotransporter
  double iKcl;           // K-Cl cotransporter
  double iCab;           // Background Ca current

  double iTrek = 0;  // Trek-1
  double iTrek_na = 0;
  double iTrek_k = 0;

  double iPca;  // Sarcolemmal Ca2+ pump
  double iKs;   // Slowly activating delayed rectifier K current
  double iKr;   // Rapidly activating delayed rectifier K current
  double iK1;   // Inwardly rectifying K current
  double iKp;   // Plateau K current
  double iTo;   // Transient outward K+ current
  double iTo2;  // Ca-dependent transient outward Cl- current
  double iClb;  // background Cl- current

  double iClt;  // total Cl- current

  double caR, caRt, iDiff;  // Ca in restricted space.
  double iUp, iLeak, iTr;   // Ca uptake, leak and translocation in SR

  double iRel;  // Ca2+ release from SR
  double gRel;
  double ryRopen;
  int sponRelflag;
  double sponRelT;
  double tRel;

  double trpn, cmdn, csqn, caJsr, caNsr, caTotal, caJsrtotal;

  double caM, fBound, fPhos, fOx, fOxP, fBlock, fI;  // CaMK activity

  double Vmito;
  double Vsr;
  double Vnsr;
  double Vjsr;
  double Vss;

  double Inafactor;
  double Inalfactor;
  double Inakfactor;
  double Inacafactorss;
  double Inacafactorbulk;
  double Icabfactor;
  double Iclbfactor;
  double Inaclfactor;
  double Ilcafactor;
  double Ipcafactor;
  double Iksfactor;
  double Ikrfactor;
  double Ikclfactor;
  double Ik1factor;
  double Ikpfactor;
  double Itofactor;
  double Ito2factor;
  double Irelfactor;
  double Itrfactor;
  double Iupfactor;
  double Ileakfactor;
  double ItrekFactor = 1;  // Two-pore K+ channel

  double TestFactor = 0;
  double Test2Factor = 1;

  //    TREK //include the trek channel
  MAKE_OPTIONS(TREK)

  enum Options opts = Options::WT;

 protected:
  void Initialize();
};

class HRD09BorderZone : public HRD09Control {
 public:
  HRD09BorderZone();
  HRD09BorderZone(const HRD09BorderZone& toCopy);
  ~HRD09BorderZone();

  HRD09BorderZone* clone() override;
  void setup() override;

  //##################################################
  // Declare functions that will be overwritten in
  // HRD09BorderZone class. All other functions/variables
  // are inherited from Control class.
  //##################################################
  void updateIna();
  void updateIcab();
  void updateIlca();
  void updateIk1();
  void updateIto();
  void updateCamk();
  const char* type() const override;

 private:
  void Initialize();
};

}  // namespace LongQt

#endif

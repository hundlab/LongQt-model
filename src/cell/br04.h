//#######################################################
// This header file contains class definition for
// Bondarenko model of mouse ventricular myocytes
//
// Bondarenko, V. E. “Computer Model of Action Potential of
// Mouse Ventricular Myocytes.” AJP: Heart and Circulatory
// Physiology, vol. 287, no. 3, 2004, pp. H1378–403,
// doi:10.1152/ajpheart.00185.2003.
//#######################################################

#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include "cell.h"  // Parent class declaration

#ifndef MODEL_BR
#define MODEL_BR

namespace LongQt {

//######################################################
// Define class for normal canine ventricular cell.
//######################################################
class Br04 : public Cell {
 public:
  Br04();
  Br04(Br04& toCopy);
  virtual ~Br04();

  Br04* clone() override;

  /*########################*/
  /*    DEFINE STRUCTS	  */
  /*########################*/

  struct {                            // Ion channel gates.
    double a_to_f = 0.002599117811;   // Ito activation
    double i_to_f = 0.999978269;      // Ito inactivation
    double a_to_s = 0.0004053390336;  // Ito activation
    double i_to_s = 0.9985979445;     // Ito inactivation
    double a_ur = 0.0004053390336;    // Ito activation
    double i_ur = 0.9985979435;       // Ito inactivation
    double a_Kss = 0.0004052991806;   // Ito activation
    double i_Kss = 1;                 // Ito inactivation
    double nks = 0.000262753;         // IKs slow activation
  } gate;

  //##################################################
  // Declare functions/variables unique to Br04
  // class. All functions/variables declared in parent
  // class (Cell) are inherited by Br04 class.
  //##################################################
  // Declare class functions
  //##################################################
  virtual void updateIlca();
  virtual void updateIcab();
  virtual void updateIpca();
  virtual void updateIto_f();
  virtual void updateIto_s();
  virtual void updateIk1();
  virtual void updateIks();
  virtual void updateIkur();
  virtual void updateIkss();
  virtual void updateIkr();
  virtual void updateItrek();
  virtual void updateInak();
  virtual void updateInaca();
  virtual void updateIclca();
  virtual void updateIna();
  virtual void updateInab();
  virtual void updateIns();
  virtual void updateCaFlux();
  virtual void updateCai();
  virtual void updateCaSs();
  virtual void updateCaJsr();
  virtual void updateCaNsr();
  virtual void updateNai();
  virtual void updateKi();
  virtual void updateCurr();
  virtual void updateConc();
  virtual double tstep(double stimt) override;
  virtual void externalStim(double stimval);
  //    virtual int tstep();
  virtual void makemap();
  virtual const char* type() const;
  //##### Declare class variables ##############
  // Ion concentrations, mM
  double naI = 9.309480051;
  double naO = 140;
  double kI = 150.429545;
  double kO = 5.4;
  double caI = 0.00008556724119;
  double caO = 1.8;

  double caSs = 0.0001101446222;  // Ca in restricted space, mM.

  double caTrpn_low = 0.07 * (caI / (caI + 0.0006));
  double caTrpn_high = 0.140 * (caI / (caI + 0.0000135));
  double csqn = 8.990557096;
  double caJsr = 1.646733367;
  double caNsr = 1.666917661;

  double iCal = -9.789053191e-12;  // L-type Ca current, uA/uF
  double iCab = -0.0673416383;     // Background Ca current, uA/uF
  double iPca = 0.02314071789;     // Sarcolemmal Ca2+ pump, uA/uF
  double iTo_f = 8.949536773e-09;  // Transient outward K+ current, uA/uF
  double iTo_s = 0;                // Transient outward K+ current, uA/uF
  double iK1 = 0.1998424643;       // Inwardly rectifying K current, uA/uF
  double iKs =
      5.639014346e-10;  // Slowly activating delayed rectifier K current, uA/uF
  double iKur = 0.0001261730481;  // ultrarapidly activating delayed rectifier K
                                  // current, uA/uF
  double iKss =
      3.017113218e-05;  // noninactivating steady-state K current, uA/uF
  double iKr = -0.0001235205802;    // Rapidly activating delayed rectifier K
                                    // current, uA/uF
  double iTrek = 0;                 // Trek-1
  double iNak = 0.09827839264;      // Na-K pump, uA/uF
  double iNaca = -0.02204080456;    // Na-Ca exchanger (20% located in subspace)
  double iClca = -5.842015343e-08;  // Na-Cl cotransporter, mM/ms
  double iNa = -0.001086915143;     // Fast inward Na current, uA/uF
  double iNab = -0.2262588962;      // Fast inward Na current, uA/uF
  double iNsk = 0;   // Fast inward Na current, uA/uF // ?Plateau K current?
  double iNsna = 0;  // Fast inward Na current, uA/uF // ?Plateau K current?

  double iCait;
  double iCasst;

  double J_xfer = -8.416376862e-15;
  double J_up = 2.08266461e-05;
  double J_leak = 2.082344545e-05;
  double J_trpn = 0;
  double J_rel = 1.37501693e-15;
  double J_tr = 1.28586299e-07;

  double O = 3.887453609e-13;
  double OK = 0.0001694387641;
  double C1 = 1;  // skip
  double C2 = 0.003150990626;
  double C3 = 3.735061326e-06;
  double C4 = 1.967731529e-09;
  double I1 = 9.87803846e-13;
  double I2 = 2.207607468e-10;
  double I3 = 5.609577541e-10;
  double IK = 3.046848211e-05;
  double CNa3 = 1;  // skip
  double CNa2 = 0.02064874485;
  double CNa1 = 0.0002707715442;
  double CK0 = 1;  // skip
  double CK1 = 0.0009763518665;
  double CK2 = 0.0006307873071;
  double ONa = 6.725398316e-07;
  double IFNa = 0.0001413951314;
  double I1Na = 5.96397459e-07;
  double I2Na = 1.323937657e-09;
  double ICNa2 = 0.01078263966;
  double ICNa3 = 0.3321288129;
  //####### RyR channel fractions in kinetic states
  double PC1 = 0.9998292337;
  double PC2 = 0.0001568252687;
  double PO1 = 1.394099483e-05;
  double PO2 = 7.818281895e-11;
  double PRyr = 5.494373473e-11;

  double tRel = 10000.0;
  bool sponRelflag = false;

  double Vnsr = 2.098E-6;  // uL
  double Vjsr = 0.12E-6;   // 0.12E-6; //uL
  double Vss = 1.485E-9;   // 3.482E-7;//1.485E-9; //uL

  //############ Factors #############
  double IcalFactor = 1;
  double IcabFactor = 1;
  double ItofFactor = 1;
  double ItosFactor = 1;
  double Ik1Factor = 1;
  double IksFactor = 1;
  double IkurFactor = 1;
  double IkssFactor = 1;
  double IkrFactor = 1;
  double ItrekFactor = 1;
  double InaFactor = 1;
  double InabFactor = 1;
  double InakFactor = 1;
  double InacaFactor = 1;
  double IclcaFactor = 1;
  double InskFactor = 1;
  double InsnaFactor = 1;
  double JrelFactor = 1;
  double JleakFactor = 1;
  double JxferFactor = 1;
  double JupFactor = 1;
  double JtrFactor = 1;
  double JtrpnFactor = 1;

  double TestFactor = 1;
  double TestFactor2 = 1;
  //    ISO //isoproterenol
  //    TREK //enable the trek channel
  //    INS //?Plateau K current?
  MAKE_OPTIONS(ISO, TREK /*, INS*/);

  enum Options opts = WT;

 private:
  void Initialize();
};
}  // namespace LongQt

#endif

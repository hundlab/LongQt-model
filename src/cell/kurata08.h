//#########################################################
// Class declaration for rabbit sinoatrial node cell model
// (central) published in Kurata et al. Biophys J.
// 2008;95:951.
//
// Kurata, Yasutaka, et al. “Regional Difference in
// Dynamical Property of Sinoatrial Node Pacemaking:
// Role of Na+channel Current.” Biophysical Journal, vol.
// 95, no. 2, 2008, pp. 951–77, doi:10.1529/biophysj.107.112854.
//#########################################################
#ifndef MODELSA_H
#define MODELSA_H

#include "cell.h"  // Parent class declaration
namespace LongQt {

//######################################################
// Define class for central rabbit SAN cell.
//######################################################
class Kurata08 : public Cell {
 public:
  Kurata08();
  Kurata08(const Kurata08& toCopy);
  ~Kurata08();

  Kurata08* clone() override;


  /*########################*/
  /*    DEFINE STRUCTS      */
  /*########################*/

  struct {       // Ion channel gates.
    double qa;   // Ist activation
    double qi;   // Ist inactivation
    double d;    // ICa activation
    double f;    // ICa volt-dependent inactivation
    double fca;  // ICa Ca-dependent inactivation
    double dt;   // T-type ICa activation
    double ft;   // T-type ICa volt-dependent inactivation
    double paf;  // IKr activation
    double pas;  // IKr activation
    double pi;   // IKr inactivation
    double n;    // IKs activation
    double q;    // Ito activation
    double r;    // Ito/Isus inactivation
    double y;    // Ih activation
  } Gate;

  //##################################################
  // Declare functions/variables unique to Central SAN
  // class. All functions/variables declared in parent
  // class (Cell) are inherited by Control class.
  //##################################################
  // Declare class functions
  //##################################################
  void updateIst();
  void updateInab();
  void updateIcal();
  void updateIcatt();
  void updateIks();
  void updateIkr();
  void updateI4ap();
  void updateItrek();
  void updateIkach();
  void updateInak();
  void updateInaca();
  void updateIh();
  void updateIup();
  void updateItr();
  void updateIdiff();
  void updateIrel();
  void updateCar();
  void updateCasr();
  void updateNai();
  void updateKi();
  void updateCai();
  void updateCurr();
  void updateConc();
  void externalStim(double stimval);
  const char* type() const;

  //##### Declare additional class variables ##############
  double naI, kI, caI;  // Intracellular Ion concentrations
  double iSt;           // Sustained inward current
  double iNab;          // Na+ background current
  double iCal;          // L-type Ca current
  double iCatt;         // T-type Ca current
  double iKs;           // Slowly activating delayed rectifier K current
  double iKr;           // Rapidly activating delayed rectifier K current
  double iTo;           // Transient outward K+ current
  double iTrek;         // Trek-1
  double iSus;          // Sustained 4-AP sensitive K+ current
  double iKach;         // Muscarinic K+ current
  double iNak;          // Na-K pump
  double iNaca;         // Na-Ca exchanger
  double iHna;          // Funny current, Na+ component
  double iHk;           // Funny current, K+ component
  double iH;            // Total funny current

  double iCait, iCart;
  double caR, iDiff;  // Ca in restricted space.
  double caJsr, caNsr;
  double iRel, iUp, iTr;  // Ca uptake, leak and translocation in SR
  double trpnCa, trpnMg, trpnMgmg, cmdnI, cmdnR, csqn;  // buffers

  //##### Declare additional class params ##############
  double naO, kO, caO;  // Fixed extracellular ion concentrations

  double istFactor;    // Sustained inward current
  double inabFactor;   // Na+ background current
  double icalFactor;   // L-type Ca current
  double icattFactor;  // T-type Ca current
  double iksFactor;    // Slowly activating delayed rectifier K current
  double ikrFactor;    // Rapidly activating delayed rectifier K current
  double itoFactor;    // Transient outward K+ current
  double itrekFactor;  // Two-pore K+ channel
  double isusFactor;   // Sustained 4-AP sensitive K+ current
  double ikachFactor;  // Muscarinic K+ current
  double inakFactor;   // Na-K pump
  double inacaFactor;  // Na-Ca exchanger
  double ihFactor;     // Total funny current

  double irelFactor;
  double iupFactor;
  double isoConc;

  double Vnsr;
  double Vjsr;
  double Vss;

  //    ISO //isoproterenol
  //    TREK //include the trek channel
  MAKE_OPTIONS(TREK)  //,ISO

  enum Options opts;

 private:
  void Initialize();
  void makemap();
};
}  // namespace LongQt

#endif

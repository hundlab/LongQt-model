//#######################################################
// This header file contains class definition for
// Bondarenko model of mouse ventricular myocytes
//
// Bondarenko, V. E. “Computer Model of Action Potential of
// Mouse Ventricular Myocytes.” AJP: Heart and Circulatory
// Physiology, vol. 287, no. 3, 2004, pp. H1378–403,
// doi:10.1152/ajpheart.00185.2003.
//#######################################################

#include "cell.h"  // Parent class declaration
#include <cmath>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>

#ifndef MODEL_BR
#define MODEL_BR

namespace LongQt {

//######################################################
//Define class for normal canine ventricular cell.
//######################################################
class Br04 : public Cell
{
  public:
    Br04();
    Br04(Br04& toCopy);
    virtual ~Br04();
    
    Br04* clone() override;


 /*########################*/
/*    DEFINE STRUCTS	  */
/*########################*/

    struct GateVariable {  //Ion channel gates.
	double a_to_f;	// Ito activation
	double i_to_f;	// Ito inactivation
	double a_to_s;	// Ito activation
	double i_to_s;	// Ito inactivation
	double a_ur;	// Ito activation
	double i_ur;	// Ito inactivation
	double a_Kss;	// Ito activation
	double i_Kss;	// Ito inactivation
	double nks;	// IKs slow activation
   };


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
    virtual int externalStim(double stimval);
//    virtual int tstep();
    virtual void makemap();
    virtual const char* type() const;
    //##### Declare class variables ##############
    double naI,naO,kI,kO,caI,caO;  // Ion concentrations, mM
    
    double iCal;		  // L-type Ca current, uA/uF
    double iCab;	  // Background Ca current, uA/uF
    double iPca;  // Sarcolemmal Ca2+ pump, uA/uF
    double iTo_f;   // Transient outward K+ current, uA/uF
    double iTo_s;   // Transient outward K+ current, uA/uF
    double iK1;	  // Inwardly rectifying K current, uA/uF
    double iKs;   // Slowly activating delayed rectifier K current, uA/uF
    double iKur;   // Slowly activating delayed rectifier K current, uA/uF
    double iKss;   // Slowly activating delayed rectifier K current, uA/uF
    double iKr;   // Rapidly activating delayed rectifier K current, uA/uF
    double iNak;          // Na-K pump, uA/uF
    double iNaca; // Na-Ca exchanger (20% located in subspace)
    double iClca;	  // Na-Cl cotransporter, mM/ms
    double iNa;           // Fast inward Na current, uA/uF
    double iNab;           // Fast inward Na current, uA/uF
    double iNsk;           // Fast inward Na current, uA/uF
    double iNsna;           // Fast inward Na current, uA/uF
    
    double iCait;
    double iCasst;

    double J_xfer;
    double J_up;
    double J_leak;
    double J_trpn;
    double J_rel;
    double J_tr;
    
    double O;
        double OK;
        double C1;  //skip
        double C2;
        double C3;
        double C4;
        double I1;
        double I2;
        double I3;
        double IK;
        double CNa3;  //skip
        double CNa2;
        double CNa1;
        double CK0;   //skip
        double CK1;
        double CK2;
        double ONa;
        double IFNa;
        double I1Na;
        double I2Na;
        double ICNa2;
        double ICNa3;
//####### RyR channel fractions in kinetic states
        double PC1;
        double PC2;
        double PO1;
        double PO2;
        double PRyr;

    double caSs;  // Ca in restricted space, mM.
    
    double caTrpn_low,caTrpn_high,csqn,caJsr,caNsr;
    double tRel;
    bool sponRelflag;
   
    double Vnsr;
    double Vjsr;
    double Vss;

    struct GateVariable gate;

    //    ISO //isoproterenol
    MAKE_OPTIONS(ISO);

    enum Options opts;

    protected:
    virtual void Initialize();
};
} //LongQt

#endif

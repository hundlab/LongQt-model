//#######################################################
// This header file contains class definition for
// Hund-Rudy dynamic models of control and border
// zone canine epicardial ventricular myocytes as 
// published in Hund et al. J Mol Cell Cardiol. 2008.

// Copyright (C) 2011 Thomas J. Hund.
//#######################################################

#include "cell.h"  // Parent class declaration
#include <cmath>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

#ifndef MODEL_BR
#define MODEL_BR

  
//######################################################
//Define class for normal canine ventricular cell.
//######################################################
class Br04 : public Cell
{
  public:
    Br04();
    Br04(Br04& toCopy);
    virtual ~Br04();
    
    void Initialize();
    Br04* clone();


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
    virtual int stim();
    virtual int tstep();
    virtual map<string, double*> makemap();
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
    double tRel, sponRelflag;
    double isoFlag;
   
    double Vnsr;
    double Vjsr;
    double Vss;

    struct GateVariable gate;
};

#endif

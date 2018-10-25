/* The Luo-Rudy Dynamic (LRd) Model of the Mammalian Ventricular Myocyte */
/* Gregory Faber */
/* This code requires a C++ compiler */
/* Detailed list of equations and model description are provided in */

/*  Circ Res 1991;68:1501-1526      */
/*  Circ Res 1994;74:1071-1096      */
/*  Circ Res 1994;74:1097-1113      */
/*  Circ Res 1995;77:140-152        */
/*  Biophys J 1995;68:949-964       */
/*  Cardiovasc Res 1997;35:256-272  */
/*  Circulation 1999;99:2466-2474   */
/*  Cardiovas Res 1999;42:530-542   */
/*  Nature 1999;400:566-569         */
/*  Circulation 2000;101:1192-1198  */
/*  Biophy J 2000;78:2392-2404      */

/*  Created by Shana Bee on 2/17/17. */
/*  Copyright © 2017 Shana Bee. All rights reserved. */

#ifndef  FABER_RUDY
#define FABER_RUDY

#include "cell.h"
#include <cmath>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
namespace LongQt {

//#define bcl 300   // Basic Cycle Length (ms)
//#define beats 20   // Number of Beats

class FR : public Cell {
    public:
        FR();
        FR(FR& toCopy);
        virtual ~FR();
    
    void Initialize();
    FR* clone();

    //##### Declare class variables ##############
    
double iKtStim;
/* Cell Geometry */
const double l = 0.01;       // Length of the cell (cm)
const double a = 0.0011;     // Radius of the cell (cm)
const double pi = 3.141592;  // Pi
//double cellLength;       // Length of the cell (cm)
//double cellRadius;     // Radius of the cell (cm)
//double Vcell;   // Cell volume (uL)
//double AGeo;    // Geometric membrane area (cm^2)
//double ACap;    // Capacitive membrane area (cm^2)
//double Vmyo;    // Myoplasm volume (uL)
double vmito;   // Mitochondria volume (uL)
double vsr;     // SR volume (uL)
double Vnsr;    // NSR volume (uL)
double Vjsr;    // JSR volume (uL)
double vcleft;  // Cleft volume (uL)

/* Voltage */
//double vOld;       // Membrane voltage (mV)
//double vNew;    // New Voltage (mV)
//double dVdt;    // Change in Voltage / Change in Time (mV/ms)
//double dvdtnew; // New dv/dt (mV/ms)
double boolien; // Boolien condition to test for dvdtmax


/* Time Step */
//double dt;      // Time step (ms)                   B B
//double t;       // Time (ms)

int i;                        // Stimulus Counter

/* Ion Valences */
double zna;  // Na valence
double zk;   // K valence
double zca;  // Ca valence

/* Ion Concentrations */
double nai;    // Intracellular Na Concentration (mM)
double nao;    // Extracellular Na Concentration (mM)
double nabm;   // Bulk Medium Na Concentration (mM)
double dnao;   // Change in Cleft Na Concentration (mM)
double ki;     // Intracellular K Concentration (mM)
double ko;     // Extracellular K Concentration (mM)
double kbm;    // Bulk Medium K Concentration (mM)
double dko;    // Change in Cleft K Concentration (mM)
double cai;    // Intracellular Ca Concentration (mM)
double cao;    // Extracellular Ca Concentration (mM)
double cabm;   // Bulk Medium Ca Concentration (mM)
double dcao;   // Change in Cleft Ca Concentration (mM)
double cmdn;   // Calmodulin Buffered Ca Concentration (mM)
double trpn;   // Troponin Buffered Ca Concentration (mM)
double nsr;    // NSR Ca Concentration (mM)
double jsr;    // JSR Ca Concentration (mM)
double csqn;   // Calsequestrin Buffered Ca Concentration (mM)
double taudiff; // Diffusion Constant for Ion Movement from Bulk Medium to Cleft Space

/* Myoplasmic Na Ion Concentration Changes */
//double iNat;  // Total Na Ion Flow (uA/uF)
double dnai;    // Change in Intracellular Na Concentration (mM)

/* Myoplasmic K Ion Concentration Changes */
//double iKt; // Total K Ion Flow (uA/uF)
double dki;   // Change in Intracellular K Concentration (mM)

/* NSR Ca Ion Concentration Changes */
double dnsr;   // Change in [Ca] in the NSR (mM)
double iup;    // Ca uptake from myo. to NSR (mM/ms)
double ileak;  // Ca leakage from NSR to myo. (mM/ms)
double kleak;  // Rate constant of Ca leakage from NSR (ms^-1)
double kmup;    // Half-saturation concentration of iup (mM)
double iupbar;  // Max. current through iup channel (mM/ms)
double nsrbar;       // Max. [Ca] in NSR (mM)

/* JSR Ca Ion Concentration Changes */
//not included in map
double djsr;                    // Change in [Ca] in the JSR (mM)
double tauon;         // Time constant of activation of Ca release from JSR (ms)
double tauoff;        // Time constant of deactivation of Ca release from JSR (ms)
double tcicr;                   // t=0 at time of CICR (ms)
double irelcicr;                // Ca release from JSR to myo. due to CICR (mM/ms)
double csqnth;     // Threshold for release of Ca from CSQN due to JSR overload (mM)
double gmaxrel;     // Max. rate constant of Ca release from JSR due to overload (ms^-1)
double grelbarjsrol;            // Rate constant of Ca release from JSR due to overload (ms^-1)
double greljsrol;               // Rate constant of Ca release from JSR due to CICR (ms^-1)
double tjsrol;                  // t=0 at time of JSR overload (ms)
double ireljsrol;               // Ca release from JSR to myo. due to JSR overload (mM/ms)
double csqnbar;      // Max. [Ca] buffered in CSQN (mM)
double kmcsqn;      // Equalibrium constant of buffering for CSQN (mM)
double bjsr;                    // b Variable for analytical computation of [Ca] in JSR (mM)
double cjsr;                    // c Variable for analytical computation of [Ca] in JSR (mM)
double on;                      // Time constant of activation of Ca release from JSR (ms)
double off;                     // Time constant of deactivation of Ca release from JSR (ms)
double magrel;                  // Magnitude of Ca release
double dcaiont;                 // Rate of change of Ca entry
double dcaiontnew;              // New rate of change of Ca entry
double caiontold;               // Old rate of change of Ca entry

/* Translocation of Ca Ions from NSR to JSR */
    //not included
double itr;                // Translocation current of Ca ions from NSR to JSR (mM/ms)
double tautr;  // Time constant of Ca transfer from NSR to JSR (ms)

/* Myoplasmic Ca Ion Concentration Changes */
    //not included
//double iCat;  // Total Ca Ion Flow (uA/uF)
double dcai;    // Change in myoplasmic Ca concentration (mM)
double catotal; // Total myoplasmic Ca concentration (mM)
double bmyo;    // b Variable for analytical computation of [Ca] in myoplasm (mM)
double cmyo;    // c Variable for analytical computation of [Ca] in myoplasm (mM)
double dmyo;    // d Variable for analytical computation of [Ca] in myoplasm (mM)
double gpig;    // Tribute to all the guinea pigs killed for the advancement of knowledge
double cmdnbar;   // Max. [Ca] buffered in CMDN (mM)
double trpnbar;   // Max. [Ca] buffered in TRPN (mM)
double kmcmdn;  // Equalibrium constant of buffering for CMDN (mM)
double kmtrpn;   // Equalibrium constant of buffering for TRPN (mM)

/* Fast Sodium Current (time dependant) */
double ina;    // Fast Na Current (uA/uF)
double gna;    // Max. Conductance of the Na Channel (mS/uF)
double ena;    // Reversal Potential of Na (mV)
double am;     // Na alpha-m rate constant (ms^-1)
double bm;     // Na beta-m rate constant (ms^-1)
double ah;     // Na alpha-h rate constant (ms^-1)
double bh;     // Na beta-h rate constant (ms^-1)
double aj;     // Na alpha-j rate constant (ms^-1)
double bj;     // Na beta-j rate constant (ms^-1)
double m;      // Na activation
double h;      // Na inactivation
double j;      // Na inactivation

/* Current through L-type Ca Channel */
double ilca;    // Ca current through L-type Ca channel (uA/uF)
double ilcana;  // Na current through L-type Ca channel (uA/uF)
double ilcak;  // K current through L-type Ca channel (uA/uF)
double ilcatot; // Total current through the L-type Ca channel (uA/uF)
double ibarca;  // Max. Ca current through Ca channel (uA/uF)
double ibarna;  // Max. Na current through Ca channel (uA/uF)
double ibark;   // Max. K current through Ca channel (uA/uF)
double d;       // Voltage dependant activation gate
double dss;     // Steady-state value of activation gate d
double taud;    // Time constant of gate d (ms^-1)
double f;       // Voltage dependant inactivation gate
double fss;     // Steady-state value of inactivation gate f
double tauf;    // Time constant of gate f (ms^-1)
double fca;     // Ca dependant inactivation gate
double kmca;     // Half-saturation concentration of Ca channel (mM)
double pca;     // Permiability of membrane to Ca (cm/s)
double gacai;         // Activity coefficient of Ca
double gacao;     // Activity coefficient of Ca
double pna; // Permiability of membrane to Na (cm/s)
double ganai;      // Activity coefficient of Na
double ganao;      // Activity coefficient of Na
double pk;  // Permiability of membrane to K (cm/s)
double gaki;       // Activity coefficient of K
double gako;       // Activity coefficient of K

/* Current through T-type Ca Channel */
double icat;    // Ca current through T-type Ca channel (uA/uF)
double gcat;    // Max. Conductance of the T-type Ca channel (mS/uF)
double eca;     // Reversal Potential of the T-type Ca channel (mV)
double b;       // Voltage dependant activation gate
double bss;     // Steady-state value of activation gate b
double taub;    // Time constant of gate b (ms^-1)
double g;       // Voltage dependant inactivation gate
double gss;     // Steady-state value of inactivation gate g
double taug;    // Time constant of gate g (ms^-1)

/* Rapidly Activating Potassium Current */
double ikr;   // Rapidly Activating K Current (uA/uF)
double gkr;   // Channel Conductance of Rapidly Activating K Current (mS/uF)
double ekr;   // Reversal Potential of Rapidly Activating K Current (mV)
double xr;    // Rapidly Activating K time-dependant activation
double xrss;  // Steady-state value of inactivation gate xr
double tauxr; // Time constant of gate xr (ms^-1)
double r;     // K time-independant inactivation

/* Slowly Activating Potassium Current */
double iks;   // Slowly Activating K Current (uA/uF)
double gks;   // Channel Conductance of Slowly Activating K Current (mS/uF)
double eks;   // Reversal Potential of Slowly Activating K Current (mV)
double xs1;    // Slowly Activating K time-dependant activation
double xs1ss;  // Steady-state value of inactivation gate xs1
double tauxs1; // Time constant of gate xs1 (ms^-1)
double xs2;    // Slowly Activating K time-dependant activation
double xs2ss;  // Steady-state value of inactivation gate xs2
double tauxs2; // Time constant of gate xs2 (ms^-1)
double prnak;  // Na/K Permiability Ratio

/* Potassium Current (time-independant) */
double iki;    // Time-independant K current (uA/uF)
double gki;    // Channel Conductance of Time Independant K Current (mS/uF)
double eki;    // Reversal Potential of Time Independant K Current (mV)
double aki;    // K alpha-ki rate constant (ms^-1)
double bki;    // K beta-ki rate constant (ms^-1)
double kin;    // K inactivation

/* Plateau Potassium Current */
double ikp;    // Plateau K current (uA/uF)
double gkp;    // Channel Conductance of Plateau K Current (mS/uF)
double ekp;    // Reversal Potential of Plateau K Current (mV)
double kp;     // K plateau factor

/* Na-Activated K Channel */
double ikna;   // Na activated K channel
double pona;   // Open probability dependant on Nai
double pov;    // Open probability dependant on Voltage
double ekna;   // Reversal potential
double gkna;   // Maximum conductance (mS/uF)
double nkna;       // Hill coefficient for Na dependance
double kdkna;       // Dissociation constant for Na dependance(mM)

/* ATP-Sensitive K Channel */
double ikatp;    // ATP-sensitive K current (uA/uF)
double ekatp;    // K reversal potential (mV)
double gkbaratp; // Conductance of the ATP-sensitive K channel (mS/uF)
double gkatp;    // Maximum conductance of the ATP-sensitive K channel (mS/uF)
double patp;     // Percentage availibility of open channels
double natp;          // K dependence of ATP-sensitive K current
double nicholsarea; // Nichol's ares (cm^2)
double atpi;             // Intracellular ATP concentraion (mM)
double hatp;             // Hill coefficient
double katp;         // Half-maximal saturation point of ATP-sensitive K current (mM)

/* Ito Transient Outward Current (Dumaine et al. Circ Res 1999;85:803-809) */
double ito;	  // Transient outward current
double gitodv;	  // Maximum conductance of Ito
double ekdv;	  // Reversal Potential of Ito
double rvdv;      // Time independant voltage dependence of Ito
double zdv;       // Ito activation
double azdv;      // Ito alpha-z rate constant
double bzdv;      // Ito beta-z rate constant
double tauzdv;	  // Time constant of z gate
double zssdv;     // Steady-state value of z gate
double ydv;       // Ito inactivation
double aydv;      // Ito alpha-y rate constant
double bydv;      // Ito beta-y rate constant
double tauydv;	  // Time constant of y gate
double yssdv;     // Steady-state value of y gate


/* Sodium-Calcium Exchanger V-S */
double inaca;               // NaCa exchanger current (uA/uF)
double c1;   // Scaling factor for inaca (uA/uF)
double c2;   // Half-saturation concentration of NaCa exhanger (mM)
double gammas;  // Position of energy barrier controlling voltage dependance of inaca

/* Sodium-Potassium Pump */
double inak;    // NaK pump current (uA/uF)
double fnak;    // Voltage-dependance parameter of inak
double sigma;   // [Na]o dependance factor of fnak
double ibarnak;   // Max. current through Na-K pump (uA/uF)
double kmnai;    // Half-saturation concentration of NaK pump (mM)
double kmko;    // Half-saturation concentration of NaK pump (mM)

/* Nonspecific Ca-activated Current */
double insna;     // Non-specific Na current (uA/uF)
double insk;      // Non-specific K current (uA/uF)
double ibarnsna;  // Max. Na current through NSCa channel (uA/uF)
double ibarnsk;   // Max. K current through NSCa channel (uA/uF)
double pnsca;  // Permiability of channel to Na and K (cm/s)
double kmnsca;      // Half-saturation concentration of NSCa channel (mM)

/* Sarcolemmal Ca Pump */
double ipca;                 // Sarcolemmal Ca pump current (uA/uF)
double ibarpca; // Max. Ca current through sarcolemmal Ca pump (uA/uF)
double kmpca; // Half-saturation concentration of sarcolemmal Ca pump (mM)

/* Ca Background Current */
double icab;  // Ca background current (uA/uF)
double gcab;  // Max. conductance of Ca background (mS/uF)
double ecan;  // Nernst potential for Ca (mV)

/* Na Background Current */
double inab;  // Na background current (uA/uF)
double gnab;  // Max. conductance of Na background (mS/uF)
double enan;  // Nernst potential for Na (mV)


// Additional class parameters
double istFactor;           // Sustained inward current
double inabFactor;          // Na+ background current
double icalFactor;          // L-type Ca current
double icattFactor;         // T-type Ca current
double iksFactor;   // Slowly activating delayed rectifier K current
double ikrFactor;   // Rapidly activating delayed rectifier K current
double ikiFactor; // Time-independent K current
double itoFactor;   // Transient outward K+ current
double itrekFactor;   // Transient outward K+ current
double isusFactor;  // Sustained 4-AP sensitive K+ current
//double ikachFactor;  // Muscarinic K+ current
double inakFactor;          // Na-K pump
double inacaFactor; // Na-Ca exchanger
//double ihFactor;		 // Total funny current



double irelFactor;
double iupFactor;


// Declare functions unique to FR class. All functions/variables
// declared in parent class (Cell) are inherited by FR class.

/* Ion Current Functions */
virtual void comp_ina ();    // Calculates Fast Na Current
virtual void comp_ical ();   // Calculates Currents through L-Type Ca Channel
virtual void comp_icat ();   // Calculates Currents through T-Type Ca Channel
virtual void comp_ikr ();    // Calculates Rapidly Activating K Current
virtual void comp_iks ();    // Calculates Slowly Activating K Current
virtual void comp_iki ();    // Calculates Time-Independant K Current
virtual void comp_ikp ();    // Calculates Plateau K Current
virtual void comp_ikna ();   // Calculates Na-activated K Current
virtual void comp_ikatp ();  // Calculates ATP-Sensitive K Current
virtual void comp_ito ();    // Calculates Transient Outward Current
virtual void comp_inaca ();  // Calculates Na-Ca Exchanger Current
virtual void comp_inak ();   // Calculates Na-K Pump Current
virtual void comp_insca ();  // Calculates Non-Specific ca-Activated Current
virtual void comp_ipca ();   // Calculates Sarcolemmal Ca Pump Current
virtual void comp_icab ();   // Calculates Ca Background Current
virtual void comp_inab ();   // Calculates Na Background Current
virtual void comp_it ();     // Calculates Total Current



/* Ion Concentration Functions */
virtual void conc_nai ();    // Calculates new myoplasmic Na ion concentration
virtual void conc_ki ();     // Calculates new myoplasmic K ion concentration
virtual void conc_nsr ();    // Calculates new NSR Ca ion concentration
virtual void conc_jsr ();    // Calculates new JSR Ca ion concentration
virtual void calc_itr ();    // Calculates Translocation of Ca from NSR to JSR
virtual void conc_cai ();    // Calculates new myoplasmic Ca ion concentration
virtual void conc_cleft ();  // Calculates new cleft ion concentrationsvirtual

virtual int externalStim(double stimval);
virtual void updateCurr();
virtual void updateConc();
virtual void makemap();
virtual const char* type() const;

};
} //LongQt

#endif

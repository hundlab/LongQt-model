//#######################################################
// This header file contains class definition for
// Courtemanche model of human atrial myocytes
//
// Courtemanche, Marc, et al. “Ionic Mechanisms Underlying
// Human Atrial Action Potential Properties: Insights from
// a Mathematical Model.” The American Journal of Physiology,
// vol. 275, no. 1 Pt 2, July 1998, pp. H301-21,
// http://www.ncbi.nlm.nih.gov/pubmed/9688927.
//#######################################################

#include "cell.h"  // Parent class declaration
#include <cmath>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

#ifndef ATRIAL_H
#define ATRIAL_H


//######################################################
//Define class for normal human atrial cell.
//######################################################
class Courtemanche98 : public Cell
{
  public:
    Courtemanche98();
    Courtemanche98(Courtemanche98& toCopy);
    virtual ~Courtemanche98();
    
    Courtemanche98* clone() override;



/*########################*/
/*    DEFINE STRUCTS      */
/*########################*/

    struct GateVariable {  //Ion channel gates.
        double d;
        double f;
        double fCa;
        double u;
        double v;
        double w;
        double m;
        double h;
        double j;
        double ua;
        double ui;
        double oa;
        double oi;
        double xr;
        double xs;
   };



  //##################################################
  // Declare functions/variables unique to Courtemanche98
  // class. All functions/variables declared in parent
  // class (Cell) are inherited by Courtemanche98 class.
  //##################################################
  // Declare class functions
  //##################################################
        //#### current updating functions ####

    virtual void updateiCal();
    virtual void updateiCab();
    virtual void updateipCa();
    virtual void updateiRel();
    virtual void updateSRFlux();
    virtual void updateiK1();
    virtual void updateiKur();
    virtual void updateiTo();
    virtual void updateiKr();
    virtual void updateiKs();
    virtual void updateiNak();
    virtual void updateiNaca();
    virtual void updateiNa();
    virtual void updateiNab();
    virtual void updateCurr();
    virtual void updateConc();

    //#### concentration updating functions ####
    virtual void updatecaI();
    virtual void updatecaUp();
    virtual void updatecaRel();
    virtual void updatenaI();
    virtual void updatekI();

    virtual int externalStim(double stimval);
    virtual void makemap();
    virtual const char* type() const;

    //##### Declare class variables ##############
    double naI,naO,kI,kO,caI,caO;  // Ion concentrations, mM
    double iNa;           // Fast inward Na current, uA/uF
    double iCal;                  // L-type Ca current, uA/uF
    double iNak;          // Na-K pump, uA/uF
    double iNaca; // Na-Ca exchanger (20% located in subspace)
    double iCab;          // Background Ca current, uA/uF
    double iNab;          // Background Na current, uA/uF

    double ipCa;  // Sarcolemmal Ca2+ pump, uA/uF
    double iKs;   // Slowly activating delayed rectifier K current, uA/uF
    double iKr;   // Rapidly activating delayed rectifier K current, uA/uF
    double iKur;  // Ultrarapidly activating delayed rectifier K current, uA/uF
    double iK1;   // Inwardly rectifying K current, uA/uF
    double iTo;   // Transient outward K+ current, uA/uF

    double iKt;   // total K+ current, uA/uF
    double iNat;  // total Na+ current, uA/uF
    double iCat;  // total Ca2+ current, uA/uF

    double caR, caRt, iDiff;  // Ca in restricted space, mM.
    double iUp,iUp_leak,iTr;     // Ca uptake, leak and translocation in SR, mM/ms

    double iRel;        // Ca2+ release from SR, mM/ms
    double gRel;
    double ryRopen;     // RyR open probability
    int sponRelflag;
    double tRel;

    double maxInak,maxInaca,maxIup;
    double trpn,cmdn,csqn,dcsqn,caJsr,caNsr,caTotal, caJsrtotal;
    double caCmdn;
    double caTrpn;
    double caCsqn;

    double Vup;
    double Vrel;

    double caUp;
    double caRel;
    double iUp_max;
    double Fn;
    double KmTrpn;
    double KmCmdn;
    double KmCsqn;
    double Cmdn_m;
    double Trpn_m;
    double Csqn_m;

    struct GateVariable Gate;
    protected:
    virtual void Initialize();
};

#endif

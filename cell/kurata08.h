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

//######################################################
//Define class for central rabbit SAN cell.
//######################################################
class ControlSa : public Cell
{
  public:
    ControlSa();
    ControlSa(const ControlSa& toCopy);
    ~ControlSa();

  virtual ControlSa* clone() override;

/*########################*/
/*    DEFINE STRUCTS      */
/*########################*/

    struct GateVariableSa {  //Ion channel gates.
        double qa;	// Ist activation
        double qi;	// Ist inactivation
        double d;       // ICa activation
        double f;       // ICa volt-dependent inactivation
        double fca;     // ICa Ca-dependent inactivation
        double dt;      // T-type ICa activation
        double ft;      // T-type ICa volt-dependent inactivation
        double paf;      // IKr activation
        double pas;      // IKr activation
        double pi;       // IKr inactivation
        double n;      // IKs activation
        double q;       // Ito activation
        double r;       // Ito/Isus inactivation
        double y;       // Ih activation
   };

  //##################################################
  // Declare functions/variables unique to Central SAN
  // class. All functions/variables declared in parent
  // class (Cell) are inherited by Control class.
  //##################################################
  // Declare class functions
  //##################################################
    virtual void updateIst();
    virtual void updateInab();
    virtual void updateIcal();
    virtual void updateIcatt();
    virtual void updateIks();
    virtual void updateIkr();
    virtual void updateI4ap();
    virtual void updateItrek();
    virtual void updateIkach();
    virtual void updateInak();
    virtual void updateInaca();
    virtual void updateIh();
    virtual void updateIup();
    virtual void updateItr();
    virtual void updateIdiff();
    virtual void updateIrel();
    virtual void updateCar();
    virtual void updateCasr();
    virtual void updateNai();
    virtual void updateKi();
    virtual void updateCai();
    virtual void updateCurr();
    virtual void updateConc();
    virtual int externalStim(double stimval);
    virtual const char* type() const;
    
    //##### Declare additional class variables ##############
    double naI,kI,caI;  // Intracellular Ion concentrations
    double iSt;           // Sustained inward current
    double iNab;          // Na+ background current
    double iCal;          // L-type Ca current
    double iCatt;        // T-type Ca current
    double iKs;     // Slowly activating delayed rectifier K current
    double iKr;     // Rapidly activating delayed rectifier K current
    double iTo;     // Transient outward K+ current
    double iTrek;     // Trek-1
    double iSus;    // Sustained 4-AP sensitive K+ current
    double iKach;   // Muscarinic K+ current
    double iNak;          // Na-K pump
    double iNaca;   // Na-Ca exchanger
    double iHna;         // Funny current, Na+ component
    double iHk;          // Funny current, K+ component
    double iH;      // Total funny current
    
    double iCait, iCart;
    double caR, iDiff;      // Ca in restricted space.
    double caJsr,caNsr;
    double iRel,iUp,iTr;     // Ca uptake, leak and translocation in SR
    double trpnCa,trpnMg,trpnMgmg,cmdnI,cmdnR,csqn;  // buffers

    struct GateVariableSa Gate;

    //##### Declare additional class params ##############
    double naO,kO,caO;  // Fixed extracellular ion concentrations
    
    double istFactor;           // Sustained inward current
    double inabFactor;          // Na+ background current
    double icalFactor;          // L-type Ca current
    double icattFactor;         // T-type Ca current
    double iksFactor;   // Slowly activating delayed rectifier K current
    double ikrFactor;   // Rapidly activating delayed rectifier K current
    double itoFactor;   // Transient outward K+ current
    double itrekFactor;   // Transient outward K+ current
    double isusFactor;  // Sustained 4-AP sensitive K+ current
    double ikachFactor;  // Muscarinic K+ current
    double inakFactor;          // Na-K pump
    double inacaFactor; // Na-Ca exchanger
    double ihFactor;		 // Total funny current
    
    double irelFactor;
    double iupFactor;
    double isoConc;

   
    double Vnsr;
    double Vjsr;
    double Vss;

    //    ISO //isoproterenol
//    MAKE_OPTIONS(ISO)

//    enum Options opts;

protected:
    virtual void Initialize();
private:
    void makemap();
};

#endif

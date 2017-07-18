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

#ifndef MODEL_TNNP
#define MODEL_TNNP


class TNNP04Control : public Cell
{
  public:
    TNNP04Control();
    TNNP04Control(const TNNP04Control& toCopy);
    ~TNNP04Control();

    void Initialize();
    virtual TNNP04Control* clone();
  
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
	virtual void updatecaI();
   	virtual void updatecaSr();
	virtual void updatekI();
	virtual void updatenaI();
 //####Current updating functions #######
	virtual void updateSRcurrents();
	virtual void updateIcal();
	virtual void updateIcab();
	virtual void updateIpca();
	virtual void updateIto();
	virtual void updateIks();
	virtual void updateIkr();
	virtual void updateIk1();
	virtual void updateIpk();
	virtual void updateInaca();
	virtual void updateInak();
	virtual void updateInab();
	virtual void updateIna();
	virtual void updateCurr();
	virtual void updateConc();
	virtual int externalStim(double stimval);
	virtual void makemap();
    virtual const char* type() const;
 
//##### Declare class variables ##############
	double Vc;
	double Vsr;

	double tRel;
	int sponRelflag;

	int isoFlag;
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
	
    	struct GateVariable Gate;
};

#endif

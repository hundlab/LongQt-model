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

#ifndef KMODEL_H
#define KMODEL_H
  

//######################################################
//Define class for normal canine ventricular cell.
//######################################################
class Ksan : public Cell
{
  public:
    Ksan();
    Ksan(Ksan& toCopy);
    virtual ~Ksan();
    
    void Initialize();
    Ksan* clone();

/*########################*/
/*    DEFINE STRUCTS	  */
/*########################*/

    struct GateVariable {  //Ion channel gates.
	double dst;
	double fst;
	double dt;
	double ft;
	double ikr_act;
	double ikr_inact;
	double ikr_inact2;
	double iks_act;
	double fl12;
	double dl12;
	double fl13;
	double dl13;
	double fca;
	double r;
	double q;
	double y_1_2;
	double m_ttxr;
	double h_ttxr;
	double j_ttxr;
	double m_ttxs;
	double h_ttxs;
	double j_ttxs;
	double open;
	double resting;
	double inactivated;
	double resting_inactivated;
   };
  
  //##################################################
  // Declare functions/variables unique to Control
  // class. All functions/variables declared in parent 
  // class (Cell) are inherited by Control class.
  //##################################################
  // Declare class functions 
  //##################################################
   virtual void updateIst();
   virtual void updateIb();
   virtual void updateIK1();
   virtual void updateICaT();
   virtual void updateIKr();
   virtual void updateIKs();
   virtual void updateICaL();
   virtual void updateINa();
   virtual void updateIf();
   virtual void updateIto();
   virtual void updateIsus();
   virtual void updateINaK();
   virtual void updateiNaCa();
   virtual void updateCurr();
   virtual void updateConc();	

   virtual map<string, double*> makemap();

    //##### Declare class variables ##############

   double naI,naO,kI,kO,caI,caO;  
   double caSub;
   double caJsr,caNsr;
   double mgI;
   double ist;
   double ibca,ibna,ibk;
   double ik1;
   double icat;

   double ikr;
   double iks;
   double ical12;
   double ical13;
   double ina_ttxr, ina_ttxs;
   double ih, ihk, ihna;
   double inak;
   double inaca;
   double isus;
   double ito;
   double Jrel, Jup, Jtr,Jcadif;
   double Ftc,Ftmc,Ftmm,Fcms,Fcmi,Fcq;

   double Vnsr;
   double Vjsr;
   double Vss;
   
   double TotCap;
   struct GateVariable Gate;
};

#endif

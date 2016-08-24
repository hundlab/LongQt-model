// 5/5/2000
//################################################
// This header file contains all class definitions
//#################################################
#include "cell.h" //parent class definitions

#ifndef GPBMODEL_H
#define GPBMODEL_H

//####################################
// subclass of Cell
//####################################
class GpbVent: public Cell
{
  public:
    // constructors
    GpbVent();
    GpbVent(GpbVent& toCopy);
    virtual ~GpbVent();
    
    void Initialize();
    GpbVent* clone();

/*########################*/
/*    DEFINE STRUCTS	  */
/*########################*/
struct GateVariable {
   double m;
   double h;
   double j;
   double xkr;
   double xks;
   double xs;
   double ys;
   double xf;
   double yf;
   double f_cabj;
   double f_cabsl;
   double d;	
   double f;	
};	

    double Vsl;
    double Vjunc;
    double Vsr;
	
//###### Concentrations #########
    double naO;
    double kO;
    double najI;
    double naslI;
    double cajI;
    double caslI;
    double caO;
    double caI;
    double kI;
    double caSr;
    double clO;
    double clI;
    double naI;
    double mgI;
//####### RyR channel fractions
    double Ryrr;
    double Ryro;
    double Ryri;
    double RI;
//###### Currents #######
    double iNajunc;
    double iNasl;
    double iNa;
    double iNabjunc;
	double iNabsl;
	double iNab;
	double iNaKjunc;
	double iNaKsl;
	double iNak;
	double iKr;
	double iKs; 
	double iKsjunc;
	double iKssl;
	double iKpjunc;
	double iKpsl;
	double iKp;
	double iTos;
	double iTof;
	double iTo;
	double iK1;
	double iClcajunc;
	double iClcasl;
	double iClca;
	double iClbk;
	double iCajunc;
	double iCasl;
	double iCa;
	double iCak;
	double iCanajunc;
	double iCanasl;
	double iCana;
	double iCaL;
	double iCatotjunc;
	double iCatotsl;
	double iNcxjunc;
	double iNcxsl;
	double iNaca;
	double ipCajunc;
	double ipCasl;
	double ipCa;
	double iCabjunc;
	double iCabsl;
	double iCab;
	double iNatotjunc;
	double iNatotsl;
	double iClt;
	
//##### Buffers ########
	//Sodium Buffers
	double Nabj; 
	double Nabsl; 
	
	//Calcium Buffers
	double TnCl;
	double TnChc;
	double TnChm;
	double CaM;
	double Myoc;
	double Myom;
	double SRB;
	
	//junctional and SL Ca buffers
	double SLLj;
	double SLLsl;
	double SLHj;
	double SLHsl;
	
	//SR Ca buffer
	double Csqnb;
	double dCsqnb;

//##### Fluxes ########    
	double Jsrcarel;
	double Jserca;
	double Jsrleak;
	double J_cabcyto;
	double J_Cabjunc; 
	double J_Cabsl;

//##### Fractional Currents ########

	double Fjunc;
	double Fsl;
	double F_juncCaL;
	double F_slCaL;

 //###Concentration updating functions ######
	virtual void updatecaI();
	virtual void updatenaI();
 //####Current updating functions #######
	/*virtual void updateSRcurrents();*/
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
	virtual void updateSRFlux();
	virtual void updatecytobuff(); //cytosolic Ca buffers
	virtual void updateJSLbuff(); //junctional and SL Ca buffers
	virtual void updateSRbuff(); //SR Ca buffer	 
	virtual void updateIclca();
	virtual void updateIclbk();
	virtual void updateCurr();
	virtual void updateConc();
    virtual int externalStim(double stimval);
    virtual map<string, double*> makemap();

    struct GateVariable gate;
};
#endif

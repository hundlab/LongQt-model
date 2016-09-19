//####################################################
// This file contains unique function definitions for
// Hund-Rudy dynamic model of epicardial cell from 
// canine infarct border zone as published in  
// Christensen et al. PLoS. 2009;5:e1000583.
//
// Copyright (C) 2011 Thomas J. Hund.
//####################################################

#include "hrd09.h"

//######################################################
// Constructor for canine infarct border zone epicardial
// ventricular model.
//######################################################
HRD09BorderZone::HRD09BorderZone()
{
    this->Initialize();
};
HRD09BorderZone::HRD09BorderZone(const HRD09BorderZone& toCopy) : HRD09Control(toCopy) {
    this->Initialize();
    CellKernel::copyVarPar(toCopy);
}
//######################################################
// Destructor for canine infarct border zone epicardial
// ventricular model.
//#####################################################
HRD09BorderZone::~HRD09BorderZone()
{
};

//##### Initialize variables ##################
void HRD09BorderZone::Initialize() {
        type = "Canine Ventricular Border Zone (Hund-Rudy 2009)";
        dVdt=dVdtmax=1.017497198e-09;
        t=0.0;
	dt=dtmin = 0.005;
   	dtmed = 0.01;
   	dtmax = 0.1;
   	dvcut = 1.0;
        vOld = vNew =  -86.18719757;
        tRel=10000.0;
        sponRelflag = 0;

        apTime = 0.0;

        naO = 140;
        caO = 1.8;
        kO = 5.4;
        clO = 100.0;

        kI = 140.177433;
        naI = 11.72194278;
        caI = 0.0001315106815;
        clI = 19.16161719;

        caNsr = 2.455083001;
        caJsr = 2.428337007;

        cellRadius=0.0011;
        cellLength=0.01;
	Rcg = 2.0;

        Vcell=1000*3.14*cellRadius*cellRadius*cellLength;
        AGeo=2*3.14*cellRadius*cellRadius+2*3.14*cellRadius*cellLength;
        ACap=AGeo*Rcg;
        Vmyo=Vcell*0.66;
        Vmito=Vcell*0.26;
        Vsr=Vcell*0.06;
        Vnsr=Vsr*0.92;
        Vjsr=Vsr*0.08;
        Vss=Vcell*0.02;

        //#### Initialize ion channel gatess ####
        Gate.m = 0.001267720639;
        Gate.h = 0.9884153643;
        Gate.j = 0.9925781401;
        Gate.ml = 0.0008371146773;
        Gate.hl = 0.3123868862;

        Gate.d = 1.544374798e-06;

        Gate.f = 0.9999636919;
        Gate.fca = 0.9646737094;
        Gate.fca2 = 0.9828143816;

        Gate.aa = 0.0008843276454;
        Gate.xs = 0.01956102304;
        Gate.xs2 = 0.01956102302;
        Gate.xr = 1.672436062e-08;

        Gate.a = 0.003888587341;
        Gate.i = Gate.i2 = 1.0;

        Gate.k1 = 0.5737548224;
        Gate.kp = 1.17091064E-7;

        ryRopen =  9.17893025e-05;
        //#################################

        caR = 0.000132943578;

        cmdn = 0.002618158912;
        csqn = 7.521943966;
        trpn = 0.01457734283;

        caM = 2.390463386e-05;
	fBlock = 0;
  	fBound = 3.352990141e-06;
	fI = 0.9999538673;
	fOx = 4.27796317e-05;
	fOxP = 6.992656517e-11;
	fPhos = 5.480670365e-12;

        caTotal = caI+cmdn+trpn;

        iCab = -0.2296685076;
        iCa = -0.0001748612881;
        iClb = -0.01182588852;
        iDiff = 7.16448284e-06;
        iK1 = 0.1368348181;
        iKcl = -1.68654752E-5;
        iKp = 3.474435959e-10;
        iKr = 1.840544305e-10;
        iKs = -0.0001598196248;
        iLeak = 0.001028066007;
        iNa = -1.901299409e-06;
        iNaca = -0.08669836545;
        iNacar = -0.02223607396;
        iNacl = 1.757016093E-5;
        iNak = 0.2013344281;
        iNal = -6.306202035e-10;
        iPca = 0.01197424589;
        iRel = 0.0002228831573;
        iTo = 0.0;
        iTo2 = -0.001742183484;
        iTr = 0.0002228832817;
        iUp = 0.001047447236;

        iCat = -2.441419895e-07;



    Inafactor=1;
    Icabfactor=1;
    Ilcafactor=1;
    Ik1factor=1;
    Itofactor=1;



        makemap();
}
HRD09BorderZone* HRD09BorderZone::clone() {
    return new HRD09BorderZone(*this);
}
//######################################################
// L-type Ca current - BZ cell has decreased 
// permeability (pca) compared to Control model.
// Also, kinetic differences exist downstream of 
// differences in CaMKII activity.
//######################################################
void HRD09BorderZone::updateIlca()
{
    double taud,finf,fcainf,fcainf2,taufca,taufca2,tauf,dinf;//,taud2,d2inf;
	double maxIca;
	
	double gamcai=1.0;
	double gamcao=0.341;
	double pca=1.56E-4;  // Max permeability
	double kmca=0.002;   // Ca2+ half-saturation coeff. for Ca2+-dependent inactivation
	
	double deltatfca=10; // Maximal CaMKII-dependent increase in tfca
	double deltat=20;  
	double KMCAM=0.05;  // CaMKII half-saturation coefficient
	double cafac;
	double camfact;
	
   	cafac=1/(1.0+caR/kmca);   

	camfact=1/(1+pow((KMCAM/caM),5.0));

	dinf=(1/(1.0+exp(-(vOld-4.0)/6.74))); 
	taud=0.59+0.8*exp(0.052*(vOld+13))/(1+exp(0.132*(vOld+13)));
	
	finf=0.7/(1+exp((vOld+17.12)/7.0))+.3;
	tauf=22+280*exp(0.062*(vOld+28.3))/(1+exp(0.25*(vOld+28.3)))+deltat*camfact;
	
    	fcainf=0.3/(1-iCa/0.05)+0.55*cafac+0.15;  
    	taufca=0.5+deltatfca*camfact+1*cafac;
    	
	fcainf2=1/(1-iCa/0.01); 
    	taufca2=100/(1+exp((-iCa-0.175)/0.04))+100;
        
    	Gate.fca=fcainf-(fcainf-Gate.fca)*exp(-dt/taufca);
    	Gate.fca2=fcainf2-(fcainf2-Gate.fca2)*exp(-dt/taufca2);

	Gate.d=dinf-(dinf-Gate.d)*exp(-dt/taud);
    	Gate.f=finf-(finf-Gate.f)*exp(-dt/tauf);	
	
	maxIca=pca*4*(vOld)*(FDAY*FDAY/(RGAS*TEMP))*(gamcai*caR*exp(2*(vOld)*FDAY/(RGAS*TEMP))-gamcao*caO)/(exp(2*(vOld)*FDAY/(RGAS*TEMP))-1.0);
	
	iCa=Ilcafactor*Gate.d*Gate.f*Gate.fca*Gate.fca2*maxIca;
};

//######################################################
// Background Ca current - BZ cell has decreased 
// conductance (gcab) compared to Control model.  
//######################################################
void HRD09BorderZone::updateIcab()
{
	double maxicab;//Ecan,
	double gamcai=1.0;
	double gamcao=0.341;
	double gcab=3.0E-7;
	
//	Ecan=(RGAS*TEMP/(2*FDAY))*log(caO/caI);	
	
	maxicab=4*(vOld)*(FDAY*FDAY/(RGAS*TEMP))*(gamcai*caI*exp(2*(vOld)*FDAY/(RGAS*TEMP))-gamcao*caO)/(exp(2*(vOld)*FDAY/(RGAS*TEMP))-1.0);

	iCab=Icabfactor*gcab*maxicab; 
};

//######################################################
// Time-independent K current - BZ cell has decreased
// maximal conductance (MaxGk1) compared to control 
// model.
//######################################################
void HRD09BorderZone::updateIk1()
{
	double Gk1, Ek1;
	double MaxGk1 =0.3;
        
	Ek1=(RGAS*TEMP/FDAY)*log(kO/kI);
	Gk1=MaxGk1*(sqrt(kO/5.4));
	
  	Rate.ak1=1.02/(1+exp(.2385*(vOld-Ek1-59.215)));
	Rate.bk1=(.49124*exp(.08032*(vOld-Ek1+5.476))+exp(.06175*(vOld-Ek1-594.31)))/(1+exp(-.5143*(vOld-Ek1+4.753)));

	Gate.k1=Rate.ak1/(Rate.ak1+Rate.bk1);
	iK1=Ik1factor*Gk1*Gate.k1*(vOld-Ek1);
};

//######################################################
// Fast Na current - BZ cell has decreased maximal
// conductance (MaxGNa) compared to control model.  
// Also, kinetic differences exist downstream of 
// differences in CaMKII activity.
//######################################################
void HRD09BorderZone::updateIna()
{
	double bj1a,bj2a,bj2b,aj1a,aj1b,aj1c;
	double ms,tm,hs,th,js,tj;
	double camfact;
	double KMCAM=0.06;  
	double vshift; 
	double deltaalpha = -.18;
	double MaxGNa, ENa;

	MaxGNa = 6.24;// different from HRd 08- 3.22;

	camfact= 1/(1+pow((KMCAM/caM),4.0));
	vshift = -3.25*camfact;

	ENa=(RGAS*TEMP/FDAY)*log(naO/naI);
	
	Rate.am=.32*(vOld+47.13)/(1-exp(-.1*(vOld+47.13)));  
	Rate.bm=.08*exp(-vOld/11.0);				 	

	if((vOld-vshift)>=-40.0)
	{
	        Rate.ah=0.0;	
		Rate.bh=1/(.13*(1+exp((vOld-vshift+10.66)/(-11.1))));  	
	}
	else
	{
	        Rate.ah=.135*exp((80.0+vOld-vshift)/(-6.8));
		Rate.bh=3.56*exp(.079*(vOld-vshift))+(3.1E5)*exp(.35*(vOld-vshift));
        }

	if((vOld-vshift)>=-40.0)
	{
		Rate.aj=0.0;	

		bj1a=exp((-2.535E-7)*(vOld-vshift));	
		Rate.bj=(.3)*bj1a/(exp(-.1*(vOld-vshift+32))+1);  
	}
	
	else
	{				 		
		aj1a=(-1.2714E5)*exp(.2444*(vOld-vshift));
		aj1b=(3.474E-5)*exp(-.04391*(vOld-vshift));
		aj1c=(vOld-vshift+37.78)/(1+exp(.311*(vOld-vshift+79.23)));
		
		Rate.aj=(1+camfact*deltaalpha)*(aj1a-aj1b)*aj1c;  

		bj2a=.1212*exp(-.01052*(vOld-vshift));
		bj2b=1+exp(-.1378*(vOld-vshift+40.14));
		Rate.bj=bj2a/bj2b;
	}
	
	ms=Rate.am/(Rate.am+Rate.bm);	
	tm=1/(Rate.am+Rate.bm);
	Gate.m=ms-(ms-Gate.m)*exp(-dt/tm);
	
	hs=Rate.ah/(Rate.ah+Rate.bh);
	th=1/(Rate.ah+Rate.bh);
	Gate.h=hs-(hs-Gate.h)*exp(-dt/th);
	
	js=Rate.aj/(Rate.aj+Rate.bj);
	tj=1/(Rate.aj+Rate.bj);
	Gate.j=js-(js-Gate.j)*exp(-dt/tj);

	iNa= Inafactor*(MaxGNa*Gate.m*Gate.m*Gate.m*Gate.h*Gate.j)*(vOld-ENa);	

};

//######################################################
// Transient outward K current - Ito is absent 
// (MaxGto = 0.0) in BZ model.
//######################################################
void HRD09BorderZone::updateIto()
{
    double tauzdv,zssdv,tauydv,tauy2dv,yssdv,yss2dv,rvdv;//azdv,bzdv,ay2dv,by2dv,aydv,bydv,
    double Ek;
    double MaxGto = 0.0;

   Ek=(RGAS*TEMP/FDAY)*log(kO/kI);
   rvdv = exp(vOld/300.0);
   
   tauzdv = 3.5*exp(-(vOld/30)*(vOld/30))+1.5;
   zssdv = 1.0/(1+exp((vOld+3.0)/(-15.0)));
	
   tauydv = 20/(1+exp((vOld+33.5)/10))+20;
   tauy2dv=3000/(1+exp((vOld+60)/10))+20;
   yssdv = 1/(1+exp(vOld+33.5)/10);
   yss2dv = 1/(1+exp(vOld+33.5)/10);

   Gate.a=zssdv-(zssdv-Gate.a)*exp(-dt/tauzdv);
   Gate.i=yssdv-(yssdv-Gate.i)*exp(-dt/tauydv);
   Gate.i2=yss2dv-(yss2dv-Gate.i2)*exp(-dt/tauy2dv);

   iTo=Itofactor*MaxGto*Gate.a*Gate.a*Gate.a*Gate.i*Gate.i2*rvdv*(vOld-Ek);

};

//######################################################
// CaMKII activity - Autophosphorylation rate (R) is 
// faster in BZ compared to Control resulting in CaMKII
// overactivity.
//######################################################
void HRD09BorderZone::updateCamk()
{
   double P=0.00003;          // rate of dephosphorylation .00003
   double calmodulin;         // free calmodulin bound to calcium.
   double calmtotal = .00006; // total free calmodulin = 60 nM.

   double va,va2,ka,tcamk;

   double ros = 1.0;          // concentration of H2O2, um
   double kib = 246.0;        // mM-1/ms-1
   double kbi = 0.0022;       // ms-1
   double kox = 0.0002909;    // ms-1
   double kred = 0.0000228;   // um/ms
   double kt;
//   double kbp = 0.0022;       // ms;
   double kn93 = 0.0;         // concentration of KN-93, um
   double kbli = 0.0022;      // ms-1
   double kibl = 0.0008536;   // um-1.ms-1

   calmodulin = calmtotal*(caR*caR*caR*caR/(caR*caR*caR*caR+0.005*0.005*0.005*0.005));
   tcamk=fBound+fPhos+fOx+fOxP;
   kt = kbi/kib/(1/tcamk-1);
   ka = kbi*kt/(kt+0.01851);
   va = ka*fBound;
   va2 = ka*fOx;

   fBound=dt*(kib*calmodulin*fI+P*fPhos+kred*fOx-(kbi+kox*ros)*fBound-va)+fBound;
   fPhos=dt*(va+kred*fOxP-(kox*ros+P)*fPhos)+fPhos;
   fOx=dt*(kox*ros*fBound+P*fOxP-kred*fOx-va2)+fOx;
   fOxP=dt*(va2-P*fOxP+kox*ros*fPhos-kred*fOxP)+fOxP;
   fBlock=dt*(kibl*kn93*fI-kbli*fBlock)+fBlock;
   fI=1-fBound-fPhos-fOx-fOxP-fBlock;
   caM=(0.75*fBound+fPhos+fOxP+.5*fOx);
};


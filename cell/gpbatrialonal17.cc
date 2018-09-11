//######################################################
// This file contains all function definitions for
// Grandi model of the human atrial action 
// potential as published in Grandi et al.
// Circ Res 2011.
//
// Copyright (C) 2012 Thomas J. Hund.
//######################################################

#include "gpbatrialonal17.h"
#include <QDebug>


GpbAtrialOnal17::GpbAtrialOnal17()
{
    this->Initialize();
}

GpbAtrialOnal17::GpbAtrialOnal17(GpbAtrialOnal17& toCopy) : Cell(toCopy) {
    this->Initialize(); 
    CellKernel::copyVarPar(toCopy);
}

GpbAtrialOnal17::~GpbAtrialOnal17()
{
}


void GpbAtrialOnal17::Initialize()
{
    Cm = 1.0; //uF/cm2
    ACap = 1.10E-4; //capacitive area, cm2
    dVdt=dVdtmax=0.0;
    t=0.0;
    dt = dtmin = 0.005;
    dtmed = 0.005;
    dtmax = 0.005;
    dvcut = 1.0;
    vOld=vNew=-80.9763;

    apTime = 0.0;
   // flag = 0;
   // num = 0;

    Vcell = 3.3E-5;//uL
    Vsr = 0.035*Vcell; 
    Vjunc = (5.39E-4)*Vcell;
    Vsl = 0.02*Vcell;
    Vmyo = 0.65*Vcell;
//###### Concentrations #########
    naI = 9.136; //mM 
    naO = 140.0; //mM
    kI = 120.0; //mM
    kO = 5.4; //mM
    caI =0.00008597401; //mM
    caO = 1.8; //mM
    caSr = 0.01; //mM
    cajI = 0.0001737475; //mM 
    caslI = 0.0001031812; //mM
    najI = 9.136; //mM
    naslI = 9.136; //mM
    clO = 150.0;//mM
    clI = 15.0; //mM
    mgI = 1.0; //mM
//##### Fractional Currents ########
    Fjunc = 0.11;
    Fsl = 1-Fjunc;
    F_juncCaL = 0.9;
    F_slCaL = 1-F_juncCaL;
//##### Buffers ######## 
//Sodium Buffers
    Nabj = 3.539892;
    Nabsl = 0.7720854; //mM
//Calcium Buffers 
    TnCl = 0.008773191;//mM
    TnChc = 0.1078283; //mM
    TnChm = 0.01524002;
    CaM = 0.0002911916; //mM
    Myoc = 0.001298754; //mM
    Myom = 0.1381982;
    SRB = 0.002143165; //mM
//junctional and SL Ca buffers 
    SLLj = 0.009566355;
    SLLsl = 0.1110363;
    SLHj = 0.007347888;
    SLHsl = 0.07297378;
//SR Ca buffer
    Csqnb = 0.2;
//##### Gating Variables #####
	Gate.m = 0.001405627;
	Gate.h = .9867005;
	Gate.j = .991562;
	Gate.xkr = .008641386;
	Gate.xks = .005412034;
	Gate.xf = 0.004051574;
	Gate.yf = .9945511;
	Gate.xkur = 0.0;
	Gate.ykur = 1.0;
	Gate.d = 0.000007175662;
	Gate.f = 1.00;
	Gate.f_cabj = .02421991;
	Gate.f_cabsl = .01452605;
	Gate.hl = .001405627;
	Gate.ml = .9867005;
	
//####### RyR channel fractions
	Ryrr = 0.8884332;
	Ryro = 8.156628E-7;
	Ryri = 1.024274E-7;
	RI = 1.0-Ryrr-Ryro-Ryri;

	iTos = iTof = iTo = 0.0;
	iKsjunc = iKssl=iKs=iKr=iKur=iKpjunc = iKpsl = iKp = iK1 = 0.0;
	iNajunc = iNasl = iNabjunc = iNabsl = iNab = iNa = 0.0;
	iCajunc = iCasl = iCa = 0.0;
	iCaL = iCab = ipCa = 0.0;
	iCak = iCanajunc = iCanasl = iCana = 0.0;
	iNaKjunc = iNaKsl = iNak = iNal = 0.0;
	iNcxjunc = iNcxsl = iNaca = 0.0;
	iClcajunc = iClcasl = iClca = iClbk = 0.0;
	ipCajunc = ipCasl = ipCa = 0.0;
	iCabjunc = iCabsl = iCab = 0.0;

    caMkii = 0.0003647207545;
   
    fBlock = 0;
	fBound = 0.0004862831625;
	fI = 0.9995137085;
	fOx = 0;
	fOxP = 0;
	fPhos = 8.382592879e-09;

	fiNalP = 0;
    RyRP = 382.6E-3;

    opts = WT;
    conflicts = {{S571E, S571A},{S2814D,S2814A}};

	Icalfactor = Icabfactor = Ipcafactor = 1;
	Itofactor =	Iksfactor =	Ikrfactor = 1;
	Ik1factor =	Ikurfactor = 1;
	Ipkfactor = Inacafactor = Inakfactor = 1;
	Inabfactor = Inafactor = Iclcafactor = 1;
	Iclbkfactor =	Inalfactor = 1;
	JSRcarelfactor = 	Jsercafactor = 1;
    IcaMkiiFactor = 1;
    InalPFactor = 1;
    Jsrleakfactor = 1;
    ROSFactor = 0;

    makemap();
}


GpbAtrialOnal17* GpbAtrialOnal17::clone() {
    return new GpbAtrialOnal17(*this);
}

void GpbAtrialOnal17::updateCamk()
{
        double P=0.00003;          // rate of dephosphorylation .00003

        double calmtotal = .00006*IcaMkiiFactor; // total free calmodulin = 60 nM.
        double calmodulin;         // free calmodulin bound to calcium. in the sl (for camkii)


        double ros = 1*ROSFactor;          // concentration of H2O2, um
        double kib = 246.0;        // mM-1/ms-1
        double kbi = 0.0022;       // ms-1
        double kox = 0.0002909;    // ms-1  
        double kred = 0.0000228;   // um/ms
        double kbp = 0.0022;       // ms;
        double kn93 = 0.0;         // concentration of KN-93, um
        double kbli = 0.0022;      // ms-1
        double kibl = 0.0008536;   // um-1.ms-1

        calmodulin = calmtotal*(caslI*caslI*caslI*caslI/(caslI*caslI*caslI*caslI+0.005*0.005*0.005*0.005));

        double tcamk=fBound+fPhos+fOx+fOxP;
        double kt = kbi/kib/(1/tcamk-1);
        double ka = kbi*kt/(kt+0.01851);
        double va = ka*fBound;
        double va2 = ka*fOx;

        fBound+=dt*(kib*calmodulin*fI+P*fPhos+kred*fOx-(kbi+kox*ros)*fBound-va);
        fPhos+=dt*(va+kred*fOxP-(kox*ros+P)*fPhos);
        fOx+=dt*(kox*ros*fBound+P*fOxP-kred*fOx-va2);
        fOxP+=dt*(va2-P*fOxP+kox*ros*fPhos-kred*fOxP);
        fBlock+=dt*(kibl*kn93*fI-kbli*fBlock);
        fI=1-fBound-fPhos-fOx-fOxP-fBlock;
        caMkii=(0.75*fBound+fPhos+fOxP+.5*fOx);
};

// Late Na current
void GpbAtrialOnal17::updateInal()
{
	double ENa;
	double ms,tml,hlinf,thl;
	double deltag = 0.0095;
		
	ENa=(RGAS*TEMP/FDAY)*log(naO/naI);
	
	double aml=.32*(vOld+47.13)/(1-exp(-.1*(vOld+47.13)));  
	double bml=.08*exp(-vOld/11.0);				 
	
	hlinf = 1/(1+exp((vOld+91)/6.1));  

	ms=aml/(aml+bml);	
	tml=1/(aml+bml);
	Gate.ml=ms-(ms-Gate.ml)*exp(-dt/tml);
	
	thl=600;
	Gate.hl=hlinf-(hlinf-Gate.hl)*exp(-dt/thl);

    double iNalNP=(0.006*5)*Gate.ml*Gate.ml*Gate.ml*Gate.hl*(vOld-ENa);
    double iNalP = 3.2*iNalNP;
    double dfiNalP = (InalPFactor*caMkii/(InalPFactor*caMkii + .15) - fiNalP)/100*dt;
    fiNalP = fiNalP + dfiNalP;
    //for SA - SE fitting atrial data, 8 - 57
    //for SA - SE fitting vent data, 63 - 213
    if(opts & S571A) { //SA
        fiNalP = 0;
    } else if(opts & S571E) { //SE
        fiNalP = 1;
    }
    double fiNalNP = 1- fiNalP;
    iNal = 	Inalfactor*(fiNalP * iNalP + fiNalNP * iNalNP);
};


void GpbAtrialOnal17::updatecaI() {

	double dCaj, dCasl, dcaI, dCasr;
	double J_cajuncsl = 8.2413E-7;
	double J_caslmyo = 3.7243E-6;

	dCaj = dt*(-iCatotjunc*(ACap/(Vjunc*2.0*FDAY))+(J_cajuncsl/Vjunc)*(caslI-cajI)-J_Cabjunc+Jsrcarel*(Vsr/Vjunc)+Jsrleak*(Vmyo/Vjunc));
	dCasl = dt*(-iCatotsl*(ACap/(Vsl*2.0*FDAY))+(J_cajuncsl/Vsl)*(cajI-caslI)-J_Cabsl+(J_caslmyo/Vsl)*(caI-caslI));
	dcaI = dt*(-Jserca*(Vsr/Vmyo)-J_cabcyto+(J_caslmyo/Vmyo)*(caslI-caI));
	dCasr = dt*(Jserca-(Jsrleak*(Vmyo/Vsr)+Jsrcarel)-dCsqnb/dt);
	
	cajI = cajI + dCaj;
	caslI = caslI + dCasl;
	caI = caI + dcaI;
	caSr = caSr + dCasr;
}


void GpbAtrialOnal17::updateSRFlux(){

	double condisomf = 1;
	double condisoca = 1;
//SR fluxes
    if (opts & ISO){
	condisomf = 0.5;
	condisoca = 2;
	}
	double dRyrr, dRyro, dRyri;
	double maxsr = 15.0;
	double minsr = 1.0;
	double kCasr, kOsrca, kIsrca;
	double ec50SR = 0.45;
	double koCa = condisoca*10.0; 
	double kiCa =0.5;
	double kim = 0.005;
	double kom = 0.06;
	double ks = 25.0;
	double alpha, beta;
	double kmf = condisomf*2.5*0.000246;
	double kmr = 1.7;
	double hillSRcaP = 1.787;
	double VmaxSRcaP = 0.0053114;
	
	//if (Csqnb > 1.2602){
	//Ryro = .0001;
	//}

	//////////// RyRser2815 Phosphorylation Eqs from Soltis-Saucerman 
	double RyRtot = 382.6E-3; //mmol/ L dyad //for SD, RyRP = 0
    RyRratio = RyRP/RyRtot;
    double kb2815 = 0.35E-3*0.7; //1/ms //refit for wider camkii effect
    double kckryr = 0.4E-3*2; //refit for wider camkii effect
	double kmckryr = 12E-3; //mM
	double PP1 = 95.6E-3;
	double PP2A = 95.6E-3;
    double kpp1ryr = 1.07E-3;
	double kmpp1ryr = 9E-3;
	double kmpp2ryr = 47E-3;
    double kpp2ryr = 0.481E-3;
	double kioapp1 = 780E-6;
	double kioapp2 = 37E-6;
    double OA = 0;
	double OApp1 = 1/(1+(OA/kioapp1)*(OA/kioapp1)*(OA/kioapp1));
	double OApp2 = 1/(1+(OA/kioapp2)*(OA/kioapp2)*(OA/kioapp2));
	
	double RyRN = RyRtot - RyRP;
	double Rxnbasal = kb2815*RyRN;
    double Rxnckryr = kckryr*(caMkii*120E-3)*RyRN/(kmckryr+RyRN);//for WT 1.2923* was old fit value
    if(opts & S2814D) { // S2814D
        Rxnckryr = kckryr*(1*120E-3)*RyRN/(kmckryr+RyRN);/*1.2923*/
    } else if(opts & S2814A) { // S2814A
        Rxnckryr = 0;
    }
	double Rxnpp1ryr = kpp1ryr*PP1*RyRP*OApp1/(kmpp1ryr+RyRP);
	double Rxnpp2ryr = kpp2ryr*PP2A*RyRP*OApp2/(kmpp2ryr+RyRP);	

    double kleak = (1./3) + (10./3)*RyRratio*0.5; //refit for wider camkii effect
    double koRyRCKII = (20./3)*(RyRratio)-(1./3);
    double PKAratio = 0.5;
	double koRyRPKA = 1.025*(PKAratio)+0.975;
	double koRyR = koRyRCKII +koRyRPKA - 1;

	////////////////////////

	kCasr = maxsr -((maxsr-minsr)/(1.0+pow((ec50SR/caSr),2.5)));

	//kOsrca = koCa/kCasr; 
	kOsrca = koRyR*koCa/kCasr; //changed to match Soltis-Saucerman
	
	kIsrca = kiCa*kCasr;
	
	
	dRyrr = dt*((kim*RI-kIsrca*cajI*Ryrr)-(kOsrca*cajI*cajI*Ryrr-kom*Ryro));
	dRyro = dt*((kOsrca*cajI*cajI*Ryrr-kom*Ryro)-(kIsrca*cajI*Ryro-kim*Ryri));
	dRyri = dt*((kIsrca*cajI*Ryro-kim*Ryri)-(kom*Ryri-kOsrca*cajI*cajI*RI));

	Ryrr = Ryrr + dRyrr;
    Ryro = Ryro + dRyro;
	Ryri = Ryri + dRyri;
    RI = 1.0-Ryrr-Ryro-Ryri;

	Jsrcarel = JSRcarelfactor* ks*Ryro*(caSr-cajI);
	
	double KMCAM = 0.2;
    double camfact = 1/(1+pow((KMCAM/caMkii),5.0));//refit for wider camkii effect
    double deltakmup =0.00011;
    double plb = deltakmup*camfact;

//	alpha = pow((caI/(kmf)),hillSRcaP)-pow((caSr/(kmr)),hillSRcaP);
//	beta = 1.0+pow((caI/(kmf)),hillSRcaP)+pow((caSr/(kmr)),hillSRcaP);
	
		
	alpha = pow((caI/(kmf-plb)),hillSRcaP)-pow((caSr/(kmr)),hillSRcaP);
	beta = 1.0+pow((caI/(kmf-plb)),hillSRcaP)+pow((caSr/(kmr)),hillSRcaP);
	
    Jserca =Jsercafactor*VmaxSRcaP*(alpha/beta);

    Jsrleak =Jsrleakfactor*kleak*5.348E-6*(caSr-cajI) *0.9; //refit for wider camkii effect
	

	double dRyRP = dt*(Rxnbasal+Rxnckryr-Rxnpp1ryr-Rxnpp2ryr);//Soltis-Saucerman
    RyRP = RyRP + dRyRP;	//Soltis-Saucerman
}
void GpbAtrialOnal17::updatecytobuff(){
	
//cytosolic Ca buffers

   double dTnCl, dTnChc, dTnChm, dCaM, dMyoc, dMyom, dSRB;
   double konTnCl = 32.7;  
   double BmaxTnCl = 0.07;
   double koffTnCl = 0.0196;
   double konTnChCa = 2.37;
   double BmaxTnCh = 0.14;
   double koffTnChCa = 0.000032;
   double konTnChMg = 0.003;
   double koffTnChMg = 0.00333;
   double konCaM = 34.0;
   double BmaxCaM = 0.024;
   double koffCaM = 0.238;
   double konmyoCa = 13.8;
   double Bmaxmyosin = 0.140;
   double koffmyoCa = 0.00046;
   double konmyoMg = 0.0157;
   double koffmyoMg = 0.000057;
   double konsr = 100.0;
   double BmaxSR = 0.0171;
   double koffsr = 0.06;

   if (opts & ISO){
	koffTnCl = .0196*1.5;
	}


   dTnCl = dt*(konTnCl*caI*(BmaxTnCl-TnCl)-koffTnCl*TnCl);
   dTnChc = dt*(konTnChCa*caI*(BmaxTnCh-TnChc-TnChm)-koffTnChCa*TnChc);
   dTnChm = dt*(konTnChMg*mgI*(BmaxTnCh-TnChc-TnChm)-koffTnChMg*TnChm);
   dCaM = dt*(konCaM*caI*(BmaxCaM-CaM)-koffCaM*CaM);
   dMyoc = dt*(konmyoCa*caI*(Bmaxmyosin-Myoc-Myom)-koffmyoCa*Myoc);
   dMyom = dt*(konmyoMg*mgI*(Bmaxmyosin-Myoc-Myom)-koffmyoMg*Myom);
   dSRB = dt*(konsr*caI*(BmaxSR-SRB)-koffsr*SRB);

   J_cabcyto = (dTnCl+dTnChc+dTnChm+dCaM+dMyoc+dMyom+dSRB)/dt;
	
   TnCl = TnCl+dTnCl;
   TnChc = TnChc+dTnChc;
   TnChm = TnChm+dTnChm;
   CaM = CaM+dCaM;
   Myoc = Myoc+dMyoc;
   Myom = Myom+dMyom;
   SRB = SRB+dSRB;
}

void GpbAtrialOnal17::updateJSLbuff(){

//junctional and SL Ca Buffers
   double konsll = 100.0;
   double koffsll = 1.3;
   double konslh = 100;
   double koffslh = 0.03;
	
   double dSLLj, dSLLsl, dSLHj, dSLHsl;
   double BmaxSLlowj = 0.00046*(Vmyo/Vjunc);
   double BmaxSLlowsl = 0.0374*(Vmyo/Vsl);
   double BmaxSLhighj = 0.000165*(Vmyo/Vjunc);
   double BmaxSLhighsl = 0.0134*(Vmyo/Vsl);

   dSLLj = dt*(konsll*cajI*(BmaxSLlowj-SLLj)-koffsll*SLLj);
   dSLLsl = dt*(konsll*caslI*(BmaxSLlowsl-SLLsl)-koffsll*SLLsl);
   dSLHj = dt*(konslh*cajI*(BmaxSLhighj-SLHj)-koffslh*SLHj);
   dSLHsl = dt*(konslh*caslI*(BmaxSLhighsl-SLHsl)-koffslh*SLHsl);
	
   SLLj = SLLj + dSLLj;
   SLLsl = SLLsl + dSLLsl;
   SLHj = SLHj + dSLHj;
   SLHsl = SLHsl +dSLHsl;
	
   J_Cabjunc = (dSLLj + dSLHj)/dt;
   J_Cabsl = (dSLLsl + dSLHsl)/dt;
}

void GpbAtrialOnal17::updateSRbuff(){
	
//SR Ca buffer	
   double koncsqn = 100;
   double koffcsqn = 65;
   double Bmaxcsqn = 0.140*(Vmyo/Vsr);
//BmaxCsqnb???
	
   dCsqnb = dt*(koncsqn*caSr*(Bmaxcsqn-Csqnb)-koffcsqn*Csqnb);
	
   Csqnb = Csqnb + dCsqnb;
}

void GpbAtrialOnal17::updateIcal(){
	double condiso = 1;
	double pca = 2.7E-4;
	double pk = 1.35E-7;
	double pna = 7.5E-9;
	double Frdy = FDAY;
	double FoRT = FDAY/(RGAS*TEMP);
    double d_inf;//,taud;
    double f_inf;//, tauf;
	double icajbar, icaslbar, icakbar, icanajbar, icanaslbar;
	double df_cabjdt, df_cabsldt;

    double deltat = 28;
	double deltad = 5;
	double KMCAM = 0.2;
	double kmca = .002;
	
	double cafacj =1/(1.0+cajI/kmca);
	double cafacsl = 1/(1.0+caslI/kmca);
    double camfact = 1;
//	double camfact = caMkii/(0.15+caMkii);
//    double camfact = 1 + 1.2*(caMkii/(0.15+pow(caMkii,4)));
    double tauf_CaMK = 1/(1+pow((KMCAM/caMkii),2.0));

	d_inf = 1/(1+exp(-(vOld+9)/6)); 
    double taud = d_inf*(1-exp(-(vOld+9)/6))/(0.035*(vOld+9));
	
//	taud = d_inf*(1-exp(-(vOld+9)/6))/(0.035*(vOld+9)); 

	f_inf = 1/(1+exp((vOld+30)/7))+0.2/(1+exp((50-vOld)/20)); 
    double tauf = 1/(0.0197*exp(-(0.0337*(vOld+25))*(0.0337*(vOld+25)))+0.02)+deltat*tauf_CaMK;

//	tauf = 1/(0.0197*exp(-(0.0337*(vOld+25))*(0.0337*(vOld+25)))+0.02);

    if (opts & ISO){
        d_inf = 1/(1+exp(-(vOld+12)/6));
        taud = d_inf*(1-exp(-(vOld+12)/6))/(0.035*(vOld+12));
        f_inf = 1/(1+exp((vOld+33)/7))+0.2/(1+exp((47-vOld)/20));
        tauf = 1/(0.0197*exp(-(0.0337*(vOld+28))*(0.0337*(vOld+28)))+0.02)+deltat*tauf_CaMK;
        pca = 1.5*2.7E-4;
        pk = 1.5*1.35E-7;
        pna = 1.5*7.5E-9;
	}
 	Gate.f = f_inf-(f_inf-Gate.f)*exp(-dt/tauf);
	Gate.d = d_inf-(d_inf-Gate.d)*exp(-dt/taud);

//	df_cabjdt = 1.7*cajI*(1-gate.f_cabj)-11.9E-3*gate.f_cabj + deltatfca*camfact; 
//	df_cabsldt = 1.7*caslI*(1-gate.f_cabsl)-11.9E-3*gate.f_cabsl +deltat*camfact; 
//	gate.f_cabj=gate.f_cabj+df_cabjdt*dt;
//	gate.f_cabsl=gate.f_cabsl+df_cabsldt*dt;

    double taufcaj = 1/(1.7*cajI + 11.9E-3);
	double fcajinf = 1.7*cajI*taufcaj;
	Gate.f_cabj = fcajinf - (fcajinf - Gate.f_cabj)*exp(-dt/taufcaj);
    double taufcasl = 1/(1.7*caslI + 11.9E-3);
	double fcaslinf = 1.7*caslI*taufcasl;
	Gate.f_cabsl = fcaslinf - (fcaslinf - Gate.f_cabsl)*exp(-dt/taufcasl);
		
	icajbar = pca*4*(vOld*Frdy*FoRT)*(0.341*cajI*exp(2*vOld*FoRT)-0.341*caO) /(exp(2*vOld*FoRT)-1);
	icaslbar = pca*4*(vOld*Frdy*FoRT)*(0.341*caslI*exp(2*vOld*FoRT)-0.341*caO) /(exp(2*vOld*FoRT)-1);
	icakbar = pk*(vOld*Frdy*FoRT)*(0.75*kI*exp(vOld*FoRT)-0.75*kO)/(exp(vOld*FoRT)-1);
	icanajbar = pna*(vOld*Frdy*FoRT)*(0.75*najI*exp(vOld*FoRT)-0.75*naO)/(exp(vOld*FoRT)-1);
	icanaslbar = pna*(vOld*Frdy*FoRT)*(0.75*naslI*exp(vOld*FoRT)-0.75*naO)/(exp(vOld*FoRT)-1);
	
    iCajunc = Icalfactor*((F_juncCaL*icajbar*Gate.d*Gate.f*((1-Gate.f_cabj)))*0.45*1);
    iCasl = Icalfactor*((F_slCaL*icaslbar*Gate.d*Gate.f*((1-Gate.f_cabsl)))*0.45*1);
	iCa = iCajunc+iCasl;
    iCak = Icalfactor*((icakbar*Gate.d*Gate.f*(F_juncCaL*((1-Gate.f_cabj))+F_slCaL*((1-Gate.f_cabsl))))*0.45*1);

    iCanajunc = Icalfactor*((F_juncCaL*icanajbar*Gate.d*Gate.f*((1-Gate.f_cabj)))*0.45*1);
    iCanasl = Icalfactor*((F_slCaL*icanaslbar*Gate.d*Gate.f*((1-Gate.f_cabsl)))*.45*1);
	iCana = iCanajunc+iCanasl;
    iCaL = camfact*(iCa+iCak+iCana);
}

void GpbAtrialOnal17::updateIcab() {
    double gcab = 6.0643E-4; //A/F
	double Ecajunc = RGAS*TEMP/(2*FDAY)*log(caO/cajI);
	double Ecasl = RGAS*TEMP/(2*FDAY)*log(caO/caslI);
	
    iCabjunc = Icabfactor*(Fjunc*gcab*(vOld-Ecajunc));
    iCabsl = Icabfactor*(Fsl*gcab*(vOld-Ecasl));
	
    iCab = (iCabjunc+iCabsl);
}

void GpbAtrialOnal17::updateIpca() {
    	double ipmcabar = 0.0471; //A/F
	
	double kmpca = 0.0005;
	
    ipCajunc = Ipcafactor*((Fjunc*ipmcabar*pow(cajI, 1.6))/(pow(kmpca,1.6)+pow(cajI, 1.6)));
    ipCasl = Ipcafactor*((Fsl*ipmcabar*pow(caslI, 1.6))/(pow(kmpca,1.6)+pow(caslI, 1.6)));

    ipCa = (ipCajunc+ipCasl);
}

void GpbAtrialOnal17::updateIclca() {
	double gclca = 0.0548; //mS/uF
	double kd_clca = 0.1;
	double Ecl = -RGAS*TEMP/FDAY*log(clO/clI);

    iClcajunc = Iclcafactor*((Fjunc*gclca*(vOld-Ecl))/(1.0+kd_clca/cajI));
    iClcasl = Iclcafactor*((Fsl*gclca*(vOld-Ecl))/(1.0+kd_clca/caslI));
    iClca = (iClcajunc + iClcasl);
}

void GpbAtrialOnal17::updateIclbk(){
        double gclb = 0.009; //ms/uF
	double Ecl = -RGAS*TEMP/FDAY*log(clO/clI);
	
	iClbk = Iclbkfactor*gclb*(vOld-Ecl);
}

void GpbAtrialOnal17::updateIto() {
	//fast component variables
	double gtof = 0.165; //ms/uF
	double xs_inf,ys_inf;
	double tau_xf, tau_yf;
	
	double Ek = RGAS*TEMP/FDAY*log(kO/kI);
	
	iTos = 0.0;//gtos*gate.xs*gate.ys*(vOld-Ek);
	
	xs_inf = ((1)/(1+exp(-(vOld+1.0)/11.0)));
	tau_xf = 3.5*exp(-((vOld/30.0)*(vOld/30.0)))+1.5;

	ys_inf = ((1.0)/(1+exp((vOld+40.5)/11.5)));
	tau_yf = 25.635*exp(-(((vOld+52.45)/15.8827)*((vOld+52.45)/15.8827)))+24.14;

	Gate.xf = xs_inf-(xs_inf-Gate.xf)*exp(-dt/tau_xf);
	Gate.yf = ys_inf-(ys_inf-Gate.yf)*exp(-dt/tau_yf);
	
	iTof = gtof*Gate.xf*Gate.yf*(vOld-Ek);
	
	iTo = Itofactor*(iTos+iTof);
}

void GpbAtrialOnal17::updateIkur() {
	double condiso = 1;
	double gkur = 0.045;
	double xkur_inf,ykur_inf;
	double tau_xkur,tau_ykur;
	
	double Ek = RGAS*TEMP/FDAY*log(kO/kI);

	xkur_inf = ((1)/(1+exp((vOld+6)/-8.6)));
	tau_xkur = 9/(1+exp((vOld+5)/12.0))+0.5;

	ykur_inf = ((1)/(1+exp((vOld+7.5)/10)));
	tau_ykur = 590/(1+exp((vOld+60)/10.0))+3050;

 	Gate.xkur = xkur_inf-(xkur_inf-Gate.xkur)*exp(-dt/tau_xkur);
	Gate.ykur = ykur_inf-(ykur_inf-Gate.ykur)*exp(-dt/tau_ykur);
	
    if (opts & ISO){
	condiso = 3;
	}
	iKur = Ikurfactor* condiso*gkur*Gate.xkur*Gate.ykur*(vOld-Ek);
}

void GpbAtrialOnal17::updateIks() {
	double condiso = 1;
	double gksjunc = 0.0035; //ms/uF
	double gkssl = 0.0035; //ms/uF

	double xs_inf, tau_xs;
    double pnak =0.01833;
	double EKsjunc = RGAS*TEMP/FDAY*log((kO+pnak*naO)/(kI+pnak*naI));
	double EKssl = RGAS*TEMP/FDAY*log((kO+pnak*naO)/(kI+pnak*naI));

	xs_inf = 1.0/(1.0+exp((-3.8-vOld)/14.25));
	tau_xs = 990.1/(1.0+exp((-2.436-vOld)/14.12));

    if (opts & ISO){
	condiso = 3;
	xs_inf = 1.0/(1.0+exp((-43.8-vOld)/14.25));
	tau_xs = 990.1/(1.0+exp((-42.436-vOld)/14.12));
	}

	Gate.xks = xs_inf-(xs_inf-Gate.xks)*exp(-dt/tau_xs);


    iKsjunc = Iksfactor*condiso*Fjunc*gksjunc*Gate.xks*Gate.xks*(vOld-EKsjunc);
    iKssl = Iksfactor*condiso*Fsl*gkssl*Gate.xks*Gate.xks*(vOld-EKssl);

    iKs = (iKsjunc+iKssl);

}

void GpbAtrialOnal17::updateIkr() {
	
	double gkr = 0.035*sqrt(kO/5.4);
	double Ek = RGAS*TEMP/FDAY*log(kO/kI);
	double r_kr;
	double xr_inf, alpha_xr, beta_xr, tau_xr;
	
	xr_inf = 1.0/(1.0+exp((-vOld-10.0)/5.0));
	alpha_xr = (550.0/(1.0+exp((-22.0-vOld)/9.0)))*(6.0/(1.0+exp((vOld+11.0)/9.0)));
	beta_xr = 230.0/(1.0+exp((vOld+40.0)/20.0));
	tau_xr = alpha_xr + beta_xr;
	Gate.xkr = xr_inf-(xr_inf-Gate.xkr)*exp(-dt/tau_xr);
	
	r_kr = 1.0/(1.0+exp((vOld+74.0)/24.0));

	iKr = Ikrfactor*gkr*Gate.xkr*r_kr*(vOld-Ek);
}

void GpbAtrialOnal17::updateIk1() {
	double alpha_K1, beta_K1, k1_inf;

	double Ek = RGAS*TEMP/FDAY*log(kO/kI);

	alpha_K1 = 1.02/(1.0+exp(0.2385*(vOld-Ek-59.215)));
	beta_K1 = (0.49124*exp(0.08032*(vOld-Ek+5.476))+exp(0.06175*(vOld-Ek-594.31)))/(1+exp(-0.5143*(vOld-Ek+4.753)));
	k1_inf = alpha_K1/(alpha_K1+beta_K1);
	
	iK1 = Ik1factor*0.0525*sqrt(kO/5.4)*k1_inf*(vOld-Ek);
}

void GpbAtrialOnal17::updateIpk() {
    	double gkp = 2*0.001;//0.002; //ms/uF
	double kp_kp;
	double Ek = RGAS*TEMP/FDAY*log(kO/kI);
	
	kp_kp = 1.0/(1.0+exp(7.488-vOld/5.98));
    iKpjunc = Ipkfactor*Fjunc*gkp*kp_kp*(vOld-Ek);
    iKpsl = Ipkfactor*Fsl*gkp*kp_kp*(vOld-Ek);
    iKp = (iKpjunc+iKpsl);
}

void GpbAtrialOnal17::updateInaca() {
	double incxbar = 3.15; //A/F
	double kajunc, kasl;
	double s1junc, s1sl, s2junc, s3junc, s2sl, s3sl;
	double kmcai = 0.00359;
	double kmnai = 12.29;
	double kmnao = 87.5;
	double kmcao = 1.3;
	double ksat = 0.27;
	double kdact = 0.384E-3;
	double nu = 0.35;
	
	kajunc = 1.0/(1.0+((kdact/cajI)*(kdact/cajI)));
	kasl = 1.0/(1.0+((kdact/caslI)*(kdact/caslI)));
	
	s1junc = exp(nu*vOld*(FDAY/RGAS/TEMP))*pow(najI,3)*caO;
	s1sl = exp(nu*vOld*(FDAY/RGAS/TEMP))*pow(naslI,3)*caO;
	s2junc = exp((nu-1)*vOld*(FDAY/RGAS/TEMP))*pow(naO,3)*cajI;
	s3junc = kmcai*pow(naO,3)*(1+pow((najI/kmnai),3))+pow(kmnao,3)*cajI*(1+cajI/kmcai)+kmcao*pow(najI,3)+pow(najI,3)*caO+pow(naO,3)*cajI;
	s2sl = exp((nu-1)*vOld*(FDAY/RGAS/TEMP))*pow(naO,3)*caslI;
	s3sl = kmcai*pow(naO,3)*(1+pow((naslI/kmnai),3))+pow(kmnao,3)*caslI*(1+(caslI/kmcai))+kmcao*pow(naslI,3)+pow(naslI,3)*caO+pow(naO,3)*caslI;
	
    iNcxjunc = Inacafactor*((Fjunc*incxbar*kajunc*(s1junc-s2junc))/(s3junc*(1+ksat*exp((nu-1)*vOld*(FDAY/RGAS/TEMP)))));
    iNcxsl = Inacafactor*((Fsl*incxbar*kasl*(s1sl-s2sl))/(s3sl*(1+ksat*exp((nu-1)*vOld*(FDAY/RGAS/TEMP)))));
	
    iNaca = (iNcxjunc+iNcxsl);
}

void GpbAtrialOnal17::updateInak() {

	double inakbar = 1.26;
	double sigma, fnak;
	double condiso = 1;

    if (opts & ISO){
	condiso = 0.75;
	}
	
	double kmnaip = condiso*11;
	double kmko = 1.5;
	double FoRT = FDAY/(RGAS*TEMP);

	sigma = (exp(naO/67.3)-1)/7;
	fnak = 1/(1+0.1245*exp(-0.1*vOld*FoRT)+0.0365*sigma*exp(-vOld*FoRT));
    iNaKjunc = Inakfactor*(Fjunc*inakbar*fnak*kO/(1+pow(kmnaip/najI,4.0))/(kO+kmko));
    iNaKsl = Inakfactor*(Fsl*inakbar*fnak*kO/(1+pow(kmnaip/naslI,4.0))/(kO+kmko));
    iNak = (iNaKjunc+iNaKsl);
}
void GpbAtrialOnal17::updatenaI() {

   double dNabjdt, dNabsldt;
   double konNa = 0.0001;
   double koffNa = 0.001;
   double BmaxNaj = 7.561;
   double BmaxNasl = 1.65;
   double dnajI, dnaslI, dnaI;
   double J_najuncsl = 1.83128E-8;
   double J_naslmyo = 1.63863E-6;

//sodium buffers
   dNabjdt = (konNa*najI*(BmaxNaj-Nabj)-koffNa*Nabj);
   dNabsldt = (konNa*naslI*(BmaxNasl-Nabsl)-koffNa*Nabsl);
	

//sodium concentration
	
	dnajI = dt*(-iNatotjunc*(ACap/(Vjunc*FDAY))+(J_najuncsl/Vjunc)*(naslI-najI)-dNabjdt);
	dnaslI = dt*(-iNatotsl*(ACap/(Vsl*FDAY))+(J_najuncsl/Vsl)*(najI-naslI)+(J_naslmyo/Vsl)*(naI-naslI)-dNabsldt);
	dnaI = dt*((J_naslmyo/Vmyo)*(naslI-naI));
	
   Nabj = Nabj + dt*dNabjdt;
   Nabsl = Nabsl + dt*dNabsldt;
	najI = najI + dnajI;
	naslI = naslI + dnaslI;
	naI = naI + dnaI;
}

//update kI? (not apart of original model. formulation from FR model
//not used
void GpbAtrialOnal17::updatekI()
{
    double dkI = -dt*(iKt*ACap)/(Vmyo*1*FDAY);
    kI += dkI;
}
void GpbAtrialOnal17::updateClI()
{
    double dclI = -dt*(iClt*ACap)/(Vmyo*-1*FDAY);
    clI += dclI;
}
//end mod

void GpbAtrialOnal17::updateIna() {
        double gna = 23.0; //mS/uF
	double Enajunc = RGAS*TEMP/FDAY*log(naO/najI);
	double Enasl = RGAS*TEMP/FDAY*log(naO/naslI);
	double m_inf, tau_m;
	double h_inf, alpha_h, beta_h, tau_h;
	double j_inf, alpha_j, beta_j, tau_j;

	m_inf = 1/((1+exp(-(56.86+vOld)/9.03))*(1+exp(-(56.86+vOld)/9.03)));
	tau_m = 0.1292*exp(-((vOld+45.79)/15.54)*((vOld+45.79)/15.54))+0.06487*exp(-((vOld-4.823)/51.12)*((vOld-4.823)/51.12));   
	
	if(vOld>=-40.0){
		alpha_h = 0.0;
		beta_h = (0.77/(0.13*(1+exp(-(vOld+10.66)/11.1))));
	}
	else {
		alpha_h=(0.057*exp(-(vOld+80)/6.8));	
		beta_h=((2.7*exp(0.079*vOld)+3.1E5*exp(0.3485*vOld)));	
	}

	h_inf = 1/((1+exp((vOld+71.55)/7.43))*(1+exp((vOld+71.55)/7.43)));
	tau_h = 1.0/(alpha_h+beta_h);

	if(vOld>=-40.0){
		alpha_j = 0.0;
		beta_j=((0.6*exp(0.057*vOld))/(1+exp(-0.1*(vOld+32))));
	}
	else {
		alpha_j=(((-2.5428E4*exp(0.2444*vOld)-6.948E-6*exp(-0.04391*vOld))*(vOld+37.78))/(1+exp(0.311*(vOld+79.23))));
		beta_j=((0.02424*exp(-0.01052*vOld))/(1+exp(-0.1378*(vOld+40.14))));
}

	tau_j = 1/(alpha_j+beta_j);
	j_inf = 1/((1+exp((vOld+71.55)/7.43))*(1+exp((vOld+71.55)/7.43)));   

	Gate.m = m_inf-(m_inf-Gate.m)*exp(-dt/tau_m);
	Gate.h = h_inf-(h_inf-Gate.h)*exp(-dt/tau_h);
	Gate.j = j_inf-(j_inf-Gate.j)*exp(-dt/tau_j);

    iNajunc = Inafactor*Fjunc*gna*Gate.m*Gate.m*Gate.m*Gate.h*Gate.j*(vOld-Enajunc);
    iNasl = Inafactor*Fsl*gna*Gate.m*Gate.m*Gate.m*Gate.h*Gate.j*(vOld-Enasl);
    iNa = (iNajunc + iNasl);
}

void GpbAtrialOnal17::updateInab() {
	double gnab = 0.597E-3; //mS/uF
	
	double Enajunc = RGAS*TEMP/FDAY*log(naO/najI);
	double Enasl = RGAS*TEMP/FDAY*log(naO/naslI);
	
    iNabjunc = Inabfactor*Fjunc*gnab*(vOld-Enajunc);
    iNabsl = Inabfactor*Fsl*gnab*(vOld-Enasl);
    iNab = (iNabjunc + iNabsl);
}

void GpbAtrialOnal17::updateCurr() {
 	updateIcal();
	updateIcab();
	updateIpca();
        updateIto();
 	updateIks();
	updateIkr();
	updateIkur();
	updateIk1();
	updateIpk();
	updateInaca();
	updateInak();
	updateInab();
	updateIna();
	updateIclca();
	updateIclbk();
  	updateInal();
	
	iNatotjunc = iNajunc+iNabjunc+3.0*iNcxjunc+3.0*iNaKjunc+iCanajunc;
	iNatotsl =iNasl+iNabsl+3.0*iNcxsl+3.0*iNaKsl+iCanasl+iNal;
	iNat = iNatotjunc+iNatotsl;
	iClt = iClca+iClbk;
	iCatotjunc=iCajunc+iCabjunc+ipCajunc-2*iNcxjunc;
	iCatotsl=iCasl+iCabsl+ipCasl-2*iNcxsl;
	iCat=iCatotjunc+iCatotsl;
    iKt =  iTo+iKr+iKs+iK1+iKur-2*iNak+iCak+iKp;

	iTot = iNat+iClt+iCat+iKt;
};

void GpbAtrialOnal17::updateConc() {
	updateSRFlux();
	updatecytobuff();
	updateJSLbuff();
	updateSRbuff();
        updateCamk();
        //concentrations
	updatecaI(); 
	updatenaI();
//    updatekI();
//    updateClI();
};

int GpbAtrialOnal17::externalStim(double stimval) {
    iTot = iTot + stimval;
    return 1;
}

// External stimulus.
//int GpbAtrialOnal17::stim()
//{
//  if(t>=stimt&&t<(stimt+dur)){
//    if(flag==0){
//      cout << "Stimulus to " << type << " at t = " << t << endl;
//      num++;
//      flag=1;
//      if(num>=numstims)
//         return 0;
//    }
//    iTot = iTot + val;
//  }
//  else if(flag==1){     //trailing edge of stimulus
//        stimt=stimt+bcl;
//        flag=0;
//        apTime = 0.0;
//  }
//
//  apTime = apTime+dt;
//
//  return 1;
//};

// Create map for easy retrieval of variable values.
void GpbAtrialOnal17::makemap()
{
  vars["vOld"]=&vOld;
  vars["t"]=&t;
  vars["dVdt"]=&dVdt;
  vars["naI"]=&naI;
  vars["kI"]=&kI;
  vars["clI"]=&clI;
  vars["caI"]=&caI;
  vars["caslI"]=&caslI;
  vars["cajI"]=&cajI;
  vars["caSr"]=&caSr;
  vars["najI"]=&najI;
  vars["naslI"]=&naslI;
  vars["mgI"]=&mgI;
  vars["CaM"]=&CaM;
  vars["Csqnb"]=&Csqnb;
  vars["iCa"]=&iCa;
  vars["iCab"]=&iCab;
  vars["iNa"]=&iNa;
  vars["iNak"]=&iNak;
  vars["iNal"]=&iNal;
  vars["iNaca"]=&iNaca;

  vars["Gate.m"]=&Gate.m;
  vars["Gate.h"]=&Gate.h;
  vars["Gate.j"]=&Gate.j;
  vars["Gate.xkr"]=&Gate.xkr;
  vars["Gate.xks"]=&Gate.xks;
  vars["Gate.xf"]=&Gate.xf;
  vars["Gate.yf"]=&Gate.yf;
  vars["Gate.xkur"]=&Gate.xkur;
  vars["Gate.ykur"]=&Gate.ykur;
  vars["Gate.d"]=&Gate.d;
  vars["Gate.f"]=&Gate.f;
  vars["Gate.f_cabj"]=&Gate.f_cabj;
  vars["Gate.f_cabsl"]=&Gate.f_cabsl;
  vars["Gate.h "]=&Gate.h;
  vars["Gate.hl"]=&Gate.hl;
  vars["Gate.ml"]=&Gate.ml;
  
  vars["iTos"]=&iTos;
  vars["iTof"]=&iTof;
  vars["iTo"]=&iTo;
  vars["iKsjunc"]=&iKsjunc;
  vars["iKssl"]=&iKssl;
  vars["iKs"]=&iKs;
  vars["iKr"]=&iKr;
  vars["iKur"]=&iKur;
  vars["iKpjunc"]=&iKpjunc;
  vars["iKpsl"]=&iKpsl;
  vars["iKp"]=&iKp;
  vars["iK1"]=&iK1;
  vars["iNajunc"]=&iNajunc;
  vars["iNasl"]=&iNasl;
  vars["iNabjunc"]=&iNabjunc;
  vars["iNabsl"]=&iNabsl;
  vars["iNab"]=&iNab;
  vars["iNa"]=&iNa;
  vars["iCajunc"]=&iCajunc;
  vars["iCasl"]=&iCasl;
  vars["iCa"]=&iCa;
  vars["iCaL"]=&iCaL;
  vars["iCab"]=&iCab;
  vars["ipCa"]=&ipCa;
  vars["iCak"]=&iCak;
  vars["iCanajunc"]=&iCanajunc;
  vars["iCanasl"]=&iCanasl;
  vars["iCana"]=&iCana;
  vars["iNaKjunc"]=&iNaKjunc;
  vars["iNaKsl"]=&iNaKsl;
  vars["iNak"]=&iNak;
  vars["iNcxjunc"]=&iNcxjunc;
  vars["iNcxsl"]=&iNcxsl;
  vars["iNaca"]=&iNaca;
  vars["iClcajunc"]=&iClcajunc;
  vars["iClcasl"]=&iClcasl;
  vars["iClca"]=&iClca;
  vars["iClbk"]=&iClbk;
  vars["iClcajunc"]=&iClcajunc;
  vars["ipCajunc"]=&ipCajunc;
  vars["ipCasl"]=&ipCasl;
  vars["ipCa"]=&ipCa;
  vars["iCabjunc"]=&iCabjunc;
  vars["iCabsl"]=&iCabsl;
  vars["iCab"]=&iCab;

  vars["CaMKII"]=&caMkii;
  vars["fBlock"]=&fBlock;
  vars["fBound"]=&fBound;
  vars["fI"]=&fI;
  vars["fOx"]=&fOx;
  vars["fOxP"]=&fOxP;
  vars["fPhos"]=&fPhos;
  vars["fiNalP"]=&fiNalP;
  vars["RyRP"]=&RyRP;
  vars["RyRratio"]=&RyRratio;
  vars["Ryro"]=&Ryro;
  vars["Ryri"]=&Ryri;
  vars["Ryrr"]=&Ryrr;

  vars["Jserca"]=&Jserca;
  vars["Jsrcarel"]=&Jsrcarel;
  vars["Jsrleak"]=&Jsrleak;

  pars["IcalFactor"]=&Icalfactor;
  pars["IcabFactor"]=&Icabfactor;
  pars["IpcaFactor"]=&Ipcafactor;
  pars["ItoFactor"]=&Itofactor;
  pars["IksFactor"]=&Iksfactor;
  pars["IkrFactor"]=&Ikrfactor;
  pars["Ik1Factor"]=&Ik1factor;
  pars["IkurFactor"]=&Ikurfactor;
  pars["IpkFactor"]=&Ipkfactor;
  pars["InacaFactor"]=&Inacafactor;
  pars["InakFactor"]=&Inakfactor;
  pars["InabFactor"]=&Inabfactor;
  pars["InaFactor"]=&Inafactor;
  pars["IclcaFactor"]=&Iclcafactor;
  pars["IclbkFactor"]=&Iclbkfactor;
  pars["InalFactor"]=&Inalfactor;
  pars["JSRcarelFactor"]=&JSRcarelfactor;
  pars["JsercaFactor"]=&Jsercafactor;
  pars["Inabfactor"]=&Inabfactor;
  pars["IcaMkiiFactor"]=&IcaMkiiFactor;
  pars["InalPFactor"]=&InalPFactor;
  pars["JsrleakFactor"]=&Jsrleakfactor;
  pars["ROSFactor"]=&ROSFactor;
}
const char* GpbAtrialOnal17::type() const {
    return "GpbAtrialOnal17";
}

MAKE_OPTIONS_FUNCTIONS(GpbAtrialOnal17)


//######################################################
// This file contains all function definitions for
// Grandi model of the human atrial action 
// potential as published in Grandi et al.
// Circ Res 2011.
//
// Copyright (C) 2012 Thomas J. Hund.
//######################################################

#include "gpbatrial.h"

GpbAtrial::GpbAtrial()
{
    this->Initialize();
}

GpbAtrial::GpbAtrial(GpbAtrial& toCopy) : Cell(toCopy) {
   this->Initialize(); 
}

GpbAtrial::~GpbAtrial()
{
}

void GpbAtrial::Initialize() {
    type = "Control";
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
//    flag = 0;
//    num = 0;

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
    Csqnb = 1.242988;
//##### Gating Variables #####
	gate.m = 0.001405627;
	gate.h = .9867005;
	gate.j = .991562;
	gate.xkr = .008641386;
	gate.xks = .005412034;
	gate.xf = 0.004051574;
	gate.yf = .9945511;
	gate.xkur = 0.0;
	gate.ykur = 1.0;
	gate.d = 0.000007175662;
	gate.f = 1.00;
	gate.f_cabj = .02421991;
	gate.f_cabsl = .01452605;
	
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
	iNaKjunc = iNaKsl = iNak = 0.0;
	iNcxjunc = iNcxsl = iNaca = 0.0;
	iClcajunc = iClcasl = iClca = iClbk = 0.0;
	ipCajunc = ipCasl = ipCa = 0.0;
	iCabjunc = iCabsl = iCab = 0.0;
    makemap();

}
GpbAtrial* GpbAtrial::clone() {
    return new GpbAtrial(*this);
}

void GpbAtrial::updateConc() {
	updateSRFlux();
	updatecytobuff(); //cytosolic Ca buffers
	updateJSLbuff(); //junctional and SL Ca buffers
	updateSRbuff(); //SR Ca buffer	 

    updatecaI();
    updatenaI();
}

void GpbAtrial::updatecaI() {

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


void GpbAtrial::updateSRFlux(){

//SR fluxes
	double dRyrr, dRyro, dRyri;
	double maxsr = 15.0;
	double minsr = 1.0;
	double kCasr, kOsrca, kIsrca;
	double ec50SR = 0.45;
	double koCa = 10.0; 
	double kiCa =0.5;
	double kim = 0.005;
	double kom = 0.06;
	double ks = 25.0;
	double alpha, beta;
	double kmf = 2.5*0.000246;
	double kmr = 1.7;
	double hillSRcaP = 1.787;
	double VmaxSRcaP = 0.0053114;
	
	kCasr = maxsr -((maxsr-minsr)/(1.0+pow((ec50SR/caSr),2.5)));
	kOsrca = koCa/kCasr;
	kIsrca = kiCa*kCasr;
	
	
	dRyrr = dt*((kim*RI-kIsrca*cajI*Ryrr)-(kOsrca*cajI*cajI*Ryrr-kom*Ryro));
	dRyro = dt*((kOsrca*cajI*cajI*Ryrr-kom*Ryro)-(kIsrca*cajI*Ryro-kim*Ryri));
	dRyri = dt*((kIsrca*cajI*Ryro-kim*Ryri)-(kom*Ryri-kOsrca*cajI*cajI*RI));
  	
	Ryrr = Ryrr + dRyrr;
	Ryro = Ryro + dRyro;
	Ryri = Ryri + dRyri;
	RI = 1.0-Ryrr-Ryro-Ryri; 
	
	Jsrcarel = ks*Ryro*(caSr-cajI);
	
	alpha = pow((caI/kmf),hillSRcaP)-pow((caSr/kmr),hillSRcaP);
	beta = 1.0+pow((caI/kmf),hillSRcaP)+pow((caSr/kmr),hillSRcaP);
	Jserca = VmaxSRcaP*(alpha/beta);
	
	Jsrleak =0.000005348*(caSr-cajI);
	
	
}
void GpbAtrial::updatecytobuff(){
	
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

void GpbAtrial::updateJSLbuff(){

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

void GpbAtrial::updateSRbuff(){
	
//SR Ca buffer	
   double koncsqn = 100;
   double koffcsqn = 65;
	
   dCsqnb = dt*(koncsqn*caSr*(0.140*(Vmyo/Vsr)-Csqnb)-koffcsqn*Csqnb);
	
   Csqnb = Csqnb + dCsqnb;
}

void GpbAtrial::updateCurr() {
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
	updateIclca();
	updateIclbk();
	updateIna();




	iNatotjunc = iNajunc+iNabjunc+3.0*iNcxjunc+3.0*iNaKjunc+iCanajunc;
	iNatotsl = iNasl+iNabsl+3.0*iNcxsl+3.0*iNaKsl+iCanasl ;
	iNat = iNatotjunc+iNatotsl;
	iClt = iClca+iClbk;
	iCatotjunc=iCajunc+iCabjunc+ipCajunc-2*iNcxjunc;
	iCatotsl=iCasl+iCabsl+ipCasl-2*iNcxsl;
	iCat=iCatotjunc+iCatotsl;
	iKt =  iTo+iKr+iKs+iK1+iKur-2*iNak+iCak+iKp;
	iTot = iNat+iClt+iCat+iKt;

	

}

void GpbAtrial::updateIcal(){
	double pca = 2.7E-4;
	double pk = 1.35E-7;
	double pna = 7.5E-9;
	double Frdy = FDAY;
	double FoRT = FDAY/(RGAS*TEMP);
	double d_inf,taud;
	double f_inf, tauf;
	double icajbar, icaslbar, icakbar, icanajbar, icanaslbar;
	double df_cabjdt, df_cabsldt;

	d_inf = 1/(1+exp(-(vOld+9)/6)); 
	taud = d_inf*(1-exp(-(vOld+9)/6))/(0.035*(vOld+9)); 
	f_inf = 1/(1+exp((vOld+30)/7))+0.2/(1+exp((50-vOld)/20)); 
	tauf = 1/(0.0197*exp(-(0.0337*(vOld+25))*(0.0337*(vOld+25)))+0.02);

 	gate.f = f_inf-(f_inf-gate.f)*exp(-dt/tauf);
	gate.d = d_inf-(d_inf-gate.d)*exp(-dt/taud);

	df_cabjdt = 1.7*cajI*(1-gate.f_cabj)-11.9E-3*gate.f_cabj; 
	df_cabsldt = 1.7*caslI*(1-gate.f_cabsl)-11.9E-3*gate.f_cabsl; 
	
	gate.f_cabj=gate.f_cabj+df_cabjdt*dt;
	gate.f_cabsl=gate.f_cabsl+df_cabsldt*dt;
	
	icajbar = pca*4*(vOld*Frdy*FoRT)*(0.341*cajI*exp(2*vOld*FoRT)-0.341*caO) /(exp(2*vOld*FoRT)-1);
	icaslbar = pca*4*(vOld*Frdy*FoRT)*(0.341*caslI*exp(2*vOld*FoRT)-0.341*caO) /(exp(2*vOld*FoRT)-1);
	icakbar = pk*(vOld*Frdy*FoRT)*(0.75*kI*exp(vOld*FoRT)-0.75*kO)/(exp(vOld*FoRT)-1);
	icanajbar = pna*(vOld*Frdy*FoRT)*(0.75*najI*exp(vOld*FoRT)-0.75*naO)/(exp(vOld*FoRT)-1);
	icanaslbar = pna*(vOld*Frdy*FoRT)*(0.75*naslI*exp(vOld*FoRT)-0.75*naO)/(exp(vOld*FoRT)-1);
	
	iCajunc = (F_juncCaL*icajbar*gate.d*gate.f*((1-gate.f_cabj)))*0.45*1;
	iCasl = (F_slCaL*icaslbar*gate.d*gate.f*((1-gate.f_cabsl)))*0.45*1;
	iCa = iCajunc+iCasl;
	iCak = (icakbar*gate.d*gate.f*(F_juncCaL*((1-gate.f_cabj))+F_slCaL*((1-gate.f_cabsl))))*0.45*1;
	iCanajunc = (F_juncCaL*icanajbar*gate.d*gate.f*((1-gate.f_cabj)))*0.45*1;
	iCanasl = (F_slCaL*icanaslbar*gate.d*gate.f*((1-gate.f_cabsl)))*.45*1;
	iCana = iCanajunc+iCanasl;
	iCaL = iCa+iCak+iCana;
}

void GpbAtrial::updateIcab() {
        double gcab = 6.0643E-4; //A/F
	double Ecajunc = RGAS*TEMP/(2*FDAY)*log(caO/cajI);
	double Ecasl = RGAS*TEMP/(2*FDAY)*log(caO/caslI);
	
	iCabjunc = Fjunc*gcab*(vOld-Ecajunc);
	iCabsl = Fsl*gcab*(vOld-Ecasl);
	
	iCab = iCabjunc+iCabsl;
}

void GpbAtrial::updateIpca() {
    	double ipmcabar = 0.0471; //A/F
	
	double kmpca = 0.0005;
	
	ipCajunc = (Fjunc*ipmcabar*pow(cajI, 1.6))/(pow(kmpca,1.6)+pow(cajI, 1.6));
	ipCasl = (Fsl*ipmcabar*pow(caslI, 1.6))/(pow(kmpca,1.6)+pow(caslI, 1.6));

	ipCa = ipCajunc+ipCasl;
}

void GpbAtrial::updateIclca() {
	double gclca = 0.0548; //mS/uF
	double kd_clca = 0.1;
	double Ecl = -RGAS*TEMP/FDAY*log(clO/clI);

	iClcajunc = (Fjunc*gclca*(vOld-Ecl))/(1.0+kd_clca/cajI);
	iClcasl = (Fsl*gclca*(vOld-Ecl))/(1.0+kd_clca/caslI);
	iClca = iClcajunc + iClcasl;
}

void GpbAtrial::updateIclbk(){
        double gclb = 0.009; //ms/uF
	double Ecl = -RGAS*TEMP/FDAY*log(clO/clI);
	
	iClbk = gclb*(vOld-Ecl);
}

void GpbAtrial::updateIto() {
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

	gate.xf = xs_inf-(xs_inf-gate.xf)*exp(-dt/tau_xf);
	gate.yf = ys_inf-(ys_inf-gate.yf)*exp(-dt/tau_yf);
	
	iTof = gtof*gate.xf*gate.yf*(vOld-Ek);
	
	iTo = iTos+iTof;
}

void GpbAtrial::updateIkur() {
	double gkur = 0.045;
	double xkur_inf,ykur_inf;
	double tau_xkur,tau_ykur;
	
	double Ek = RGAS*TEMP/FDAY*log(kO/kI);

	xkur_inf = ((1)/(1+exp((vOld+6)/-8.6)));
	tau_xkur = 9/(1+exp((vOld+5)/12.0))+0.5;

	ykur_inf = ((1)/(1+exp((vOld+7.5)/10)));
	tau_ykur = 590/(1+exp((vOld+60)/10.0))+3050;

 	gate.xkur = xkur_inf-(xkur_inf-gate.xkur)*exp(-dt/tau_xkur);
	gate.ykur = ykur_inf-(ykur_inf-gate.ykur)*exp(-dt/tau_ykur);
	
	iKur = 1*gkur*gate.xkur*gate.ykur*(vOld-Ek);
}

void GpbAtrial::updateIks() {
	double gksjunc = 0.0035; //ms/uF
	double gkssl = 0.0035; //ms/uF

	double xs_inf, tau_xs;
    	double pnak =0.01833; 
	double EKsjunc = RGAS*TEMP/FDAY*log((kO+pnak*naO)/(kI+pnak*naI));
	double EKssl = RGAS*TEMP/FDAY*log((kO+pnak*naO)/(kI+pnak*naI));
	
	xs_inf = 1.0/(1.0+exp((-3.8-vOld)/14.25));
	tau_xs = 990.1/(1.0+exp((-2.436-vOld)/14.12));

	gate.xks = xs_inf-(xs_inf-gate.xks)*exp(-dt/tau_xs);

	iKsjunc = Fjunc*gksjunc*gate.xks*gate.xks*(vOld-EKsjunc);
	iKssl = Fsl*gkssl*gate.xks*gate.xks*(vOld-EKssl);

	iKs = iKsjunc+iKssl;

}

void GpbAtrial::updateIkr() {
	
	double gkr = 0.035*sqrt(kO/5.4);
	double Ek = RGAS*TEMP/FDAY*log(kO/kI);
	double r_kr;
	double xr_inf, alpha_xr, beta_xr, tau_xr;
	
	xr_inf = 1.0/(1.0+exp((-vOld-10.0)/5.0));
	alpha_xr = (550.0/(1.0+exp((-22.0-vOld)/9.0)))*(6.0/(1.0+exp((vOld+11.0)/9.0)));
	beta_xr = 230.0/(1.0+exp((vOld+40.0)/20.0));
	tau_xr = alpha_xr + beta_xr;
	gate.xkr = xr_inf-(xr_inf-gate.xkr)*exp(-dt/tau_xr);
	
	r_kr = 1.0/(1.0+exp((vOld+74.0)/24.0));

	iKr = gkr*gate.xkr*r_kr*(vOld-Ek);
}

void GpbAtrial::updateIk1() {
	double alpha_K1, beta_K1, k1_inf;

	double Ek = RGAS*TEMP/FDAY*log(kO/kI);

	alpha_K1 = 1.02/(1.0+exp(0.2385*(vOld-Ek-59.215)));
	beta_K1 = (0.49124*exp(0.08032*(vOld-Ek+5.476))+exp(0.06175*(vOld-Ek-594.31)))/(1+exp(-0.5143*(vOld-Ek+4.753)));
	k1_inf = alpha_K1/(alpha_K1+beta_K1);
	
	iK1 = 0.0525*sqrt(kO/5.4)*k1_inf*(vOld-Ek);
}

void GpbAtrial::updateIpk() {
    	double gkp = 2*0.001;//0.002; //ms/uF
	double kp_kp;
	double Ek = RGAS*TEMP/FDAY*log(kO/kI);
	
	kp_kp = 1.0/(1.0+exp(7.488-vOld/5.98));
	iKpjunc = Fjunc*gkp*kp_kp*(vOld-Ek);
	iKpsl = Fsl*gkp*kp_kp*(vOld-Ek);
	iKp = iKpjunc+iKpsl;
}

void GpbAtrial::updateInaca() {
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
	
	iNcxjunc = (Fjunc*incxbar*kajunc*(s1junc-s2junc))/(s3junc*(1+ksat*exp((nu-1)*vOld*(FDAY/RGAS/TEMP))));
	iNcxsl = (Fsl*incxbar*kasl*(s1sl-s2sl))/(s3sl*(1+ksat*exp((nu-1)*vOld*(FDAY/RGAS/TEMP))));
	
	iNaca = iNcxjunc+iNcxsl;
}

void GpbAtrial::updateInak() {
	double inakbar = 1.26;
	double sigma, fnak;
	double kmnaip = 11;
	double kmko = 1.5;
	double FoRT = FDAY/(RGAS*TEMP);

	sigma = (exp(naO/67.3)-1)/7;
	fnak = 1/(1+0.1245*exp(-0.1*vOld*FoRT)+0.0365*sigma*exp(-vOld*FoRT));
	iNaKjunc = 1*Fjunc*inakbar*fnak*kO/(1+pow(kmnaip/najI,4.0))/(kO+kmko);
	iNaKsl = 1*Fsl*inakbar*fnak*kO/(1+pow(kmnaip/naslI,4.0))/(kO+kmko);
	iNak = iNaKjunc+iNaKsl;
}
void GpbAtrial::updatenaI() {

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

void GpbAtrial::updateIna() {
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

	gate.m = m_inf-(m_inf-gate.m)*exp(-dt/tau_m);
	gate.h = h_inf-(h_inf-gate.h)*exp(-dt/tau_h);
	gate.j = j_inf-(j_inf-gate.j)*exp(-dt/tau_j);

	iNajunc = Fjunc*gna*gate.m*gate.m*gate.m*gate.h*gate.j*(vOld-Enajunc);
	iNasl = Fsl*gna*gate.m*gate.m*gate.m*gate.h*gate.j*(vOld-Enasl);		
	iNa = iNajunc + iNasl;
}

void GpbAtrial::updateInab() {
	double gnab = 0.597E-3; //mS/uF
	
	double Enajunc = RGAS*TEMP/FDAY*log(naO/najI);
	double Enasl = RGAS*TEMP/FDAY*log(naO/naslI);
	
	iNabjunc = Fjunc*gnab*(vOld-Enajunc);	
	iNabsl = Fsl*gnab*(vOld-Enasl);
	iNab = iNabjunc + iNabsl;
}

// External stimulus.
int GpbAtrial::externalStim(double stimval) {
    iTot = iTot + stimval;
    return 1;
}
/*
int GpbAtrial::stim()
{
  if(t>=stimt&&t<(stimt+dur)){
    if(flag==0){
      cout << "Stimulus to " << type << " at t = " << t << endl;
      num++;
      flag=1;
      if(num>=numstims)
         return 0;
    }
    iTot = iTot + val;
  }
  else if(flag==1){     //trailing edge of stimulus
        stimt=stimt+bcl;
        flag=0;
        apTime = 0.0;
  }

  apTime = apTime+dt;

  return 1;
};
*/
// Create map for easy retrieval of variable values.
void GpbAtrial::makemap()
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
  vars["iNaca"]=&iNaca;

  vars["gate.m"]=&gate.m;
  vars["gate.h"]=&gate.h;
  vars["gate.j"]=&gate.j;
  vars["gate.xkr"]=&gate.xkr;
  vars["gate.xks"]=&gate.xks;
  vars["gate.xf"]=&gate.xf;
  vars["gate.yf"]=&gate.yf;
  vars["gate.xkur"]=&gate.xkur;
  vars["gate.ykur"]=&gate.ykur;
  vars["gate.d"]=&gate.d;
  vars["gate.f"]=&gate.f;
  vars["gate.f_cabj"]=&gate.f_cabj;
  vars["gate.f_cabsl"]=&gate.f_cabsl;
  vars["gate.h "]=&gate.h;

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
}

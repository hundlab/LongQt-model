//######################################################
// This file contains all function definitions for
// Hund-Rudy dynamic model as published in Hund et al.
// J Mol Cell Cardiol 2008.
//
// Copyright (C) 2011 Thomas J. Hund.
//######################################################

#include "tnnp04.h"

//######################################################
// Constructor
//######################################################
TNNP04Control::TNNP04Control()
{
    this->Initialize();
}

TNNP04Control::TNNP04Control(const TNNP04Control& toCopy) : Cell(toCopy) {
    this->Initialize();
    CellKernel::copyVarPar(toCopy);
}

TNNP04Control::~TNNP04Control()
{
}

void TNNP04Control::Initialize() {
	RGAS = 8314.472;
        TEMP = 310.0;
        FDAY=96485.3415;

	type = "Human Ventricular (Ten Tusscher 2004)";
        dVdt=dVdtmax=-7.293176907E-7;
        t=0.0;
        dt=dtmin=0.005;
	dtmed=0.01;
        dtmax=0.1;
	dvcut = 1.0;
        vOld = vNew =  -86.2;
        tRel=10000.0;
        sponRelflag = 0;

        apTime = 0.0;

	ACap = 1.85E-4;// cm2
    	Vc = 1.64E-5; //uL
    	Vsr = 1.094E-5; //uL

    	isoFlag = 0;
//###### Concentrations #########
    	naI = 11.6; //uM
    	naO = 140.0; //uM
    	kI = 138.3; //uM
    	kO = 5.4; //uM
    	caI = 0.00008; //uM
    	caO = 2.0; //uM
    	caSr = 0.56; //uM
    
    	caSrbufc=10.0*(caSr/(caSr+0.3));
	caIbufc=0.15*(caI/(caI+0.001));
//##### Gating Variables #####
	Gate.g = 1.0;
	Gate.d = 0.0;
	Gate.f = 1.0;
	Gate.fca = 1.0;
	Gate.r = 0.0;
	Gate.s = 1.0;
	Gate.xs = 0.0;
	Gate.xr1 = 0.0;
	Gate.xr2 = 1.0;
	Gate.m = 0.0;
	Gate.h = 1.0;
	Gate.j = 1.0;

	iNa = iNab = iNak = iNaca = 0.0;
	iCal = iCab = iPca = 0.0;
	iK1 = iTo = iKr = iKs = ipK = 0.0;

    makemap();
}

TNNP04Control* TNNP04Control::clone() {
    return new TNNP04Control(*this);
}

void TNNP04Control::updateIcal(){
	double d_inf, tau_d, gamma_d, alpha_d, beta_d;
	double f_inf, tau_f;
	double fca_inf, tau_fca, gamma_fca, alpha_fca, beta_fca;
    double fcaold;//dfca,
	double gcal = 0.000175;
	double condfact = 1.0;

	d_inf = 1.0/(1.0+exp((-5.0-vOld)/7.5));
	alpha_d = 1.4/(1.0+exp((-35-vOld)/13.0))+0.25;
	beta_d = 1.4/(1.0+exp((vOld+5)/5.0));
	gamma_d = 1.0/(1.0+exp((50-vOld)/20.0));
	tau_d = alpha_d*beta_d+gamma_d;
	
	Gate.d = d_inf-(d_inf-Gate.d)*exp(-dt/tau_d);
	
	f_inf = 1.0/(1.0+exp((vOld+20)/7.0));
	tau_f = 1125*exp(-1.0*(vOld+27)*(vOld+27)/300.0)+165.0/(1.0+exp((25.0-vOld)/10.0))+80.0;
	
	Gate.f = f_inf-(f_inf-Gate.f)*exp(-dt/tau_f);
	
	alpha_fca = 1.0/(1.0+pow((caI/0.000325),8));
	beta_fca = 0.1/(1.0+exp((caI-0.0005)/0.0001));
	gamma_fca = 0.2/(1.0+exp((caI-0.00075)/0.0008));
	fca_inf = (alpha_fca+beta_fca+gamma_fca+0.23)/1.46;
	
	tau_fca = 2; //ms
	fcaold=Gate.fca;
	Gate.fca = fca_inf-(fca_inf-Gate.fca)*exp(-dt/tau_fca);
	if((Gate.fca>fcaold)&&(vOld>-37.0))
		Gate.fca=fcaold;
	
	if(isoFlag==1)
	  condfact = 2.429;

	iCal = gcal*condfact*Gate.d*Gate.f*Gate.fca*4*(vOld*FDAY*FDAY/(RGAS*TEMP))*((caI*exp(2*vOld*FDAY/(RGAS*TEMP))-0.341*caO)/(exp(2*vOld*FDAY/(RGAS*TEMP))-1.0));

};

void TNNP04Control::updateIcab() {
	double gcab = 0.000592;
	double Eca = RGAS*TEMP/(2*FDAY)*log(caO/caI);
        
	iCab = gcab*(vOld-Eca);	
};

void TNNP04Control::updateIpca() {
	double kpca = 0.0005;
	double gpca = 0.825;

	iPca = gpca*caI/(kpca+caI);
};

void TNNP04Control::updateIna() {
	double m_inf, alpha_m, beta_m, tau_m;
	double h_inf, alpha_h, beta_h, tau_h;
	double j_inf, alpha_j, beta_j, tau_j;
	double ENa;
	double gna = 14.838;

	ENa = RGAS*TEMP/FDAY*log(naO/naI);
	m_inf = 1/(1+exp((-56.86-vOld)/9.03)*(1+exp((-56.86-vOld)/9.03)));
	alpha_m = 1/(1+exp((-60-vOld)/5));
	beta_m = (0.1/(1+exp((vOld+35)/5)))+(0.1/(1+exp((vOld-50)/200)));
	tau_m = alpha_m*beta_m;

	h_inf = 1/(1+exp((vOld+71.55)/7.43))/(1+exp((vOld+71.55)/7.43));

	if(vOld>=-40){
		alpha_h = 0;
	}
	else {
		alpha_h = 0.057*exp((-vOld-80)/6.8);
	}

	if(vOld>=-40){
		beta_h = 0.77/(0.13*(1+exp((-vOld-10.66)/11.1)));
	}
	else {
		beta_h = 2.7*exp(0.079*vOld)+(3.1E5)*exp(0.3485*vOld);
	}

	tau_h = 1/(alpha_h+beta_h);

	j_inf = 1/(1+exp((vOld+71.55)/7.43))/(1+exp((vOld+71.55)/7.43));

	if(vOld>=-40){
		alpha_j = 0;
	}
	else {
		alpha_j = ((-2.5428E4*exp(0.2444*vOld)-6.948E-6*exp(-0.04391*vOld))*(vOld+37.78))/(1+exp(0.311*(vOld+79.23)));
}

	if(vOld>=-40) {
		beta_j = 0.6*exp(0.057*vOld)/(1+exp(-0.1*(vOld+32)));
	}
	else {
		beta_j = 0.02424*exp(-0.01052*vOld)/(1+exp(-0.1378*(vOld+40.14)));
	}
	tau_j = 1/(alpha_j+beta_j);
	
	Gate.m = m_inf-(m_inf-Gate.m)*exp(-dt/tau_m);
	Gate.h = h_inf-(h_inf-Gate.h)*exp(-dt/tau_h);
	Gate.j = j_inf-(j_inf-Gate.j)*exp(-dt/tau_j);
	
	iNa = gna*Gate.m*Gate.m*Gate.m*Gate.h*Gate.j*(vOld-ENa);
};

void TNNP04Control::updateInab() {
	double ENa;
	double gnab = 0.00029;

	ENa = RGAS*TEMP/FDAY*log(naO/naI);
	iNab = gnab*(vOld-ENa);
}

void TNNP04Control::updateIto() {
	double r_inf, tau_r;//for epicardial and M Cells
	double s_inf, tau_s;//for epicardial and M Cells
	double gto = 0.294;  //endo = 0.073;
	double EK;

	r_inf = 1/(1+exp((20-vOld)/6));
	tau_r = (9.5*exp(-1*(vOld+40)*(vOld+40)/1800))+0.8;

	s_inf = 1/(1+exp((vOld+20)/5));
	tau_s = (85*exp(-1*(vOld+45)*(vOld+45)/320))+(5/(1+exp((vOld-20)/5)))+3;

	Gate.r = r_inf-(r_inf-Gate.r)*exp(-dt/tau_r);
	Gate.s = s_inf-(s_inf-Gate.s)*exp(-dt/tau_s);

	EK = RGAS*TEMP/FDAY*log(kO/kI);
	iTo = gto*Gate.r*Gate.s*(vOld-EK);

}

void TNNP04Control::updateIks() {
	double xs_inf, alpha_xs, beta_xs, tau_xs;
	double condfact = 1.0;
	double pKna = 0.03;
	double EKs;
	double gks = 0.245; //M = 0.062;

	xs_inf = 1/(1+exp((-5-vOld)/14));
	alpha_xs = 1100/(sqrt(1+exp((-10-vOld)/6)));
	beta_xs = 1/(1+exp((vOld-60)/20));
	tau_xs = alpha_xs*beta_xs;

	Gate.xs = xs_inf-(xs_inf-Gate.xs)*exp(-dt/tau_xs);

	if(isoFlag==1)
	  condfact = 1.8;

	EKs = RGAS*TEMP/FDAY*log((kO+pKna*naO)/(kI+pKna*naI));
	iKs = gks*condfact*Gate.xs*Gate.xs*(vOld-EKs);
}

void TNNP04Control::updateIkr() {
	double xr1_inf, alpha_xr1, beta_xr1, tau_xr1;
	double xr2_inf, alpha_xr2, beta_xr2, tau_xr2;
	double EK;
	double gkr = 0.096;

	xr1_inf = 1/(1+exp((-26-vOld)/7));
	alpha_xr1 = 450/(1+exp((-45-vOld)/10));
	beta_xr1 = 6/(1+exp((vOld+30)/11.5));
	tau_xr1 = alpha_xr1*beta_xr1;

	xr2_inf = 1/(1+exp((vOld+88)/24));
	alpha_xr2 = 3/(1+exp((-60-vOld)/20));
	beta_xr2 = 1.12/(1+exp((vOld-60)/20));
	tau_xr2 = alpha_xr2*beta_xr2;

	Gate.xr1 = xr1_inf-(xr1_inf-Gate.xr1)*exp(-dt/tau_xr1);
	Gate.xr2 = xr2_inf-(xr2_inf-Gate.xr2)*exp(-dt/tau_xr2);

	EK = RGAS*TEMP/FDAY*log(kO/kI);
	iKr = gkr*pow(kO/5.4,0.5)*Gate.xr1*Gate.xr2*(vOld-EK);

}

void TNNP04Control::updateIpk() {
	double EK;
	double gpk = 0.0146;

	EK = RGAS*TEMP/FDAY*log(kO/kI);
	ipK = gpk*(vOld-EK)/(1+exp((25-vOld)/5.98));
}

void TNNP04Control::updateIk1() {
	double alpha_K1, beta_K1, xK1_inf;
	double EK;
	double gk1 = 5.405;

	EK = RGAS*TEMP/FDAY*log(kO/kI);

	alpha_K1 = 0.1/(1+exp(0.06*(vOld-EK-200)));
	beta_K1 = (3*exp(0.0002*(vOld-EK+100))+exp(0.1*(vOld-EK-10)))/(1+exp(-0.5*(vOld-EK)));
	xK1_inf = alpha_K1/(alpha_K1+beta_K1);
	
	iK1 = gk1*pow(kO/5.4,0.5)*xK1_inf*(vOld-EK);
	
}

void TNNP04Control::updateInaca() {
	double kNaca = 1000.0;
	double gamma = 0.35;
	double KmNai = 87.5;
	double KmCa = 1.38;
	double ksat = 0.1;
	double alpha = 2.5;

	iNaca = ((kNaca*exp(gamma*vOld/RGAS/TEMP)*naI*naI*naI*caO)-(kNaca*exp((gamma-1)*vOld*FDAY/RGAS/TEMP)*naO*naO*naO*caI*alpha))/((KmNai*KmNai*KmNai+naO*naO*naO)*(KmCa+caO)*(1+(ksat*exp((gamma-1)*vOld*FDAY/RGAS/TEMP))));

}

void TNNP04Control::updateInak() {
	double pnak = 1.362;
	double KmK = 1.0;
	double KmNa = 40.0;
	double condfact = 1.0;

	if(isoFlag==1)
	  condfact = 1.4;

	iNak = pnak*condfact*kO*naI/((kO+KmK)*(naI+KmNa)*(1+0.1245*exp(-0.1*vOld*FDAY/RGAS/TEMP)+0.0353*exp(-vOld*FDAY/RGAS/TEMP)));

}

void TNNP04Control::updatecaI() {
	double dcaI;
	double b1,c1;
	double cmdnbar = .15;
	double kmcmdn = 0.001;
	
        caIbufc=cmdnbar*(caI/(caI+kmcmdn));
	dcaI = dt*(-(iCat)*ACap/(2*Vc*FDAY)+iLeak-iUp+iRel);
        b1=cmdnbar-caIbufc-dcaI-caI+kmcmdn;
        c1=kmcmdn*(caIbufc+dcaI+caI);

        caI=(sqrt(b1*b1+4.0*c1)-b1)/2.0;
}

void TNNP04Control::updatecaSr() {
	double dcaSr;
	double b1,c1;
	double csqnbar = 10.0;
	double kmcsqn = 0.3;
	
        caSrbufc=csqnbar*(caSr/(caSr+kmcsqn));
	dcaSr = dt*(Vc/Vsr*(-iLeak+iUp-iRel));
        b1=csqnbar-caSrbufc-dcaSr-caSr+kmcsqn;
        c1=kmcsqn*(caSrbufc+dcaSr+caSr);

        caSr=(sqrt(b1*b1+4.0*c1)-b1)/2.0;
}

void TNNP04Control::updateSRcurrents(){

	double g_inf, tau_g;
	double gold;
	double a_rel = 0.016464;//16.464;
	double b_rel = 0.25;
	double c_rel = 0.008232;//8.232;
	double csqnth = 9.0;
	double Vleak = 0.00008;
	double Vmaxup = 0.000425;
	double Kup = 0.00025;
	double iupcondfact = 1.0;

	if(caI<0.00035){
		g_inf = 1/(1+(pow((caI/0.00035),6)));
	}
	else {
		g_inf = 1/(1+(pow((caI/0.00035),16)));

	}
	if(caSrbufc>csqnth){
	  if(sponRelflag==0){
	    sponRelflag = 1;
	    tRel = 0.0;	
	    cout << "Spontaneous release at t = " << t << endl;
	  }
	  Gate.g = 1;
	}
	tRel = tRel+dt;
	
 	if(tRel>100.0)
	   sponRelflag = 0;

	tau_g = 2; //ms
	if(sponRelflag==1)
	  tau_g = 5.0;

	gold = Gate.g;
	Gate.g = g_inf-(g_inf-Gate.g)*exp(-dt/tau_g); 
	if((Gate.g>gold)&&(vOld>-37))
		Gate.g=gold;

	if(isoFlag==1)
	  iupcondfact = 1.2;

	iLeak = Vleak*iupcondfact*(caSr-caI);
	iUp = Vmaxup/(1+(Kup*Kup/(caI*caI)));
	iRel = (((a_rel*caSr*caSr)/(b_rel*b_rel+caSr*caSr))+c_rel)*Gate.d*Gate.g;
	if(sponRelflag==1)
	   iRel = (((a_rel*caSr*caSr)/(b_rel*b_rel+caSr*caSr))+c_rel)*Gate.g;
}

void TNNP04Control::updatekI(){
	double dkI;

	dkI = dt*((-iKt*ACap/(Vc*FDAY)));

	kI = kI + dkI;

}

void TNNP04Control::updatenaI() {
	double dnaI;

	dnaI = dt*(-(iNat)*ACap/(Vc*FDAY));

	naI = naI + dnaI;
}

void TNNP04Control::updateCurr(){
	updateIcal();
	updateIcab();
	updateIpca();
	updateIto();
	updateIks();
	updateIkr();
	updateIk1();
	updateIpk();
	updateInaca();
	updateInak();
	updateInab();
	updateIna();	

	iNat=iNa+iNab+3*iNak+3*iNaca;
	iCat=iCal+iCab+iPca-2*iNaca;
	iKt = iK1+iTo+iKr+iKs+ipK-2*iNak;
	iTot=iNat+iCat+iKt;
}
void TNNP04Control::updateConc(){
	updateSRcurrents();
	updatecaI(); 
	updatecaSr(); 
	updatekI();
	updatenaI();
}



int TNNP04Control::externalStim(double stimval)
{
    iKt = iKt + stimval;
    iTot = iTot + stimval;
    return 1;
};

void TNNP04Control::makemap()
{
  vars["vOld"]=&vOld;
  vars["t"]=&t;
  vars["dVdt"]=&dVdt;
  vars["caI"]=&caI;
  vars["caSr"]=&caSr;
  vars["naI"]=&naI;
  vars["kI"]=&kI;
  vars["iCal"]=&iCal;
  vars["iCab"]=&iCab;
  vars["iPca"]=&iPca;
  vars["iTo"]=&iTo;
  vars["iK1"]=&iK1;
  vars["iKs"]=&iKs;
  vars["iKr"]=&iKr;
  vars["ipK"]=&ipK;
  vars["iNak"]=&iNak;
  vars["iNaca"]=&iNaca;
  vars["iNa"]=&iNa;
  vars["iNab"]=&iNab;
  vars["iUp"]=&iUp;
  vars["iLeak"]=&iLeak;
  vars["Gate.g"]=&Gate.g;
  vars["Gate.d"]=&Gate.d;
  vars["Gate.f"]=&Gate.f;
  vars["Gate.fca"]=&Gate.fca;
  vars["Gate.r"]=&Gate.r;
  vars["Gate.s"]=&Gate.s;
  vars["Gate.xs"]=&Gate.xs;
  vars["Gate.xr1"]=&Gate.xr1;
  vars["Gate.xr2"]=&Gate.xr2;
  vars["Gate.m"]=&Gate.m;
  vars["Gate.h"]=&Gate.h;
  vars["Gate.j"]=&Gate.j;
};

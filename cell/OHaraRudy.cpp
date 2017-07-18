#include <math.h>
using namespace std;
#include "OHaraRudy.h"

OHaraRudy::OHaraRudy() {
	this->Initialize();
};
OHaraRudy::~OHaraRudy() {};

OHaraRudy::OHaraRudy(OHaraRudy& toCopy) : Cell(toCopy) {
    this->Initialize(); 
    CellKernel::copyVarPar(toCopy);
}

void OHaraRudy::Initialize() {
	vOld=-87.5;
	this->makemap();
}

OHaraRudy* OHaraRudy::clone() {
    return new OHaraRudy(*this);
}
// External stimulus.
// default stimval -80
int OHaraRudy::externalStim(double stimval)
{
	iKt = iKt + stimval;
	iTot = iTot + stimval;

    return 1;
}

const char *OHaraRudy::type() const
{
    return "Human Ventricular (O'Hara-Rudy 2011)";
};
/*
   void OHaraRudy::stimulus()
   {
   if ((t>(start+n*CL) && t<(start+duration+n*CL-dt)))
   {
   if (Ist==0)
   {
   vrest=v;
   } 
   Ist=amp;
   }    
   else if (t>(start+duration+n*CL-dt))
   {   
   Ist=0.0;
   n=n+1;
   }
   }*/

/*t=t+dt; //fixed time step
  revpots();
  RGC();
  stimulus();
  vOld=v;
  voltage();
  dVdt_APD();
  FBC();*/

void OHaraRudy::revpots()
{
	ena=(RGAS*TEMP/FDAY)*log(naO/naI);
	ek=(RGAS*TEMP/FDAY)*log(kO/kI);
	eks=(RGAS*TEMP/FDAY)*log((kO+0.01833*naO)/(kI+0.01833*naI));
}

//current
void OHaraRudy::updateCaMKa() {
	CaMKb=CaMKo*(1.0-CaMKt)/(1.0+KmCaM/cass);
	CaMKa=CaMKb+CaMKt;
	CaMKt+=dt*(aCaMK*CaMKb*(CaMKb+CaMKt)-bCaMK*CaMKt);
}

void OHaraRudy::updateINa() {
	double mss=1.0/(1.0+exp((-(vOld+39.57))/9.871));
	double tm=1.0/(6.765*exp((vOld+11.64)/34.77)+8.552*exp(-(vOld+77.42)/5.955));
	m=mss-(mss-m)*exp(-dt/tm);
	double hss=1.0/(1+exp((vOld+82.90)/6.086));
	double thf=1.0/(1.432e-5*exp(-(vOld+1.196)/6.285)+6.149*exp((vOld+0.5096)/20.27));
	double ths=1.0/(0.009794*exp(-(vOld+17.95)/28.05)+0.3343*exp((vOld+5.730)/56.66));
	double Ahf=0.99;
	double Ahs=1.0-Ahf;
	hf=hss-(hss-hf)*exp(-dt/thf);
	hs=hss-(hss-hs)*exp(-dt/ths);
	double h=Ahf*hf+Ahs*hs;
	double jss=hss;
	double tj=2.038+1.0/(0.02136*exp(-(vOld+100.6)/8.281)+0.3052*exp((vOld+0.9941)/38.45));
	j=jss-(jss-j)*exp(-dt/tj);
	double hssp=1.0/(1+exp((vOld+89.1)/6.086));
	double thsp=3.0*ths;
	hsp=hssp-(hssp-hsp)*exp(-dt/thsp);
	double hp=Ahf*hf+Ahs*hsp;
	double tjp=1.46*tj;
	jp=jss-(jss-jp)*exp(-dt/tjp);
	double GNa=75;
	double fINap=(1.0/(1.0+KmCaMK/CaMKa));
	INa=GNa*(vOld-ena)*m*m*m*((1.0-fINap)*h*j+fINap*hp*jp);

	double mLss=1.0/(1.0+exp((-(vOld+42.85))/5.264));
	double tmL=tm;
	mL=mLss-(mLss-mL)*exp(-dt/tmL);
	double hLss=1.0/(1.0+exp((vOld+87.61)/7.488));
	double thL=200.0;
	hL=hLss-(hLss-hL)*exp(-dt/thL);
	double hLssp=1.0/(1.0+exp((vOld+93.81)/7.488));
	double thLp=3.0*thL;
	hLp=hLssp-(hLssp-hLp)*exp(-dt/thLp);
	double GNaL=0.0075;
	if (celltype==epi)
	{
		GNaL*=0.6;
	}
	double fINaLp=(1.0/(1.0+KmCaMK/CaMKa));
	INaL=GNaL*(vOld-ena)*mL*((1.0-fINaLp)*hL+fINaLp*hLp);

}

void OHaraRudy::updateIto() {
	double ass=1.0/(1.0+exp((-(vOld-14.34))/14.82));
	double ta=1.0515/(1.0/(1.2089*(1.0+exp(-(vOld-18.4099)/29.3814)))+3.5/(1.0+exp((vOld+100.0)/29.3814)));
	a=ass-(ass-a)*exp(-dt/ta);
	double iss=1.0/(1.0+exp((vOld+43.94)/5.711));
	double delta_epi;
	if (celltype==epi)
	{
		delta_epi=1.0-(0.95/(1.0+exp((vOld+70.0)/5.0)));
	}
	else
	{
		delta_epi=1.0;
	}
	double tiF=4.562+1/(0.3933*exp((-(vOld+100.0))/100.0)+0.08004*exp((vOld+50.0)/16.59));
	double tiS=23.62+1/(0.001416*exp((-(vOld+96.52))/59.05)+1.780e-8*exp((vOld+114.1)/8.079));
	tiF*=delta_epi;
	tiS*=delta_epi;
	double AiF=1.0/(1.0+exp((vOld-213.6)/151.2));
	double AiS=1.0-AiF;
	iF=iss-(iss-iF)*exp(-dt/tiF);
	iS=iss-(iss-iS)*exp(-dt/tiS);
	double i=AiF*iF+AiS*iS;
	double assp=1.0/(1.0+exp((-(vOld-24.34))/14.82));
	ap=assp-(assp-ap)*exp(-dt/ta);
	double dti_develop=1.354+1.0e-4/(exp((vOld-167.4)/15.89)+exp(-(vOld-12.23)/0.2154));
	double dti_recover=1.0-0.5/(1.0+exp((vOld+70.0)/20.0));
	double tiFp=dti_develop*dti_recover*tiF;
	double tiSp=dti_develop*dti_recover*tiS;
	iFp=iss-(iss-iFp)*exp(-dt/tiFp);
	iSp=iss-(iss-iSp)*exp(-dt/tiSp);
	double ip=AiF*iFp+AiS*iSp;
	double Gto=0.02;
	if (celltype==epi)
	{
		Gto*=4.0;
	}
	if (celltype==M)
	{
		Gto*=4.0;
	}
	double fItop=(1.0/(1.0+KmCaMK/CaMKa));
	Ito=Gto*(vOld-ek)*((1.0-fItop)*a*i+fItop*ap*ip);
}

void OHaraRudy::updateICa() {
	double vffrt=vOld*FDAY*FDAY/(RGAS*TEMP);
	double vfrt=vOld*FDAY/(RGAS*TEMP);

	double dss=1.0/(1.0+exp((-(vOld+3.940))/4.230));
	double td=0.6+1.0/(exp(-0.05*(vOld+6.0))+exp(0.09*(vOld+14.0)));
	d=dss-(dss-d)*exp(-dt/td);
	double fss=1.0/(1.0+exp((vOld+19.58)/3.696));
	double tff=7.0+1.0/(0.0045*exp(-(vOld+20.0)/10.0)+0.0045*exp((vOld+20.0)/10.0));
	double tfs=1000.0+1.0/(0.000035*exp(-(vOld+5.0)/4.0)+0.000035*exp((vOld+5.0)/6.0));
	double Aff=0.6;
	double Afs=1.0-Aff;
	ff=fss-(fss-ff)*exp(-dt/tff);
	fs=fss-(fss-fs)*exp(-dt/tfs);
	double f=Aff*ff+Afs*fs;
	double fcass=fss;
	double tfcaf=7.0+1.0/(0.04*exp(-(vOld-4.0)/7.0)+0.04*exp((vOld-4.0)/7.0));
	double tfcas=100.0+1.0/(0.00012*exp(-vOld/3.0)+0.00012*exp(vOld/7.0));
	double Afcaf=0.3+0.6/(1.0+exp((vOld-10.0)/10.0));
	double Afcas=1.0-Afcaf;
	fcaf=fcass-(fcass-fcaf)*exp(-dt/tfcaf);
	fcas=fcass-(fcass-fcas)*exp(-dt/tfcas);
	double fca=Afcaf*fcaf+Afcas*fcas;
	double tjca=75.0;
	jca=fcass-(fcass-jca)*exp(-dt/tjca);
	double tffp=2.5*tff;
	ffp=fss-(fss-ffp)*exp(-dt/tffp);
	double fp=Aff*ffp+Afs*fs;
	double tfcafp=2.5*tfcaf;
	fcafp=fcass-(fcass-fcafp)*exp(-dt/tfcafp);
	double fcap=Afcaf*fcafp+Afcas*fcas;
	double Kmn=0.002;
	double k2n=1000.0;
	double km2n=jca*1.0;
	double anca=1.0/(k2n/km2n+pow(1.0+Kmn/cass,4.0));
	nca=anca*k2n/km2n-(anca*k2n/km2n-nca)*exp(-km2n*dt);
	double PhiCaL=4.0*vffrt*(cass*exp(2.0*vfrt)-0.341*cao)/(exp(2.0*vfrt)-1.0);
	double PhiCaNa=1.0*vffrt*(0.75*nass*exp(1.0*vfrt)-0.75*naO)/(exp(1.0*vfrt)-1.0);
	double PhiCaK=1.0*vffrt*(0.75*kss*exp(1.0*vfrt)-0.75*kO)/(exp(1.0*vfrt)-1.0);
	double PCa=0.0001;
	if (celltype==epi)
	{
		PCa*=1.2;
	}
	if (celltype==M)
	{
		PCa*=2.5;
	}
	double PCap=1.1*PCa;
	double PCaNa=0.00125*PCa;
	double PCaK=3.574e-4*PCa;
	double PCaNap=0.00125*PCap;
	double PCaKp=3.574e-4*PCap;
	double fICaLp=(1.0/(1.0+KmCaMK/CaMKa));
	ICaL=(1.0-fICaLp)*PCa*PhiCaL*d*(f*(1.0-nca)+jca*fca*nca)+fICaLp*PCap*PhiCaL*d*(fp*(1.0-nca)+jca*fcap*nca);
	ICaNa=(1.0-fICaLp)*PCaNa*PhiCaNa*d*(f*(1.0-nca)+jca*fca*nca)+fICaLp*PCaNap*PhiCaNa*d*(fp*(1.0-nca)+jca*fcap*nca);
	ICaK=(1.0-fICaLp)*PCaK*PhiCaK*d*(f*(1.0-nca)+jca*fca*nca)+fICaLp*PCaKp*PhiCaK*d*(fp*(1.0-nca)+jca*fcap*nca);

}

void OHaraRudy::updateIkr() {
	double xrss=1.0/(1.0+exp((-(vOld+8.337))/6.789));
	double txrf=12.98+1.0/(0.3652*exp((vOld-31.66)/3.869)+4.123e-5*exp((-(vOld-47.78))/20.38));
	double txrs=1.865+1.0/(0.06629*exp((vOld-34.70)/7.355)+1.128e-5*exp((-(vOld-29.74))/25.94));
	double Axrf=1.0/(1.0+exp((vOld+54.81)/38.21));
	double Axrs=1.0-Axrf;
	xrf=xrss-(xrss-xrf)*exp(-dt/txrf);
	xrs=xrss-(xrss-xrs)*exp(-dt/txrs);
	double xr=Axrf*xrf+Axrs*xrs;
	double rkr=1.0/(1.0+exp((vOld+55.0)/75.0))*1.0/(1.0+exp((vOld-10.0)/30.0));
	double GKr=0.046;
	if (celltype==epi)
	{
		GKr*=1.3;
	}
	if (celltype==M)
	{
		GKr*=0.8;
	}
	IKr=GKr*sqrt(kO/5.4)*xr*rkr*(vOld-ek);
}

void OHaraRudy::updateIKs() {
	double xs1ss=1.0/(1.0+exp((-(vOld+11.60))/8.932));
	double txs1=817.3+1.0/(2.326e-4*exp((vOld+48.28)/17.80)+0.001292*exp((-(vOld+210.0))/230.0));
	xs1=xs1ss-(xs1ss-xs1)*exp(-dt/txs1);
	double xs2ss=xs1ss;
	double txs2=1.0/(0.01*exp((vOld-50.0)/20.0)+0.0193*exp((-(vOld+66.54))/31.0));
	xs2=xs2ss-(xs2ss-xs2)*exp(-dt/txs2);
	double KsCa=1.0+0.6/(1.0+pow(3.8e-5/cai,1.4));
	double GKs=0.0034;
	if (celltype==epi)
	{
		GKs*=1.4;
	}
	IKs=GKs*KsCa*xs1*xs2*(vOld-eks);
}

void OHaraRudy::updateIK1() {
	double xk1ss=1.0/(1.0+exp(-(vOld+2.5538*kO+144.59)/(1.5692*kO+3.8115)));
	double txk1=122.2/(exp((-(vOld+127.2))/20.36)+exp((vOld+236.8)/69.33));
	xk1=xk1ss-(xk1ss-xk1)*exp(-dt/txk1);
	double rk1=1.0/(1.0+exp((vOld+105.8-2.6*kO)/9.493));
	double GK1=0.1908;
	if (celltype==epi)
	{
		GK1*=1.2;
	}
	if (celltype==M)
	{
		GK1*=1.3;
	}
	IK1=GK1*sqrt(kO)*rk1*xk1*(vOld-ek);
}

void OHaraRudy::updateINaCa() {
	double zca=2.0;
	double kna1=15.0;
	double kna2=5.0;
	double kna3=88.12;
	double kasymm=12.5;
	double wna=6.0e4;
	double wca=6.0e4;
	double wnaca=5.0e3;
	double kcaon=1.5e6;
	double kcaoff=5.0e3;
	double qna=0.5224;
	double qca=0.1670;
	double hca=exp((qca*vOld*FDAY)/(RGAS*TEMP));
	double hna=exp((qna*vOld*FDAY)/(RGAS*TEMP));
	double h1=1+naI/kna3*(1+hna);
	double h2=(naI*hna)/(kna3*h1);
	double h3=1.0/h1;
	double h4=1.0+naI/kna1*(1+naI/kna2);
	double h5=naI*naI/(h4*kna1*kna2);
	double h6=1.0/h4;
	double h7=1.0+naO/kna3*(1.0+1.0/hna);
	double h8=naO/(kna3*hna*h7);
	double h9=1.0/h7;
	double h10=kasymm+1.0+naO/kna1*(1.0+naO/kna2);
	double h11=naO*naO/(h10*kna1*kna2);
	double h12=1.0/h10;
	double k1=h12*cao*kcaon;
	double k2=kcaoff;
	double k3p=h9*wca;
	double k3pp=h8*wnaca;
	double k3=k3p+k3pp;
	double k4p=h3*wca/hca;
	double k4pp=h2*wnaca;
	double k4=k4p+k4pp;
	double k5=kcaoff;
	double k6=h6*cai*kcaon;
	double k7=h5*h2*wna;
	double k8=h8*h11*wna;
	double x1=k2*k4*(k7+k6)+k5*k7*(k2+k3);
	double x2=k1*k7*(k4+k5)+k4*k6*(k1+k8);
	double x3=k1*k3*(k7+k6)+k8*k6*(k2+k3);
	double x4=k2*k8*(k4+k5)+k3*k5*(k1+k8);
	double E1=x1/(x1+x2+x3+x4);
	double E2=x2/(x1+x2+x3+x4);
	double E3=x3/(x1+x2+x3+x4);
	double E4=x4/(x1+x2+x3+x4);
	double KmCaAct=150.0e-6;
	double allo=1.0/(1.0+pow(KmCaAct/cai,2.0));
	double zna=1.0;
	double JncxNa=3.0*(E4*k7-E1*k8)+E3*k4pp-E2*k3pp;
	double JncxCa=E2*k2-E1*k1;
	double Gncx=0.0008;
	if (celltype==epi)
	{
		Gncx*=1.1;
	}
	if (celltype==M)
	{
		Gncx*=1.4;
	}
	INaCa_i=0.8*Gncx*allo*(zna*JncxNa+zca*JncxCa);

	h1=1+nass/kna3*(1+hna);
	h2=(nass*hna)/(kna3*h1);
	h3=1.0/h1;
	h4=1.0+nass/kna1*(1+nass/kna2);
	h5=nass*nass/(h4*kna1*kna2);
	h6=1.0/h4;
	h7=1.0+naO/kna3*(1.0+1.0/hna);
	h8=naO/(kna3*hna*h7);
	h9=1.0/h7;
	h10=kasymm+1.0+naO/kna1*(1+naO/kna2);
	h11=naO*naO/(h10*kna1*kna2);
	h12=1.0/h10;
	k1=h12*cao*kcaon;
	k2=kcaoff;
	k3p=h9*wca;
	k3pp=h8*wnaca;
	k3=k3p+k3pp;
	k4p=h3*wca/hca;
	k4pp=h2*wnaca;
	k4=k4p+k4pp;
	k5=kcaoff;
	k6=h6*cass*kcaon;
	k7=h5*h2*wna;
	k8=h8*h11*wna;
	x1=k2*k4*(k7+k6)+k5*k7*(k2+k3);
	x2=k1*k7*(k4+k5)+k4*k6*(k1+k8);
	x3=k1*k3*(k7+k6)+k8*k6*(k2+k3);
	x4=k2*k8*(k4+k5)+k3*k5*(k1+k8);
	E1=x1/(x1+x2+x3+x4);
	E2=x2/(x1+x2+x3+x4);
	E3=x3/(x1+x2+x3+x4);
	E4=x4/(x1+x2+x3+x4);
	KmCaAct=150.0e-6;
	allo=1.0/(1.0+pow(KmCaAct/cass,2.0));
	JncxNa=3.0*(E4*k7-E1*k8)+E3*k4pp-E2*k3pp;
	JncxCa=E2*k2-E1*k1;
	INaCa_ss=0.2*Gncx*allo*(zna*JncxNa+zca*JncxCa);

	INaCa=INaCa_i+INaCa_ss;
}
void OHaraRudy::updateINaK() {
	double zna=1.0;
	double k1p=949.5;
	double k1m=182.4;
	double k2p=687.2;
	double k2m=39.4;
	double k3p=1899.0;
	double k3m=79300.0;
	double k4p=639.0;
	double k4m=40.0;
	double Knai0=9.073;
	double Knao0=27.78;
	double delta=-0.1550;
	double Knai=Knai0*exp((delta*vOld*FDAY)/(3.0*RGAS*TEMP));
	double Knao=Knao0*exp(((1.0-delta)*vOld*FDAY)/(3.0*RGAS*TEMP));
	double Kki=0.5;
	double Kko=0.3582;
	double MgADP=0.05;
	double MgATP=9.8;
	double Kmgatp=1.698e-7;
	double H=1.0e-7;
	double eP=4.2;
	double Khp=1.698e-7;
	double Knap=224.0;
	double Kxkur=292.0;
	double P=eP/(1.0+H/Khp+naI/Knap+kI/Kxkur);
	double a1=(k1p*pow(naI/Knai,3.0))/(pow(1.0+naI/Knai,3.0)+pow(1.0+kI/Kki,2.0)-1.0);
	double b1=k1m*MgADP;
	double a2=k2p;
	double b2=(k2m*pow(naO/Knao,3.0))/(pow(1.0+naO/Knao,3.0)+pow(1.0+kO/Kko,2.0)-1.0);
	double a3=(k3p*pow(kO/Kko,2.0))/(pow(1.0+naO/Knao,3.0)+pow(1.0+kO/Kko,2.0)-1.0);
	double b3=(k3m*P*H)/(1.0+MgATP/Kmgatp);
	double a4=(k4p*MgATP/Kmgatp)/(1.0+MgATP/Kmgatp);
	double b4=(k4m*pow(kI/Kki,2.0))/(pow(1.0+naI/Knai,3.0)+pow(1.0+kI/Kki,2.0)-1.0);
	double x1=a4*a1*a2+b2*b4*b3+a2*b4*b3+b3*a1*a2;
	double x2=b2*b1*b4+a1*a2*a3+a3*b1*b4+a2*a3*b4;
	double x3=a2*a3*a4+b3*b2*b1+b2*b1*a4+a3*a4*b1;
	double x4=b4*b3*b2+a3*a4*a1+b2*a4*a1+b3*b2*a1;
	double E1=x1/(x1+x2+x3+x4);
	double E2=x2/(x1+x2+x3+x4);
	double E3=x3/(x1+x2+x3+x4);
	double E4=x4/(x1+x2+x3+x4);
	double zk=1.0;
	double JnakNa=3.0*(E1*a3-E2*b3);
	double JnakK=2.0*(E4*b1-E3*a1);
	double Pnak=30;
	if (celltype==epi)
	{
		Pnak*=0.9;
	}
	if (celltype==M)
	{
		Pnak*=0.7;
	}
	INaK=Pnak*(zna*JnakNa+zk*JnakK);
}

void OHaraRudy::updateIpCa()
{
	double vffrt=vOld*FDAY*FDAY/(RGAS*TEMP);
	double vfrt=vOld*FDAY/(RGAS*TEMP);

	double xkb=1.0/(1.0+exp(-(vOld-14.48)/18.34));
	double GKb=0.003;
	if (celltype==epi)
	{
		GKb*=0.6;
	}
	IKb=GKb*xkb*(vOld-ek);

	double PNab=3.75e-10;
	INab=PNab*vffrt*(naI*exp(vfrt)-naO)/(exp(vfrt)-1.0);

	double PCab=2.5e-8;
	ICab=PCab*4.0*vffrt*(cai*exp(2.0*vfrt)-0.341*cao)/(exp(2.0*vfrt)-1.0);

	double GpCa=0.0005;
	IpCa=GpCa*cai/(0.0005+cai);
}
//end curr

//update conc
void OHaraRudy::updateJrel() {
	double bt=4.75;
	double a_rel=0.5*bt;
	double Jrel_inf=a_rel*(-ICaL)/(1.0+pow(1.5/cajsr,8.0));
	if (celltype==M)
	{
		Jrel_inf*=1.7;
	}
	double tau_rel=bt/(1.0+0.0123/cajsr);
	if (tau_rel<0.005)
	{
		tau_rel=0.005;
	}
	Jrelnp=Jrel_inf-(Jrel_inf-Jrelnp)*exp(-dt/tau_rel);
	double btp=1.25*bt;
	double a_relp=0.5*btp;
	double Jrel_infp=a_relp*(-ICaL)/(1.0+pow(1.5/cajsr,8.0));
	if (celltype==M)
	{
		Jrel_infp*=1.7;
	}
	double tau_relp=btp/(1.0+0.0123/cajsr);
	if (tau_relp<0.005)
	{
		tau_relp=0.005;
	}
	Jrelp=Jrel_infp-(Jrel_infp-Jrelp)*exp(-dt/tau_relp);
	double fJrelp=(1.0/(1.0+KmCaMK/CaMKa));
	Jrel=(1.0-fJrelp)*Jrelnp+fJrelp*Jrelp;
}

void OHaraRudy::updatenaI() {
	double Jupnp=0.004375*cai/(cai+0.00092);
	double Jupp=2.75*0.004375*cai/(cai+0.00092-0.00017);
	if (celltype==epi)
	{
		Jupnp*=1.3;
		Jupp*=1.3;
	}
	double fJupp=(1.0/(1.0+KmCaMK/CaMKa));
	Jleak=0.0039375*cansr/15.0;
	Jup=(1.0-fJupp)*Jupnp+fJupp*Jupp-Jleak;

	Jtr=(cansr-cajsr)/100.0;

	Jdiff=(cass-cai)/0.2;
	JdiffNa=(nass-naI)/2.0;
	naI+=dt*(-(iNat)*Acap/(FDAY*vmyo)+JdiffNa*vss/vmyo);
	nass+=dt*(-(ICaNa+3.0*INaCa_ss)*Acap/(FDAY*vss)-JdiffNa);
}

void OHaraRudy::updatekI() {
	JdiffK=(kss-kI)/2.0;
	kI+=dt*(-(iKt)*Acap/(FDAY*vmyo)+JdiffK*vss/vmyo);
	kss+=dt*(-(ICaK)*Acap/(FDAY*vss)-JdiffK);
}

void OHaraRudy::updatecajsr() {
	double Bcai;
	if (celltype==epi)
	{
		Bcai=1.0/(1.0+1.3*cmdnmax*kmcmdn/pow(kmcmdn+cai,2.0)+trpnmax*kmtrpn/pow(kmtrpn+cai,2.0));
	}
	else
	{
		Bcai=1.0/(1.0+cmdnmax*kmcmdn/pow(kmcmdn+cai,2.0)+trpnmax*kmtrpn/pow(kmtrpn+cai,2.0));
	}
	cai+=dt*(Bcai*(-(iCat)*Acap/(2.0*FDAY*vmyo)-Jup*vnsr/vmyo+Jdiff*vss/vmyo));

	double Bcass=1.0/(1.0+BSRmax*KmBSR/pow(KmBSR+cass,2.0)+BSLmax*KmBSL/pow(KmBSL+cass,2.0));
	cass+=dt*(Bcass*(-(ICaL-2.0*INaCa_ss)*Acap/(2.0*FDAY*vss)+Jrel*vjsr/vss-Jdiff));

	cansr+=dt*(Jup-Jtr*vjsr/vnsr);

	double Bcajsr=1.0/(1.0+csqnmax*kmcsqn/pow(kmcsqn+cajsr,2.0));
	cajsr+=dt*(Bcajsr*(Jtr-Jrel));
}
//end Conc

/*void OHaraRudy::voltage()
  {
  vNew=-dt*iTot;
  vOld += vNew;
//(INa+INaL+Ito+ICaL+ICaNa+ICaK+IKr+IKs+IK1+INaCa+INaK+INab+IKb+IpCa+ICab+Ist);
}*/

void OHaraRudy::updateCurr() {
	revpots();
	updateCaMKa();
	updateINa();
	updateIto();
	updateICa();
	updateIkr();
	updateIKs();
	updateIK1();
	updateINaCa();
	updateINaK();
	updateIpCa();
	//check my iNat,iCat&iKt equations
	iNat=INa+INaL+3.0*INaCa_i+3.0*INaK+INab;
	//	iCart;
	iCat=IpCa+ICab-2.0*INaCa_i;
	iKt=Ito+IKr+IKs+IK1+IKb-2.0*INaK;	
	iTotold=iTot;
	iTot=INa+INaL+Ito+ICaL+ICaNa+ICaK+IKr+IKs+IK1+INaCa+INaK+INab+IKb+IpCa+ICab;
}

void OHaraRudy::updateConc() {
	updateJrel();
	updatenaI();
	updatekI();
	updatecajsr();

}

/*void OHaraRudy::dVdt_APD()
  {
  vdot_old=vdot;
  vdot=(v-vOld)/dt;
  if (APD_flag==0 && v>-40 && vdot<vdot_old)
  {
  vdot_max=vdot_old;
  t_vdot_max=t-dt;
  APD_flag=1;
  }
  if	(APD_flag==1 && v<0.9*vrest)
  {
  APD=t-t_vdot_max;
  APD_flag=0;
  }
  }*/

// Create map for easy retrieval of variable values.
void OHaraRudy::makemap()
{
 
	vars["naI"]=&vOld;
	vars["iNa"]=&INa;
	vars["iNaL"]=&INaL;
	vars["iTo"]=&Ito;
	vars["iCaL"]=&ICaL;
	vars["iCana"]=&ICaNa;
	vars["iCaK"]=&ICaK;
	vars["iKr"]=&IKr;
	vars["iKs"]=&IKs;
	vars["iK1"]=&IK1;
	vars["iNaca_i"]=&INaCa_i;
	vars["iNaca_ss"]=&INaCa_ss;
	vars["iNaca"]=&INaCa;
	vars["iNaK"]=&INaK;
	vars["iKb"]=&IKb;
	vars["iNab"]=&INab;
	vars["iPca"]=&IpCa;
	vars["iCab"]=&ICab;
	vars["Jrel"]=&Jrel;
	vars["Jup"]=&Jup;
	vars["Jtr"]=&Jtr;
	vars["Jdiff"]=&Jdiff;
	vars["JdiffNa"]=&JdiffNa;
	vars["JdiffK"]=&JdiffK;
	vars["Jleak"]=&Jleak;
	vars["CaMKa"]=&CaMKa;
	vars["CaMKb"]=&CaMKb;
 
	vars["naI"]=&naI;
	vars["nass"]=&nass;
	vars["kI"]=&kI;
	vars["kss"]=&kss;
	vars["cai"]=&cai;
	vars["cass"]=&cass;
	vars["cansr"]=&cansr;
	vars["cajsr"]=&cajsr;
	vars["m"]=&m;
	vars["hf"]=&hf;
	vars["hs"]=&hs;
	vars["j"]=&j;
	vars["hsp"]=&hsp;
	vars["jp"]=&jp;
	vars["mL"]=&mL;
	vars["hL"]=&hL;
	vars["hLp"]=&hLp;
	vars["a"]=&a;
	vars["iF"]=&iF;
	vars["iS"]=&iS;
	vars["ap"]=&ap;
	vars["iFp"]=&iFp;
	vars["iSp"]=&iSp;
	vars["d"]=&d;
	vars["ff"]=&ff;
	vars["fs"]=&fs;
	vars["fcaf"]=&fcaf;
	vars["fcas"]=&fcas;
	vars["jca"]=&jca;
	vars["nca"]=&nca;
	vars["ffp"]=&ffp;
	vars["fcafp"]=&fcafp;
	vars["xrf"]=&xrf;
	vars["xrs"]=&xrs;
	vars["xs1"]=&xs1;
	vars["xs2"]=&xs2;
	vars["xk1"]=&xk1;
	vars["Jrelnp"]=&Jrelnp;
	vars["Jrelp"]=&Jrelp;
	vars["CaMKt"]=&CaMKt;

	pars["naO"]=&naO;
	pars["cao"]=&cao;
	pars["kO"]=&kO;

		//buffer paramaters
	pars["BSRmax"]=&BSRmax;
	pars["KmBSR"]=&KmBSR;
	pars["BSLmax"]=&BSLmax;
	pars["KmBSL"]=&KmBSL;
	pars["cmdnmax"]=&cmdnmax;
	pars["kmcmdn"]=&kmcmdn;
	pars["trpnmax"]=&trpnmax;
	pars["kmtrpn"]=&kmtrpn;
	pars["csqnmax"]=&csqnmax;
	pars["kmcsqn"]=&kmcsqn;

		//cell geometry
	pars["L"]=&L;
	pars["rad"]=&rad;
	pars["vcell"]=&vcell;
	pars["Ageo"]=&Ageo;
		//AGeo
	pars["Acap"]=&Acap;
	pars["vmyo"]=&vmyo;
	pars["vmito"]=&vmito;
	pars["vsr"]=&vsr;
	pars["vnsr"]=&vnsr;
	pars["vjsr"]=&vjsr;
	pars["vss"]=&vss;


		//CaMK paramaters
	pars["aCaMK"]=&aCaMK;
	pars["bCaMK"]=&bCaMK;
	pars["CaMKo"]=&CaMKo;
	pars["KmCaM"]=&KmCaM;
	pars["KmCaMK"]=&KmCaMK;

};


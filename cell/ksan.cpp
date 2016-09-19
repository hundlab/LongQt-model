//######################################################
// This file contains all function definitions for
// Hund-Rudy dynamic model as published in Hund et al.
// J Mol Cell Cardiol 2008.
//
// Copyright (C) 2011 Thomas J. Hund.
//######################################################

#include "ksan.h"

//######################################################
// Constructor for control canine epicardial
// ventricular model.
//######################################################
Ksan::Ksan()
{
    this->Initialize();
}

Ksan::Ksan(Ksan& toCopy) : Cell(toCopy) {
    this->Initialize(); 
    CellKernel::copyVarPar(toCopy);
}

Ksan::~Ksan()
{
}


void Ksan::Initialize() {
//##### Initialize variables ###################

type = "Canine Ventricular Myocyte (Hund-Rudy 2008)";
dtmin=dtmed=dtmax=0.005;

vOld = -64.5216286940;

Gate.dst = 0.6246780312;
Gate.fst = 0.4537033169;
Gate.dt = 0.0016256324;
Gate.ft = 0.4264459666;
Gate.ikr_act = 0.4043600437;
Gate.ikr_inact = 0.9250035423;
Gate.ikr_inact2 = 0.1875749806;
Gate.iks_act = 0.0127086259;
Gate.fl12 = 0.9968141226;
Gate.dl12 = 0.0000045583;
Gate.fl13 = 0.9809298233;
Gate.dl13 = 0.0002036671;
Gate.fca = 0.7649576191;
Gate.r = 0.0046263658;
Gate.m_ttxr = 0.4014088304;
Gate.h_ttxr = 0.2724817537;
Gate.j_ttxr = 0.0249208708;
Gate.m_ttxs = 0.1079085266;
Gate.h_ttxs = 0.4500098710;
Gate.j_ttxs = 0.0268486392;
Gate.y_1_2 = 0.0279984462;
Gate.q = 0.6107148187;
Gate.resting = 0.7720290515;
Gate.open = 0.0000000760;
Gate.inactivated = 0.0000000213;
Gate.resting_inactivated = 0.2162168926;

Ftc = 0.0063427103;
Ftmc = 0.1296677919;
Ftmm = 0.7688656371;
Fcms = 0.0242054739;
Fcmi = 0.0138533048;
Fcq = 0.1203184861;

caI = 0.0000319121;    	//mM
naI = 8.1179761505;
kI = 139.8854603066;
caJsr = 0.1187281829;
caNsr = 1.5768287365;
caSub = 0.0000560497;

naO = 140;
caO = 1.8;
kO = 5.4;
mgI=2.5;

Vcell = 3.0;		//pL
Vmyo = 1.34671883;	//pL
Vss = 0.03328117;	//pL
Vnsr = 0.0348;		//pL
Vjsr = 0.0036; 		//pL
TotCap=0.025;   	//nF
ACap=0.000025;		//cm2
Rcg = 1.72204;		//Used same as kurata

cellRadius=0.0003792956;    //cm
cellLength=0.00663767257;   //cm

RGAS = 8.314472;
TEMP = 310.5;
FDAY = 96.4845;

Istfactor=1;
Ibnafactor=1;
Ibcafactor=1;
Ibkfactor=1;
IK1factor=1;
ICaTfactor=1;
IKrfactor=1;
IKsfactor=1;
ICaL12factor=1;
ICaL13factor=1;
INafactorxr=1;
INafactorxs=1;
Iffactor=1;
Itofactor=1;
Isusfactor=1;
INaKfactor=1;
iNaCafactor=1;


 
this->makemap();
}
Ksan* Ksan::clone() {
    return new Ksan(*this);
}
/*Ist********************************************************************/
void Ksan::updateIst()
{
double qa, qi, tauqa,tauqi,alphaqa,betaqa,alphaqi,betaqi;
double gst=0.00006;
double eist=17.0;

qa = 1.0/(1.0 + exp(-(vOld+67.0)/5.0));
alphaqa = 1.0/(0.15*exp(-(vOld)/11.0)+0.2*exp(-(vOld)/700.0));
betaqa  =  1.0/(16.0*exp((vOld)/8.0)+15.0*exp((vOld)/50.0));
tauqa = 1.0/(alphaqa + betaqa);
alphaqi = 0.15*1.0/(3100.0*exp((vOld+10.0)/13.0)+700.3*exp((vOld+10.0)/70.0));
betaqi =  0.15*1.0/(95.7*exp(-(vOld+10.0)/10.0) + 50.0*exp(-(vOld+10.0)/700.0)) + 0.000229/(1+exp(-(vOld+10.0)/5.0));
qi = alphaqi/(alphaqi + betaqi);
tauqi = 1.0/(alphaqi + betaqi);
Gate.dst = Gate.dst + dt*((qa-Gate.dst)/tauqa);
Gate.fst = Gate.fst + dt*((qi-Gate.fst)/tauqi);
ist = Istfactor*gst*Gate.dst*Gate.fst*(vOld - eist);
}

/* Ib ************************************************************************/
void Ksan::updateIb()
{
  double gbna=0.0001215;
  double gbca=0.000015;
  double gbk=0.0000025;
  double eca = (RGAS*TEMP/(2*FDAY))*log(caO/caSub);
  double ek  = (RGAS*TEMP/FDAY)*log(kO/kI);
  double ena = (RGAS*TEMP/FDAY)*log(naO/naI);

  ibna = Ibnafactor*gbna*(vOld - ena);
  ibca = Ibcafactor*gbca*(vOld - eca);
  ibk  =  Ibkfactor*gbk*(vOld - ek);
 } 
/*IK1**********************************************************************/
void Ksan::updateIK1()
{
  double xk1inf;
  double gk1 =0.229*0.0039228*0.9;
  double ek  = (RGAS*TEMP/FDAY)*log(kO/kI);

  xk1inf = 1.0/(1.0 + exp(0.070727*(vOld - ek)));
  ik1 = IK1factor*gk1*xk1inf*(kO/(kO + 0.228880))*(vOld - ek);
}
/**ICaT Cav3.1**************************************************************/
void Ksan::updateICaT()
{
  double dt_inf, tau_dt, ft_inf, tau_ft;
  double gcat=0.75*0.01862; 
  double ecat = 45.0;

  tau_dt = 1.0/(1.068*exp((vOld + 26.3)/30.0)+1.068*exp(-(vOld+26.3)/30.0));
  dt_inf = 1.0/(1.0+exp(-(vOld + 26.0)/6.0));
  Gate.dt = Gate.dt + dt*((dt_inf - Gate.dt)/tau_dt);
  tau_ft = 1.0/(0.0153*exp(-(vOld+61.7)/83.3)+0.015*exp((vOld+61.7)/15.38));
  ft_inf = 1.0/(1.0+exp((vOld + 61.7)/5.6));
  Gate.ft = Gate.ft + dt*((ft_inf - Gate.ft)/tau_ft);
  icat = ICaTfactor*gcat*Gate.ft*Gate.dt*(vOld - ecat);          
}
/*Ikr********************************************************************/
void Ksan::updateIKr()
{
  double ikr_act_inf, tau_ikr_act;
  double ikr_inact_inf,tau_ikr_inact,tau_ikr_inact2;
  double gkr=0.8*0.002955;
  double ek  = (RGAS*TEMP/FDAY)*log(kO/kI);

  ikr_act_inf = 1.0/(1.0 + exp(-(vOld+21.173694)/9.757086));
  tau_ikr_act = 0.699821/(0.003596*exp((vOld)/15.339290) + 0.000177*exp(-(vOld)/25.868423));
  Gate.ikr_act = Gate.ikr_act + dt*(ikr_act_inf-Gate.ikr_act)/tau_ikr_act;     
  ikr_inact_inf = 1.0/(1.0 + exp((vOld+20.758474-4.0)/(19.0)));
  tau_ikr_inact = 0.2+0.9*1.0/(0.1*exp(vOld/54.645)+0.656*exp(vOld/106.157));
  Gate.ikr_inact = Gate.ikr_inact + dt*(ikr_inact_inf - Gate.ikr_inact)/tau_ikr_inact;
  ikr = IKrfactor*gkr*Gate.ikr_act*Gate.ikr_inact*(vOld - ek);
}
/**IKs********************************************************************/
void Ksan::updateIKs()
{
  double iks_act_inf, tau_iks_act;
  double gks=0.000299;
  double eks = ((RGAS*TEMP)/FDAY)*log((kO+0.12*naO)/(kI+0.12*naI));

  iks_act_inf = 1.0/(1.0 + exp(-(vOld-20.876040)/11.852723));
  tau_iks_act =  1000.0/(13.097938/(1.0 + exp(-(vOld-48.910584)/10.630272)) + exp(-(vOld)/35.316539));
  
  Gate.iks_act = Gate.iks_act + dt*(iks_act_inf - Gate.iks_act)/tau_iks_act;
  
  iks = IKsfactor*gks*Gate.iks_act*Gate.iks_act*(vOld - eks);
}
/*ICaL*******************************************************************/
void Ksan::updateICaL()
{
  double alpha_dl, beta_dl, tau_dl, alpha_fl, beta_fl, tau_fl;
  double dl12_inf, fl12_inf, dl13_inf, fl13_inf, fca_inf;
  double taufca;
  double gcal12 = 0.0010*4.0*1.5;
  double gcal13 = 0.0030*4.0*1.5;
  double kmfca = 0.00035;
  double alpha_fca=0.021;

  double ecal = 47.0;

  if(fabs(vOld)<=0.001){
    alpha_dl  = -28.39*(vOld+35.0)/(exp(-(vOld+35.0)/2.5)-1.0)+408.173;
} 
  else
  if(fabs(vOld+35.0)<=0.001){
    alpha_dl  = 70.975-84.9*vOld/(exp(-0.208*vOld)-1.0);
  }
  else
  if(fabs(vOld)>0.001&&fabs(vOld+35.0)>0.001){
    alpha_dl  = -28.39*(vOld+35.0)/(exp(-(vOld+35.0)/2.5)-1.0)-84.9*vOld/(exp(-0.208*vOld)-1.0);
  }

  if(fabs(vOld-5.0)<=0.001)
    beta_dl   = 28.575;
  else
  if(fabs(vOld-5.0)>0.001)
    beta_dl   = 11.43*(vOld-5.0)/(exp(0.4*(vOld-5.0))-1.0);

  tau_dl  = 2000.0/(alpha_dl +beta_dl);
  dl13_inf = 1.0/(1+exp(-(vOld+13.5)/6.0));
  fl13_inf = 1.0/(1+exp((vOld+35.0)/7.3));
  tau_fl = (7.4 + 45.77*exp(-0.5*(vOld+28.1)*(vOld+28.1)/(11*11)));
  Gate.dl13 = Gate.dl13 + dt*(dl13_inf - Gate.dl13)/tau_dl;
  Gate.fl13 = Gate.fl13 + dt*(fl13_inf - Gate.fl13)/tau_fl;
  dl12_inf = 1.0/(1+exp(-(vOld+3.0)/5.0));
  fl12_inf = 1.0/(1+exp((vOld+36.0)/4.6));
  Gate.dl12 = Gate.dl12 + dt*(dl12_inf - Gate.dl12)/tau_dl;
  Gate.fl12 = Gate.fl12 + dt*(fl12_inf - Gate.fl12)/tau_fl;
  fca_inf = kmfca/(kmfca+caSub);
  taufca = fca_inf/alpha_fca;
  Gate.fca = Gate.fca + dt*(fca_inf - Gate.fca)/taufca;
  ical12 =   ICaL12factor*gcal12*Gate.fl12*Gate.dl12*Gate.fca*(vOld-ecal);
  ical13 = ICaL13factor*gcal13*Gate.fl13*Gate.dl13*Gate.fca*(vOld-ecal);
}
/**INa**********************************************************************/
void Ksan::updateINa()
{
  double fna;
  double m3_inf_ttxr, h_inf_ttxr;
  double m3_inf_ttxs, h_inf_ttxs;
  double m_inf_ttxr,m_inf_ttxs, hs,hsr;
  double tau_m,tau_h,tau_j,tau_mr,tau_hr,tau_jr;
  double ena = (RGAS*TEMP/FDAY)*log(naO/naI);
  double gna_ttxs = 0.1*5.925e-05;
  double gna_ttxr = 0.1*5.925e-05;
  double enattxr = 41.5761;

  fna = (9.52e-02*exp(-6.3e-2*(vOld+34.4))/(1+1.66*exp(-0.225*(vOld+63.7))))+8.69e-2; 
  m3_inf_ttxr = 1.0/(1.0 + exp(-(vOld+45.213705)/7.219547));
  h_inf_ttxr = 1.0/(1.0 + exp(-(vOld+62.578120 )/(-6.084036)));
  m3_inf_ttxs = 1.0/(1.0 + exp(-(vOld+36.097331-5.0)/5.0));
  h_inf_ttxs = 1.0/(1.0 + exp((vOld+56.0)/3.0));
  m_inf_ttxr = pow(m3_inf_ttxr,0.333);
  m_inf_ttxs = pow(m3_inf_ttxs,0.333);
  tau_m = 1000.0*((0.6247e-03/(0.832*exp(-0.335*(vOld+56.7))+0.627*exp(0.082*(vOld+65.01))))+0.0000492);
  tau_h = 1000.0*(((3.717e-06*exp(-0.2815*(vOld+17.11)))/(1+0.003732*exp(-0.3426*(vOld + 37.76))))+0.0005977);
  tau_j = 1000.0*(((0.00000003186*exp(-0.6219*(vOld+18.8)))/(1+0.00007189*exp(-0.6683*(vOld+34.07))))+0.003556);
  Gate.m_ttxs = Gate.m_ttxs + dt*(m_inf_ttxs - Gate.m_ttxs)/tau_m;
  Gate.h_ttxs = Gate.h_ttxs + dt*(h_inf_ttxs - Gate.h_ttxs)/tau_h;
  Gate.j_ttxs = Gate.j_ttxs + dt*(h_inf_ttxs - Gate.j_ttxs)/tau_j;
  hs = (1.0-fna)*Gate.h_ttxs+fna*Gate.j_ttxs;
  tau_mr = 1000.0*((0.6247e-03/(0.832*exp(-0.335*(vOld+56.7))+0.627*exp(0.082*(vOld+65.01))))+0.0000492);
  tau_hr = 1000.0*(((3.717e-06*exp(-0.2815*(vOld+17.11)))/(1+0.003732*exp(-0.3426*(vOld + 37.76))))+0.0005977);
  tau_jr = 1000.0*(((0.00000003186*exp(-0.6219*(vOld+18.8)))/(1+0.00007189*exp(-0.6683*(vOld+34.07))))+0.003556);
  Gate.m_ttxr = Gate.m_ttxr + dt*(m_inf_ttxr - Gate.m_ttxr)/tau_mr;
  Gate.h_ttxr = Gate.h_ttxr + dt*(h_inf_ttxr - Gate.h_ttxr)/tau_hr;
  Gate.j_ttxr = Gate.j_ttxr + dt*(h_inf_ttxr - Gate.j_ttxr)/tau_jr;
  hsr = (1.0-fna)*Gate.h_ttxr+fna*Gate.j_ttxr;
  
  if(fabs(vOld)>0.005)
    ina_ttxs= INafactorxs*gna_ttxs*Gate.m_ttxs*Gate.m_ttxs*Gate.m_ttxs*hs*naO*(FDAY*FDAY/(RGAS*TEMP))*((exp((vOld-ena)*FDAY/(RGAS*TEMP))-1.0)/(exp(vOld*FDAY/(RGAS*TEMP))-1.0))*vOld;
  else
    ina_ttxs= INafactorxs*gna_ttxs*Gate.m_ttxs*Gate.m_ttxs*Gate.m_ttxs*hs*naO*FDAY*((exp((vOld-ena)*FDAY/(RGAS*TEMP))-1.0));
  if(fabs(vOld)>0.005)
    ina_ttxr = INafactorxr*gna_ttxr*Gate.m_ttxr*Gate.m_ttxr*Gate.m_ttxr*hsr*naO*(FDAY*FDAY/(RGAS*TEMP))*((exp((vOld-enattxr)*FDAY/(RGAS*TEMP))-1.0)/(exp(vOld*FDAY/(RGAS*TEMP))-1.0))*vOld;
  else
    ina_ttxr = INafactorxr*gna_ttxr*Gate.m_ttxr*Gate.m_ttxr*Gate.m_ttxr*hsr*naO*FDAY*((exp((vOld-enattxr)*FDAY/(RGAS*TEMP))-1.0));
}
/**If**************************************************************************/
void Ksan::updateIf()
{
  double y_inf, tau_y_1_2;
  double gh = 0.0057; 
  double ek  = (RGAS*TEMP/FDAY)*log(kO/kI);
  double ena = (RGAS*TEMP/FDAY)*log(naO/naI);
  double vhalf_gh = 106.8;

  y_inf = 1.0/(1.0 + exp((vOld+vhalf_gh)/16.3));
  tau_y_1_2 = 1.5049/(exp(-(vOld+590.3)*0.01094)+ exp((vOld-85.1)/17.2));
  Gate.y_1_2 = Gate.y_1_2 + dt*(y_inf - Gate.y_1_2)/tau_y_1_2;
  ihk  = 0.6167*gh*Gate.y_1_2*(vOld - ek);
  ihna = 0.3833*gh*Gate.y_1_2*(vOld - ena);
  ih = Iffactor*(ihk + ihna);
}
/*Ito*************************************************************************/
void Ksan::updateIto()
{
  double q_inf, tau_q, r_inf, tau_r;
  double gto = 0.00492;
  double ek  = (RGAS*TEMP/FDAY)*log(kO/kI);
  
  q_inf = 1.0/(1.0+exp((vOld+49.0)/13.0));
  tau_q = (6.06 + 39.102/(0.57*exp(-0.08*(vOld+44.0))+0.065*exp(0.1*(vOld+45.93))))/0.67; 
  Gate.q = Gate.q + dt*((q_inf-Gate.q)/tau_q);
  r_inf = 1.0/(1.0+exp(-(vOld-19.3)/15.0));
  tau_r = (2.75+14.40516/(1.037*exp(0.09*(vOld+30.61))+0.369*exp(-0.12*(vOld+23.84))))/0.303;
  Gate.r = Gate.r + dt*((r_inf-Gate.r)/tau_r);
  ito = Itofactor*gto*Gate.q*Gate.r*(vOld-ek);
 }
/*Isus***********************************************************************/
void Ksan::updateIsus()
{
  double gsus=0.00039060;
  double ek  = (RGAS*TEMP/FDAY)*log(kO/kI);

  isus = Isusfactor*gsus*Gate.r*(vOld-ek);
}
/*Inak***********************************************************************/
void Ksan::updateINaK()
{
  double inakmax_multiplier =1.85;
  double inakmax=inakmax_multiplier*0.077;
  double kmnap=14.0;
  double kmkp=1.4;
  double ena = (RGAS*TEMP/FDAY)*log(naO/naI);

  inak = INaKfactor*inakmax*(pow(kO,1.2)/(pow(kmkp,1.2)+pow(kO,1.2)))*(pow(naI,1.3)/(pow(kmnap,1.3)+pow(naI,1.3)))/(1.0+exp(-(vOld-ena+120.0)/30.0));
}
/****iNaCa*******************************************************************/

void Ksan::updateiNaCa()
{
  double kNaCa = 5.5;
  double di,doo,k43,k12,k14,k41,k34,k21,k23,k32,x1,x2,x3,x4;
  double K1ni=395.3;
  double K1no=1628;
  double K2ni=2.289;
  double K2no=561.4;
  double K3ni=26.44;
  double K3no=4.663;
  double Kci=0.0207;
  double Kco=3.663;
  double Kcni=26.44;
  double Qci=0.1369;
  double Qco=0.0;
  double Qn=0.4315;
  double FRT=FDAY/(RGAS*TEMP);

  di=1+(caSub/Kci)*(1+exp(-Qci*vOld*FRT)+naI/Kcni)+(naI/K1ni)*(1+(naI/K2ni)*(1+naI/K3ni));
  doo=1+(caO/Kco)*(1+exp(Qco*vOld*FRT))+(naO/K1no)*(1+(naO/K2no)*(1+naO/K3no));
  k43=naI/(K3ni+naI);
  k12=(caSub/Kci)*exp(-Qci*vOld*FRT)/di;
  k14=(naI/K1ni)*(naI/K2ni)*(1+naI/K3ni)*exp(Qn*vOld*FRT/2.0)/di;
  k41=exp(-Qn*vOld*FRT/2.0);
  k34=naO/(K3no+naO);
  k21=(caO/Kco)*exp(Qco*vOld*FRT)/doo;
  k23=(naO/K1no)*(naO/K2no)*(1+naO/K3no)*exp(-Qn*vOld*FRT/2.0)/doo;
  k32=exp(Qn*vOld*FRT/2);
  x1=k34*k41*(k23+k21)+k21*k32*(k43+k41);
  x2=k43*k32*(k14+k12)+k41*k12*(k34+k32);
  x3=k43*k14*(k23+k21)+k12*k23*(k43+k41);
  x4=k34*k23*(k14+k12)+k21*k14*(k34+k32);

  inaca = iNaCafactor*kNaCa*(k21*x2-k12*x1)/(x1+x2+x3+x4);   
}

void Ksan:: updateConc()
{
  double ca_flux;
  double kcasr,kosrca,kisrca;
  double dFtc,dFtmc,dFtmm,dFcms,dFcmi,dFcq;
  double ks = 1300000.0;
  double Pup = 0.04;
  double pumpkmf = 0.00008;
  double pumpkmr = 4.5;
  double pumphill=2.0;
  double tdifca=0.04;
  double Ttr=40.0;
  double ConcTC=0.031;
  double ConcTMC=0.062;
  double kfTC=88.8;
  double kfTMC=237.7;
  double kbTC=0.446;
  double kbTMC=0.00751;
  double kfTMM=2.277;
  double kbTMM=0.751;
  double ConcCM=0.045;
  double kfCM=237.7;
  double kbCM=0.542;
  double ConcCQ=10.0;
  double kfCQ=0.534;
  double kbCQ=0.445;
  double koca=10.0;
  double kom=0.06;
  double kica=0.5;
  double kim=0.005;
  double eca50sr=0.45;
  double maxsr=15.0;
  double minsr= 1.0;
  double hsrr=2.5;

  ca_flux = iCat/(2.0*FDAY);
  Jcadif = (caSub - caI)/tdifca;
  kcasr = maxsr - (maxsr - minsr)/(1.0 + pow(eca50sr/caJsr,hsrr));
  kosrca = koca/kcasr;
  kisrca = kica*kcasr;

  Gate.resting = Gate.resting + dt*(kim*Gate.resting_inactivated - kisrca*caSub*Gate.resting - kosrca*caSub*caSub*Gate.resting + kom*Gate.open);

  Gate.open    = Gate.open    + dt*(kosrca*caSub*caSub*Gate.resting - kom*Gate.open - kisrca*caSub*Gate.open + kim*Gate.inactivated);

  Gate.inactivated = Gate.inactivated + dt*(kisrca*caSub*Gate.open - kim*Gate.inactivated - kom*Gate.inactivated + kosrca*caSub*caSub*Gate.resting_inactivated);

  Gate.resting_inactivated = Gate.resting_inactivated + dt*(kom*Gate.inactivated - kosrca*caSub*caSub*Gate.resting_inactivated - kim*Gate.resting_inactivated + kisrca*caSub*Gate.resting);

  Jrel = ks*Gate.open*(caJsr - caSub);
  Jup = Pup*(pow(caI/pumpkmf,pumphill) - pow(caNsr/pumpkmr,pumphill))/(1.0 + pow(caI/pumpkmf,pumphill) + pow(caNsr/pumpkmr,pumphill));

  //Jup = Pup*(pow(caI/pumpkmf,pumphill) - pow(caNsr/pumpkmr,pumphill))/(1.0 + pow(caI/pumpkmf,pumphill) + pow(caNsr/pumpkmr,pumphill));
  Jtr  = (caNsr - caJsr)/Ttr;
  dFtc  = kfTC*caI*(1.0-Ftc)-kbTC*Ftc;
  dFtmc = kfTMC*caI*(1.0-Ftmc-Ftmm)-kbTMC*Ftmc;
  dFtmm = kfTMM*mgI*(1.0-Ftmc-Ftmm)-kbTMM*Ftmm;
  dFcms = kfCM*caSub*(1.0-Fcms)-kbCM*Fcms;
  dFcmi = kfCM*caI*(1.0-Fcmi)-kbCM*Fcmi;
  dFcq  = kfCQ*caJsr*(1.0-Fcq)-kbCQ*Fcq;
  Ftc = Ftc + dt*dFtc;        
  Ftmc = Ftmc + dt*dFtmc;     
  Ftmm = Ftmm + dt*dFtmm;     
  Fcms = Fcms + dt*dFcms;     
  Fcmi = Fcmi + dt*dFcmi;     
  Fcq = Fcq + dt*dFcq;        
  
  caSub = caSub + dt*((-ca_flux+Jrel*Vjsr)/Vss-Jcadif-ConcCM*dFcms);
  caI = caI + dt*((Jcadif*Vss-Jup*Vnsr)/Vmyo - (ConcCM*dFcmi + ConcTC*dFtc + ConcTMC*dFtmc)); 
  caJsr = caJsr + dt*(Jtr - Jrel - ConcCQ*dFcq);
  caNsr = caNsr + dt*(Jup-Jtr*Vjsr/Vnsr);
  naI = naI - dt*(iNat)/(FDAY*Vmyo);
  kI = kI - dt*(iKt)/(FDAY*Vmyo);

};

void Ksan::updateCurr()
{

	updateIst();
	updateIb();
	updateIK1();
	updateICaT();
	updateIKr();
	updateIKs();
	updateICaL();
	updateINa();
	updateIf();
	updateIto();
	updateIsus();
	updateINaK();
	updateiNaCa();  
	 
  	iNat=ihna+ina_ttxr+ina_ttxs+3.0*inak+3.0*inaca+ist+ibna;
        iKt = ihk+iks+ikr+ik1+ibk-2.0*inak+isus+ito;
        iCat = ical12+ical13+icat-2.0*inaca+ibca;
 
	iTot=(iNat+iCat+iKt)/TotCap;
}

void Ksan::makemap()
{
  vars["vOld"]=&vOld;
  vars["t"]=&t;
  vars["dVdt"]=&dVdt;

  vars["naI"]=&naI;
  vars["kI"]=&kI;
  vars["caI"]=&caI;
  vars["mgI"]=&caI;

  vars["Gate.dst"]=&Gate.dst;
  vars["Gate.fst"]=&Gate.fst;
  vars["Gate.dt"]=&Gate.dt;
  vars["Gate.ft"]=&Gate.ft;
  vars["Gate.ikr_act"]=&Gate.ikr_act;
  vars["Gate.ikr_inact"]=&Gate.ikr_inact;
  vars["Gate.ikr_inact2"]=&Gate.ikr_inact2;
  vars["Gate.iks_act"]=&Gate.iks_act;
  vars["Gate.fl12"]=&Gate.fl12;
  vars["Gate.dl12"]=&Gate.dl12 ;
  vars["Gate.fl13"]=&Gate.fl13 ;
  vars["Gate.dl13"]=&Gate.dl13 ;
  vars["Gate.fca"]=&Gate.fca;
  vars["Gate.r"]=&Gate.r;
  vars["Gate.m_ttxr"]=&Gate.m_ttxr;
  vars["Gate.h_ttxr"]=&Gate.h_ttxr;
  vars["Gate.j_ttxr"]=&Gate.j_ttxr;
  vars["Gate.m_ttxs"]=&Gate.m_ttxs;
  vars["Gate.h_ttxs"]=&Gate.h_ttxs;
  vars["Gate.j_ttxs"]=&Gate.j_ttxs;
  vars["Gate.y_1_2"]=&Gate.y_1_2;
  vars["Gate.q"]=&Gate.q;
  vars["Gate.resting"]=&Gate.resting;
  vars["Gate.open"]=&Gate.open;
  vars["Gate.inactivated"]=&Gate.inactivated;
  vars["Gate.resting_inactivated"]=&Gate.resting_inactivated;

  vars["Ftc"]=&Ftc;
  vars["Ftmc"]=&Ftmc;
  vars["Ftmm"]=&Ftmm;
  vars["Fcms"]=&Fcms;
  vars["Fcmi"]=&Fcmi;
  vars["Fcq"]=&Fcq;


   vars["caSub"]=&caSub;
  vars["caJsr"]=&caJsr;
  vars["caNsr"]=&caNsr;

  vars["ist"]=&ist;
  vars["ibca"]=&ibca;
  vars["ibna"]=&ibna;
  vars["ibk"]=&ibk;
  vars["ik1"]=&ik1;
  vars["icat"]=&icat;
  vars["ikr"]=&ikr;
  vars["iks"]=&iks;
  vars["ical12"]=&ical12;
  vars["ical13"]=&ical13;
  vars["ina_ttxr"]=&ina_ttxr;
  vars["ina_ttxs"]=&ina_ttxs;
  vars["ih"]=&ih;
  vars["ihk"]=&ihk;
  vars["ihna"]=&ihna;
  vars["inak"]=&inak;
  vars["inaca"]=&inaca;
  vars["isus"]=&isus;
  vars["ito"]=&ito;
  vars["Jrel"]=&Jrel;
  vars["Jup"]=&Jup;
  vars["Jtr"]=&Jtr;
  vars["Jcadif"]=&Jcadif;

}

// External stimulus.
int Ksan::externalStim(double stimval) {
    iTot = iTot + stimval;
    return 1;
}


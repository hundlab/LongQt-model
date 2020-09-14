#include "gpbatrial_different_ina.h"

#include <array>
using namespace LongQt;
GpbAtrialdiffINa::GpbAtrialdiffINa() { this->Initialize(); }

GpbAtrialdiffINa::GpbAtrialdiffINa(GpbAtrialdiffINa &toCopy) : Cell(toCopy) {
  this->Initialize();
  CellKernel::copyVarPar(toCopy);
}

GpbAtrialdiffINa::~GpbAtrialdiffINa() {}

void GpbAtrialdiffINa::Initialize() {
  Cm = 1.0;        // uF/cm2
  ACap = 1.10E-4;  // capacitive area, cm2
  dVdt = /*dVdtmax=*/0.0;
  t = 0.0;
  dt = dtmin = 0.005;
  dtmed = 0.005;
  dtmax = 0.005;
  dvcut = 1.0;
  vOld = /*vNew=*/-80.9763;

  apTime = 0.0;
  //    flag = 0;
  //    num = 0;

  Vcell = 3.3E-5;  // uL

  //###### Concentrations #########
  naI = 9.136;           // mM
  naO = 140.0;           // mM
  kI = 120.0;            // mM
  kO = 5.4;              // mM
  caI = 0.00008597401;   // mM
  caO = 1.8;             // mM
  caSr = 0.01;           // mM
  cajI = 0.0001737475;   // mM
  caslI = 0.0001031812;  // mM
  najI = 9.136;          // mM
  naslI = 9.136;         // mM
  clO = 150.0;           // mM
  clI = 15.0;            // mM
  mgI = 1.0;             // mM
  //##### Fractional Currents ########
  Fjunc = 0.11;
  F_juncCaL = 0.9;
  //##### Buffers ########
  // Sodium Buffers
  Nabj = 3.539892;
  Nabsl = 0.7720854;  // mM
  // Calcium Buffers
  TnCl = 0.008773191;  // mM
  TnChc = 0.1078283;   // mM
  TnChm = 0.01524002;
  CaM = 0.0002911916;  // mM
  Myoc = 0.001298754;  // mM
  Myom = 0.1381982;
  SRB = 0.002143165;  // mM
  // junctional and SL Ca buffers
  SLLj = 0.009566355;
  SLLsl = 0.1110363;
  SLHj = 0.007347888;
  SLHsl = 0.07297378;
  // SR Ca buffer
  Csqnb = 1.242988;
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

  //####### RyR channel fractions
  Ryrr = 0.8884332;
  Ryro = 8.156628E-7;
  Ryri = 1.024274E-7;

  iTos = iTof = iTo = 0.0;
  iKsjunc = iKssl = iKs = iKr = iKur = iKpjunc = iKpsl = iKp = iK1 = 0.0;
  iNajunc = iNasl = iNabjunc = iNabsl = iNab = iNa = 0.0;
  iCajunc = iCasl = iCa = 0.0;
  iCaL = iCab = ipCa = 0.0;
  iCak = iCanajunc = iCanasl = iCana = 0.0;
  iNaKjunc = iNaKsl = iNak = 0.0;
  iNcxjunc = iNcxsl = iNaca = 0.0;
  iClcajunc = iClcasl = iClca = iClbk = 0.0;
  ipCajunc = ipCasl = ipCa = 0.0;
  iCabjunc = iCabsl = iCab = 0.0;

  Icajuncfactor = 1;
  Icaslfactor = 1;
  Icakfactor = 1;
  Icanajuncfactor = 1;
  Icanaslfactor = 1;
  Icabslfactor = 1;
  Icabjuncfactor = 1;
  Ipcaslfactor = 1;
  Ipcajuncfactor = 1;
  Itofactor = 1;
  Iksslfactor = 1;
  Iksjuncfactor = 1;
  Ikrfactor = 1;
  Ik1factor = 1;
  Ikurfactor = 1;
  Ipkslfactor = 1;
  Ipkjuncfactor = 1;
  Inacaslfactor = 1;
  Inacajuncfactor = 1;
  Inakslfactor = 1;
  Inakjuncfactor = 1;
  Inabslfactor = 1;
  Inabjuncfactor = 1;
  Inaslfactor = 1;
  Inajuncfactor = 1;
  Jsrcarelfactor = 1;
  Jsrleakfactor = 1;
  Jsercafactor = 1;
  Iclcaslfactor = 1;
  Iclcajuncfactor = 1;
  Iclbkfactor = 1;
  Icalfactor = 1;

  this->insertOpt("OHaraRudy", &OHaraRudyFlag, "OHaraRudy Ina");
  this->insertOpt("Novel2020", &NovelFlag, "Novel 2020 Ina");
  this->insertOpt("Clancy", &ClancyFlag, "Clancy Ina");

  this->insertConflicts({"OHaraRudy", "Novel2020", "Clancy"});

  makemap();
}
GpbAtrialdiffINa *GpbAtrialdiffINa::clone() { return new GpbAtrialdiffINa(*this); }

void GpbAtrialdiffINa::setup() {
  Cell::setup();
  Vsr = 0.035 * Vcell;
  Vjunc = (5.39E-4) * Vcell;
  Vsl = 0.02 * Vcell;
  Vmyo = 0.65 * Vcell;

  Fsl = 1 - Fjunc;
  F_slCaL = 1 - F_juncCaL;
  RI = 1.0 - Ryrr - Ryro - Ryri;
//###########################OHARA RUDY
  if (OHaraRudyFlag) {
// scaling currents 0
this->OHR.GNa = 75*exp(this->OHRF.GNaFactor);
this->OHR.GNaL = 0.0075*exp(this->OHRF.GNaLFactor);

//m gate 2
this->OHR.mss_tau = 9.871*exp(this->OHRF.mss_tauFactor);

this->OHR.tm_max = 0.473824721*exp(this->OHRF.tm_maxFactor);
this->OHR.tm_tau1 = 34.77*exp(this->OHRF.tm_tau1Factor);
this->OHR.tm_shift = -57.6379999+this->OHRF.tm_shiftFactor;
this->OHR.tm_tau2 = 5.955*exp(this->OHRF.tm_tau2Factor);
double tm_cshift = log(this->OHR.tm_tau1/this->OHR.tm_tau2)/(1/this->OHR.tm_tau1+1/this->OHR.tm_tau2);
double tm_cmax = exp(tm_cshift/this->OHR.tm_tau1) + exp(-tm_cshift/this->OHR.tm_tau2);
this->OHR.tm_shift -= tm_cshift; //shift correction
this->OHR.tm_max *= tm_cmax; //height correction

//h gate 7
this->OHR.hss_tau = 6.086*exp(this->OHRF.hss_tauFactor);

this->OHR.thf_max = 3.594172982376325*exp(this->OHRF.thf_maxFactor);
this->OHR.thf_tau1 = 6.285*exp(this->OHRF.thf_tau1Factor);
this->OHR.thf_shift = -57.639999999606744+this->OHRF.thf_shiftFactor;
this->OHR.thf_tau2 = 20.27*exp(this->OHRF.thf_tau2Factor);
double thf_cshift = log(this->OHR.thf_tau2/this->OHR.thf_tau1)/(1/this->OHR.thf_tau2+1/this->OHR.thf_tau1);
double thf_cmax = exp(thf_cshift/this->OHR.thf_tau2) + exp(-thf_cshift/this->OHR.thf_tau1);
this->OHR.thf_shift -= thf_cshift;
this->OHR.thf_max *= thf_cmax;

//12
this->OHR.ths_max = 5.894233734241695*exp(this->OHRF.ths_maxFactor);
this->OHR.ths_tau1 = 28.05*exp(this->OHRF.ths_tau1Factor);
this->OHR.ths_shift = -66.94699999965118+this->OHRF.ths_shiftFactor;
this->OHR.ths_tau2 = 56.66*exp(this->OHRF.ths_tau2Factor);
double ths_cshift = log(this->OHR.ths_tau2/this->OHR.ths_tau1)/(1/this->OHR.ths_tau2+1/this->OHR.ths_tau1);
double ths_cmax = exp(ths_cshift/this->OHR.ths_tau2) + exp(-ths_cshift/this->OHR.ths_tau1);
this->OHR.ths_shift -= ths_cshift;
this->OHR.ths_max *= ths_cmax;

double mixingodds = exp(this->OHRF.Ahf_multFactor+log(99));
this->OHR.Ahf_mult = mixingodds/(mixingodds+1);// exp(Ahf_multFactor)

//j gate 17
this->OHR.tj_baseline = 2.038*exp(this->OHRF.tj_baselineFactor);
this->OHR.tj_max = 27.726847365476033*exp(this->OHRF.tj_maxFactor);
this->OHR.tj_tau1 = 8.281*exp(this->OHRF.tj_tau1Factor);
this->OHR.tj_shift = -90.60799999976416+this->OHRF.tj_shiftFactor;
this->OHR.tj_tau2 = 38.45*exp(this->OHRF.tj_tau2Factor);
double tj_cshift = log(this->OHR.tj_tau2/this->OHR.tj_tau1)/(1/this->OHR.tj_tau2+1/this->OHR.tj_tau1);
double tj_cmax = exp(tj_cshift/this->OHR.tj_tau2) + exp(-tj_cshift/this->OHR.tj_tau1);
this->OHR.tj_shift -= tj_cshift;
this->OHR.tj_max *= tj_cmax;

// phosphorylated gates
this->OHR.hssp_tau = 6.086*exp(this->OHRF.hssp_tauFactor);
this->OHR.tssp_mult = 3.0*exp(this->OHRF.tssp_multFactor);
this->OHR.tjp_mult = 1.46*exp(this->OHRF.tjp_multFactor);

//late gates & late gate phosphorylation
this->OHR.mLss_tau = 5.264*exp(this->OHRF.mLss_tauFactor);
this->OHR.hLss_tau = 7.488*exp(this->OHRF.hLss_tauFactor);
this->OHR.hLssp_tau = this->OHR.hLss_tau;

this->OHR.thL_baseline = 200.0*exp(this->OHRF.thL_baselineFactor);
this->OHR.thLp_mult = 3*exp(this->OHRF.thLp_multFactor);

//added later 29
this->OHR.mss_shift = 39.57+this->OHRF.mss_shiftFactor;
this->OHR.hss_shift = 82.90+this->OHRF.hss_shiftFactor;
this->OHR.jss_shift = 82.90+this->OHRF.jss_shiftFactor;
this->OHR.jss_tau = 6.086*exp(this->OHRF.jss_tauFactor);
  }
//############### NOVEL
  if(NovelFlag) {
// scaling currents 0
  Nov.GNa = exp(NovF.GNaFactor);

//fastest tau 1
  Nov.baseline = 2.038*exp(NovF.baselineFactor);

//m gate 2
  Nov.mss_tau = 9.871*exp(NovF.mss_tauFactor);
  Nov.mss_shift = 51.57+NovF.mss_shiftFactor;

  Nov.tm_max = 0.474*exp(NovF.tm_maxFactor);
  Nov.tm_tau1 = 34.77*exp(NovF.tm_tau1Factor);
  Nov.tm_shift = -57.6+NovF.tm_shiftFactor;
  Nov.tm_tau2 = 5.955*exp(NovF.tm_tau2Factor);
  double tm_cshift = log(Nov.tm_tau1/Nov.tm_tau2)/(1/Nov.tm_tau1+1/Nov.tm_tau2);
  double tm_cmax = exp(tm_cshift/Nov.tm_tau1) + exp(-tm_cshift/Nov.tm_tau2);
  Nov.tm_shift -= tm_cshift; //shift correction
  Nov.tm_max *= tm_cmax; //height correction

//h gate 8
  Nov.hss_tau = 14.086*exp(NovF.hss_tauFactor);
  Nov.hfss_shift = -76+NovF.hss_shiftFactor;

  Nov.thf_max = 5*exp(NovF.thf_maxFactor);
  Nov.thf_tau1 = 6.285*exp(NovF.thf_tau1Factor);
  Nov.thf_shift = -57.639999999606744+NovF.thf_shiftFactor;
  Nov.thf_tau2 = 15*exp(NovF.thf_tau2Factor);
  double thf_cshift = log(Nov.thf_tau2/Nov.thf_tau1)/(1/Nov.thf_tau2+1/Nov.thf_tau1);
  double thf_cmax = exp(thf_cshift/Nov.thf_tau2) + exp(-thf_cshift/Nov.thf_tau1);
  Nov.thf_shift -= thf_cshift;
  Nov.thf_max *= thf_cmax;

  Nov.ths_max = 5.894233734241695*exp(NovF.ths_maxFactor);
  Nov.ths_tau1 = 28.05*exp(NovF.ths_tau1Factor);
  Nov.ths_shift = -66.94699999965118+NovF.ths_shiftFactor;
  Nov.ths_tau2 = 40*exp(NovF.ths_tau2Factor);
  double ths_cshift = log(Nov.ths_tau2/Nov.ths_tau1)/(1/Nov.ths_tau2+1/Nov.ths_tau1);
  double ths_cmax = exp(ths_cshift/Nov.ths_tau2) + exp(-ths_cshift/Nov.ths_tau1);
  Nov.ths_shift -= ths_cshift;
  Nov.ths_max *= ths_cmax;


  double mixingodds = exp(NovF.Ahf_multFactor);
  Nov.Ahf_mult = mixingodds/(mixingodds+1);// np.exp(Ahf_multFactor);

//j gate 18
  Nov.jss_tau = 20*exp(NovF.jss_tauFactor);
  Nov.jss_shift = -110+NovF.jss_shiftFactor;

  Nov.tj_max = 100*exp(NovF.tj_maxFactor);
  Nov.tj_tau2 = 10*exp(NovF.tj_tau2Factor);
  Nov.tj_tau1 = 20*exp(NovF.tj_tau1Factor);
  Nov.tj_shift = -80+NovF.tj_shiftFactor;
  double tj_cshift = log(Nov.tj_tau1/Nov.tj_tau2)/(1/Nov.tj_tau1+1/Nov.tj_tau2);
  double tj_cmax = exp(tj_cshift/Nov.tj_tau1) + exp(-tj_cshift/Nov.tj_tau2);
  Nov.tj_shift -= tj_cshift; //shift correction
  Nov.tj_max *= tj_cmax; //height correction
  }
}

void GpbAtrialdiffINa::updateConc() {
  updateSRFlux();
  updatecytobuff();  // cytosolic Ca buffers
  updateJSLbuff();   // junctional and SL Ca buffers
  updateSRbuff();    // SR Ca buffer

  updatecaI();
  updatenaI();
}

void GpbAtrialdiffINa::updatecaI() {
  double dCaj, dCasl, dcaI, dCasr;
  double J_cajuncsl = 8.2413E-7;
  double J_caslmyo = 3.7243E-6;

  dCaj = dt * (-iCatotjunc * (ACap / (Vjunc * 2.0 * FDAY)) +
               (J_cajuncsl / Vjunc) * (caslI - cajI) - J_Cabjunc +
               Jsrcarel * (Vsr / Vjunc) + Jsrleak * (Vmyo / Vjunc));
  dCasl = dt * (-iCatotsl * (ACap / (Vsl * 2.0 * FDAY)) +
                (J_cajuncsl / Vsl) * (cajI - caslI) - J_Cabsl +
                (J_caslmyo / Vsl) * (caI - caslI));
  dcaI = dt * (-Jserca * (Vsr / Vmyo) - J_cabcyto +
               (J_caslmyo / Vmyo) * (caslI - caI));
  dCasr = dt * (Jserca - (Jsrleak * (Vmyo / Vsr) + Jsrcarel) - dCsqnb / dt);

  cajI = cajI + dCaj;
  caslI = caslI + dCasl;
  caI = caI + dcaI;
  caSr = caSr + dCasr;
}

void GpbAtrialdiffINa::updateSRFlux() {
  // SR fluxes
  double dRyrr, dRyro, dRyri;
  double maxsr = 15.0;
  double minsr = 1.0;
  double kCasr, kOsrca, kIsrca;
  double ec50SR = 0.45;
  double koCa = 10.0;
  double kiCa = 0.5;
  double kim = 0.005;
  double kom = 0.06;
  double ks = 25.0;
  double alpha, beta;
  double kmf = 2.5 * 0.000246;
  double kmr = 1.7;
  double hillSRcaP = 1.787;
  double VmaxSRcaP = 0.0053114;

  kCasr = maxsr - ((maxsr - minsr) / (1.0 + pow((ec50SR / caSr), 2.5)));
  kOsrca = koCa / kCasr;
  kIsrca = kiCa * kCasr;

  dRyrr = dt * ((kim * RI - kIsrca * cajI * Ryrr) -
                (kOsrca * cajI * cajI * Ryrr - kom * Ryro));
  dRyro = dt * ((kOsrca * cajI * cajI * Ryrr - kom * Ryro) -
                (kIsrca * cajI * Ryro - kim * Ryri));
  dRyri = dt * ((kIsrca * cajI * Ryro - kim * Ryri) -
                (kom * Ryri - kOsrca * cajI * cajI * RI));

  Ryrr = Ryrr + dRyrr;
  Ryro = Ryro + dRyro;
  Ryri = Ryri + dRyri;
  RI = 1.0 - Ryrr - Ryro - Ryri;

  Jsrcarel = Jsrcarelfactor * ks * Ryro * (caSr - cajI);

  alpha = pow((caI / kmf), hillSRcaP) - pow((caSr / kmr), hillSRcaP);
  beta = 1.0 + pow((caI / kmf), hillSRcaP) + pow((caSr / kmr), hillSRcaP);
  Jserca = Jsercafactor * VmaxSRcaP * (alpha / beta);

  Jsrleak = Jsrleakfactor * 0.000005348 * (caSr - cajI);
}
void GpbAtrialdiffINa::updatecytobuff() {
  // cytosolic Ca buffers

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

  dTnCl = dt * (konTnCl * caI * (BmaxTnCl - TnCl) - koffTnCl * TnCl);
  dTnChc =
      dt * (konTnChCa * caI * (BmaxTnCh - TnChc - TnChm) - koffTnChCa * TnChc);
  dTnChm =
      dt * (konTnChMg * mgI * (BmaxTnCh - TnChc - TnChm) - koffTnChMg * TnChm);
  dCaM = dt * (konCaM * caI * (BmaxCaM - CaM) - koffCaM * CaM);
  dMyoc = dt * (konmyoCa * caI * (Bmaxmyosin - Myoc - Myom) - koffmyoCa * Myoc);
  dMyom = dt * (konmyoMg * mgI * (Bmaxmyosin - Myoc - Myom) - koffmyoMg * Myom);
  dSRB = dt * (konsr * caI * (BmaxSR - SRB) - koffsr * SRB);

  J_cabcyto = (dTnCl + dTnChc + dTnChm + dCaM + dMyoc + dMyom + dSRB) / dt;

  TnCl = TnCl + dTnCl;
  TnChc = TnChc + dTnChc;
  TnChm = TnChm + dTnChm;
  CaM = CaM + dCaM;
  Myoc = Myoc + dMyoc;
  Myom = Myom + dMyom;
  SRB = SRB + dSRB;
}

void GpbAtrialdiffINa::updateJSLbuff() {
  // junctional and SL Ca Buffers
  double konsll = 100.0;
  double koffsll = 1.3;
  double konslh = 100;
  double koffslh = 0.03;

  double dSLLj, dSLLsl, dSLHj, dSLHsl;
  double BmaxSLlowj = 0.00046 * (Vmyo / Vjunc);
  double BmaxSLlowsl = 0.0374 * (Vmyo / Vsl);
  double BmaxSLhighj = 0.000165 * (Vmyo / Vjunc);
  double BmaxSLhighsl = 0.0134 * (Vmyo / Vsl);

  dSLLj = dt * (konsll * cajI * (BmaxSLlowj - SLLj) - koffsll * SLLj);
  dSLLsl = dt * (konsll * caslI * (BmaxSLlowsl - SLLsl) - koffsll * SLLsl);
  dSLHj = dt * (konslh * cajI * (BmaxSLhighj - SLHj) - koffslh * SLHj);
  dSLHsl = dt * (konslh * caslI * (BmaxSLhighsl - SLHsl) - koffslh * SLHsl);

  SLLj = SLLj + dSLLj;
  SLLsl = SLLsl + dSLLsl;
  SLHj = SLHj + dSLHj;
  SLHsl = SLHsl + dSLHsl;

  J_Cabjunc = (dSLLj + dSLHj) / dt;
  J_Cabsl = (dSLLsl + dSLHsl) / dt;
}

void GpbAtrialdiffINa::updateSRbuff() {
  // SR Ca buffer
  double koncsqn = 100;
  double koffcsqn = 65;

  dCsqnb =
      dt * (koncsqn * caSr * (0.140 * (Vmyo / Vsr) - Csqnb) - koffcsqn * Csqnb);

  Csqnb = Csqnb + dCsqnb;
}

void GpbAtrialdiffINa::updateCurr() {
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

  iNatotjunc = iNajunc + iNabjunc + 3.0 * iNcxjunc + 3.0 * iNaKjunc + iCanajunc;
  iNatotsl = iNasl + iNabsl + 3.0 * iNcxsl + 3.0 * iNaKsl + iCanasl;
  iNat = iNatotjunc + iNatotsl;
  iClt = iClca + iClbk;
  iCatotjunc = iCajunc + iCabjunc + ipCajunc - 2 * iNcxjunc;
  iCatotsl = iCasl + iCabsl + ipCasl - 2 * iNcxsl;
  iCat = iCatotjunc + iCatotsl;
  iKt = iTo + iKr + iKs + iK1 + iKur - 2 * iNak + iCak + iKp;
  iTot = iNat + iClt + iCat + iKt;
}

void GpbAtrialdiffINa::updateIcal() {
  double pca = 2.7E-4;
  double pk = 1.35E-7;
  double pna = 7.5E-9;
  double Frdy = FDAY;
  double FoRT = FDAY / (RGAS * TEMP);
  double d_inf, taud;
  double f_inf, tauf;
  double icajbar, icaslbar, icakbar, icanajbar, icanaslbar;
  double df_cabjdt, df_cabsldt;

  d_inf = 1 / (1 + exp(-(vOld + 9) / 6));
  taud = d_inf * (1 - exp(-(vOld + 9) / 6)) / (0.035 * (vOld + 9));
  f_inf = 1 / (1 + exp((vOld + 30) / 7)) + 0.2 / (1 + exp((50 - vOld) / 20));
  tauf = 1 / (0.0197 * exp(-(0.0337 * (vOld + 25)) * (0.0337 * (vOld + 25))) +
              0.02);

  Gate.f = f_inf - (f_inf - Gate.f) * exp(-dt / tauf);
  Gate.d = d_inf - (d_inf - Gate.d) * exp(-dt / taud);

  df_cabjdt = 1.7 * cajI * (1 - Gate.f_cabj) - 11.9E-3 * Gate.f_cabj;
  df_cabsldt = 1.7 * caslI * (1 - Gate.f_cabsl) - 11.9E-3 * Gate.f_cabsl;

  Gate.f_cabj = Gate.f_cabj + df_cabjdt * dt;
  Gate.f_cabsl = Gate.f_cabsl + df_cabsldt * dt;

  icajbar = pca * 4 * (vOld * Frdy * FoRT) *
            (0.341 * cajI * exp(2 * vOld * FoRT) - 0.341 * caO) /
            (exp(2 * vOld * FoRT) - 1);
  icaslbar = pca * 4 * (vOld * Frdy * FoRT) *
             (0.341 * caslI * exp(2 * vOld * FoRT) - 0.341 * caO) /
             (exp(2 * vOld * FoRT) - 1);
  icakbar = pk * (vOld * Frdy * FoRT) *
            (0.75 * kI * exp(vOld * FoRT) - 0.75 * kO) / (exp(vOld * FoRT) - 1);
  icanajbar = pna * (vOld * Frdy * FoRT) *
              (0.75 * najI * exp(vOld * FoRT) - 0.75 * naO) /
              (exp(vOld * FoRT) - 1);
  icanaslbar = pna * (vOld * Frdy * FoRT) *
               (0.75 * naslI * exp(vOld * FoRT) - 0.75 * naO) /
               (exp(vOld * FoRT) - 1);

  iCajunc = Icajuncfactor *
            (F_juncCaL * icajbar * Gate.d * Gate.f * ((1 - Gate.f_cabj))) *
            0.45 * 1;
  iCasl = Icaslfactor *
          (F_slCaL * icaslbar * Gate.d * Gate.f * ((1 - Gate.f_cabsl))) * 0.45 *
          1;
  iCa = iCajunc + iCasl;
  iCak = Icakfactor *
         ((icakbar * Gate.d * Gate.f *
           (F_juncCaL * ((1 - Gate.f_cabj)) + F_slCaL * ((1 - Gate.f_cabsl)))) *
          0.45 * 1);
  iCanajunc = Icanajuncfactor *
              (F_juncCaL * icanajbar * Gate.d * Gate.f * ((1 - Gate.f_cabj))) *
              0.45 * 1;
  iCanasl = Icanaslfactor *
            (F_slCaL * icanaslbar * Gate.d * Gate.f * ((1 - Gate.f_cabsl))) *
            .45 * 1;
  iCana = iCanajunc + iCanasl;
  iCaL = Icalfactor * (iCa + iCak + iCana);
}

void GpbAtrialdiffINa::updateIcab() {
  double gcab = 6.0643E-4;  // A/F
  double Ecajunc = RGAS * TEMP / (2 * FDAY) * log(caO / cajI);
  double Ecasl = RGAS * TEMP / (2 * FDAY) * log(caO / caslI);

  iCabjunc = Icabjuncfactor * Fjunc * gcab * (vOld - Ecajunc);
  iCabsl = Icabslfactor * Fsl * gcab * (vOld - Ecasl);

  iCab = iCabjunc + iCabsl;
}

void GpbAtrialdiffINa::updateIpca() {
  double ipmcabar = 0.0471;  // A/F

  double kmpca = 0.0005;

  ipCajunc = Ipcajuncfactor * (Fjunc * ipmcabar * pow(cajI, 1.6)) /
             (pow(kmpca, 1.6) + pow(cajI, 1.6));
  ipCasl = Ipcaslfactor * (Fsl * ipmcabar * pow(caslI, 1.6)) /
           (pow(kmpca, 1.6) + pow(caslI, 1.6));

  ipCa = ipCajunc + ipCasl;
}

void GpbAtrialdiffINa::updateIclca() {
  double gclca = 0.0548;  // mS/uF
  double kd_clca = 0.1;
  double Ecl = -RGAS * TEMP / FDAY * log(clO / clI);

  iClcajunc =
      Iclcajuncfactor * (Fjunc * gclca * (vOld - Ecl)) / (1.0 + kd_clca / cajI);
  iClcasl =
      Iclcaslfactor * (Fsl * gclca * (vOld - Ecl)) / (1.0 + kd_clca / caslI);
  iClca = iClcajunc + iClcasl;
}

void GpbAtrialdiffINa::updateIclbk() {
  double gclb = 0.009;  // ms/uF
  double Ecl = -RGAS * TEMP / FDAY * log(clO / clI);

  iClbk = Iclbkfactor * gclb * (vOld - Ecl);
}

void GpbAtrialdiffINa::updateIto() {
  // fast component variables
  double gtof = 0.165;  // ms/uF
  double xs_inf, ys_inf;
  double tau_xf, tau_yf;

  double Ek = RGAS * TEMP / FDAY * log(kO / kI);

  iTos = 0.0;  // gtos*Gate.xs*Gate.ys*(vOld-Ek);

  xs_inf = ((1) / (1 + exp(-(vOld + 1.0) / 11.0)));
  tau_xf = 3.5 * exp(-((vOld / 30.0) * (vOld / 30.0))) + 1.5;

  ys_inf = ((1.0) / (1 + exp((vOld + 40.5) / 11.5)));
  tau_yf =
      25.635 * exp(-(((vOld + 52.45) / 15.8827) * ((vOld + 52.45) / 15.8827))) +
      24.14;

  Gate.xf = xs_inf - (xs_inf - Gate.xf) * exp(-dt / tau_xf);
  Gate.yf = ys_inf - (ys_inf - Gate.yf) * exp(-dt / tau_yf);

  iTof = Itofactor * gtof * Gate.xf * Gate.yf * (vOld - Ek);

  iTo = iTos + iTof;
}

void GpbAtrialdiffINa::updateIkur() {
  double gkur = 0.045;
  double xkur_inf, ykur_inf;
  double tau_xkur, tau_ykur;

  double Ek = RGAS * TEMP / FDAY * log(kO / kI);

  xkur_inf = ((1) / (1 + exp((vOld + 6) / -8.6)));
  tau_xkur = 9 / (1 + exp((vOld + 5) / 12.0)) + 0.5;

  ykur_inf = ((1) / (1 + exp((vOld + 7.5) / 10)));
  tau_ykur = 590 / (1 + exp((vOld + 60) / 10.0)) + 3050;

  Gate.xkur = xkur_inf - (xkur_inf - Gate.xkur) * exp(-dt / tau_xkur);
  Gate.ykur = ykur_inf - (ykur_inf - Gate.ykur) * exp(-dt / tau_ykur);

  iKur = Ikurfactor * 1 * gkur * Gate.xkur * Gate.ykur * (vOld - Ek);
}

void GpbAtrialdiffINa::updateIks() {
  double gksjunc = 0.0035;  // ms/uF
  double gkssl = 0.0035;    // ms/uF

  double xs_inf, tau_xs;
  double pnak = 0.01833;
  double EKsjunc =
      RGAS * TEMP / FDAY * log((kO + pnak * naO) / (kI + pnak * naI));
  double EKssl =
      RGAS * TEMP / FDAY * log((kO + pnak * naO) / (kI + pnak * naI));

  xs_inf = 1.0 / (1.0 + exp((-3.8 - vOld) / 14.25));
  tau_xs = 990.1 / (1.0 + exp((-2.436 - vOld) / 14.12));

  Gate.xks = xs_inf - (xs_inf - Gate.xks) * exp(-dt / tau_xs);

  iKsjunc =
      Iksjuncfactor * Fjunc * gksjunc * Gate.xks * Gate.xks * (vOld - EKsjunc);
  iKssl = Iksslfactor * Fsl * gkssl * Gate.xks * Gate.xks * (vOld - EKssl);

  iKs = iKsjunc + iKssl;
}

void GpbAtrialdiffINa::updateIkr() {
  double gkr = 0.035 * sqrt(kO / 5.4);
  double Ek = RGAS * TEMP / FDAY * log(kO / kI);
  double r_kr;
  double xr_inf, alpha_xr, beta_xr, tau_xr;

  xr_inf = 1.0 / (1.0 + exp((-vOld - 10.0) / 5.0));
  alpha_xr = (550.0 / (1.0 + exp((-22.0 - vOld) / 9.0))) *
             (6.0 / (1.0 + exp((vOld + 11.0) / 9.0)));
  beta_xr = 230.0 / (1.0 + exp((vOld + 40.0) / 20.0));
  tau_xr = alpha_xr + beta_xr;
  Gate.xkr = xr_inf - (xr_inf - Gate.xkr) * exp(-dt / tau_xr);

  r_kr = 1.0 / (1.0 + exp((vOld + 74.0) / 24.0));

  iKr = Ikrfactor * gkr * Gate.xkr * r_kr * (vOld - Ek);
}

void GpbAtrialdiffINa::updateIk1() {
  double alpha_K1, beta_K1, k1_inf;

  double Ek = RGAS * TEMP / FDAY * log(kO / kI);

  alpha_K1 = 1.02 / (1.0 + exp(0.2385 * (vOld - Ek - 59.215)));
  beta_K1 = (0.49124 * exp(0.08032 * (vOld - Ek + 5.476)) +
             exp(0.06175 * (vOld - Ek - 594.31))) /
            (1 + exp(-0.5143 * (vOld - Ek + 4.753)));
  k1_inf = alpha_K1 / (alpha_K1 + beta_K1);

  iK1 = Ik1factor * 0.0525 * sqrt(kO / 5.4) * k1_inf * (vOld - Ek);
}

void GpbAtrialdiffINa::updateIpk() {
  double gkp = 2 * 0.001;  // 0.002; //ms/uF
  double kp_kp;
  double Ek = RGAS * TEMP / FDAY * log(kO / kI);

  kp_kp = 1.0 / (1.0 + exp(7.488 - vOld / 5.98));
  iKpjunc = Ipkjuncfactor * Fjunc * gkp * kp_kp * (vOld - Ek);
  iKpsl = Ipkslfactor * Fsl * gkp * kp_kp * (vOld - Ek);
  iKp = iKpjunc + iKpsl;
}

void GpbAtrialdiffINa::updateInaca() {
  double incxbar = 3.15;  // A/F
  double kajunc, kasl;
  double s1junc, s1sl, s2junc, s3junc, s2sl, s3sl;
  double kmcai = 0.00359;
  double kmnai = 12.29;
  double kmnao = 87.5;
  double kmcao = 1.3;
  double ksat = 0.27;
  double kdact = 0.384E-3;
  double nu = 0.35;

  kajunc = 1.0 / (1.0 + ((kdact / cajI) * (kdact / cajI)));
  kasl = 1.0 / (1.0 + ((kdact / caslI) * (kdact / caslI)));

  s1junc = exp(nu * vOld * (FDAY / RGAS / TEMP)) * pow(najI, 3) * caO;
  s1sl = exp(nu * vOld * (FDAY / RGAS / TEMP)) * pow(naslI, 3) * caO;
  s2junc = exp((nu - 1) * vOld * (FDAY / RGAS / TEMP)) * pow(naO, 3) * cajI;
  s3junc = kmcai * pow(naO, 3) * (1 + pow((najI / kmnai), 3)) +
           pow(kmnao, 3) * cajI * (1 + cajI / kmcai) + kmcao * pow(najI, 3) +
           pow(najI, 3) * caO + pow(naO, 3) * cajI;
  s2sl = exp((nu - 1) * vOld * (FDAY / RGAS / TEMP)) * pow(naO, 3) * caslI;
  s3sl = kmcai * pow(naO, 3) * (1 + pow((naslI / kmnai), 3)) +
         pow(kmnao, 3) * caslI * (1 + (caslI / kmcai)) + kmcao * pow(naslI, 3) +
         pow(naslI, 3) * caO + pow(naO, 3) * caslI;

  iNcxjunc =
      Inacajuncfactor * (Fjunc * incxbar * kajunc * (s1junc - s2junc)) /
      (s3junc * (1 + ksat * exp((nu - 1) * vOld * (FDAY / RGAS / TEMP))));
  iNcxsl = Inacaslfactor * (Fsl * incxbar * kasl * (s1sl - s2sl)) /
           (s3sl * (1 + ksat * exp((nu - 1) * vOld * (FDAY / RGAS / TEMP))));

  iNaca = iNcxjunc + iNcxsl;
}

void GpbAtrialdiffINa::updateInak() {
  double inakbar = 1.26;
  double sigma, fnak;
  double kmnaip = 11;
  double kmko = 1.5;
  double FoRT = FDAY / (RGAS * TEMP);

  sigma = (exp(naO / 67.3) - 1) / 7;
  fnak = 1 / (1 + 0.1245 * exp(-0.1 * vOld * FoRT) +
              0.0365 * sigma * exp(-vOld * FoRT));
  iNaKjunc = Inakjuncfactor * 1 * Fjunc * inakbar * fnak * kO /
             (1 + pow(kmnaip / najI, 4.0)) / (kO + kmko);
  iNaKsl = Inakslfactor * 1 * Fsl * inakbar * fnak * kO /
           (1 + pow(kmnaip / naslI, 4.0)) / (kO + kmko);
  iNak = iNaKjunc + iNaKsl;
}
void GpbAtrialdiffINa::updatenaI() {
  double dNabjdt, dNabsldt;
  double konNa = 0.0001;
  double koffNa = 0.001;
  double BmaxNaj = 7.561;
  double BmaxNasl = 1.65;
  double dnajI, dnaslI, dnaI;
  double J_najuncsl = 1.83128E-8;
  double J_naslmyo = 1.63863E-6;

  // sodium buffers
  dNabjdt = (konNa * najI * (BmaxNaj - Nabj) - koffNa * Nabj);
  dNabsldt = (konNa * naslI * (BmaxNasl - Nabsl) - koffNa * Nabsl);

  // sodium concentration

  dnajI = dt * (-iNatotjunc * (ACap / (Vjunc * FDAY)) +
                (J_najuncsl / Vjunc) * (naslI - najI) - dNabjdt);
  dnaslI = dt * (-iNatotsl * (ACap / (Vsl * FDAY)) +
                 (J_najuncsl / Vsl) * (najI - naslI) +
                 (J_naslmyo / Vsl) * (naI - naslI) - dNabsldt);
  dnaI = dt * ((J_naslmyo / Vmyo) * (naslI - naI));

  Nabj = Nabj + dt * dNabjdt;
  Nabsl = Nabsl + dt * dNabsldt;
  najI = najI + dnajI;
  naslI = naslI + dnaslI;
  naI = naI + dnaI;
}

void GpbAtrialdiffINa::updateIna() {
    double Enajunc = RGAS * TEMP / FDAY * log(naO / najI);
    double Enasl = RGAS * TEMP / FDAY * log(naO / naslI);
  if(NovelFlag) {
      std::array<double, 5> a;
      std::array<double, 5> b;


      double mss = 1.0 / (1.0 + exp((-(vOld + Nov.mss_shift)) / Nov.mss_tau));
      double tm = Nov.baseline/15+ Nov.tm_max/(exp((vOld-Nov.tm_shift)/Nov.tm_tau1)
                                              + exp(-(vOld-Nov.tm_shift)/Nov.tm_tau2));

      a[0] = mss/ tm;
      b[0] = (1-mss)/ tm;

      double hfss = 1.0 / (1 + exp((vOld - Nov.hfss_shift) / (Nov.hss_tau)));
      double thf = Nov.baseline/10 + Nov.thf_max/(exp((vOld-Nov.thf_shift)/Nov.thf_tau2)
                                              + exp(-(vOld-Nov.thf_shift)/Nov.thf_tau1));

      a[1] = hfss/ thf;
      b[1] = (1-hfss)/thf;

      double hsss = hfss;
      double ths = Nov.baseline +  Nov.ths_max/(exp((vOld-Nov.ths_shift)/Nov.ths_tau2)
                                              + exp(-(vOld-Nov.ths_shift)/Nov.ths_tau1));

      a[2] = hsss/ths;
      b[2] = (1-hsss)/ ths;

      double jss = 1.0 / (1 + exp((vOld - Nov.jss_shift) / (Nov.jss_tau)));//hss;
      double tj = Nov.baseline + (Nov.tj_max-Nov.baseline)/(exp((vOld-Nov.tj_shift)/Nov.tj_tau1)
                                              + exp(-(vOld-Nov.tj_shift)/Nov.tj_tau2));

      a[3] = jss/ tj;
      b[3] = (1-jss)/ tj;

      a[4] = 0;
      b[4] = 0;

      //m
      NovG.m = mss - (mss - NovG.m) * exp(-dt / tm);

    //hf jf if i2f
    double dhf = NovG.jf*a[1] - NovG.hf*b[1];
    double djf = a[3]*NovG.i1f+b[1]*NovG.hf - NovG.jf*(b[3]+a[1]);
    double di1f = b[3]*NovG.jf+a[4]*NovG.i2f - NovG.i1f*(a[3]+b[4]);
    double di2f = b[4]*NovG.i1f - NovG.i2f*a[4];

    //hs js is i2s
    double dhs = NovG.js*a[2] - NovG.hs*b[2];
    double djs = a[3]*NovG.i1s+b[2]*NovG.hs - NovG.js*(b[3]+a[2]);
    double di1s = b[3]*NovG.js+a[4]*NovG.i2s - NovG.i1s*(a[3]+b[4]);
    double di2s = b[4]*NovG.i1s - NovG.i2s*a[4];

    NovG.hf  += dt*dhf;
    NovG.jf  += dt*djf;
    NovG.i1f  += dt*di1f;
    NovG.i2f  += dt*di2f;

    NovG.hs  += dt*dhs;
    NovG.js  += dt*djs;
    NovG.i1s  += dt*di1s;
    NovG.i2s  += dt*di2s;

    double Ahf = Nov.Ahf_mult;
    double Ahs = 1.0 - Ahf;

    double h = Ahf*NovG.hf + Ahs*NovG.hs;
    double oprob = NovG.m * NovG.m * NovG.m * h;

    iNajunc = Inajuncfactor * Fjunc * Nov.GNa * oprob * (vOld - Enajunc);
    iNasl = Inaslfactor * Fsl * Nov.GNa * oprob * (vOld - Enasl);
    iNa = iNajunc + iNasl;

  } else if(OHaraRudyFlag){
      double mss = 1.0 / (1.0 + exp((-(vOld + this->OHR.mss_shift)) / this->OHR.mss_tau));
      double tm = this->OHR.tm_max/(exp((vOld-this->OHR.tm_shift)/this->OHR.tm_tau1)
                                              + exp(-(vOld-this->OHR.tm_shift)/this->OHR.tm_tau2));

      double hss = 1.0 / (1 + exp((vOld + this->OHR.hss_shift) / this->OHR.hss_tau));
      double thf = this->OHR.thf_max/(exp((vOld-this->OHR.thf_shift)/this->OHR.thf_tau2)
                                              + exp(-(vOld-this->OHR.thf_shift)/this->OHR.thf_tau1));
      double ths = this->OHR.ths_max/(exp((vOld-this->OHR.ths_shift)/this->OHR.ths_tau2)
                                              + exp(-(vOld-this->OHR.ths_shift)/this->OHR.ths_tau1));


      double jss = hss;
      double tj = this->OHR.tj_baseline + this->OHR.tj_max/(exp((vOld-this->OHR.tj_shift)/this->OHR.tj_tau2)
                                              + exp(-(vOld-this->OHR.tj_shift)/this->OHR.tj_tau1));

      double hssp = 1.0 / (1 + exp((vOld + 89.1) / this->OHR.hssp_tau));
      double thsp = this->OHR.tssp_mult * ths;


      double tjp = this->OHR.tjp_mult * tj;

      double mLss = 1.0 / (1.0 + exp((-(vOld + 42.85)) / this->OHR.mLss_tau));
      double tmL = tm;

      double hLss = 1.0 / (1.0 + exp((vOld + 87.61) / this->OHR.hLss_tau));
      double thL = this->OHR.thL_baseline;

      double hLssp = 1.0 / (1.0 + exp((vOld + 93.81) / this->OHR.hLssp_tau));
      double thLp = this->OHR.thLp_mult * thL;

      this->OHRG.m = mss - (mss - this->OHRG.m) * exp(-dt / tm);
      this->OHRG.hf = hss - (hss - this->OHRG.hf) * exp(-dt / thf);
      this->OHRG.hs = hss - (hss - this->OHRG.hs) * exp(-dt / ths);
      this->OHRG.j = jss - (jss - this->OHRG.j) * exp(-dt / tj);

      this->OHRG.hsp = hssp - (hssp - this->OHRG.hsp) * exp(-dt / thsp);
      this->OHRG.jp = jss - (jss - this->OHRG.jp) * exp(-dt / tjp);

      this->OHRG.mL = mLss - (mLss - this->OHRG.mL) * exp(-dt / tmL);
      this->OHRG.hL = hLss - (hLss - this->OHRG.hL) * exp(-dt / thL);
      this->OHRG.hLp = hLssp - (hLssp - this->OHRG.hLp) * exp(-dt / thLp);

      double fINap = 6.666222251849878e-05;
      double fINaLp = 6.666222251849878e-05;

      double Ahf = 0.99*OHR.Ahf_mult;
      double Ahs = 1.0 - Ahf;
      double h = Ahf * OHRG.hf + Ahs * OHRG.hs;
      double hp = Ahf * OHRG.hf + Ahs *OHRG.hsp;
      double oprob = OHRG.m*OHRG.m*OHRG.m * ((1.0 - fINap) *  h * OHRG.j + fINap * hp * OHRG.jp);
      double loprob = OHRG.mL * ((1.0 - fINaLp) * OHRG.hL + fINaLp * OHRG.hLp);

      iNajunc = Inajuncfactor * Fjunc * (OHR.GNa * oprob + OHR.GNaL * loprob) * (vOld - Enajunc);
      iNasl = Inaslfactor * Fsl * (OHR.GNa * oprob + OHR.GNaL * loprob) * (vOld - Enasl);
      iNa = iNajunc + iNasl;

  }
  else {
      double gna = 23.0;  // mS/uF
      double Enajunc = RGAS * TEMP / FDAY * log(naO / najI);
      double Enasl = RGAS * TEMP / FDAY * log(naO / naslI);
      double m_inf, tau_m;
      double h_inf, alpha_h, beta_h, tau_h;
      double j_inf, alpha_j, beta_j, tau_j;

      m_inf = 1 / ((1 + exp(-(56.86 + vOld) / 9.03)) *
                   (1 + exp(-(56.86 + vOld) / 9.03)));
      tau_m = 0.1292 * exp(-((vOld + 45.79) / 15.54) * ((vOld + 45.79) / 15.54)) +
              0.06487 * exp(-((vOld - 4.823) / 51.12) * ((vOld - 4.823) / 51.12));

      if (vOld >= -40.0) {
        alpha_h = 0.0;
        beta_h = (0.77 / (0.13 * (1 + exp(-(vOld + 10.66) / 11.1))));
      } else {
        alpha_h = (0.057 * exp(-(vOld + 80) / 6.8));
        beta_h = ((2.7 * exp(0.079 * vOld) + 3.1E5 * exp(0.3485 * vOld)));
      }

      h_inf =
          1 / ((1 + exp((vOld + 71.55) / 7.43)) * (1 + exp((vOld + 71.55) / 7.43)));
      tau_h = 1.0 / (alpha_h + beta_h);

      if (vOld >= -40.0) {
        alpha_j = 0.0;
        beta_j = ((0.6 * exp(0.057 * vOld)) / (1 + exp(-0.1 * (vOld + 32))));
      } else {
        alpha_j =
            (((-2.5428E4 * exp(0.2444 * vOld) - 6.948E-6 * exp(-0.04391 * vOld)) *
              (vOld + 37.78)) /
             (1 + exp(0.311 * (vOld + 79.23))));
        beta_j = ((0.02424 * exp(-0.01052 * vOld)) /
                  (1 + exp(-0.1378 * (vOld + 40.14))));
      }

      tau_j = 1 / (alpha_j + beta_j);
      j_inf =
          1 / ((1 + exp((vOld + 71.55) / 7.43)) * (1 + exp((vOld + 71.55) / 7.43)));

      Gate.m = m_inf - (m_inf - Gate.m) * exp(-dt / tau_m);
      Gate.h = h_inf - (h_inf - Gate.h) * exp(-dt / tau_h);
      Gate.j = j_inf - (j_inf - Gate.j) * exp(-dt / tau_j);

      iNajunc = Inajuncfactor * Fjunc * gna * Gate.m * Gate.m * Gate.m * Gate.h *
                Gate.j * (vOld - Enajunc);
      iNasl = Inaslfactor * Fsl * gna * Gate.m * Gate.m * Gate.m * Gate.h * Gate.j *
              (vOld - Enasl);
      iNa = iNajunc + iNasl;
  }

}

void GpbAtrialdiffINa::updateInab() {
  double gnab = 0.597E-3;  // mS/uF

  double Enajunc = RGAS * TEMP / FDAY * log(naO / najI);
  double Enasl = RGAS * TEMP / FDAY * log(naO / naslI);

  iNabjunc = Inabjuncfactor * Fjunc * gnab * (vOld - Enajunc);
  iNabsl = Inabslfactor * Fsl * gnab * (vOld - Enasl);
  iNab = iNabjunc + iNabsl;
}

// Create map for easy retrieval of variable values.
void GpbAtrialdiffINa::makemap() {
  CellKernel::insertVar("vOld", &vOld);
  CellKernel::insertVar("t", &t);
  CellKernel::insertVar("dVdt", &dVdt);
  CellKernel::insertVar("naI", &naI);
  CellKernel::insertVar("kI", &kI);
  CellKernel::insertVar("clI", &clI);
  CellKernel::insertVar("caI", &caI);
  CellKernel::insertVar("caslI", &caslI);
  CellKernel::insertVar("cajI", &cajI);
  CellKernel::insertVar("caSr", &caSr);
  CellKernel::insertVar("najI", &najI);
  CellKernel::insertVar("naslI", &naslI);
  CellKernel::insertVar("mgI", &mgI);
  CellKernel::insertVar("CaM", &CaM);
  CellKernel::insertVar("Csqnb", &Csqnb);
  CellKernel::insertVar("iCa", &iCa);
  CellKernel::insertVar("iCab", &iCab);
  CellKernel::insertVar("iNa", &iNa);
  CellKernel::insertVar("iNak", &iNak);
  CellKernel::insertVar("iNaca", &iNaca);

  CellKernel::insertVar("Gate.m", &Gate.m);
  CellKernel::insertVar("Gate.h", &Gate.h);
  CellKernel::insertVar("Gate.j", &Gate.j);
  CellKernel::insertVar("Gate.xkr", &Gate.xkr);
  CellKernel::insertVar("Gate.xks", &Gate.xks);
  CellKernel::insertVar("Gate.xf", &Gate.xf);
  CellKernel::insertVar("Gate.yf", &Gate.yf);
  CellKernel::insertVar("Gate.xkur", &Gate.xkur);
  CellKernel::insertVar("Gate.ykur", &Gate.ykur);
  CellKernel::insertVar("Gate.d", &Gate.d);
  CellKernel::insertVar("Gate.f", &Gate.f);
  CellKernel::insertVar("Gate.f_cabj", &Gate.f_cabj);
  CellKernel::insertVar("Gate.f_cabsl", &Gate.f_cabsl);

  CellKernel::insertVar("iTos", &iTos);
  CellKernel::insertVar("iTof", &iTof);
  CellKernel::insertVar("iTo", &iTo);
  CellKernel::insertVar("iKsjunc", &iKsjunc);
  CellKernel::insertVar("iKssl", &iKssl);
  CellKernel::insertVar("iKs", &iKs);
  CellKernel::insertVar("iKr", &iKr);
  CellKernel::insertVar("iKur", &iKur);
  CellKernel::insertVar("iKpjunc", &iKpjunc);
  CellKernel::insertVar("iKpsl", &iKpsl);
  CellKernel::insertVar("iKp", &iKp);
  CellKernel::insertVar("iK1", &iK1);
  CellKernel::insertVar("iNajunc", &iNajunc);
  CellKernel::insertVar("iNasl", &iNasl);
  CellKernel::insertVar("iNabjunc", &iNabjunc);
  CellKernel::insertVar("iNabsl", &iNabsl);
  CellKernel::insertVar("iNab", &iNab);
  CellKernel::insertVar("iNa", &iNa);
  CellKernel::insertVar("iCajunc", &iCajunc);
  CellKernel::insertVar("iCasl", &iCasl);
  CellKernel::insertVar("iCa", &iCa);
  CellKernel::insertVar("iCaL", &iCaL);
  CellKernel::insertVar("iCab", &iCab);
  CellKernel::insertVar("ipCa", &ipCa);
  CellKernel::insertVar("iCak", &iCak);
  CellKernel::insertVar("iCanajunc", &iCanajunc);
  CellKernel::insertVar("iCanasl", &iCanasl);
  CellKernel::insertVar("iCana", &iCana);
  CellKernel::insertVar("iNaKjunc", &iNaKjunc);
  CellKernel::insertVar("iNaKsl", &iNaKsl);
  CellKernel::insertVar("iNak", &iNak);
  CellKernel::insertVar("iNcxjunc", &iNcxjunc);
  CellKernel::insertVar("iNcxsl", &iNcxsl);
  CellKernel::insertVar("iNaca", &iNaca);
  CellKernel::insertVar("iClcajunc", &iClcajunc);
  CellKernel::insertVar("iClcasl", &iClcasl);
  CellKernel::insertVar("iClca", &iClca);
  CellKernel::insertVar("iClbk", &iClbk);
  CellKernel::insertVar("iClcajunc", &iClcajunc);
  CellKernel::insertVar("ipCajunc", &ipCajunc);
  CellKernel::insertVar("ipCasl", &ipCasl);
  CellKernel::insertVar("ipCa", &ipCa);
  CellKernel::insertVar("iCabjunc", &iCabjunc);
  CellKernel::insertVar("iCabsl", &iCabsl);
  CellKernel::insertVar("iCab", &iCab);
  CellKernel::insertVar("Jsrleak", &Jsrleak);
  CellKernel::insertVar("Jsrcarel", &Jsrcarel);
  CellKernel::insertVar("Jserca", &Jserca);

  CellKernel::insertPar("IcajuncFactor", &Icajuncfactor);
  CellKernel::insertPar("IcaslFactor", &Icaslfactor);
  CellKernel::insertPar("IcakFactor", &Icakfactor);
  CellKernel::insertPar("IcanajuncFactor", &Icanajuncfactor);
  CellKernel::insertPar("IcanaslFactor", &Icanaslfactor);
  CellKernel::insertPar("IcabslFactor", &Icabslfactor);
  CellKernel::insertPar("IcabjuncFactor", &Icabjuncfactor);
  CellKernel::insertPar("IpcaslFactor", &Ipcaslfactor);
  CellKernel::insertPar("IpcajuncFactor", &Ipcajuncfactor);
  CellKernel::insertPar("ItoFactor", &Itofactor);
  CellKernel::insertPar("IksslFactor", &Iksslfactor);
  CellKernel::insertPar("IksjuncFactor", &Iksjuncfactor);
  CellKernel::insertPar("IkrFactor", &Ikrfactor);
  CellKernel::insertPar("Ik1Factor", &Ik1factor);
  CellKernel::insertPar("IkurFactor", &Ikurfactor);
  CellKernel::insertPar("IpkslFactor", &Ipkslfactor);
  CellKernel::insertPar("IpkjuncFactor", &Ipkjuncfactor);
  CellKernel::insertPar("InacaslFactor", &Inacaslfactor);
  CellKernel::insertPar("InacajuncFactor", &Inacajuncfactor);
  CellKernel::insertPar("InakslFactor", &Inakslfactor);
  CellKernel::insertPar("InakjuncFactor", &Inakjuncfactor);
  CellKernel::insertPar("InabslFactor", &Inabslfactor);
  CellKernel::insertPar("InabjuncFactor", &Inabjuncfactor);
  CellKernel::insertPar("InaslFactor", &Inaslfactor);
  CellKernel::insertPar("InajuncFactor", &Inajuncfactor);
  CellKernel::insertPar("JsrcarelFactor", &Jsrcarelfactor);
  CellKernel::insertPar("JsrleakFactor", &Jsrleakfactor);
  CellKernel::insertPar("JsercaFactor", &Jsercafactor);
  CellKernel::insertPar("IclcaslFactor", &Iclcaslfactor);
  CellKernel::insertPar("IclcajuncFactor", &Iclcajuncfactor);
  CellKernel::insertPar("IclbkFactor", &Iclbkfactor);
  CellKernel::insertPar("IcalFactor", &Icalfactor);
  // add potenttially needed values to pars"]=&
  CellKernel::insertPar("Ryrr", &Ryrr);
  CellKernel::insertPar("Ryro", &Ryro);
  CellKernel::insertPar("Ryri", &Ryri);
  CellKernel::insertPar("RI", &RI);
  CellKernel::insertPar("Nabj", &Nabj);
  CellKernel::insertPar("Nabsl", &Nabsl);
  CellKernel::insertPar("TnCl", &TnCl);
  CellKernel::insertPar("TnChc", &TnChc);
  CellKernel::insertPar("TnChm", &TnChm);
  CellKernel::insertPar("Myoc", &Myoc);
  CellKernel::insertPar("Myom", &Myom);
  CellKernel::insertPar("SRB", &SRB);
  CellKernel::insertPar("SLLj", &SLLj);
  CellKernel::insertPar("SLLsl", &SLLsl);
  CellKernel::insertPar("SLHj", &SLHj);
  CellKernel::insertPar("SLHsl", &SLHsl);

  //###sodium channel vars
CellKernel::insertPar("OHR.GNaFactor", &OHRF.GNaFactor);
CellKernel::insertPar("OHR.GNaLFactor", &OHRF.GNaLFactor);
CellKernel::insertPar("OHR.mss_tauFactor", &OHRF.mss_tauFactor);
CellKernel::insertPar("OHR.tm_maxFactor", &OHRF.tm_maxFactor);
CellKernel::insertPar("OHR.tm_tau1Factor", &OHRF.tm_tau1Factor);
CellKernel::insertPar("OHR.tm_shiftFactor", &OHRF.tm_shiftFactor);
CellKernel::insertPar("OHR.tm_tau2Factor", &OHRF.tm_tau2Factor);
CellKernel::insertPar("OHR.hss_tauFactor", &OHRF.hss_tauFactor);
CellKernel::insertPar("OHR.thf_maxFactor", &OHRF.thf_maxFactor);
CellKernel::insertPar("OHR.thf_tau1Factor", &OHRF.thf_tau1Factor);
CellKernel::insertPar("OHR.thf_shiftFactor", &OHRF.thf_shiftFactor);
CellKernel::insertPar("OHR.thf_tau2Factor", &OHRF.thf_tau2Factor);
CellKernel::insertPar("OHR.ths_maxFactor", &OHRF.ths_maxFactor);
CellKernel::insertPar("OHR.ths_tau1Factor", &OHRF.ths_tau1Factor);
CellKernel::insertPar("OHR.ths_shiftFactor", &OHRF.ths_shiftFactor);
CellKernel::insertPar("OHR.ths_tau2Factor", &OHRF.ths_tau2Factor);
CellKernel::insertPar("OHR.Ahf_multFactor", &OHRF.Ahf_multFactor);
CellKernel::insertPar("OHR.tj_baselineFactor", &OHRF.tj_baselineFactor);
CellKernel::insertPar("OHR.tj_maxFactor", &OHRF.tj_maxFactor);
CellKernel::insertPar("OHR.tj_tau1Factor", &OHRF.tj_tau1Factor);
CellKernel::insertPar("OHR.tj_shiftFactor", &OHRF.tj_shiftFactor);
CellKernel::insertPar("OHR.tj_tau2Factor", &OHRF.tj_tau2Factor);
CellKernel::insertPar("OHR.hssp_tauFactor", &OHRF.hssp_tauFactor);
CellKernel::insertPar("OHR.tssp_multFactor", &OHRF.tssp_multFactor);
CellKernel::insertPar("OHR.tjp_multFactor", &OHRF.tjp_multFactor);
CellKernel::insertPar("OHR.mLss_tauFactor", &OHRF.mLss_tauFactor);
CellKernel::insertPar("OHR.hLss_tauFactor", &OHRF.hLss_tauFactor);
CellKernel::insertPar("OHR.thL_baselineFactor", &OHRF.thL_baselineFactor);
CellKernel::insertPar("OHR.thLp_multFactor", &OHRF.thLp_multFactor);
CellKernel::insertPar("OHR.mss_shiftFactor", &OHRF.mss_shiftFactor);
CellKernel::insertPar("OHR.hss_shiftFactor", &OHRF.hss_shiftFactor);
CellKernel::insertPar("OHR.jss_shiftFactor", &OHRF.jss_shiftFactor);
CellKernel::insertPar("OHR.jss_tauFactor", &OHRF.jss_tauFactor);

CellKernel::insertVar("OHR.m", &OHRG.m);
CellKernel::insertVar("OHR.hf", &OHRG.hf);
CellKernel::insertVar("OHR.hs", &OHRG.hs);
CellKernel::insertVar("OHR.j", &OHRG.j);
CellKernel::insertVar("OHR.hsp", &OHRG.hsp);
CellKernel::insertVar("OHR.jp", &OHRG.jp);
CellKernel::insertVar("OHR.mL", &OHRG.mL);
CellKernel::insertVar("OHR.hL", &OHRG.hL);
CellKernel::insertVar("OHR.hLp", &OHRG.hLp);


CellKernel::insertPar("Novel.GNaFactor", &NovF.GNaFactor);
CellKernel::insertPar("Novel.baselineFactor", &NovF.baselineFactor);
CellKernel::insertPar("Novel.mss_tauFactor", &NovF.mss_tauFactor);
CellKernel::insertPar("Novel.mss_shiftFactor", &NovF.mss_shiftFactor);
CellKernel::insertPar("Novel.tm_maxFactor", &NovF.tm_maxFactor);
CellKernel::insertPar("Novel.tm_tau1Factor", &NovF.tm_tau1Factor);
CellKernel::insertPar("Novel.tm_shiftFactor", &NovF.tm_shiftFactor);
CellKernel::insertPar("Novel.tm_tau2Factor", &NovF.tm_tau2Factor);
CellKernel::insertPar("Novel.hss_tauFactor", &NovF.hss_tauFactor);
CellKernel::insertPar("Novel.hss_shiftFactor", &NovF.hss_shiftFactor);
CellKernel::insertPar("Novel.thf_maxFactor", &NovF.thf_maxFactor);
CellKernel::insertPar("Novel.thf_shiftFactor", &NovF.thf_shiftFactor);
CellKernel::insertPar("Novel.thf_tau1Factor", &NovF.thf_tau1Factor);
CellKernel::insertPar("Novel.thf_tau2Factor", &NovF.thf_tau2Factor);
CellKernel::insertPar("Novel.ths_maxFactor", &NovF.ths_maxFactor);
CellKernel::insertPar("Novel.ths_shiftFactor", &NovF.ths_shiftFactor);
CellKernel::insertPar("Novel.ths_tau1Factor", &NovF.ths_tau1Factor);
CellKernel::insertPar("Novel.ths_tau2Factor", &NovF.ths_tau2Factor);
CellKernel::insertPar("Novel.Ahf_multFactor", &NovF.Ahf_multFactor);
CellKernel::insertPar("Novel.jss_tauFactor", &NovF.jss_tauFactor);
CellKernel::insertPar("Novel.jss_shiftFactor", &NovF.jss_shiftFactor);
CellKernel::insertPar("Novel.tj_maxFactor", &NovF.tj_maxFactor);
CellKernel::insertPar("Novel.tj_shiftFactor", &NovF.tj_shiftFactor);
CellKernel::insertPar("Novel.tj_tau2Factor", &NovF.tj_tau2Factor);
CellKernel::insertPar("Novel.tj_tau1Factor", &NovF.tj_tau1Factor);

CellKernel::insertVar("Novel.m", &NovG.m);
CellKernel::insertVar("Novel.hf", &NovG.hf);
CellKernel::insertVar("Novel.jf", &NovG.jf);
CellKernel::insertVar("Novel.i1f", &NovG.i1f);
CellKernel::insertVar("Novel.i2f", &NovG.i2f);
CellKernel::insertVar("Novel.hs", &NovG.hs);
CellKernel::insertVar("Novel.js", &NovG.js);
CellKernel::insertVar("Novel.i1s", &NovG.i1s);
CellKernel::insertVar("Novel.i2s", &NovG.i2s);

}

const char *GpbAtrialdiffINa::type() const { return "Human Atrial (Modified)"; }

const char *GpbAtrialdiffINa::citation() const
{
    return "";
}

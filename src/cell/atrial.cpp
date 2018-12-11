#include "atrial.h"
using namespace LongQt;
//######################################################
// Constructor for control human
// atrial model.
//######################################################
Courtemanche98::Courtemanche98() { this->Initialize(); };
Courtemanche98::Courtemanche98(Courtemanche98 &toCopy) : Cell(toCopy) {
  this->Initialize();
  CellKernel::copyVarPar(toCopy);
}
void Courtemanche98::Initialize() {
  //##### Initialize variables ###################
  dVdt = /*dVdtmax=*/5.434230843e-10;
  t = 0.0;
  dt = dtmin = 0.005;
  dtmed = 0.005;
  dtmax = 0.005;
  dvcut = 1.0;
  vOld = /*vNew = */ -81.2;  // mV

  apTime = 0.0;

  naO = 140;  // mM
  caO = 1.8;  // mM
  kO = 5.4;   // mM

  kI = 139.0;    // mM
  naI = 11.2;    // mM
  caI = 0.0001;  // mM

  cellRadius = 0.0008;
  cellLength = 0.01;
  Rcg = 1.0;

  Vcell = 1000 * 3.14 * cellRadius * cellRadius * cellLength;
  AGeo =
      2 * 3.14 * cellRadius * cellRadius + 2 * 3.14 * cellRadius * cellLength;
  ACap = AGeo;
  Vmyo = 0.68 * Vcell;  // 13668; //uL, 13668 um3
  Vup = 0.0552 *
        Vcell;  // 1109.52; //uL,  1109.52 um3... SR compartment uptake volume
  Vrel = 0.0048 *
         Vcell;  // 96.48; //uL,  96.48 um3... SR compartment release volume
  //#### Initialize ion channel gates ####
  Gate.m = 0.0;
  Gate.h = 1.0;
  Gate.j = 1.0;
  Gate.u = 0.0;
  Gate.w = 1.0;
  Gate.v = 1.0;
  Gate.oa = 0.0;
  Gate.oi = 1.0;
  Gate.ua = 0.0;
  Gate.ui = 1.0;
  Gate.xs = 0.0;
  Gate.xr = 0.0;
  Gate.d = 0.0;
  Gate.f = 1.0;
  Gate.fCa = 1.0;

  Fn = -0.01044130462;
  caCmdn = 0.002036188292;
  caCsqn = 6.497904428;
  caRel = 1.484346568;
  caTrpn = 0.01176732932;
  caUp = 1.484344899;

  iCab = -0.2396118956;
  iCal = -0.001903483692;
  iK1 = 0.2617007416;
  iKr = 5.07987127e-06;
  iKs = 0.0002497697852;
  iKur = 3.357162725e-09;
  iNa = -2.608313929e-05;
  iNab = -0.1003509547;
  iNaca = -0.09765287842;
  iNak = 0.1313352625;
  iRel = 7.356241441e-223;
  iTot = -5.208120935e-09;
  iTr = -9.268178221e-09;
  iUp = 0.0004947771704;
  iUp_leak = 0.0004947816331;
  ipCa = 0.04622879375;
  iTo = 2.563942345e-05;

  iUp_max = 0.005;  // mM/ms
  caRel = 1.49;

  KmCmdn = 0.00238;  // mM
  KmTrpn = 0.0005;   // mM
  KmCsqn = 0.8;      // mM

  Cmdn_m = 0.05;  // mM, calmodulin concentration in myoplasm
  Trpn_m = 0.07;  // mM
  Csqn_m = 10;    // mM
  this->makemap();
}
Courtemanche98 *Courtemanche98::clone() { return new Courtemanche98(*this); }
//######################################################
// Destructor for control human
// atrial model.
//#####################################################
Courtemanche98::~Courtemanche98(){};

// L-type Ca current
void Courtemanche98::updateiCal() {  // slow inward Ca2+ current, L-type

  double gCal = 0.1238;  // nS/pF
  double d_inf, tau_d;
  double f_inf, tau_f;
  double fCa_inf, tau_fCa;

  tau_d = (1 - exp((-vOld - 10) / 6.24)) /
          (0.035 * (vOld + 10) * (1 + exp((-vOld - 10) / 6.24)));
  d_inf = 1 / (1 + exp((-vOld - 10) / 8.0));
  Gate.d = d_inf - (d_inf - Gate.d) * exp(-dt / tau_d);  // gating variable d
  tau_f = 9.0 /
          (0.0197 * exp(-(0.0337 * 0.0337) * (vOld + 10) * (vOld + 10)) + 0.02);
  f_inf = 1 / (1 + exp((vOld + 28) / 6.9));
  Gate.f = f_inf - (f_inf - Gate.f) * exp(-dt / tau_f);  // gating variable f
  tau_fCa = 2.0;
  fCa_inf = 1 / (1 + (caI / 0.00035));
  Gate.fCa = fCa_inf -
             (fCa_inf - Gate.fCa) * exp(-dt / tau_fCa);  // gating variable fCa

  iCal = gCal * Gate.d * Gate.f * Gate.fCa * (vOld - 65.0);
}

// Background Ca current
void Courtemanche98::updateiCab() {  // background calcium current

  double gCab = 0.00113;  // nS/pF

  double ECa = 0.5 * RGAS * TEMP * log(caO / caI) / FDAY;
  iCab = gCab * (vOld - ECa);
}

// Sarcolemmal Ca pump
void Courtemanche98::updateipCa() {  // Calcium pump current

  double ipCam = 0.275;  // pA/pF

  ipCa = ipCam * caI / (0.0005 + caI);
}

// Slow delayed rectifier K current
void Courtemanche98::updateiKs() {
  double gKs = 0.129;  // nS/pF

  double alpha_xs, beta_xs, tau_xs, xs_inf;
  double EK = RGAS * TEMP * log(kO / kI) / FDAY;

  alpha_xs = 4E-5 * (vOld - 19.9) / (1 - exp((-vOld + 19.9) / 17));

  beta_xs = 3.5E-5 * (vOld - 19.9) / (exp((vOld - 19.9) / 9) - 1);

  tau_xs = 0.5 / (alpha_xs + beta_xs);

  xs_inf = pow((1 + exp((-vOld + 19.9) / 12.7)), -0.5);

  Gate.xs =
      xs_inf - (xs_inf - Gate.xs) * exp(-dt / tau_xs);  // gating variable xs

  iKs = gKs * Gate.xs * Gate.xs * (vOld - EK);
}

// Rapid delayed rectifier K current
void Courtemanche98::updateiKr() {  // Rapid delayed outward rectifier K+
                                    // current

  double gKr = 0.0294;  // nS/pF

  double alpha_xr, beta_xr, tau_xr, xr_inf;
  double EK = RGAS * TEMP * log(kO / kI) / FDAY;

  alpha_xr = 0.0003 * (vOld + 14.1) / (1 - exp((-vOld - 14.1) / 5));

  beta_xr = 7.3898E-5 * (vOld - 3.3328) / (exp((vOld - 3.3328) / 5.1237) - 1);

  tau_xr = 1 / (alpha_xr + beta_xr);

  xr_inf = 1 / (1 + exp((-vOld - 14.1) / 6.5));

  Gate.xr =
      xr_inf - (xr_inf - Gate.xr) * exp(-dt / tau_xr);  // gating variable xr

  iKr = (gKr * Gate.xr * (vOld - EK)) / (1 + exp((vOld + 15) / 22.4));
}
// Ultrarapid Rectifier K current
void Courtemanche98::updateiKur() {
  double gKur;
  double KQ_10 = 3;  // scaling factor
  double alpha_ua, beta_ua, tau_ua, ua_inf;
  double alpha_ui, beta_ui, tau_ui, ui_inf;

  double EK = RGAS * TEMP * log(kO / kI) / FDAY;

  gKur = 0.005 + (0.05 / (1 + exp(-(vOld - 15) / 13)));
  alpha_ua = 0.65 / (exp((-vOld - 10) / 8.5) + exp(-(vOld - 30) / 59.0));
  beta_ua = 0.65 / (2.5 + exp((vOld + 82) / 17.0));
  tau_ua = 1 / ((alpha_ua + beta_ua) * KQ_10);
  ua_inf = 1 / (1 + exp((-vOld - 30.3) / 9.6));
  Gate.ua =
      ua_inf - (ua_inf - Gate.ua) * exp(-dt / tau_ua);  // gating variable ua
  alpha_ui = 1 / (21 + exp((-vOld + 185) / 28.0));
  beta_ui = exp((vOld - 158) / 16.0);
  tau_ui = 1 / ((alpha_ui + beta_ui) * KQ_10);
  ui_inf = 1 / (1 + exp((vOld - 99.45) / 27.48));
  Gate.ui =
      ui_inf - (ui_inf - Gate.ui) * exp(-dt / tau_ui);  // gating variable ui

  iKur = gKur * Gate.ua * Gate.ua * Gate.ua * Gate.ui * (vOld - EK);
}

// Time-independent K current
void Courtemanche98::updateiK1() {  // inwardly rectifying K+ current

  double gK1 = 0.09;  // nS/pF

  double EK = RGAS * TEMP * log(kO / kI) / FDAY;

  iK1 = (gK1 * (vOld - EK)) / (1 + exp(0.07 * (vOld + 80.0)));
}

// Fast Na current
void Courtemanche98::updateiNa() {
  double gNa = 7.8;  // nS/pF

  double alpha_m, beta_m, tau_m, m_inf;
  double alpha_h, beta_h, tau_h, h_inf;
  double alpha_j, beta_j, tau_j, j_inf;

  double ENa = RGAS * TEMP * log(naO / naI) / FDAY;

  if ((vOld > -47.13) || (vOld < -47.13)) {
    alpha_m = 0.32 * (vOld + 47.13) / (1 - exp(-0.1 * (vOld + 47.13)));
  } else {
    alpha_m = 3.2;
  }

  beta_m = 0.08 * exp(-vOld / 11.0);

  tau_m = 1 / (alpha_m + beta_m);
  m_inf = alpha_m * tau_m;
  Gate.m = m_inf - (m_inf - Gate.m) * exp(-dt / tau_m);

  //########################################################
  if (vOld < -40) {  //-40
    alpha_h = 0.135 * exp((-vOld - 80) / 6.8);
  } else {
    alpha_h = 0;
  }

  //########################################################
  if (vOld < -40) {
    beta_h = 3.56 * exp(0.079 * vOld) + 3.1E5 * exp(0.35 * vOld);
  } else {
    beta_h = 1 / (0.13 * (1 + exp((-vOld - 10.66) / 11.1)));
  }

  tau_h = 1 / (alpha_h + beta_h);
  h_inf = alpha_h * tau_h;
  Gate.h = h_inf - (h_inf - Gate.h) * exp(-dt / tau_h);

  if (vOld < -40) {
    alpha_j = (-127140 * exp(0.2444 * vOld) - 3.474E-5 * exp(-0.04391 * vOld)) *
              ((vOld + 37.78) / (1 + exp(0.311 * (vOld + 79.23))));
  } else {
    alpha_j = 0;
  }
  if (vOld < -40) {
    beta_j =
        0.1212 * exp(-0.01052 * vOld) / (1 + exp(-0.1378 * (vOld + 40.14)));
  } else {
    beta_j = 0.3 * exp(-2.535E-7 * vOld) / (1 + exp(-0.1 * (vOld + 32)));
  }

  tau_j = 1 / (alpha_j + beta_j);
  j_inf = alpha_j * tau_j;
  Gate.j = j_inf - (j_inf - Gate.j) * exp(-dt / tau_j);

  iNa = gNa * Gate.m * Gate.m * Gate.m * Gate.h * Gate.j * (vOld - ENa);
}

// Background Na Current
void Courtemanche98::updateiNab() {
  double gNab = 0.000674;  // nS/pF
  double ENa = RGAS * TEMP * log(naO / naI) / FDAY;

  iNab = gNab * (vOld - ENa);
}

// Na-Ca exchanger
void Courtemanche98::updateiNaca() {
  double iNaca_max = 1600;  // pA/pF
  double gam = 0.35;        // voltage dependent param. for iNaca
  double KmNa = 87.5;       // mM
  double KmCa = 1.38;       // mM
  double ksat = 0.1;        // saturation factor for iNaca

  iNaca =
      iNaca_max *
      (exp(gam * vOld * FDAY / (RGAS * TEMP)) * naI * naI * naI * caO -
       exp((gam - 1) * vOld * FDAY / (RGAS * TEMP)) * naO * naO * naO * caI) /
      ((KmNa * KmNa * KmNa + naO * naO * naO) * (KmCa + caO) *
       (1 + ksat * exp((gam - 1) * vOld * FDAY / (RGAS * TEMP))));
}

// Na-K ATPase
void Courtemanche98::updateiNak() {  // Na+/K+ pump

  double iNak_max = 0.60;  // pA/pF
  double KmNai = 10;       // mM
  double KmKo = 1.5;       // mM
  double theta;
  double fNak;

  theta = (1.0 / 7.0) * (exp((naO / 67.3)) - 1);
  fNak = 1 / (1 + 0.1245 * exp(-0.1 * FDAY * vOld / (RGAS * TEMP)) +
              0.0365 * theta * exp(-FDAY * vOld / (RGAS * TEMP)));

  iNak = iNak_max * fNak * kO / ((1 + pow(KmNai / naI, 1.5)) * (kO + KmKo));
}

// Transient outward K current
void Courtemanche98::updateiTo() {
  double KQ_10 = 3;     // scaling factor
  double gto = 0.1652;  // nS/pF

  double alpha_oa, beta_oa, tau_oa, oa_inf;
  double alpha_oi, beta_oi, tau_oi, oi_inf;

  double EK = RGAS * TEMP * log(kO / kI) / FDAY;

  alpha_oa = 0.65 / (exp((-vOld - 10) / 8.5) + exp((-vOld + 30) / 59.0));
  beta_oa = 0.65 / (2.5 + exp((vOld + 82) / 17.0));
  tau_oa = 1 / ((alpha_oa + beta_oa) * KQ_10);
  oa_inf = 1 / (1 + exp((-vOld - 20.47) / 17.54));
  Gate.oa =
      oa_inf - (oa_inf - Gate.oa) * exp(-dt / tau_oa);  // gating variable, oa
  alpha_oi = 1 / (18.53 + exp((vOld + 113.7) / 10.95));
  beta_oi = 1 / (35.56 + exp((-vOld - 1.26) / 7.44));
  tau_oi = 1 / ((alpha_oi + beta_oi) * KQ_10);
  oi_inf = 1 / (1 + exp((vOld + 43.1) / 5.3));
  Gate.oi =
      oi_inf - (oi_inf - Gate.oi) * exp(-dt / tau_oi);  // gating variable, oi

  iTo = gto * Gate.oa * Gate.oa * Gate.oa * Gate.oi *
        (vOld - EK);  // transient outward K+ current
}

// Ca release from sarcoplasmic reticulum
void Courtemanche98::updateiRel() {
  double u_inf, tau_u;
  double v_inf, tau_v;
  double w_inf, tau_w;

  tau_u = 8.0;

  Fn = (0.9646 * iRel - 0.562 * (0.5 * iCal - 0.2 * iNaca));

  // u_inf=1/(1+exp((-Fn+3.4175E-13)/(13.67E-16)));
  u_inf = 1 / (1 + exp((-Fn + 3.4175E-1) / (13.67E-4)));
  Gate.u = u_inf - (u_inf - Gate.u) * exp(-dt / tau_u);  // gating variable u

  // tau_v=1.91+(2.09/(1+exp((-Fn+3.4175E-13)/(13.67E-16))));
  tau_v = 1.91 + (2.09 / (1 + exp((-Fn + 3.4175E-1) / (13.67E-4))));

  // v_inf=1-(1/(1+exp((-Fn+6.835E-14)/(13.67E-16))));
  v_inf = 1 - (1 / (1 + exp((-Fn + 6.835E-2) / (13.67E-4))));

  Gate.v = v_inf - (v_inf - Gate.v) * exp(-dt / tau_v);  // gating variable v

  tau_w = 6.0 * (1 - exp((-vOld + 7.9) / 5)) /
          ((1 + 0.3 * exp((-vOld + 7.9) / 5)) * (vOld - 7.9));

  w_inf = 1 - (1 / (1 + exp((-vOld + 40) / 17)));

  Gate.w = w_inf - (w_inf - Gate.w) * exp(-dt / tau_w);  // gating variable w

  iRel = 30.0 * Gate.u * Gate.u * Gate.v * Gate.w * (caRel - caI);
}

// Transfer current from NSR to JSR
// Ca transport into/out of sarcoplasmic reticulum
void Courtemanche98::updateSRFlux() {
  double tau_tr = 180;
  double Kup = 0.00092;  // mM
  double caUp_max = 15;  // mM

  iUp = iUp_max / (1 + (Kup / caI));
  iUp_leak = caUp * iUp_max / caUp_max;
  iTr = (caUp - caRel) / tau_tr;
}

// Na concentration in myoplasm
void Courtemanche98::updatenaI() {
  double dnaI;
  dnaI = dt * (-iNat * ACap / (FDAY * Vmyo));
  naI = naI + dnaI;
}

// K concentration in myoplasm
void Courtemanche98::updatekI() {
  double dkI;
  dkI = dt * (-iKt * ACap / (FDAY * Vmyo));
  kI = kI + dkI;
}

void Courtemanche98::updatecaI() {
  double dcaI, cait;
  double b, c, d;

  dcaI = -dt * ((-2 * iNaca + ipCa + iCab + iCal) * ACap / (2 * FDAY * Vmyo) +
                (iUp - iUp_leak) * Vup / Vmyo - iRel * Vrel / Vmyo);

  caCmdn = Cmdn_m * caI / (caI + KmCmdn);
  caTrpn = Trpn_m * caI / (caI + KmTrpn);

  cait = caI + caTrpn + caCmdn + dcaI;

  // Analytical solution
  b = Cmdn_m + Trpn_m - cait + KmTrpn + KmCmdn;
  c = (KmTrpn * KmCmdn) - (cait * (KmTrpn + KmCmdn)) + Trpn_m * KmCmdn +
      Cmdn_m * KmTrpn;
  d = -1 * KmTrpn * KmCmdn * cait;

  caI = (2.0 / 3.0) * sqrt(b * b - 3.0 * c) *
            cos(acos((9.0 * b * c - 2 * b * b * b - 27 * d) /
                     (2.0 * pow((b * b - 3.0 * c), 1.5))) /
                3.0) -
        b / 3.0;

  caTotal = caI + caCmdn + caTrpn;
  // caI=caI+dcaI;
}

void Courtemanche98::updatecaUp() {
  double dcaUp;

  dcaUp = dt * (iUp - iUp_leak - iTr * (Vrel / Vup));

  caUp = caUp + dcaUp;
}

void Courtemanche98::updatecaRel() {
  double dcaRel;
  double b1, c1;
  // dcaRel=dt*(iTr-iRel)/(1+((Csqn_m*KmCsqn)/((caRel+KmCsqn)*(caRel+KmCsqn))));
  dcaRel = dt * (iTr - iRel);
  b1 = Csqn_m - caCsqn - dcaRel - caRel + KmCsqn;
  c1 = KmCsqn * (caCsqn + dcaRel + caRel);
  caRel = (sqrt(b1 * b1 + 4.0 * c1) - b1) / 2.0;

  caCsqn = Csqn_m * caRel / (caRel + KmCsqn);

  // caRel=caRel+dcaRel;
}

void Courtemanche98::updateCurr() {
  updateiNa();    // Fast Na current
  updateiNak();   // Na-K pump
  updateiNaca();  // Na-Ca exchanger
  updateiCab();   // Background Ca current
  updateiNab();   // Background Na current
  updateiCal();   // L-type Ca current
  updateipCa();   // Sarcolemmal Ca pump
  updateiKs();    // Slow delayed rectifier current
  updateiKr();    // Rapid delayed rectifier current
  updateiK1();    // Time-independent K current
  updateiTo();    // Transient outward K current
  updateiKur();   // Ultrarapid delayed rectifier current

  iNat = iNa + 3 * iNak + 3 * iNaca + iNab;
  iCat = iCal + iCab + ipCa - 2 * iNaca;
  iKt = iKs + iKr + iKur + iTo + iK1 - 2 * iNak;

  iTotold = iTot;
  iTot = iNat + iCat + iKt;
};

void Courtemanche98::updateConc() {
  updateSRFlux();
  updateiRel();  // Calcium release from Sr
  updatenaI();   // [Na]i
  updatekI();    // [K]i
  updatecaI();   // [Ca]i
  updatecaUp();
  updatecaRel();
};
// External stimulus.
void Courtemanche98::externalStim(double stimval) { iTot = iTot + stimval; }
// Create map for easy retrieval of variable values.
void Courtemanche98::makemap() {
  __vars["vOld"] = &vOld;
  __vars["t"] = &t;
  __vars["dVdt"] = &dVdt;
  __vars["naI"] = &naI;
  __vars["kI"] = &kI;
  __vars["caI"] = &caI;
  __vars["caUp"] = &caUp;
  __vars["caRel"] = &caRel;
  __vars["trpn"] = &caTrpn;
  __vars["cmdn"] = &caCmdn;
  __vars["csqn"] = &caCsqn;
  __vars["iRel"] = &iRel;
  __vars["iUp"] = &iUp;
  __vars["iLeak"] = &iUp_leak;
  __vars["iTr"] = &iTr;
  __vars["Gate.d"] = &Gate.d;
  __vars["Gate.f"] = &Gate.f;
  __vars["Gate.fCa"] = &Gate.fCa;
  __vars["Gate.u"] = &Gate.u;
  __vars["iCab"] = &iCab;
  __vars["iNab"] = &iNab;
  __vars["ipCa"] = &ipCa;
  __vars["iNa"] = &iNa;
  __vars["iCal"] = &iCal;
  __vars["Gate.v"] = &Gate.v;
  __vars["Gate.w"] = &Gate.w;
  __vars["Gate.m"] = &Gate.m;
  __vars["Gate.h"] = &Gate.h;
  __vars["Gate.j"] = &Gate.j;
  __vars["iNak"] = &iNak;
  __vars["iNaca"] = &iNaca;
  __vars["iTo"] = &iTo;
  __vars["iKur"] = &iKur;
  __vars["Gate.ua"] = &Gate.ua;
  __vars["Gate.ui"] = &Gate.ui;
  __vars["Gate.oa"] = &Gate.oa;
  __vars["Gate.oi"] = &Gate.oi;
  __vars["iKs"] = &iKs;
  __vars["Gate.xs"] = &Gate.xs;
  __vars["iKr"] = &iKr;
  __vars["Gate.xr"] = &Gate.xr;
  __vars["iK1"] = &iK1;

  __vars["iCat"] = &iCat;
  __vars["iNat"] = &iNat;
  __vars["iKt"] = &iKt;
  // vars["iTot"] = &iTot;
}

const char *Courtemanche98::type() const {
  return "Human Atrial (Courtemanche 1998)";
}

#include "br04.h"

using namespace LongQt;
Br04::Br04() { this->Initialize(); };
Br04::Br04(Br04 &toCopy) : Cell(toCopy) {
  this->Initialize();
  CellKernel::copyVarPar(toCopy);
}
void Br04::Initialize() {
  //##### Initialize variables from base class ###################
  RGAS = 8314.4;
  TEMP = 298.0;
  FDAY = 96485.0;
  dVdt = /*dVdtmax=*/-7.293176907E-7;
  t = 0.0;
  dt = 0.0005;
  vOld = /* vNew =  */ -88.66157024;

  apTime = 0.0;

  cellRadius = 0.0011;
  cellLength = 0.01;

  ACap = 1.534E-4;  // cm2
  Vmyo = 25.84E-6;  // uL
  this->makemap();
}
Br04 *Br04::clone() { return new Br04(*this); }
//######################################################
// Destructor for control canine epicardial
// ventricular model.
//#####################################################
Br04::~Br04(){};

void Br04::updateIlca() {
  double dO, dC2, dC3, dC4, dI1, dI2, dI3;
  double alpha, beta, gamma;
  double Kpcf, Kpcb;
  double Kpc_max, Kpc_half;
  double diCal;
  double condfact = 1;
  double gcal = 0.1729;
  double Eca = 63.0;

  Kpc_max = 0.11662;  // 0.23324; //ms-1
  Kpc_half = 0.01;    // 0.02; //mM
  Kpcb = 0.0005;
  Kpcf = 2.5;  // 13.0*(1-exp(-1*(vOld-14.5)*(vOld-14.5)/100.0));
  alpha = 0.4 * exp((vOld + 15.0) / 15.0);
  beta = 0.13 * exp((-vOld - 15.0) / 18.0);
  gamma = (Kpc_max * caSs) / (Kpc_half + caSs);

  dO = dt * (alpha * C4 - 4 * beta * O + Kpcb * I1 - gamma * O +
             0.001 * (alpha * I2 - Kpcf * O));
  dC2 = dt * (4 * alpha * C1 - beta * C2 + 2 * beta * C3 - 3 * alpha * C2);
  dC3 = dt * (3 * alpha * C2 - 2 * beta * C3 + 3 * beta * C4 - 2 * alpha * C3);
  dC4 = dt * (2 * alpha * C3 - 3 * beta * C4 + 4 * beta * O - alpha * C4 +
              0.01 * (4 * Kpcb * beta * I1 - alpha * gamma * C4) +
              0.002 * (4 * beta * I2 - Kpcf * C4) + 4 * beta * Kpcb * I3 -
              gamma * Kpcf * C4);
  dI1 = dt * (gamma * O - Kpcb * I1 + 0.001 * (alpha * I3 - Kpcf * I1) +
              0.01 * (alpha * gamma * C4 - 4 * beta * Kpcb * I1));
  dI2 = dt * (0.001 * (Kpcf * O - alpha * I2) + Kpcb * I3 - gamma * I2 +
              0.002 * (Kpcf * C4 - 4 * beta * I2));
  dI3 = dt * (0.001 * (Kpcf * I1 - alpha * I3) + gamma * I2 - Kpcb * I3 +
              gamma * Kpcf * C4 - 4 * beta * Kpcb * I3);

  O = O + dO;
  C2 = C2 + dC2;
  C3 = C3 + dC3;
  C4 = C4 + dC4;
  I1 = I1 + dI1;
  I2 = I2 + dI2;
  I3 = I3 + dI3;
  C1 = 1 - O - C2 - C3 - C4 - I1 - I2 - I3;

  if (opts & ISO) condfact = 2.429;

  iCal = IcalFactor * condfact * gcal * O * (vOld - Eca);
};

void Br04::updateIcab() {
  double ECan;
  double gcab = 0.000325;  // 0.000165; 0.000367; My change 0.000325;

  ECan = RGAS * TEMP / (2.0 * FDAY) * log(caO / caI);

  iCab = IcabFactor * gcab * (vOld - ECan);
};

void Br04::updateIpca() {
  double ipca_max = 0.5;  // 0.17; 1.0; my change 0.5;
  double kmpca = 0.0005;

  iPca = ipca_max * caI * caI / ((kmpca * kmpca) + (caI * caI));
};

void Br04::updateIto_f() {
  double alpha_a, beta_a, alpha_i, beta_i;
  double taua, ainf;
  double taui, iinf;
  double EK;
  double gkto_f = 0.3362;  // septum = 0.0798

  EK = RGAS * TEMP / FDAY * log(kO / kI);

  alpha_a = 0.18064 * exp(0.03577 * (vOld + 30.0));
  beta_a = 0.3956 * exp(-0.06237 * (vOld + 30.0));
  alpha_i =
      2.2094E-5 * exp(-0.1429 * vOld) / (5.6002E-4 * exp(-0.1429 * vOld) + 1);
  beta_i = 0.1138 * exp(0.1429 * vOld) / (6.14926 * exp(0.1429 * vOld) + 1);

  taua = 1 / (alpha_a + beta_a);
  ainf = alpha_a / (alpha_a + beta_a);
  taui = 1 / (alpha_i + beta_i);
  iinf = alpha_i / (alpha_i + beta_i);

  gate.a_to_f = ainf - (ainf - gate.a_to_f) * exp(-dt / taua);
  gate.i_to_f = iinf - (iinf - gate.i_to_f) * exp(-dt / taui);

  iTo_f = ItofFactor * gkto_f * gate.a_to_f * gate.a_to_f * gate.a_to_f *
          gate.i_to_f * (vOld - EK);  // gKto_f_apex or septum???? TJH
};

void Br04::updateIto_s() {
  double tau_ta_s, tau_ti_s;
  double a_ss, i_ss;
  double da_to_s, di_to_s;
  double EK;
  double gkto_s = 0.0;  // septum = 0.0629;
  EK = RGAS * TEMP / FDAY * log(kO / kI);

  tau_ta_s = 0.493 * exp(-0.0629 * vOld) + 2.058;
  tau_ti_s = 270.0 + (1050.0 / (1 + exp((vOld + 45.2) / 5.7)));

  a_ss = 1 / (1 + exp((-vOld - 22.5) / 7.7));
  i_ss = 1 / (1 + exp((vOld + 45.2) / 5.7));

  gate.a_to_s = a_ss - (a_ss - gate.a_to_s) * exp(-dt / tau_ta_s);
  gate.i_to_s = i_ss - (i_ss - gate.i_to_s) * exp(-dt / tau_ti_s);

  iTo_s = ItosFactor * gkto_s * gate.a_to_s * gate.i_to_s *
          (vOld - EK);  // gKto_s_apex or septum??? TJH
};

void Br04::updateIk1() {
  double EK;

  EK = RGAS * TEMP / FDAY * log(kO / kI);

  iK1 = Ik1Factor * 0.2938 * (kO / (kO + 0.210)) *
        ((vOld - EK) / (1 + exp(0.0896 * (vOld - EK))));
};

void Br04::updateIks() {
  double alpha_n, beta_n;
  double nksinf, taunks;
  double EK;
  double condfact = 1.0;
  double gks = 0.00575;

  EK = RGAS * TEMP / FDAY * log(kO / kI);

  alpha_n = 0.00000481333 * (vOld + 26.5) / (1 - exp(-0.128 * (vOld + 26.5)));
  beta_n = 0.0000953333 * exp(-0.038 * (vOld + 26.5));
  taunks = 1 / (alpha_n + beta_n);
  nksinf = alpha_n / (alpha_n + beta_n);
  gate.nks = nksinf - (nksinf - gate.nks) * exp(-dt / taunks);

  if (opts & ISO) condfact = 1.8;

  iKs = IksFactor * condfact * gks * gate.nks * gate.nks * (vOld - EK);
};

void Br04::updateIkur() {
  double tau_aur, tau_iur;
  double a_ss, i_ss;
  double EK;
  double gkur = 0.2056;  // septum = 0.0975;
  EK = RGAS * TEMP / FDAY * log(kO / kI);

  tau_aur = 0.493 * exp(-0.0629 * vOld) + 2.058;
  tau_iur = 1200.0 - (170.0 / (1 + exp((vOld + 45.2) / 5.7)));

  a_ss = 1 / (1 + exp((-vOld - 22.5) / 7.7));
  i_ss = 1 / (1 + exp((vOld + 45.2) / 5.7));

  gate.a_ur = a_ss - (a_ss - gate.a_ur) * exp(-dt / tau_aur);
  gate.i_ur = i_ss - (i_ss - gate.i_ur) * exp(-dt / tau_iur);

  iKur = IkurFactor * gkur * gate.a_ur * gate.i_ur *
         (vOld - EK);  // apex vs. septum??? TJH
};

void Br04::updateIkss() {
  double a_ss, tau_Kss;
  double EK;
  double gkss = 0.0491;  // septum = 0.0324

  EK = RGAS * TEMP / FDAY * log(kO / kI);

  tau_Kss = 39.3 * exp(-0.08628 * vOld) + 13.17;
  a_ss = 1 / (1 + exp((-vOld - 22.5) / 7.7));

  gate.a_Kss = a_ss - (a_ss - gate.a_Kss) * exp(-dt / tau_Kss);

  iKss = IkssFactor * gkss * gate.a_Kss * (vOld - EK);  // apex vs. septum???
                                                        // TJH
};

void Br04::updateIkr() {
  double alpha_a0, beta_a0, alpha_a1, beta_a1, alpha_i, beta_i;
  double dCK1, dCK2, dOK, dIK;

  double kb = 0.036778;
  double kf = 0.023761;
  double EK;

  double gkr = 0.078;

  EK = RGAS * TEMP / FDAY * log(kO / kI);

  alpha_a0 = 0.022348 * exp(0.01176 * vOld);
  beta_a0 = 0.047002 * exp(-0.0631 * vOld);
  alpha_a1 = 0.013733 * exp(0.038198 * vOld);
  beta_a1 = 0.0000689 * exp(-0.04178 * vOld);
  alpha_i = 0.090821 * exp(0.023391 * (vOld + 5.0));
  beta_i = 0.006497 * exp(-0.03268 * (vOld + 5.0));

  dCK1 = dt * (alpha_a0 * CK0 - beta_a0 * CK1 + kb * CK2 - kf * CK1);
  dCK2 = dt * (kf * CK1 - kb * CK2 + beta_a1 * OK - alpha_a1 * CK2);
  dOK = dt * (alpha_a1 * CK2 - beta_a1 * OK + beta_i * IK - alpha_i * OK);
  dIK = dt * (alpha_i * OK - beta_i * IK);

  CK1 = CK1 + dCK1;
  CK2 = CK2 + dCK2;
  OK = OK + dOK;
  IK = IK + dIK;
  CK0 = 1 - CK1 - CK2 - OK - IK;

  iKr = IkrFactor * OK * gkr *
        (vOld - RGAS * TEMP / FDAY *
                    log((0.98 * kO + 0.02 * naO) / (0.98 * kI + 0.02 * naI)));
};

void Br04::updateItrek() {
  // TREK-1 with more rectification
  double gk = ItrekFactor * .03011;
  double EK = RGAS * TEMP / FDAY * log(kO / kI);

  double aa = 1 / (1 + exp(-(vOld - 65) / 52));  // from Kim J Physiol 1992

  iTrek = gk * aa * (vOld - EK);  // apex vs. septum??? TJH
}

void Br04::updateIna() {
  double alpha_Na11, alpha_Na12, alpha_Na13;
  double alpha_Na2, alpha_Na3, alpha_Na4, alpha_Na5;
  double beta_Na11, beta_Na12, beta_Na13;
  double beta_Na2, beta_Na3, beta_Na4, beta_Na5;
  double dCNa1, dCNa2, dIFNa, dI1Na, dI2Na, dICNa2, dICNa3, dONa;
  double ENa;

  double gna = 13.0;
  alpha_Na11 = 3.802 / (0.088655 * exp(-0.05882 * vOld) +
                        0.19669 * exp(-0.006667 * vOld));
  alpha_Na12 = 3.802 / (0.086934 * exp(-0.06667 * vOld) +
                        0.226198 * exp(-0.006667 * vOld));
  alpha_Na13 = 3.802 / (0.083386 * exp(-0.08333 * vOld) +
                        0.245868 * exp(-0.006667 * vOld));

  beta_Na11 = 0.169487 * exp(-0.04926 * vOld);
  beta_Na12 = 0.226211 * exp(-0.04926 * vOld);
  beta_Na13 = 0.318329 * exp(-0.04926 * vOld);

  alpha_Na2 = 1.0 / (0.123641 * exp(-0.06024 * vOld) + 0.393956);
  alpha_Na3 = 2.82E-7 * exp(-0.12987 * vOld);
  alpha_Na4 = alpha_Na2 / 1000.0;
  alpha_Na5 = alpha_Na2 / 95000.0;

  beta_Na3 = 0.0084 + 0.00002 * vOld;
  beta_Na2 = alpha_Na13 * alpha_Na2 * alpha_Na3 / (beta_Na13 * beta_Na3);
  beta_Na4 = alpha_Na3;
  beta_Na5 = alpha_Na3 / 50.0;

  dCNa1 = dt * (alpha_Na12 * CNa2 - beta_Na12 * CNa1 + beta_Na13 * ONa -
                alpha_Na13 * CNa1 + alpha_Na3 * IFNa - beta_Na3 * CNa1);
  dCNa2 = dt * (alpha_Na11 * CNa3 - beta_Na11 * CNa2 + beta_Na12 * CNa1 -
                alpha_Na12 * CNa2 + alpha_Na3 * ICNa2 - beta_Na3 * CNa2);
  dONa = dt * (alpha_Na13 * CNa1 - beta_Na13 * ONa + beta_Na2 * IFNa -
               alpha_Na2 * ONa);
  dIFNa = dt * (alpha_Na2 * ONa - beta_Na2 * IFNa + beta_Na3 * CNa1 -
                alpha_Na3 * IFNa + beta_Na4 * I1Na - alpha_Na4 * IFNa +
                alpha_Na12 * ICNa2 - beta_Na12 * IFNa);
  dI1Na = dt * (alpha_Na4 * IFNa - beta_Na4 * I1Na + beta_Na5 * I2Na -
                alpha_Na5 * I1Na);
  dI2Na = dt * (alpha_Na5 * I1Na - beta_Na5 * I2Na);
  dICNa2 = dt * (alpha_Na11 * ICNa3 - beta_Na11 * ICNa2 + beta_Na12 * IFNa -
                 alpha_Na12 * ICNa2 + beta_Na3 * CNa2 - alpha_Na3 * ICNa2);
  dICNa3 = dt * (beta_Na11 * ICNa2 - alpha_Na11 * ICNa3 + beta_Na3 * CNa3 -
                 alpha_Na3 * ICNa3);

  CNa1 = CNa1 + dCNa1;
  CNa2 = CNa2 + dCNa2;
  ONa = ONa + dONa;
  IFNa = IFNa + dIFNa;
  I1Na = I1Na + dI1Na;
  I2Na = I2Na + dI2Na;
  ICNa2 = ICNa2 + dICNa2;
  ICNa3 = ICNa3 + dICNa3;

  CNa3 = 1 - ONa - CNa1 - CNa2 - IFNa - I1Na - I2Na - ICNa2 - ICNa3;

  ENa =
      RGAS * TEMP / FDAY * log((0.9 * naO + 0.1 * kO) / (0.9 * naI + 0.1 * kI));
  iNa = InaFactor * gna * ONa * (vOld - ENa);
};

void Br04::updateInab() {
  double ENa;
  double gnab = 0.00182;  // 0.0026;My change 0.00182;

  ENa =
      RGAS * TEMP / FDAY * log((0.9 * naO + 0.1 * kO) / (0.9 * naI + 0.1 * kI));
  iNab = InabFactor * gnab * (vOld - ENa);
};

void Br04::updateInak() {
  double fNak;
  double sigma;
  double KmNai = 21;
  double KmKo = 1.5;
  double inakmax = 0.88;
  double condfact = 1.0;

  sigma = (1.0 / 7.0) * (exp(naO / 67.3) - 1.0);
  fNak = 1.0 / (1.0 + 0.1245 * exp(-0.1 * vOld * FDAY / RGAS / TEMP) +
                0.0365 * sigma * exp(-vOld * FDAY / (RGAS * TEMP)));

  if (opts & ISO) condfact = 1.4;

  iNak = InakFactor * condfact *
         (inakmax * fNak / (1 + pow((KmNai / naI), 1.5))) * (kO / (kO + KmKo));
}

void Br04::updateInaca() {
  double KmNa = 87.5;
  double KmCa = 1.38;
  double kNaca = 292.8;
  double Eta = 0.35;
  double ksat = 0.1;

  iNaca =
      InacaFactor * (kNaca / (KmNa * KmNa * KmNa + naO * naO * naO)) *
      (1 / (KmCa + caO)) *
      (1 / (1 + (ksat * exp((Eta - 1) * vOld * FDAY / (RGAS * TEMP))))) *
      ((exp(Eta * vOld * FDAY / (RGAS * TEMP)) * naI * naI * naI * caO) -
       (exp((Eta - 1) * vOld * FDAY / (RGAS * TEMP)) * naO * naO * naO * caI));
}

void Br04::updateIclca() {
  double OClca;
  double gclca = 10.0;
  double Ecl = -40.0;
  double kmcl = 0.01;

  OClca = 0.2 / (1 + exp((-vOld + 46.7) / 7.8));

  iClca = IclcaFactor * gclca * OClca * caI * (vOld - Ecl) / (caI + kmcl);
}

void Br04::updateIns() {
  double insnabar, inskbar;
  double pns = 0.000000175;  // LRd online uses 0.000000175
  double gamnai, gamnao, gamki, gamko;
  double kmns = 0.0012;
  gamnai = gamnao = gamki = gamko = 0.75;

  insnabar = pns * vOld * FDAY * FDAY / (RGAS * TEMP) *
             (gamnai * naI * exp(vOld * FDAY / (RGAS * TEMP)) - gamnao * naO) /
             (exp(vOld * FDAY / (RGAS * TEMP)) - 1.0);
  inskbar = pns * vOld * FDAY * FDAY / (RGAS * TEMP) *
            (gamki * kI * exp(vOld * FDAY / (RGAS * TEMP)) - gamko * kO) /
            (exp(vOld * FDAY / (RGAS * TEMP)) - 1.0);

  iNsk =
      InskFactor * inskbar / (1 + (kmns / caI) * (kmns / caI) * (kmns / caI));
  iNsna =
      InsnaFactor * insnabar / (1 + (kmns / caI) * (kmns / caI) * (kmns / caI));
}

void Br04::updateCaFlux() {
  double dPO1, dPO2, dPC2, dPRyr;
  double v1 = 1.5;            // ms-1
  double v2 = 1.74E-5;        // ms-1
  double v3 = 0.00045;        // mM/ms
  double ka_open = 0.00675;   // mM-4/ms
  double ka_close = 0.07125;  // ms-1
  double kb_open = 0.00405;   // mM-3/ms
  double kb_close = 0.965;    // ms-1
  double kc_open = 0.009;     // ms-1;
  double kc_close = 0.0008;   // ms-1;

  double kh_open = 0.00237;  // uM-1/ms
  double kh_close = 3.2E-5;  // ms-1
  double kl_open = 0.0327;   // uM-1/ms
  double kl_close = 0.0196;  // ms-1
  double ltrpnbar = 0.07;    // 0.07;mM
  double htrpnbar = 0.14;    // 0.14; mM

  double Km_up = 0.0005;
  double csqnth = 11.5;  // 10.75??
  double iupcondfact = 1.0;
  double irelcondfact = 1.0;

  dPO1 =
      dt * (pow((caSs * 1000.0), 4.0) * ka_open * PC1 + kb_close * PO2 +
            kc_close * PC2 -
            (ka_close + kc_open + pow((caSs * 1000.0), 3.0) * kb_open) * PO1);

  dPO2 = dt * (pow((caSs * 1000.0), 3.0) * PO1 * kb_open - kb_close * PO2);
  dPC2 = dt * (kc_open * PO1 - kc_close * PC2);
  if (opts & ISO) irelcondfact = 2.429;

  dPRyr = dt * (-0.04 * PRyr - 0.1 * (iCal / (irelcondfact * 7.0)) *
                                   exp(-1.0 * (vOld) / 30.0));

  PO1 = PO1 + dPO1;
  PO2 = PO2 + dPO2;
  PC2 = PC2 + dPC2;

  if (sponRelflag) dPRyr = dt * (-0.04 * PRyr);

  PRyr = PRyr + dPRyr;

  if (csqn > csqnth) {
    if (sponRelflag == 0) {
      sponRelflag = 1;
      tRel = 0.0;
      Logger::getInstance()->write("Br04: Spontaneous release at t = {}", t);
    }
    PO1 = 0.6;
    PRyr = 1.0;
  }

  tRel = tRel + dt;
  if (tRel > 100.0) sponRelflag = 0;

  PC1 = 1 - PC2 - PO1 - PO2;

  if (opts & ISO) iupcondfact = 1.2;

  J_rel = JrelFactor * v1 * (PO1 + PO2) * (caJsr - caSs) * PRyr;

  J_leak = JleakFactor * v2 * (caNsr - caI);
  J_xfer = JxferFactor * (caSs - caI) / 8.0;
  J_up =
      JupFactor * iupcondfact * (v3 * caI * caI) / (Km_up * Km_up + caI * caI);
  J_tr = JtrFactor * (caNsr - caJsr) / 20.0;

  caTrpn_low = dt * (kl_open * caI * 1000 * (ltrpnbar - caTrpn_low) -
                     kl_close * (caTrpn_low)) +
               caTrpn_low;
  caTrpn_high = dt * (kh_open * caI * 1000 * (htrpnbar - caTrpn_high) -
                      kh_close * (caTrpn_high)) +
                caTrpn_high;

  J_trpn = JtrpnFactor * (kh_open * caI * 1000 * (htrpnbar - caTrpn_high) -
                          kh_close * caTrpn_high +
                          kl_open * caI * 1000 * (ltrpnbar - caTrpn_low) -
                          kl_close * caTrpn_low);
};

void Br04::updateCai() {
  double dcai, cait, b, c, d;
  double Bi;
  double trpnbar = 0.07;
  double kmtrpn = 0.0006;
  double htrpnbar = 0.140;
  double kmhtrpn = 0.0000135;  // egta = 0.000149 from kurata et al.
  double cmdnbar = 0.05;
  double kmcmdn = 0.000238;

  Bi = 1.0 / (1.0 + cmdnbar * kmcmdn / ((kmcmdn + caI) * (kmcmdn + caI)));
  dcai = dt * Bi *
         (-(iCab - 2 * iNaca + iPca) * ACap * Cm / (Vmyo * 2.0 * FDAY) +
          J_leak - J_up + J_xfer - J_trpn);

  caI = caI + dcai;
};

void Br04::updateCaSs() {
  double Bss;
  double dcaSs;
  double b1, c1;
  double cmdnbar = 0.05;
  double kmcmdn = 0.000238;

  Bss = 1.0 / (1.0 + (cmdnbar * kmcmdn / ((kmcmdn + caSs) * (kmcmdn + caSs))));
  dcaSs = Bss * dt *
          (J_rel * Vjsr / Vss - J_xfer * Vmyo / Vss -
           (iCal * ACap * Cm / (2 * Vss * FDAY)));
  caSs = caSs + dcaSs;
};

void Br04::updateCaJsr() {
  double Bjsr;
  double dcaJsr;
  double b1, c1;
  double csqnbar = 15.0;
  double kmcsqn = 0.8;

  Bjsr = 1.0 /
         (1.0 + ((csqnbar * kmcsqn) / ((kmcsqn + caJsr) * (kmcsqn + caJsr))));

  dcaJsr = dt * Bjsr * (J_tr - J_rel);

  csqn = csqnbar * (caJsr / (caJsr + kmcsqn));

  caJsr = caJsr + dcaJsr;
};

void Br04::updateCaNsr() {
  double dcaNsr;

  dcaNsr = dt * ((J_up - J_leak) * Vmyo / Vnsr - J_tr * Vjsr / Vnsr);

  caNsr = caNsr + dcaNsr;
};

void Br04::updateKi() {
  double dkI;

  dkI = dt * (-iKt) * ACap * Cm / (Vmyo * FDAY);

  kI = kI + dkI;
};

void Br04::updateNai() {
  double dnaI;

  dnaI = dt * (-iNat) * ACap * Cm / (Vmyo * FDAY);

  naI = naI + dnaI;
};
// External stimulus.
void Br04::externalStim(double stimval) { iTot = iTot + stimval; }

void Br04::updateCurr() {
  updateIlca();                    // L-type Ca current
  updateIcab();                    // Background Ca current
  updateIpca();                    // Sarcolemmal Ca pump
  updateIto_f();                   // Transient outward K current
  updateIto_s();                   // Transient outward K current
  updateIk1();                     // Time-independent K current
  updateIks();                     // Slow delayed rectifier current
  updateIkur();                    // Slow delayed rectifier current
  updateIkss();                    // Slow delayed rectifier current
  updateIkr();                     // Rapid delayed rectifier current
  if (opts & TREK) updateItrek();  // Transient outward K current
  updateInak();                    // Na-K pump
  updateInaca();                   // Na-Ca exchanger
  updateIclca();                   // Na-Ca exchanger
  updateIna();                     // Fast Na current
  updateInab();                    // Late Na current
                                   /*    if(opts & INS)
                                           updateIns();*/    // Plateau K current

  iNat = iNa + iNab + 3 * iNak + 3 * iNaca + iNsna;
  iCait = iCab - 2 * iNaca + iPca;
  iCasst = iCal;
  iCat = iCait + iCasst;
  iKt = iTo_f + iTo_s + iK1 + iKs + iKss + iKur + iKr - 2 * iNak + iNsk + iTrek;
  iTot = iNat + iCat + iKt;
}

void Br04::updateConc() {
  updateCaFlux();  // SR Ca release
  updateCai();     // [Ca]i
  updateCaSs();    // [Ca]subspace
  updateCaJsr();   // [Ca]subspace
  updateCaNsr();   // [Ca]subspace
  updateKi();      // [K]i
  updateNai();     // [Na]i
}

double Br04::tstep(double stimt) {
  t += dt;
  // using the dynamic time step
  if (caSs > 0.08)
    dt = 0.0000005;  // 0.008
  else if (caSs > 0.07)
    dt = 0.000001;  // 0.008
  else if (caSs > 0.06)
    dt = 0.000005;  // 0.008
  else if (caSs > 0.05)
    dt = 0.00001;  // 0.008
  else if (caSs > 0.045)
    dt = 0.00001;  // 0.008
  else if (caSs > 0.04)
    dt = 0.00005;  // 0.008
  else if (caSs > 0.03)
    dt = 0.0001;  // 0.008
  else if (caSs > 0.025)
    dt = 0.00025;  // 0.008
  else if (dVdt >= 0.5)
    dt = 0.0005;  // 0.008
  else if (dVdt >= 0.1)
    dt = 0.0005;  // 0.008
  else
    dt = 0.0005;  // 0.008
  return t;
}

// Create map for easy retrieval of variable values.
void Br04::makemap() {
  __vars["vOld"] = &vOld;
  __vars["t"] = &t;
  __vars["dVdt"] = &dVdt;
  __vars["caI"] = &caI;
  __vars["caSs"] = &caSs;
  __vars["caJsr"] = &caJsr;
  __vars["caNsr"] = &caNsr;
  __vars["csqn"] = &csqn;
  __vars["naI"] = &naI;
  __vars["kI"] = &kI;
  __vars["iCal"] = &iCal;
  __vars["iCab"] = &iCab;
  __vars["iPca"] = &iPca;
  __vars["iTo_f"] = &iTo_f;
  __vars["iTo_s"] = &iTo_s;
  __vars["iK1"] = &iK1;
  __vars["iKs"] = &iKs;
  __vars["iKur"] = &iKur;
  __vars["iKss"] = &iKss;
  __vars["iKr"] = &iKr;
  __vars["iTrek"] = &iTrek;
  __vars["iNak"] = &iNak;
  __vars["iNaca"] = &iNaca;
  __vars["iClca"] = &iClca;
  __vars["iNa"] = &iNa;
  __vars["iNab"] = &iNab;
  __vars["J_rel"] = &J_rel;
  __vars["tRel"] = &tRel;
  __vars["J_up"] = &J_up;
  __vars["J_leak"] = &J_leak;
  __vars["J_tr"] = &J_tr;
  __vars["J_xfer"] = &J_xfer;
  __vars["Gate.a_to_f"] = &gate.a_to_f;
  __vars["Gate.i_to_f"] = &gate.i_to_f;
  __vars["Hate.nks"] = &gate.nks;
  __vars["Gate.a_to_s"] = &gate.a_to_s;
  __vars["Gate.i_to_s"] = &gate.i_to_s;
  __vars["Gate.a_ur"] = &gate.a_ur;
  __vars["Gate.i_ur"] = &gate.i_ur;
  __vars["Gate.a_Kss"] = &gate.a_Kss;
  __vars["Gate.i_Kss"] = &gate.i_Kss;
  __vars["PO1"] = &PO1;
  __vars["PO2"] = &PO2;
  __vars["PC1"] = &PC1;
  __vars["PC2"] = &PC2;
  __vars["PRyr"] = &PRyr;
  __vars["O"] = &O;
  __vars["OK"] = &OK;
  __vars["C2"] = &C2;
  __vars["C3"] = &C3;
  __vars["C4"] = &C4;
  __vars["I1"] = &I1;
  __vars["I2"] = &I2;
  __vars["I3"] = &I3;
  __vars["IK"] = &IK;
  __vars["CNa2"] = &CNa2;
  __vars["CNa1"] = &CNa1;
  __vars["CK1"] = &CK1;
  __vars["CK2"] = &CK2;
  __vars["ONa"] = &ONa;
  __vars["IFNa"] = &IFNa;
  __vars["I1Na"] = &I1Na;
  __vars["I2Na"] = &I2Na;
  __vars["ICNa2"] = &ICNa2;
  __vars["ICNa3"] = &ICNa3;
  __vars["caTrpn_low"] = &caTrpn_low;
  __vars["caTrpn_high"] = &caTrpn_high;

  __pars["IcalFactor"] = &IcalFactor;
  __pars["IcabFactor"] = &IcabFactor;
  __pars["ItofFactor"] = &ItofFactor;
  __pars["ItosFactor"] = &ItosFactor;
  __pars["Ik1Factor"] = &Ik1Factor;
  __pars["IksFactor"] = &IksFactor;
  __pars["IkurFactor"] = &IkurFactor;
  __pars["IkssFactor"] = &IkssFactor;
  __pars["IkrFactor"] = &IkrFactor;
  __pars["ItrekFactor"] = &ItrekFactor;
  __pars["InaFactor"] = &InaFactor;
  __pars["InabFactor"] = &InabFactor;
  __pars["InakFactor"] = &InakFactor;
  __pars["InacaFactor"] = &InacaFactor;
  __pars["IclcaFactor"] = &IclcaFactor;
  __pars["InskFactor"] = &InskFactor;
  __pars["InsnaFactor"] = &InsnaFactor;
  __pars["JrelFactor"] = &JrelFactor;
  __pars["JleakFactor"] = &JleakFactor;
  __pars["JxferFactor"] = &JxferFactor;
  __pars["JupFactor"] = &JupFactor;
  __pars["JtrFactor"] = &JtrFactor;
  __pars["JtrpnFactor"] = &JtrpnFactor;
  //    __pars["TestFactor"] = &TestFactor;
  //    __pars["TestFactor2"] = &TestFactor2;
}

const char *Br04::type() const {
  return "Mouse Ventricular (Bondarenko 2004)";
};

MAKE_OPTIONS_FUNCTIONS(Br04)
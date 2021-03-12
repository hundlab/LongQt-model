#include "Koivumaki.h"
#include <math.h>
using namespace std;
using namespace LongQt;
Koivumaki::Koivumaki() : Cell() { this->Initialize(); }
Koivumaki::~Koivumaki() {}

void Koivumaki::setup() {
  Cell::setup();
  V1 = 3.14 * cellLength * pow(1*junctWidth, 2) * 1e-6 * 0.5;
  V2 = 3.14 * cellLength * (pow(2*junctWidth, 2) - pow(1*junctWidth, 2)) * 1e-6 * 0.5;
  V3 = 3.14 * cellLength * (pow(3*junctWidth, 2) - pow(2*junctWidth, 2)) * 1e-6 * 0.5;
  V4 = 3.14 * cellLength * (pow(4*junctWidth, 2) - pow(3*junctWidth, 2)) * 1e-6 * 0.5;
  Vcyto = V1 + V2 + V3 + V4 + Vss;
  Vsr1 = 0.0225 * V1;
  Vsr2 = 0.0225 * V2;
  Vsr3 = 0.0225 * V3;
  Vsr4 = 0.0225 * V4;
}

Koivumaki::Koivumaki(Koivumaki &toCopy) : Cell(toCopy) {
  this->Initialize();
  CellKernel::copyVarPar(toCopy);
}

void Koivumaki::Initialize() {
  // variables from CellKernel
  //Cm = 50;             //uF, cell membrane capcitance
  junctRadius = 6.5;  //cm, radius of the bulk cytosol
  junctWidth = 1.625; // cm, width of the bulk cytosol
  Vss = 4.99232* 1e-5;      //uL, volume of the subspace
  vOld = -77.1494;
  cellLength = 122.051;  //cm, length of the cell
  dtmin = 0.0005;
  dtmax = 0.01;
  dtmed = 0.001;

  this->makemap();
}

Koivumaki *Koivumaki::clone() { return new Koivumaki(*this); }
// External stimulus.
// default stimval -80
void Koivumaki::externalStim(double stimval) {
  iKt = iKt + stimval;
  iTot = iTot + stimval;
  apTime = 0;
}

const char *Koivumaki::type() const {
  return "Human Atrial (Koivumaki 2011)";
}

const char *Koivumaki::citation() const
{
  return "Koivuma¨ki JT, Korhonen T, Tavi P (2011) Impact of Sarcoplasmic Reticulum\n"
         "\tCalcium Release on Calcium Dynamics and Action Potential Morphology in\n"
         "\tHuman Atrial Myocytes: A Computational Study. PLoS Comput Biol 7(1):\n"
         "\te1001067. doi:10.1371/journal.pcbi.1001067.";
};

void Koivumaki::revpots() {
  ena = (RGAS * TEMP / FDAY) * log(naO / naI);
  ek = (RGAS * TEMP / FDAY) * log(kO / kI);
  eca = (RGAS * TEMP / 2 / FDAY) * log(cao / cass);
}

// current
void Koivumaki::updateINa() {
  double ffrt = FDAY * FDAY / RGAS / TEMP; //
  //double Pna = 0.00182;         //nL/s/uF, permeability of Ina
  double Pna = 3.64 * 1e-5;         //nL/s/uF, permeability of Ina
  double mss = 1.0 / (1.0 + exp((vOld + 27.12) / -8.21));
  //double taom = 0.000042 * exp(-pow((vOld + 25.57) / 28.8, 2)) + 0.000024;
  double taom = 0.042 * exp(-pow((vOld + 25.57) / 28.8, 2)) + 0.024;
  Inam = mss - (mss - Inam) * exp(-dt / taom);
  double hss= 1.0 / (1.0 + exp((vOld + 63.6) / 5.3));
  //double taoh1 = 0.03 / (1.0 + exp((vOld + 35.1) / 3.2)) + 0.0003;
  double taoh1 = 30 / (1.0 + exp((vOld + 35.1) / 3.2)) + 0.3;
  //double taoh2 = 0.12 / (1.0 + exp((vOld + 35.1) / 3.2)) + 0.003;
  double taoh2 = 120 / (1.0 + exp((vOld + 35.1) / 3.2)) + 3;
  Inah1 = hss - (hss - Inah1) * exp(- dt/ taoh1);
  Inah2 = hss - (hss - Inah2) * exp(- dt/ taoh2);
  INa = InaFactor * (Pna * Inam * Inam * Inam * (0.9 * Inah1 + 0.1 * Inah2) * naO * vOld * ffrt * (exp((vOld - ena) * FDAY / RGAS / TEMP) - 1) / (exp(vOld * FDAY / RGAS / TEMP) - 1));
}

void Koivumaki::updateICa() {
   double ECaapp = 60;
   double gcaL = 0.5062;//25.3125; //mS/uF, maximum conductance of IcaL
   double kCa = 1e-3;
   double kCan = 2;
   double dLs = 1.0 / (1.0 + exp((vOld + 9.0) / -5.8));
   //double taodL = 0.0027 * exp(-pow((vOld + 35.0) / 30, 2)) + 0.002;
   double taodL = 2.7 * exp(-pow((vOld + 35.0) / 30, 2)) + 2;
   Icald = dLs - (dLs - Icald) * exp(-dt / taodL);
   double fLs = 1.0 / (1.0 + exp((vOld + 27.4) / 7.1));
   //double L1 = 0.986898 * exp(-pow((vOld + 30.16047) / 7.09396, 2));
   //double L2 = 0.04275 / (1.0 + exp((vOld - 51.6155) / -80.61331));
   //double L3 = 0.03576 / (1.0 + exp((vOld + 29.57272) / 13.21758)) - 0.00821;
   double L1 = 986.898 * exp(-pow((vOld + 30.16047) / 7.09396, 2));
   double L2 = 42.75 / (1.0 + exp((vOld - 51.6155) / -80.61331));
   double L3 = 35.76 / (1.0 + exp((vOld + 29.57272) / 13.21758)) - 8.21;
   double taofL1 = L1 + L2 + L3;
   //double taofL2 = 1.3323 * exp(-pow((vOld + 40) / 14.2, 2)) + 0.0626;
   double taofL2 = 1332.3 * exp(-pow((vOld + 40) / 14.2, 2)) + 62.6;
   Icalf1 = fLs - (fLs - Icalf1) * exp(-dt / taofL1);
   Icalf2 = fLs - (fLs - Icalf2) * exp(-dt / taofL2);
   double fcas = 1.0 - 1.0 / (1.0 + pow((kCa / cass), kCan));
   //double taofca = 0.002;
   double taofca = 2;
   Icalfca = fcas - (fcas - Icalfca) *exp(-dt / taofca);
   ICaL = IcalFactor * gcaL * Icald * Icalfca * Icalf1 * Icalf2 * (vOld - ECaapp);
}

void Koivumaki::updateIto() {
  double gt = 0.15;//2.75;
  double rss = 1.0 / (1.0 + exp((vOld - 1.0)/ -11));
  //double taor = 0.0035 * exp(-pow((vOld / 30), 2)) + 0.0015;
  double taor = 3.5 * exp(-pow((vOld / 30), 2)) + 1.5;
  Itr = rss - (rss - Itr) * exp(-dt / taor);
  double ss = 1.0 / (1 + exp((vOld + 40.5) / 11.5));
  //double taos = 0.025635 * exp(-pow((vOld + 52.45) / 15.8827, 2)) + 0.01414;
  double taos = 25.635 * exp(-pow((vOld + 52.45) / 15.8827, 2)) + 14.14;
  Its = ss - (ss - Its) * exp(-dt / taos);
  Ito = ItoFactor * gt * Itr * Its * (vOld - ek);
}

void Koivumaki::updateIsus() {
  double gsus = 0.055;//7.5; //nS
  double rsuss = 1.0 / (1.0 + exp((vOld + 6.0) / -8.6));
  //double taorsus = 0.009 / (1.0 + exp((vOld + 5) / 12)) + 0.0005;
  double taorsus = 9 / (1.0 + exp((vOld + 5) / 12)) + 0.5;
  Isusr = rsuss - (rsuss - Isusr) * exp(-dt / taorsus);
  double ssuss = 1.0 / (1.0 + exp((vOld + 7.5) / 10));
  //double taossus = 0.59 / (1.0 + exp((vOld + 60) / 10)) + 3.05;
  double taossus = 47 / (1.0 + exp((vOld + 60) / 10)) + 300;
  Isuss = ssuss - (ssuss - Isuss) * exp(-dt / taossus);
  Isus = IsusFactor * gsus * Isusr * Isuss * (vOld - ek);

}

void Koivumaki::updateIKs() {
  double gks = 0.02;//1; //nS
  double ns = 1.0 / (1.0 + exp((vOld - 19.9) / -12.7));
  //double taon = 0.7 + 0.4 * exp(-pow((vOld - 20) / 20, 2.0));
  double taon = 700 + 400 * exp(-pow((vOld - 20) / 20, 2.0));
  Iksn = ns - (ns - Iksn) * exp(-dt / taon);
  IKs = IksFactor * gks * Iksn * (vOld - ek);
}

void Koivumaki::updateIkr() {
  double gkr = 0.01;//0.5; //nS
  double Pi = 1.0 / (1.0 + exp((vOld + 55) / 24));
  double pas = 1.0 / (1.0 + exp((vOld + 15) / -6));
  //double taopa = 0.03118 + 0.21718 * exp(-pow((vOld + 20.1376) / 22.1996, 2.0));
  double taopa = 31.18 + 217.18 * exp(-pow((vOld + 20.1376) / 22.1996, 2.0));
  Ikrpa = pas - (pas - Ikrpa) * exp(-dt / taopa);
  IKr = IkrFactor * gkr * Ikrpa * Pi * (vOld - ek);
}

void Koivumaki::updateIK1() {
  double gIK1 = 0.0765;//3.825; //nS, conductance of Ik1
  IK1 = Ik1Factor * (gIK1 * pow( kO, 0.4457) * (vOld - ek) / (1 + exp(1.5 * (vOld - ek + 3.6)*(FDAY / RGAS / TEMP))));
}

void Koivumaki::updateIf() {
  double gf = 0.02;//1; //nS
  double ys = 1.0 / (1.0 + exp((vOld + 97.82874) / 12.48025));
  //double taoy = 1.0 / (0.00332 * exp(-vOld / 16.54103) + 23.71839 * exp(vOld/16.54103));
  double taoy = 1000 / (0.00332 * exp(-vOld / 16.54103) + 23718.39 * exp(vOld/16.54103));
  Ify = ys - (ys - Ify) * exp(-dt / taoy);
  IfNa = gf * Ify * (0.2677 * (vOld - ena));
  IfK = gf * Ify * ((1 - 0.2677) * (vOld - ek));
  If = IfFactor * (IfK + IfNa);
}

void Koivumaki::updateINaCa() {
  double gINCX = 1.68 * 1e-4;//0.0084; //uA/uF/(mmol/L)^4, conductance of Incx
  double gamma = 0.45; // postition of energy barrier controlling voltage dependence of Inaca
  double dnaca = 0.0003; //(mmol/L)^4, denominator constant for Incx
  double fCaNCX = 1;
  INaCa = InacaFactor * (gINCX * ((pow(naI, 3) * cao * exp(gamma * vOld * FDAY / RGAS / TEMP) - pow(naO, 3) * cass * fCaNCX * exp((gamma - 1) * vOld * FDAY / RGAS / TEMP)) / (1 + dnaca * (pow(naO,3) * cass * fCaNCX + pow(naI, 3) * cao))));
}

void Koivumaki::updateINaK() {
  double InaKs = 1.4165;//70.8253; //uA/uF
  double knaKK = 1; //mmol/L
  double knaKNa = 11; //mmol/L
  INaK = InakFactor * (InaKs * (kO / (kO + knaKK)) * (pow(naI, 1.5) / (pow(naI, 1.5) + pow(knaKNa, 1.5))) * (vOld + 150) / (vOld + 200));//ADD Nass??
}

void Koivumaki::updateIpCa() {
  double IcaPmax = 0.04;//2; //uA/uF, maximum pump rate of SL calcium pump
  double kcaP = 0.0005; //mmol/L
  IpCa = IpcaFactor * IcaPmax * (cass / (cass + kcaP));
}

void Koivumaki::updateInab() {
  double gInab = 0.0012;//0.060599; //nS
  INab = InabFactor * gInab * (vOld - ena);
}

void Koivumaki::updateIcab() {
  double gIcab = 0.0019;//0.0952; //mS/uF, conductance of background calcium current
  ICab = IcabFactor * gIcab * (vOld - eca);
}

// end curr

// update conc
void Koivumaki::updateSERCA() {
  double Kmf = 0.00025; //mM, SERCA half-maximal binding in cytosol
  double Kmr = 1.8; //mM, SERCA half-maximal binding in SR
  //double k4 = 7.5; //s^-1, pumping rate from SERCA to SR
  double k4 = 0.0075; //ms^-1, pumping rate from SERCA to SR
  double k3 = k4 / pow(Kmr, 2);
  double k1 = k4 * pow(10, 6);
  double k2 = k1 * pow(Kmf, 2);
  double cpumps = 0.04; //mM, concentration of SERCA
  Jsercasr1 = (-k3 * pow(casr1, 2) * (cpumps - SERCACa1) + k4 * SERCACa1) * V1 * 2;
  Jsercasr2 = (-k3 * pow(casr2, 2) * (cpumps - SERCACa2) + k4 * SERCACa2) * V2 * 2;
  Jsercasr3 = (-k3 * pow(casr3, 2) * (cpumps - SERCACa3) + k4 * SERCACa3) * V3 * 2;
  Jsercasrss = (-k3 * pow(casr4, 2) * (cpumps - SERCACass) + k4 * SERCACass) * Vss * 2;
  J1serca = (k1 * pow(caic1, 2) * (cpumps - SERCACa1) - k2 * (SERCACa1)) * V1 * 2;
  J2serca = (k1 * pow(caic2, 2) * (cpumps - SERCACa2) - k2 * (SERCACa2)) * V2 * 2;
  J3serca = (k1 * pow(caic3, 2) * (cpumps - SERCACa3) - k2 * (SERCACa3)) * V3 * 2;
  Jssserca = (k1 * pow(cass, 2) * (cpumps - SERCACass) - k2 * (SERCACass)) * Vss * 2;

  SERCACa1 += dt * 0.5 * (J1serca - Jsercasr1) / V1;
  SERCACa2 += dt * 0.5 * (J2serca - Jsercasr2) / V2;
  SERCACa3 += dt * 0.5 * (J3serca - Jsercasr3) / V3;
  SERCACass += dt * 0.5 * (Jssserca - Jsercasrss) / Vss;
}

void Koivumaki::updateRyR() {
  /*double TRyRadapt = 1; //s, RyR adaptation time constant
  double TRyRactss = 0.005; //s, RyR activation time constant
  double TRyRinactss = 0.015; //s, RyR inactivation time constant
  double vss = 625;*/
  double TRyRadapt = 1000; //ms, RyR adaptation time constant
  double TRyRactss = 5; //ms, RyR activation time constant
  double TRyRinactss = 15; //ms, RyR inactivation time constant
  double vss = 0.625;//  1/ms
  double RyRsrCa4 = 1.0 - 1.0 / (1.0 + exp((casr4 - 0.3) / 0.1));
  Jrelss = vss * RyRoss * RyRcss * RyRsrCa4 * (casr4 - cass) * Vss;
  double RyRainf4 = 0.505 - 0.427 / (1.0 + exp((cass * 1000 - 0.29) / 0.082));
  double RyRoinf4 = 1.0 - 1.0 / (1.0 + exp((1000 * cass - (RyRass + 0.22)) / 0.03));
  double RyRcinf4 = 1.0 / (1.0 + exp((1000 * cass - (RyRass + 0.02)) / 0.01));
  RyRoss = RyRoinf4 - (RyRoinf4 - RyRoss) * exp(-dt / TRyRactss);
  RyRcss = RyRcinf4 - (RyRcinf4 - RyRcss) * exp(-dt / TRyRinactss);
  RyRass = RyRainf4 - (RyRainf4 - RyRass) * exp(-dt / TRyRadapt);

  /*double TRyRact = 0.01875; //s, RyR activation time constant
  double TRyRinact = 0.0875; //s, RyR inactivation time constant
  double v = 1; //s^-1, RyR maximum flux (625 s^-1 in ss)*/
  double TRyRact = 18.75; //s, RyR activation time constant
  double TRyRinact = 87.5; //s, RyR inactivation time constant
  double v = 0.001; //s^-1, RyR maximum flux (625 s^-1 in ss)
  double RyRsrCa1 = 1.0 - 1.0 / (1.0 + exp((casr1 - 0.3) / 0.1));
  Jrel1 = v * RyRoc1 * RyRcc1 * RyRsrCa1 * (casr1 - caic1) * V1;
  double RyRainf1 = 0.505 - 0.427 / (1.0 + exp((1000 * caic1 - 0.29) / 0.082));
  double RyRoinf1 = 1.0 - 1.0 / (1.0 + exp((1000 * caic1 - (RyRac1 + 0.22)) / 0.03));
  double RyRcinf1 = 1.0 / (1.0 + exp((1000 * caic1 - (RyRac1 + 0.02)) / 0.01));
  RyRoc1 = RyRoinf1 - (RyRoinf1 - RyRoc1) * exp(-dt / TRyRact);
  RyRcc1 = RyRcinf1 - (RyRcinf1 - RyRcc1) * exp(-dt / TRyRinact);
  RyRac1 = RyRainf1 - (RyRainf1 - RyRac1) * exp(-dt / TRyRadapt);

  double RyRsrCa2 = 1.0 - 1.0 / (1 + exp((casr2 - 0.3) / 0.1));
  Jrel2 = v * RyRoc2 * RyRcc2 * RyRsrCa2 * (casr2 - caic2) * V2;
  double RyRainf2 = 0.505 - 0.427 / (1 + exp((1000 * caic2 - 0.29) / 0.082));
  double RyRoinf2 = 1.0 - 1.0 / (1.0 + exp((1000 * caic2 - (RyRac2 + 0.22)) / 0.03));
  double RyRcinf2 = 1.0 / (1.0 + exp((1000 * caic2 - (RyRac2 + 0.02)) / 0.01));
  RyRoc2 = RyRoinf2 - (RyRoinf2 - RyRoc2) * exp(-dt / TRyRact);
  RyRcc2 = RyRcinf2 - (RyRcinf2 - RyRcc2) * exp(-dt / TRyRinact);
  RyRac2 = RyRainf2 - (RyRainf2 - RyRac2) * exp(-dt / TRyRadapt);

  double RyRsrCa3 = 1.0 - 1.0 / (1.0 + exp((casr3 - 0.3) / 0.1));
  Jrel3 = v * RyRoc3 * RyRcc3 * RyRsrCa3 * (casr3 - caic3) * V3;
  double RyRainf3 = 0.505 - 0.427 / (1.0 + exp((1000 * caic3 - 0.29) / 0.082));
  double RyRoinf3 = 1.0 - 1.0 / (1.0 + exp((1000 * caic3 - (RyRac3 + 0.22)) / 0.03));
  double RyRcinf3 = 1.0 / (1.0 + exp((1000 * caic3 - (RyRac3 + 0.02)) / 0.01));
  RyRoc3 = RyRoinf3 - (RyRoinf3 - RyRoc3) * exp(-dt / TRyRact);
  RyRcc3 = RyRcinf3 - (RyRcinf3 - RyRcc3) * exp(-dt / TRyRinact);
  RyRac3 = RyRainf3 - (RyRainf3 - RyRac3) * exp(-dt / TRyRadapt);
}

void Koivumaki::updatenaI() {
  //double BNa = 0.49 * 2.31;
  //double KdBNa = 10;
  //double BNass = 1.0 / (1.0 + BNa * KdBNa / pow(Nass + KdBNa, 2));
  naI += -dt * (INa + INab + 3 * INaK + 3 * INaCa + IfNa) / (Vcyto * FDAY*1000); //CHANGE TO CYTOSOL
}

void Koivumaki::updatekI() {
  kI += -dt * (Ito + Isus + IK1 + IKs + IKr - 2 * INaK + IfK) / (Vcyto * FDAY*1000);
}

void Koivumaki::updatecasr(){
  double DcaSR = 0.044;
  double CSQN =  6.7;
  double KdCSQN = 0.8;
  //double ksrleak = 0.006;
  double ksrleak = 6e-6;//ms
  Jsrleak1 = ksrleak * (casr1 - caic1) * V1;
  Jsrleak2 = ksrleak * (casr2 - caic2) * V2;
  Jsrleak3 = ksrleak * (casr3 - caic3) * V3;
  Jsrleakss = ksrleak * (casr4 - cass) * Vss;
  double Bsr1 = 1 / (1.0 + CSQN * KdCSQN / pow(casr1 + KdCSQN, 2));
  double Jsrca1 = Jsercasr1 - Jsrleak1 - Jrel1;
  casr1 += dt * (Bsr1 * DcaSR * ((casr2 - casr1) / pow(junctWidth, 2) + (casr2 - casr1) / (2 * pow(junctWidth, 2))) + Jsrca1 / Vsr1 * Bsr1);
  double Bsr2 = 1 / (1.0 + CSQN * KdCSQN / pow(casr2 + KdCSQN, 2));
  double Jsrca2 = Jsercasr2 - Jsrleak2 - Jrel2;
  casr2 += dt * (Bsr2 * DcaSR * ((casr3 - 2 * casr2 + casr1) / pow(junctWidth, 2) + (casr3 - casr1) / (2 * 2 * pow(junctWidth, 2)))
          + Jsrca2 / Vsr2 * Bsr2);
  double Bsr3 = 1 / (1.0 + CSQN * KdCSQN / pow(casr3 + KdCSQN, 2));
  double Jsrca3 = Jsercasr3 - Jsrleak3 - Jrel3;
  casr3 += dt * (Bsr3 * DcaSR * ((casr4 - 2 * casr3 + casr2) / pow(junctWidth, 2) + (casr4 - casr2) / (2 * 3 * pow(junctWidth, 2)))
          + Jsrca3 / Vsr3 * Bsr3);
  double Bsr4 = 1 / (1.0 + CSQN * KdCSQN / pow(casr4 + KdCSQN, 2));
  double Jsrca4 = Jsercasrss - Jsrleakss - Jrelss;
  casr4 += dt * (Bsr4 * DcaSR * ((-casr4 + casr3) / pow(junctWidth, 2) + (casr4 - casr3) / (2 * 4 * pow(junctWidth, 2))) + Jsrca4 / Vsr4 * Bsr4);
}

void Koivumaki::updatecai() {
    // Ca Diffusion
  double Dca = 0.780;
  double DcaBm = 0.025;
  double Ajnj = 2 * 3.1416 * junctRadius * cellLength * 0.5; //Area accessible for diffusion between junctional and non-junction cytosol
  double xjnj = (0.2 / 2) + (junctWidth / 2); //Distance between centers of junctional and bulk4 compartments
  double Jjnj = Dca * (Ajnj / xjnj) * (cass - caic4) * pow(10, -6);
  double BCa = 0.024; //mM, Arbitrary cytosol Ca2+ buffer
  double SLlow = 165; //mM, Phospholipid concentration (low affinity sites)
  double SLhigh = 13; //mM, Phospholipid concentration (high affinity sites)
  double KdBCa = 0.00238; //mM, Dissociation constant for arbitrary cytosol Ca2+ buffer
  double KdSLlow = 1.1; //mM, Dissociation constant for low affinity phospholipid sites
  double KdSLhigh = 0.013; //mM, Dissociation constant for high affinity phospholipid sites
  double Bi1 = 1 / (1 + BCa * KdBCa / pow(caic1 + KdBCa, 2));
  double Yi1 = BCa * KdBCa / pow(caic1 + KdBCa, 2);
  double Jca1 = -J1serca + Jsrleak1 + Jrel1;
  caic1 += dt * (Bi1 * (Dca + Yi1 * DcaBm) * ((caic2 - 2 * caic1 + caic1) / pow(junctWidth, 2) + (caic2 - caic1) / (2 * 1 * pow(junctWidth, 2)))
          - 2 * Bi1 * Yi1 * DcaBm / (KdBCa + caic1) * pow((caic2 - caic1) / (2*junctWidth), 2) + Jca1 / V1 * Bi1);
  double Bi2 = 1 / (1 + BCa * KdBCa / pow(caic2 + KdBCa, 2));
  double Yi2 = BCa * KdBCa / pow(caic2 + KdBCa, 2);
  double Jca2 = -J2serca + Jsrleak2 + Jrel2;
  caic2 += dt * (Bi2 * (Dca + Yi2 * DcaBm) * ((caic3 - 2 * caic2 + caic1) / pow(junctWidth, 2) + (caic3 - caic1) / (2 * 2 * pow(junctWidth, 2)))
          - 2 * Bi2 * Yi2 * DcaBm / (KdBCa + caic2) * pow((caic3 - caic1) / (2 * junctWidth), 2) + Jca2 / V2 * Bi2);
  double Bi3 = 1 / (1 + BCa * KdBCa / pow(caic3 + KdBCa, 2));
  double Yi3 = BCa * KdBCa / pow(caic3 + KdBCa, 2);
  double Jca3 = -J3serca + Jsrleak3 + Jrel3;
  caic3 += dt * (Bi3 * (Dca + Yi3 * DcaBm) * ((caic4 - 2 * caic3 + caic2) / pow(junctWidth, 2) + (caic4 - caic2) / (2 * 3 * pow(junctWidth, 2)))
          - 2 * Bi3 * Yi3 * DcaBm / (KdBCa + caic3) * pow((caic4 - caic2) / (2 * junctWidth), 2) + Jca3 / V3 * Bi3);
  double Bi4 = 1 / (1 + BCa * KdBCa / pow(caic4 + KdBCa, 2));
  double Yi4 = BCa * KdBCa / pow(caic4 + KdBCa, 2);
  double Jca4 = Jjnj;
  caic4 += dt * (Bi4 * (Dca + Yi4 * DcaBm) * ((caic4 - 2 * caic4 + caic3) / pow(junctWidth, 2) + (caic4 - caic3) / (2 * 4 * pow(junctWidth, 2)))
          - 2 * Bi4 * Yi4 * DcaBm / (KdBCa + caic4) * pow((caic4 - caic3) / (2 * junctWidth), 2) + Jca4 / V4 * Bi4);
  double Bss = 1 / (1 + SLlow * KdSLlow / pow(cass + KdSLlow, 2) + SLhigh * KdSLhigh / pow(cass + KdSLhigh, 2)
                    + BCa * KdBCa / pow(cass + KdBCa, 2));
  double Jcass = -Jca4 + Jsrleakss - Jssserca + Jrelss;
  cass += dt * (Bss * (Jcass / Vss + (-ICaL - ICab - IpCa + 2 * INaCa) / (2 * Vss * FDAY*1000)));
}
// end Conc

void Koivumaki::updateCurr() {
  revpots();
  updateINa();
  updateICa();
  updateIto();
  updateIsus();
  updateIKs();
  updateIkr();
  updateIK1();
  updateIf();
  updateINaCa();
  updateINaK();
  updateIpCa();
  updateInab();
  updateIcab();

  // check my iNat,iCat&iKt equations
  iNat = INa + 3.0 * INaK + 3.0 * INaCa + IfNa + INab;
  //	iCart;
  iCat = IpCa + ICab - 2.0 * INaCa + ICaL;
  iKt = Ito + Isus + IKr + IKs + IK1 - 2.0 * INaK + IfK;
  iTotold = iTot;
  iTot = (INa + Ito + ICaL + IKr + IKs + IK1 + INaCa + Isus + If +
         INaK + INab + IpCa + ICab)/(Cm);
}

void Koivumaki::updateConc() {
  updateSERCA();
  updateRyR();
  updatenaI();
  updatekI();
  updatecasr();
  updatecai();
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
void Koivumaki::makemap() {
  CellKernel::insertVar("iNa", &INa);
  CellKernel::insertVar("iCaL", &ICaL);
  CellKernel::insertVar("iTo", &Ito);
  CellKernel::insertVar("iSus", &Isus);
  CellKernel::insertVar("iKs", &IKs);
  CellKernel::insertVar("iKr", &IKr);
  CellKernel::insertVar("iK1", &IK1);
  CellKernel::insertVar("iFNa", &IfNa);
  CellKernel::insertVar("iFK", &IfK);
  CellKernel::insertVar("iF", &If);
  CellKernel::insertVar("iNaca", &INaCa);
  CellKernel::insertVar("iNaK", &INaK);
  CellKernel::insertVar("iPca", &IpCa);
  CellKernel::insertVar("iNab", &INab);
  CellKernel::insertVar("iCab", &ICab);

  CellKernel::insertVar("JsercaSR1", &Jsercasr1);
  CellKernel::insertVar("JsercaSR2", &Jsercasr2);
  CellKernel::insertVar("JsercaSR3", &Jsercasr3);
  CellKernel::insertVar("JsercaSRss", &Jsercasrss);
  CellKernel::insertVar("J1serca", &J1serca);
  CellKernel::insertVar("J2serca", &J2serca);
  CellKernel::insertVar("J3serca", &J3serca);
  CellKernel::insertVar("Jssserca", &Jssserca);
  CellKernel::insertVar("Jrel1", &Jrel1);
  CellKernel::insertVar("Jrel2", &Jrel2);
  CellKernel::insertVar("Jrel3", &Jrel3);
  CellKernel::insertVar("Jrelss", &Jrelss);
  CellKernel::insertVar("JSRleak1", &Jsrleak1);
  CellKernel::insertVar("JSRleak2", &Jsrleak2);
  CellKernel::insertVar("JSRleak3", &Jsrleak3);
  CellKernel::insertVar("JSRleakss", &Jsrleakss);

  CellKernel::insertVar("vOld",&vOld);
  CellKernel::insertVar("t",&t);
  CellKernel::insertVar("iNam", &Inam);
  CellKernel::insertVar("iNah1", &Inah1);
  CellKernel::insertVar("iNah2", &Inah2);
  CellKernel::insertVar("iCaLd", &Icald);
  CellKernel::insertVar("iCaLf1", &Icalf1);
  CellKernel::insertVar("iCaLf2", &Icalf2);
  CellKernel::insertVar("iCaLfca", &Icalfca);
  CellKernel::insertVar("iTr", &Itr);
  CellKernel::insertVar("iTs", &Its);
  CellKernel::insertVar("iSusr", &Isusr);
  CellKernel::insertVar("iSuss", &Isuss);
  CellKernel::insertVar("iKsn", &Iksn);
  CellKernel::insertVar("iKrpa", &Ikrpa);
  CellKernel::insertVar("iFy", &Ify);
  CellKernel::insertVar("SERCAca1", &SERCACa1);
  CellKernel::insertVar("SERCAca2", &SERCACa2);
  CellKernel::insertVar("SERCAca3", &SERCACa3);
  CellKernel::insertVar("SERCAcass", &SERCACass);
  CellKernel::insertVar("RyRoss", &RyRoss);
  CellKernel::insertVar("RyRcss", &RyRcss);
  CellKernel::insertVar("RyRass", &RyRass);
  CellKernel::insertVar("RyRoC1", &RyRoc1);
  CellKernel::insertVar("RyRcC1", &RyRcc1);
  CellKernel::insertVar("RyRaC1", &RyRac1);
  CellKernel::insertVar("RyRoC2", &RyRoc2);
  CellKernel::insertVar("RyRcC2", &RyRcc2);
  CellKernel::insertVar("RyRaC2", &RyRac2);
  CellKernel::insertVar("RyRoC3", &RyRoc3);
  CellKernel::insertVar("RyRcC3", &RyRcc3);
  CellKernel::insertVar("RyRaC3", &RyRac3);
  CellKernel::insertVar("naI", &naI);
  CellKernel::insertVar("kI", &kI);
  CellKernel::insertVar("Cass", &cass);
  CellKernel::insertVar("CaC1", &caic1);
  CellKernel::insertVar("CaC2", &caic2);
  CellKernel::insertVar("CaC3", &caic3);
  CellKernel::insertVar("CaC4", &caic4);
  CellKernel::insertVar("CaSR1", &casr1);
  CellKernel::insertVar("CaSR2", &casr2);
  CellKernel::insertVar("CaSR3", &casr3);
  CellKernel::insertVar("CaSR4", &casr4);

  CellKernel::insertPar("naO", &naO);
  CellKernel::insertPar("cao", &cao);
  CellKernel::insertPar("kO", &kO);
  CellKernel::insertPar("Cm", &Cm);

  // AGeo
  //CellKernel::insertPar("Acap", &ACap);
  //CellKernel::insertPar("Vmyo", &Vmyo);
  //CellKernel::insertPar("Vmito", &Vmito);
  //CellKernel::insertPar("Vsr", &Vsr);
  //CellKernel::insertPar("Vnsr", &Vnsr);
  //CellKernel::insertPar("Vjsr", &Vjsr);
  CellKernel::insertPar("junctRadius", &junctRadius);
  CellKernel::insertPar("junctWidth", &junctWidth);
  CellKernel::insertPar("cellLength", &cellLength);
  CellKernel::insertPar("Vss", &Vss);
  CellKernel::insertPar("V1", &V1);
  CellKernel::insertPar("V2", &V2);
  CellKernel::insertPar("V3", &V3);
  CellKernel::insertPar("V4", &V4);
  CellKernel::insertPar("Vcyto", &Vcyto);
  CellKernel::insertPar("Vsr1", &Vsr1);
  CellKernel::insertPar("Vsr2", &Vsr2);
  CellKernel::insertPar("Vsr3", &Vsr3);
  CellKernel::insertPar("Vsr4", &Vsr4);

  // factors
  CellKernel::insertPar("InaFactor", &InaFactor);
  CellKernel::insertPar("IcalFactor", &IcalFactor);
  CellKernel::insertPar("ItoFactor", &ItoFactor);
  CellKernel::insertPar("IsusFactor", &IsusFactor);
  CellKernel::insertPar("IkrFactor", &IkrFactor);
  CellKernel::insertPar("IksFactor", &IksFactor);
  CellKernel::insertPar("Ik1Factor", &Ik1Factor);
  CellKernel::insertPar("IfFactor", &IfFactor);
  CellKernel::insertPar("InacaFactor", &InacaFactor);
  CellKernel::insertPar("InakFactor", &InakFactor);
  CellKernel::insertPar("IpcaFactor", &IpcaFactor);
  CellKernel::insertPar("InabFactor", &InabFactor);
  CellKernel::insertPar("IcabFactor", &IcabFactor);
};

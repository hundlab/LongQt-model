//#######################################################
// This header file contains class definition for
// Grandi model of human atrial myocytes
//
// Grandi, E., et al. “Human Atrial Action Potential and
// Ca2+ Model: Sinus Rhythm and Chronic Atrial Fibrillation.”
// Circulation Research, vol. 109, no. 9, 2011, pp. 1055–66,
// doi:10.1161/CIRCRESAHA.111.253955.
//#######################################################
#include "cell.h"  //parent class definitions

#ifndef GPBATRIALDIFFINA_H
#define GPBATRIALDIFFINA_H
namespace LongQt {

//####################################
// subclass of Cell
//####################################
class GpbAtrialdiffINa : public Cell {
 public:
  // constructors
  GpbAtrialdiffINa();
  GpbAtrialdiffINa(GpbAtrialdiffINa& toCopy);
  ~GpbAtrialdiffINa();

  GpbAtrialdiffINa* clone() override;

  void setup() override;

  /*########################*/
  /*    DEFINE STRUCTS	  */
  /*########################*/
  struct GateVariable {
    double m;
    double h;
    double j;
    double xkr;
    double xks;
    double xkur;
    double ykur;
    double xf;
    double yf;
    double f_cabj;
    double f_cabsl;
    double d;
    double f;
  };

  double Vsl;
  double Vjunc;
  double Vsr;

  //###### Concentrations #########
  double naO;
  double kO;
  double najI;
  double naslI;
  double cajI;
  double caslI;
  double caO;
  double caI;
  double kI;
  double caSr;
  double clO;
  double clI;
  double naI;
  double mgI;
  //####### RyR channel fractions
  double Ryrr;
  double Ryro;
  double Ryri;
  double RI;
  //###### Currents #######
  double iNajunc;
  double iNasl;
  double iNa;
  double iNabjunc;
  double iNabsl;
  double iNab;
  double iNaKjunc;
  double iNaKsl;
  double iNak;
  double iKr;
  double iKs;
  double iKsjunc;
  double iKssl;
  double iKpjunc;
  double iKpsl;
  double iKp;
  double iTos;
  double iTof;
  double iTo;
  double iK1;
  double iKur;
  double iClcajunc;
  double iClcasl;
  double iClca;
  double iClbk;
  double iCajunc;
  double iCasl;
  double iCa;
  double iCak;
  double iCanajunc;
  double iCanasl;
  double iCana;
  double iCaL;
  double iCatotjunc;
  double iCatotsl;
  double iNcxjunc;
  double iNcxsl;
  double iNaca;
  double ipCajunc;
  double ipCasl;
  double ipCa;
  double iCabjunc;
  double iCabsl;
  double iCab;
  double iNatotjunc;
  double iNatotsl;
  double iClt;

  //##### Buffers ########
  // Sodium Buffers
  double Nabj;
  double Nabsl;

  // Calcium Buffers
  double TnCl;
  double TnChc;
  double TnChm;
  double CaM;
  double Myoc;
  double Myom;
  double SRB;

  // junctional and SL Ca buffers
  double SLLj;
  double SLLsl;
  double SLHj;
  double SLHsl;

  // SR Ca buffer
  double Csqnb;
  double dCsqnb;

  //##### Fluxes ########
  double Jsrcarel;
  double Jserca;
  double Jsrleak;
  double J_cabcyto;
  double J_Cabjunc;
  double J_Cabsl;

  //##### Fractional Currents ########

  double Fjunc;
  double Fsl;
  double F_juncCaL;
  double F_slCaL;

  double Icajuncfactor;
  double Icaslfactor;
  double Icakfactor;
  double Icanajuncfactor;
  double Icanaslfactor;
  double Icabslfactor;
  double Icabjuncfactor;
  double Ipcaslfactor;
  double Ipcajuncfactor;
  double Itofactor;
  double Iksslfactor;
  double Iksjuncfactor;
  double Ikrfactor;
  double Ik1factor;
  double Ikurfactor;
  double Ipkslfactor;
  double Ipkjuncfactor;
  double Inacaslfactor;
  double Inacajuncfactor;
  double Inakslfactor;
  double Inakjuncfactor;
  double Inabslfactor;
  double Inabjuncfactor;
  double Inaslfactor;
  double Inajuncfactor;
  double Jsrcarelfactor;
  double Jsrleakfactor;
  double Jsercafactor;
  double Iclcaslfactor;
  double Iclcajuncfactor;
  double Iclbkfactor;
  double Icalfactor;

  //###Flags###
  bool OHaraRudyFlag = false;
  bool NovelFlag = false;
  bool ClancyFlag = false;

  //### Sodium channel models
  // OHaraRudy
  struct {
      double GNaFactor=0;      double GNaLFactor=0;
       double mss_tauFactor=0;double  tm_maxFactor=0; double tm_tau1Factor=0;
     double   tm_shiftFactor=0; double tm_tau2Factor=0;
     double   hss_tauFactor=0; double thf_maxFactor=0; double thf_tau1Factor=0;
     double   thf_shiftFactor=0; double thf_tau2Factor=0;
     double   ths_maxFactor=0; double ths_tau1Factor=0;
    double    ths_shiftFactor=0; double ths_tau2Factor=0;
   double     Ahf_multFactor=0;
    double    tj_baselineFactor=0;double  tj_maxFactor=0; double tj_tau1Factor=0;
   double     tj_shiftFactor=0; double tj_tau2Factor=0;
   double     hssp_tauFactor=0; double tssp_multFactor=0;double  tjp_multFactor=0;
   double     mLss_tauFactor=0;double  hLss_tauFactor=0;
  double      thL_baselineFactor=0;double  thLp_multFactor=0;
   double     mss_shiftFactor=0;double  hss_shiftFactor=0;
   double     jss_shiftFactor=0;double  jss_tauFactor=0;
  } OHRF;

  struct {
    // scaling currents 0
    double GNa;
    double GNaL;

    //m gate 2
    double mss_tau;

    double tm_max;
    double tm_tau1;
    double tm_shift;
    double tm_tau2;

    //h gate 7
    double hss_tau;

    double thf_max;
    double thf_tau1;
    double thf_shift;
    double thf_tau2;

    //12
    double ths_max;
    double ths_tau1;
    double ths_shift;
    double ths_tau2;

    double Ahf_mult;

    //j gate 17
    double tj_baseline;
    double tj_max;
    double tj_tau1;
    double tj_shift;
    double tj_tau2;

    // phosphorylated gates
    double hssp_tau;
    double tssp_mult;
    double tjp_mult;

    //late gates & late gate phosphorylation
    double mLss_tau;
    double hLss_tau;
    double hLssp_tau;

    double thL_baseline;
    double thLp_mult;

    //added later 29
    double mss_shift;
    double hss_shift;
    double jss_shift;
    double jss_tau;
  } OHR;

  struct {
    double m = 0;
    double hf = 1;
    double hs = 1;
    double j = 1;
    double hsp = 1;
    double jp = 1;
    double mL = 0;
    double hL = 1;
    double hLp = 1;
  } OHRG;

//### Novel

  struct {
   double GNaFactor=0;

   double baselineFactor=0;

   double mss_tauFactor=0; double mss_shiftFactor=0;
   double tm_maxFactor=0; double tm_tau1Factor=0;
   double tm_shiftFactor=0; double tm_tau2Factor=0;

   double hss_tauFactor=0; double hss_shiftFactor=0;
   double thf_maxFactor=0; double thf_shiftFactor=0;
   double thf_tau1Factor=0; double thf_tau2Factor=0;
   double ths_maxFactor=0; double ths_shiftFactor=0;
   double ths_tau1Factor=0; double ths_tau2Factor=0;
   double Ahf_multFactor=0;

   double jss_tauFactor=0; double jss_shiftFactor=0;
   double tj_maxFactor=0; double tj_shiftFactor=0;
   double tj_tau2Factor=0; double tj_tau1Factor=0;
  } NovF;

  struct {
// scaling currents 0
  double GNa;

//fastest tau 1
  double baseline;

//m gate 2
  double mss_tau;
  double mss_shift;

  double tm_max;
  double tm_tau1;
  double tm_shift;
  double tm_tau2;

//h gate 8
  double hss_tau;
  double hfss_shift;

  double thf_max;
  double thf_tau1;
  double thf_shift;
  double thf_tau2;

  double ths_max;
  double ths_tau1;
  double ths_shift;
  double ths_tau2;

  double Ahf_mult;

//j gate 18
  double jss_tau;
  double jss_shift;

  double tj_max;
  double tj_tau2;
  double tj_tau1;
  double tj_shift;
  } Nov;

  struct {
     double m = 0;
     double hf = 1;
     double jf = 0;
     double i1f = 0;
     double i2f = 0;
     double hs = 1;
     double js = 0;
     double i1s = 0;
     double i2s = 0;
  } NovG;

  //###Concentration updating functions ######
  void updateConc() override;
  void updatecaI();
  void updatenaI();
  //####Current updating functions #######
  /*void updateSRcurrents();*/
  void updateCurr() override;
  void updateIcal();
  void updateIcab();
  void updateIpca();
  void updateIto();
  void updateIks();
  void updateIkr();
  void updateIk1();
  void updateIkur();
  void updateIpk();
  void updateInaca();
  void updateInak();
  void updateInab();
  void updateIna();
  void updateSRFlux();
  void updatecytobuff();  // cytosolic Ca buffers
  void updateJSLbuff();   // junctional and SL Ca buffers
  void updateSRbuff();    // SR Ca buffer
  void updateIclca();
  void updateIclbk();
  //##################
  void makemap();
  const char* type() const override;
  const char* citation() const override;
  struct GateVariable Gate;

 private:
  void Initialize();
};
}  // namespace LongQt

#endif

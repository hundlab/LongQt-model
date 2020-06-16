//#######################################################
// This header file contains class definition for
// Onal model of human atrial myocytes
//
// Onal, Birce, et al. “Ca 2+ /Calmodulin Kinase II-Dependent
// Regulation of Atrial Myocyte Late Na+ Current, Ca 2+ Cycling
// and Excitability: A Mathematical Modeling Study.” American
// Journal of Physiology - Heart and Circulatory Physiology, 2017,
// p. ajpheart.00185.2017, doi:10.1152/ajpheart.00185.2017.
//#######################################################
#include "cell.h"  //parent class definitions

#ifndef GPBATRIALONAL17_H
#define GPBATRIALONAL17_H
namespace LongQt {

//####################################
// subclass of Cell
//####################################
class GpbAtrialOnal17 : public Cell {
  void Initialize();

 public:
  // constructors
  GpbAtrialOnal17();
  GpbAtrialOnal17(GpbAtrialOnal17& toCopy);
  ~GpbAtrialOnal17();
  void setup() override;
  GpbAtrialOnal17* clone() override;

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
    double ml;
    double hl;
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
  double iNal;

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

  //##### camkii variables ######
  double fBound;
  double fPhos;
  double fOx;
  double fOxP;
  double fBlock;
  double fI;
  double caMkii;

  double fiNalP;

  double RyRP;
  double RyRratio;

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
  virtual void updateIna();
  void updateSRFlux();
  void updateCamk();
  void updatecytobuff();  // cytosolic Ca buffers
  void updateJSLbuff();   // junctional and SL Ca buffers
  void updateSRbuff();    // SR Ca buffer
  void updateIclca();
  void updateIclbk();
  void updateInal();
  void updatekI();
  void updateClI();

  double Icalfactor;
  double Icabfactor;
  double Ipcafactor;
  double Itofactor;
  double Iksfactor;
  double Ikrfactor;
  double Ik1factor;
  double Ikurfactor;
  double Ipkfactor;
  double Inacafactor;
  double Inakfactor;
  double Inabfactor;
  double Inafactor;
  double Iclcafactor;
  double Iclbkfactor;
  double Inalfactor;
  double JSRcarelfactor;
  double Jsercafactor;
  double IcaMkiiFactor;
  double InalPFactor;
  double Jsrleakfactor;
  double ROSConc;

  bool isoFlag = false;
  bool s571eFlag = false;
  bool s571aFlag = false;
  bool s2814dFlag = false;
  bool s2814aFlag = false;

  // testing
  double testFactor;
  //

  void makemap();
  struct GateVariable Gate;
  const char* type() const override;
  const char* citation() const override;
};
}  // namespace LongQt

#endif

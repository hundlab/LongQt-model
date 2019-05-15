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

#ifndef GPBATRIAL_H
#define GPBATRIAL_H
namespace LongQt {

//####################################
// subclass of Cell
//####################################
class GpbAtrial : public Cell {
 public:
  // constructors
  GpbAtrial();
  GpbAtrial(GpbAtrial& toCopy);
  ~GpbAtrial();

  GpbAtrial* clone() override;

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

  //###Concentration updating functions ######
  void updateConc();
  void updatecaI();
  void updatenaI();
  //####Current updating functions #######
  /*void updateSRcurrents();*/
  void updateCurr();
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
  const char* type() const;
  struct GateVariable Gate;

 private:
  void Initialize();
};
}  // namespace LongQt

#endif

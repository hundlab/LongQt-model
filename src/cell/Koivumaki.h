/* Copyright (c) 2011 by Koivumaki et al
*                            Washington University in St. Louis.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
* 1. Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
*
* 3. Neither the names of the copyright holders nor the names of its
* contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
* IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
* TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
* USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
* THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*

* C++ Implementation of the O'Hara-Rudy dynamic (ORd) model for the
* undiseased human ventricular action potential and calcium transient
*
* The ORd model is described in the article "Simulation of the Undiseased
* Human Cardiac Ventricular Action Potential: Model Formulation and
* Experimental Validation"
* by Thomas O'Hara, Laszlo Virag, Andras Varro, and Yoram Rudy
*
* The article and supplemental materails are freely available in the
* Open Access jounal PLoS Computational Biology
* Link to Article:
* http://www.ploscompbiol.org/article/info:doi/10.1371/journal.pcbi.1002061

* Email: tom.ohara@gmail.com / rudy@wustl.edu
* Web: http://rudylab.wustl.edu
*/
#ifndef MODEL_KOIVUMAKI_H
#define MODEL_KOIVUMAKI_H

#include "cell.h"  // Parent class declaration
namespace LongQt {

class Koivumaki : public Cell {
 public:
  Koivumaki();
  Koivumaki(Koivumaki& toCopy);
  ~Koivumaki();

  void setup();
  Koivumaki* clone() override;

  void makemap();
  double Inam = 0.002775812;
  double Inah1 = 0.90391;
  double Inah2 = 0.9039673;
  double Icald = 1.060917e-5;
  double Icalf1 = 0.9988566;
  double Icalf2 = 0.9988624;
  double Icalfca = 0.9744374;
  double Itr = 9.594258e-4;
  double Its = 0.954338;
  double Isusr = 3.111703e-4;
  double Isuss = 0.9751094;
  double Iksn = 0.004109751;
  double Ikrpa = 4.189417e-5;
  double Ify = 0.05620665;
  double SERCACa1 = 0.004638565;
  double SERCACa2 = 0.004512078;
  double SERCACa3 = 0.004326409;
  double SERCACass = 0.004250445;
  double RyRoss = 3.975097e-5;
  double RyRcss = 0.9999717;
  double RyRass = 0.2455297;
  double RyRoc1 = 9.478514e-5;
  double RyRcc1 = 0.9993722;
  double RyRac1 = 0.1925362;
  double RyRoc2 = 7.765503e-5;
  double RyRcc2 = 0.9995086;
  double RyRac2 = 0.2010345;
  double RyRoc3 = 5.674947e-5;
  double RyRcc3 = 0.9995604;
  double RyRac3 = 0.2163122;
  double naI = 9.28686;
  double nass = 8.6915;
  double kI = 134.6313;
  double cass = 1.619377e-4;
  double caic1 = 1.354965e-4;
  double caic2 = 1.381421e-4;
  double caic3 = 1.442087e-4;
  double caic4 = 1.561844e-4;
  double casr1 = 0.6189225;
  double casr2 = 0.6076289;
  double casr3 = 0.5905266;
  double casr4 = 0.5738108;

  // constants
  double naO = 130.0;  // extracellular sodium in mM
  double cao = 1.8;    // extracellular calcium in mM
  double kO = 5.4;     // extracellular potassium in mM

  // AGeo
  double junctWidth;
  double Vss;
  double junctRadius;
  double V1;
  double V2;
  double V3;
  double V4;
  double Vcyto;
  double Vsr1;
  double Vsr2;
  double Vsr3;
  double Vsr4;

  // factors
  double InaFactor = 1;
  double IcalFactor = 1;
  double ItoFactor = 1;
  double IsusFactor = 1;
  double IkrFactor = 1;
  double IksFactor = 1;
  double Ik1Factor = 1;
  double IfFactor = 1;
  double InacaFactor = 1;
  double InakFactor = 1;
  double IpcaFactor = 1;
  double InabFactor = 1;
  double IcabFactor = 1;

  // introduce varaibles for reversal potentials, currents, fluxes, and CaMK
  double ena=0, ek=0, eca=0;
  double INa=0, ICaL=0, Ito=0, Isus=0, IKs=0, IKr=0, IK1=0, IfNa=0, IfK=0, If=0,
      INaCa=0, INaK=0, IpCa=0, INab=0, ICab=0, Ist =0;
  double Jsercasr1=0, Jsercasr2=0, Jsercasr3=0, Jsercasrss=0, J1serca=0,
      J2serca=0, J3serca=0, Jssserca=0, Jrel1=0, Jrel2=0, Jrel3=0, Jrelss=0,
      Jsrleak1=0, Jsrleak2=0, Jsrleak3=0, Jsrleakss =0;

  void revpots();  // compute reversal potentials
  // compute rates, gates, and currents
  void updateINa();
  void updateICa();
  void updateIto();
  void updateIsus();
  void updateIKs();
  void updateIkr();
  void updateIK1();
  void updateIf();
  void updateINaCa();
  void updateINaK();
  void updateIpCa();
  void updateInab();
  void updateIcab();
  // calculate fluxes, buffers, and concentrations
  void updatenaI();
  void updatekI();
  void updateSERCA();
  void updateRyR();
  void updatecasr();
  void updatecai();

  void updateCurr() override;
  void updateConc() override;
  void externalStim(double stimval) override;
  const char* type() const override;
  const char* citation() const override;

 private:
  void Initialize();
};
}  // namespace LongQt

#endif

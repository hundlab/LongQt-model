/* Copyright (c) 2011-2015 by Thomas O'Hara, Yoram Rudy,
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
#ifndef MODEL_OHARARUDY_H
#define MODEL_OHARARUDY_H

#include "cell.h"  // Parent class declaration
namespace LongQt {

class OHaraRudy : public Cell {
 public:
  enum cellType {
    endo = 0,
    epi = 1,
    M = 2,
  };

  OHaraRudy();
  OHaraRudy(OHaraRudy& toCopy);
  ~OHaraRudy();

  void setup();
  OHaraRudy* clone() override;

  void makemap();

  double naI = 7;
  double nass = naI;
  double kI = 145;
  double kss = kI;
  double cai = 1.0e-4;
  double cass = cai;
  double cansr = 1.2;
  double cajsr;
  double m = 0;
  double hf = 1;
  double hs = 1;
  double j = 1;
  double hsp = 1;
  double jp = 1;
  double mL = 0;
  double hL = 1;
  double hLp = 1;
  double a = 0;
  double iF = 1;
  double iS = 1;
  double ap = 0;
  double iFp = 1;
  double iSp = 1;
  double d = 0;
  double ff = 1;
  double fs = 1;
  double fcaf = 1;
  double fcas = 1;
  double jca = 1;
  double nca = 0;
  double ffp = 1;
  double fcafp = 1;
  double xrf = 0;
  double xrs = 0;
  double xs1 = 0;
  double xs2 = 0;
  double xk1 = 1;
  double Jrelnp = 0;
  double Jrelp = 0;
  double CaMKt = 0;
  // constants
  double naO = 140.0;  // extracellular sodium in mM
  double cao = 1.8;    // extracellular calcium in mM
  double kO = 5.4;     // extracellular potassium in mM

  // buffer paramaters
  double BSRmax = 0.047;
  double KmBSR = 0.00087;
  double BSLmax = 1.124;
  double KmBSL = 0.0087;
  double cmdnmax = 0.05;
  double kmcmdn = 0.00238;
  double trpnmax = 0.07;
  double kmtrpn = 0.0005;
  double csqnmax = 10.0;
  double kmcsqn = 0.8;

  // AGeo
  double Vmito;
  double Vsr;
  double Vnsr;
  double Vjsr;
  double Vss;

  // CaMK paramaters
  double aCaMK = 0.05;
  double bCaMK = 0.00068;
  double CaMKo = 0.05;
  double KmCaM = 0.0015;
  double KmCaMK = 0.15;

  // factors
  double InaFactor = 1;
  double InalFactor = 1;
  double ItoFactor = 1;
  double IcalFactor = 1;
  double IkrFactor = 1;
  double IksFactor = 1;
  double Ik1Factor = 1;
  double InacaFactor = 1;
  double InakFactor = 1;
  double InabFactor = 1;
  double IpcaFactor = 1;
  double IcabFactor = 1;

  // cell type
  cellType celltype = endo;  // endo = 0, epi = 1, M = 2

  // introduce varaibles for reversal potentials, currents, fluxes, and CaMK
  double ena, ek, eks;
  double INa, INaL, Ito, ICaL, ICaNa, ICaK, IKr, IKs, IK1, INaCa_i, INaCa_ss,
      INaCa, INaK, IKb, INab, IpCa, ICab;
  double Jrel, Jup, Jtr, Jdiff, JdiffNa, JdiffK, Jleak;
  double CaMKa, CaMKb;

  void revpots();  // compute reversal potentials
  // compute rates, gates, and currents
  void updateCaMKa();
  void updateINa();
  void updateIto();
  void updateICa();
  void updateIkr();
  void updateIKs();
  void updateIK1();
  void updateINaCa();
  void updateINaK();
  void updateIpCa();
  // calculate fluxes, buffers, and concentrations
  void updateJrel();
  void updatenaI();
  void updatekI();
  void updatecajsr();

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

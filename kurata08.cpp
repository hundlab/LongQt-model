//##########################################################
// Class definitions for rabbit sinoatrial node cell models
// (central and peripheral) published in Kurata et al. 
// Biophys J. 2008;95:951.
//
// Copyright (C) 2011 Thomas J. Hund.
//##########################################################
#include "kurata08.h"
//######################################################
// Constructor for centrol rabbit sinoatrial node
// cell model.
//######################################################
ControlSa::ControlSa()
{
    //##### Assign default parameters ##################
    dt=dtmin = 0.005;
    dtmed = 0.05;
    dtmax = 0.05;
    dvcut = 1.0;
    type = "Rabbit Sinus Node (Kurata 2008)";
    apTime = 0.0;
     
    naO = 140;
    caO = 2.0;
    kO = 5.4;
    
    cellRadius=0.0004;  //0.0004cm
    cellLength=0.007;  //0.007cm
    Rcg = 1.72204;
    
    Vcell=2.19911E-6;
    ACap=20.0E-6;
    Vmyo=0.98965E-6; //uL
    Vss=0.021936E-6;
    Vnsr= 0.0255097E-6;//uL
    Vjsr= 0.0026389E-6;//uL
    
    icalFactor = 1;
    icattFactor = 1;
    ikrFactor = 1;
    iksFactor = 1;
    itoFactor = 1;
    itrekFactor = 1;
    isusFactor = 1;
    ikachFactor = 1;
    istFactor = 1;
    inabFactor = 1;
    inakFactor = 1;
    inacaFactor = 1;
    ihFactor = 1;
    iupFactor = 1;
    irelFactor = 1;
    
    //##### Initialize variables ##################
    dVdt=dVdtmax=5.434230843e-10;
    t=0.0;
    dt = dtmin;
	vOld = vNew = -58.600291137693;

	caI = 0.000312494921;// kurata, min. diastolic Ca2+
  	caR = 0.000190747410; //ditto
  	caJsr = .296742023718; // from kurata
  	caNsr = 1.462338380106; //ditto
  	kI = 139.98414649;
  	naI = 9.438646305915;   //ditto

  	Gate.d=0.000602055134;//0.0006;    //L-type Ca-current gates
  	Gate.f=.626999773853;
  	Gate.fca=.589580408056;

 	Gate.dt=0.004571884917;   //T-type Ca-current gates
  	Gate.ft=.249637570396;

  	Gate.paf=0.349363709533;  //IKr gates
  	Gate.pas=0.629323128348;
  	Gate.pi=.852396631172;

  	Gate.n=0.054409723782;//0.0;  //IKs gate

  	Gate.q = .531447; //Ito
  	Gate.r = 0.005504894455; //Ito and Isus

  	Gate.qa = 0.426018100136;  //Ist
  	Gate.qi = 0.333330378068;

  	Gate.y = 0.067156687129;  //Ih

	csqn = 0.26032;//only for kurata buffering equations
  	cmdnI = 0.11694722;  //only for kurata buffering equations
  	cmdnR = 0.074631965653;
  	trpnCa = 0.05920629;
  	trpnMg = 0.60295511;
  	trpnMgmg = 0.350600896;

	iSt=iNab=0.0;
	iCal=iCatt=0.0;
	iKs=iKr=iTo=iSus=iKach=0.0;
	iNak=iNaca=iHna=iHk=0.0;
	iRel=iUp=iTr=iDiff=0.0;

	iCait=iCart=0.0;
    
    // add to vars map declared/defined in cell.h and cell.cpp
    vars["naI"]=&naI;
    vars["kI"]=&kI;
    vars["caI"]=&caI;
    vars["caR"]=&caR;
    vars["caJsr"]=&caJsr;
    vars["caNsr"]=&caNsr;
    vars["trpnCa"]=&trpnCa;
    vars["trpnMg"]=&trpnMg;
    vars["trpnMgmg"]=&trpnMgmg;
    vars["cmdnI"]=&cmdnI;
    vars["cmdnR"]=&cmdnR;
    vars["csqn"]=&csqn;
    vars["iRel"]=&iRel;
    vars["iUp"]=&iUp;
    vars["iTr"]=&iTr;
    vars["iDiff"]=&iDiff;
    vars["iSt"]=&iSt;
    vars["Gate.qa"]=&Gate.qa;
    vars["Gate.qi"]=&Gate.qi;
    vars["iNab"]=&iNab;
    vars["iCal"]=&iCal;
    vars["Gate.d"]=&Gate.d;
    vars["Gate.f"]=&Gate.f;
    vars["Gate.fca"]=&Gate.fca;
    vars["iCatt"]=&iCatt;
    vars["Gate.dt"]=&Gate.dt;
    vars["Gate.ft"]=&Gate.ft;
    vars["iNak"]=&iNak;
    vars["iNaca"]=&iNaca;
    vars["iTo"]=&iTo;
    vars["iTrek"]=&iTrek;
    vars["iSus"]=&iSus;
    vars["Gate.q"]=&Gate.q;
    vars["Gate.r"]=&Gate.r;
    vars["iKs"]=&iKs;
    vars["Gate.n"]=&Gate.n;
    vars["iKr"]=&iKr;
    vars["Gate.paf"]=&Gate.paf;
    vars["Gate.pas"]=&Gate.pas;
    vars["Gate.pi"]=&Gate.pi;
    vars["iKach"]=&iKach;
    vars["iH"]=&iH;
    vars["iHna"]=&iHna;
    vars["iHk"]=&iHk;
    vars["Gate.y"]=&Gate.y;
    vars["iCait"]=&iCait;
    vars["iCart"]=&iCart;
    
    pars["Vnsr"] = &Vnsr;
    pars["Vjsr"] = &Vjsr;
    pars["Vss"] = &Vss;
    pars["naO"] = &naO;
    pars["kO"] = &kO;
    pars["caO"] = &caO;
    pars["icalFactor"] = &icalFactor;
    pars["icattFactor"] = &icattFactor;
    pars["ikrFactor"] = &ikrFactor;
    pars["iksFactor"] = &iksFactor;
    pars["itoFactor"] = &itoFactor;
    pars["itrekFactor"] = &itrekFactor;
    pars["isusFactor"] = &isusFactor;
    pars["ikachFactor"] = &ikachFactor;
    pars["istFactor"] = &istFactor;
    pars["inabFactor"] = &inabFactor;
    pars["inakFactor"] = &inakFactor;
    pars["inacaFactor"] = &inacaFactor;
    pars["ihFactor"] = &ihFactor;
    pars["iupFactor"] = &iupFactor;
    pars["irelFactor"] = &irelFactor;

    
};
//############################################
//Constructor for deep copy
//############################################
ControlSa::ControlSa(const ControlSa& toCopy ) : Cell(toCopy)
{
    //##### Assign default parameters ##################
    dt=dt;
    dtmin = toCopy.dtmin;
    dtmed = toCopy.dtmed;
    dtmax = toCopy.dtmax;
    dvcut = toCopy.dvcut;
    type = "ControlSa";
    apTime = toCopy.apTime;
     
    naO = toCopy.naO;
    caO = toCopy.caO;
    kO = toCopy.kO;
    
    cellRadius= toCopy.cellRadius;  //0.0004cm
    cellLength= toCopy.cellLength;  //0.007cm
    Rcg = toCopy.Rcg;
    
    Vcell= toCopy.Vcell;
    ACap= toCopy.ACap;
    Vmyo= toCopy.Vmyo; //uL
    Vss= toCopy.Vss;
    Vnsr= toCopy.Vnsr;//uL
    Vjsr= toCopy.Vjsr;//uL
    
    icalFactor = toCopy.icalFactor;
    icattFactor = toCopy.icattFactor;
    ikrFactor = toCopy.ikrFactor;
    iksFactor = toCopy.iksFactor;
    itoFactor = toCopy.itoFactor;
    itrekFactor = toCopy.itrekFactor;
    isusFactor = toCopy.isusFactor;
    ikachFactor = toCopy.ikachFactor;
    istFactor = toCopy.istFactor;
    inabFactor = toCopy.inabFactor;
    inakFactor = toCopy.inakFactor;
    inacaFactor = toCopy.inacaFactor;
    ihFactor = toCopy.ihFactor;
    iupFactor = toCopy.iupFactor;
    irelFactor = toCopy.irelFactor;
    
    //##### Initialize variables ##################
    dVdt= toCopy.dVdt;
    dVdtmax= toCopy.dVdtmax;
    t= toCopy.t;
    dt = toCopy.dt;
    dtmin = toCopy.dtmin;
    vOld = toCopy.vOld;
    vNew = toCopy.vNew;

	caI = toCopy.caI;// kurata, min. diastolic Ca2+
  	caR = toCopy.caR; //ditto
  	caJsr = toCopy.caJsr; // from kurata
  	caNsr = toCopy.caNsr; //ditto
  	kI = toCopy.kI;
  	naI = toCopy.naI;   //ditto

  	Gate.d= toCopy.Gate.d; //L-type Ca-current gates
  	Gate.f= toCopy.Gate.f;
  	Gate.fca= toCopy.Gate.fca;

 	Gate.dt= toCopy.Gate.dt;   //T-type Ca-current gates
  	Gate.ft= toCopy.Gate.ft;
  	Gate.paf= toCopy.Gate.paf;  //IKr gates
  	Gate.pas= toCopy.Gate.pas;
  	Gate.pi= toCopy.Gate.pi;

  	Gate.n= toCopy.Gate.n;//0.0;  //IKs gate

  	Gate.q = toCopy.Gate.q; //Ito
  	Gate.r = toCopy.Gate.r; //Ito and Isus

  	Gate.qa = toCopy.Gate.qa;  //Ist
  	Gate.qi = toCopy.Gate.qi;

  	Gate.y = toCopy.Gate.y;  //Ih

	csqn = toCopy.csqn;//only for kurata buffering equations
  	cmdnI = toCopy.cmdnI;  //only for kurata buffering equations
  	cmdnR = toCopy.cmdnR;
  	trpnCa = toCopy.trpnCa;
  	trpnMg = toCopy.trpnMg;
  	trpnMgmg = toCopy.trpnMgmg;

	iSt= toCopy.iSt;
    iNab= toCopy.iNab;
	iCal= toCopy.iCal;
    iCatt= toCopy.iCatt;
	iKs= toCopy.iKs;
    iKr= toCopy.iKr;
    iTo= toCopy.iTo;
    iSus= toCopy.iSus;
    iKach= toCopy.iKach;
	iNak= toCopy.iNak;
    iNaca= toCopy.iNaca;
    iHna= toCopy.iHna;
    iHk= toCopy.iHk;
	iRel= toCopy.iRel;
    iUp= toCopy.iUp;
    iTr= toCopy.iTr;
    iDiff= toCopy.iDiff;

	iCait=iCart=0.0;
    
    // add to vars map declared/defined in cell.h and cell.cpp
    vars["naI"]=&naI;
    vars["kI"]=&kI;
    vars["caI"]=&caI;
    vars["caR"]=&caR;
    vars["caJsr"]=&caJsr;
    vars["caNsr"]=&caNsr;
    vars["trpnCa"]=&trpnCa;
    vars["trpnMg"]=&trpnMg;
    vars["trpnMgmg"]=&trpnMgmg;
    vars["cmdnI"]=&cmdnI;
    vars["cmdnR"]=&cmdnR;
    vars["csqn"]=&csqn;
    vars["iRel"]=&iRel;
    vars["iUp"]=&iUp;
    vars["iTr"]=&iTr;
    vars["iDiff"]=&iDiff;
    vars["iSt"]=&iSt;
    vars["Gate.qa"]=&Gate.qa;
    vars["Gate.qi"]=&Gate.qi;
    vars["iNab"]=&iNab;
    vars["iCal"]=&iCal;
    vars["Gate.d"]=&Gate.d;
    vars["Gate.f"]=&Gate.f;
    vars["Gate.fca"]=&Gate.fca;
    vars["iCatt"]=&iCatt;
    vars["Gate.dt"]=&Gate.dt;
    vars["Gate.ft"]=&Gate.ft;
    vars["iNak"]=&iNak;
    vars["iNaca"]=&iNaca;
    vars["iTo"]=&iTo;
    vars["iTrek"]=&iTrek;
    vars["iSus"]=&iSus;
    vars["Gate.q"]=&Gate.q;
    vars["Gate.r"]=&Gate.r;
    vars["iKs"]=&iKs;
    vars["Gate.n"]=&Gate.n;
    vars["iKr"]=&iKr;
    vars["Gate.paf"]=&Gate.paf;
    vars["Gate.pas"]=&Gate.pas;
    vars["Gate.pi"]=&Gate.pi;
    vars["iKach"]=&iKach;
    vars["iH"]=&iH;
    vars["iHna"]=&iHna;
    vars["iHk"]=&iHk;
    vars["Gate.y"]=&Gate.y;
    vars["iCait"]=&iCait;
    vars["iCart"]=&iCart;
    
    pars["Vnsr"] = &Vnsr;
    pars["Vjsr"] = &Vjsr;
    pars["Vss"] = &Vss;
    pars["naO"] = &naO;
    pars["kO"] = &kO;
    pars["caO"] = &caO;
    pars["icalFactor"] = &icalFactor;
    pars["icattFactor"] = &icattFactor;
    pars["ikrFactor"] = &ikrFactor;
    pars["iksFactor"] = &iksFactor;
    pars["itoFactor"] = &itoFactor;
    pars["itrekFactor"] = &itrekFactor;
    pars["isusFactor"] = &isusFactor;
    pars["ikachFactor"] = &ikachFactor;
    pars["istFactor"] = &istFactor;
    pars["inabFactor"] = &inabFactor;
    pars["inakFactor"] = &inakFactor;
    pars["inacaFactor"] = &inacaFactor;
    pars["ihFactor"] = &ihFactor;
    pars["iupFactor"] = &iupFactor;
    pars["irelFactor"] = &irelFactor;

    
};
//######################################################
// Destructor for sinus node model.
//#####################################################
ControlSa::~ControlSa()
{
};
//overriden deep copy funtion
ControlSa* ControlSa::clone(){
    return new ControlSa(*this);
};
//L-type Ca2+ current
void ControlSa::updateIcal()
{
        double dinf,ddinf,finf,fdinf,fcainf,taud,taudd,tauf,taufd,taufca;
        double alphad, betad;
        double alphadd, betadd;
	
        double gCal = icalFactor*0.58;  // nS/pF
        double kmfca = 0.00035; //0.00035mM
        double alphafca = 0.021; //ms-1
        double betafca = 60; //mM-1*ms-1
        double ecal = 45.0; //mV

        dinf = 1/(1+exp(-(vOld+14.1)/6.0));
        alphad = -0.02839*(vOld+35.0)/(exp(-(vOld+35.0)/2.5)-1.0)-0.0849*vOld/(exp(-vOld/4.808)-1.0);
        betad = 0.01143*(vOld-5.0)/(exp((vOld-5.0)/2.5)-1.0);
        taud = 1/(alphad + betad);

        finf = 1/(1+exp((vOld+30.0)/5.0));
        tauf = 257.1*exp(-((vOld+32.5)/13.9)*((vOld+32.5)/13.9))+44.3;

        fcainf = kmfca/(kmfca + caR);
        taufca = fcainf/alphafca;

        Gate.d=dinf-(dinf-Gate.d)*exp(-dt/taud);
        Gate.f=finf-(finf-Gate.f)*exp(-dt/tauf);
        Gate.fca=fcainf-(fcainf-Gate.fca)*exp(-dt/taufca);

        iCal = gCal*Gate.d*Gate.f*Gate.fca*(vOld-ecal);
};
//T-type Ca2+ current
void ControlSa::updateIcatt()
{
        double dtinf,ftinf,taudt,tauft;
	
        double gCatt = icattFactor*0.458;  //nS/pF
        double ecat = 45.0; //mV

        dtinf = 1/(1+exp(-(vOld+26.3)/6.0)); //6.0 in paper
        taudt = 1/(1.068*exp((vOld+26.3)/30.0)+1.068*exp(-(vOld+26.3)/30.0)); //1.068 in paper

        ftinf = 1/(1+exp((vOld+61.7)/5.6));
        tauft = 1/(0.0153*exp(-(vOld+61.7)/83.3)+0.015*exp((vOld+61.7)/15.38)); //83.3 .0153 and .015

        Gate.dt=dtinf-(dtinf-Gate.dt)*exp(-dt/taudt);
        Gate.ft=ftinf-(ftinf-Gate.ft)*exp(-dt/tauft);

        iCatt = gCatt*Gate.dt*Gate.ft*(vOld-ecat);

};
//Rapidly activating delayed rectifier K+ current
void ControlSa::updateIkr()
{
        double pafinf, pasinf, piinf, taupaf, taupas,taupi;

        double gkr = ikrFactor*0.025*pow(kO,0.59); //nS/pF
        double ek = (RGAS*TEMP/FDAY)*log(kO/kI); //mV
        double isofact;

        pafinf = pasinf = 1/(1+exp(-(vOld+23.2)/10.6));  //+23.2 or 33.2 for iso
        //Zhang et al.
        //pafinf = pasinf = 1/(1+exp(-(vOld+14.2)/10.6));  //+23.2 or 33.2 for iso
        taupaf = 0.84655354/(0.0372*exp(vOld/15.9)+0.00096*exp(-vOld/22.5));
        taupas = 0.84655354/(0.0042*exp(vOld/17.0)+0.00015*exp(-vOld/21.6));

        piinf = 1/(1+exp((vOld+28.6)/17.1));
        //Zhang et al.
        //piinf = 1/(1+exp((vOld+18.6)/10.1));
        taupi = 1/(0.1*exp(-vOld/54.645)+0.656*exp(vOld/106.157));

        Gate.paf=pafinf-(pafinf-Gate.paf)*exp(-dt/taupaf);
        Gate.pas=pasinf-(pasinf-Gate.pas)*exp(-dt/taupas);
        Gate.pi=piinf-(piinf-Gate.pi)*exp(-dt/taupi);

        //from Demir et al. AJP Heart 1999
        isofact=1.0;//0.62*(1+2.6129*(cAmp/(cAmp+9.0)))-0.025;
        iKr = gkr*isofact*(0.6*Gate.paf+0.4*Gate.pas)*Gate.pi*(vOld-ek);
};
//Slowly activating delayed rectifier K+ current
void ControlSa::updateIks()
{
        double ninf, taun;
        double alphan, betan;
	
	double gKs = iksFactor*0.0259;
        double eks = (RGAS*TEMP/FDAY)*log((kO+0.12*naO)/(kI+0.12*naI)); //mV

        alphan = 0.014/(1+exp(-(vOld-40.0)/9.0));
        betan = 0.001*exp(-vOld/45.0);

        ninf = alphan/(alphan + betan);
        taun = 1/(alphan+betan);

        Gate.n = ninf-(ninf-Gate.n)*exp(-dt/taun);

        iKs = gKs*(vOld-eks)*Gate.n*Gate.n;
};
void ControlSa::updateI4ap()
{
        double qinf,rinf,tauq,taur;
	
	double gTo = itoFactor*0.18;
	double gSus = isusFactor*0.02;
        double ek = (RGAS*TEMP/FDAY)*log(kO/kI); //mV

	 qinf = 1/(1+exp((vOld+49.0)/13.0));
        //Kurata tauq * factor in Kurata 2008.
        tauq = 0.67*(65.17/(0.57*exp(-0.08*(vOld+44.0))+0.065*exp(0.1*(vOld+45.93)))+10.1);

        rinf = 1/(1+exp(-(vOld-19.3)/15.0));
        //Kurata taur * factor in Kurata 2008.
        taur = 0.303*(21.826/(1.037*exp(0.09*(vOld+30.61))+0.369*exp(-0.12*(vOld+23.84)))+4.172);

        Gate.q = qinf-(qinf-Gate.q)*exp(-dt/tauq);
        Gate.r = rinf-(rinf-Gate.r)*exp(-dt/taur);

        iTo = gTo*Gate.q*Gate.r*(vOld-ek);
        iSus = gSus*Gate.r*(vOld-ek);
};
void ControlSa::updateItrek()
{
    //neuro TREK-1
//    double gK2P = itrekFactor*.040716*10.5; //nS/pF
//    double xpK2P = exp(-vOld*FDAY/(RGAS*TEMP));
//    
//    iTrek = vOld*(gK2P/kI)*(kI - (kO*xpK2P))/(1-xpK2P);
//    
//
    
    //TREK-1 with more rectification
    double gk = itrekFactor*.03011;
    double EK = RGAS*TEMP/FDAY*log(kO/kI);
    
    double aa = 1/(1+exp(-(vOld-65)/52)); //from Kim J Physiol 1992
    
    iTrek = gk*aa*(vOld-EK);  //apex vs. septum??? TJH
    
}
void ControlSa::updateIkach()
{
        double gkach = ikachFactor*0.0011*pow(kO,0.41);

        iKach = gkach*(kI-kO*exp(-vOld*FDAY/(RGAS*TEMP)));
};
//Sustained inward current
void ControlSa::updateIst()
{
        double qainf, qiinf, tauqa, tauqi;
        double alphaqa,betaqa,alphaqi,betaqi;
	
	double gSt = istFactor*0.0075;
        double est = 37.4;  //mV
        double isofact;  //effect of iso on conductance
        double vhalf;

        vhalf = -57.0;//-15.0*isoConc/(0.00226+isoConc);
        alphaqa = 1/(0.15*exp(-vOld/11.0)+0.2*exp(-vOld/700.0));
        betaqa = 1/(16.0*exp(vOld/8.0)+15.0*exp(vOld/50.0));
        qainf = 1/(1+exp(-(vOld-vhalf)/5.0));
        tauqa = 1/(alphaqa+betaqa);

        alphaqi = 1/(3100.0*exp(vOld/13.0)+700.0*exp(vOld/70.0));
        betaqi = 1/(95*exp(-vOld/10.0)+50.0*exp(-vOld/700.0))+0.000229/(1.0+exp(-vOld/5.0));
        qiinf = alphaqi/(alphaqi+betaqi);
        tauqi = 6.65/(alphaqi+betaqi);

        Gate.qa=qainf-(qainf-Gate.qa)*exp(-dt/tauqa);
        Gate.qi=qiinf-(qiinf-Gate.qi)*exp(-dt/tauqi);

        isofact=0.0;//0.964*isoConc/(0.00226+isoConc);
        iSt = gSt*(1+isofact)*Gate.qa*Gate.qi*(vOld-est);
};
//Na+ background current
void ControlSa::updateInab()
{
	double gNab = inabFactor*0.0054;  // nS/pF
        double ena = (RGAS*TEMP/FDAY)*log(naO/naI); //mV;

        iNab = gNab*(vOld-ena);
};
//Na+-K+ pump
void ControlSa::updateInak()
{
        double iNakmax = inakFactor*3.6;
	double kmnai = 14.0;
        double kmko = 1.4;
        double ena = (RGAS*TEMP/FDAY)*log(naO/naI);

        iNak=iNakmax/(1+pow((kmko/kO),1.2))*1/(1+pow((kmnai/naI),1.3))*1/(1+exp(-(vOld-ena+120.0)/30.0));
};
void ControlSa::updateInaca()
{
        double di,dout;
        double k43,k12,k14,k41,k34,k21,k23,k32;
        double x1,x2,x3,x4;

        double kNaca = inacaFactor*125.0;
	double qci = 0.1369;
        double qco = 0.0;
        double qn = 0.4315;

        double kci = 0.0207; //mM
        double kcni = 26.44;
        double k1ni = 395.3;
        double k2ni = 2.289;
        double k3ni = 26.44;
        double kco = 3.663;
        double k1no = 1628.0;
        double k2no = 561.4;
        double k3no = 4.663;

        di = 1+(caR/kci)*(1+exp(-qci*vOld*FDAY/(RGAS*TEMP))+naI/kcni)+(naI/k1ni)*(1+(naI/k2ni)*(1+naI/k3ni));
        dout = 1+(caO/kco)*(1+exp(qco*vOld*FDAY/(RGAS*TEMP)))+(naO/k1no)*(1+(naO/k2no)*(1+naO/k3no));
        k43 = naI/(k3ni+naI);
        k12 = (caR/kci)*exp(-qci*vOld*FDAY/(RGAS*TEMP))/di;
        k14 = (naI/k1ni)*(naI/k2ni)*(1+naI/k3ni)*exp(qn*vOld*FDAY/(2*RGAS*TEMP))/di;
        k41 = exp(-qn*vOld*FDAY/(2*RGAS*TEMP));
        k34 = naO/(k3no+naO);
        k21 = (caO/kco)*exp(qco*vOld*FDAY/(RGAS*TEMP))/dout;
        k23 = (naO/k1no)*(naO/k2no)*(1+naO/k3no)*exp(-qn*vOld*FDAY/(2*RGAS*TEMP))/dout;
        k32 = exp(qn*vOld*FDAY/(2*RGAS*TEMP));
        x1 = k34*k41*(k23+k21)+k21*k32*(k43+k41);
        x2 = k43*k32*(k14+k12)+k41*k12*(k34+k32);
        x3 = k43*k14*(k23+k21)+k12*k23*(k43+k41);
        x4 = k34*k23*(k14+k12)+k21*k14*(k34+k32);

        iNaca = kNaca*(k21*x2-k12*x1)/(x1+x2+x3+x4);
};
//Hyperpolarization activated current
void ControlSa::updateIh()
{
        double yinf, tauy;
	double alphay, betay;

	double gH = ihFactor*0.27325;
        double ena = (RGAS*TEMP/FDAY)*log(naO/naI); //mV
        double ek = (RGAS*TEMP/FDAY)*log(kO/kI); //mV

 	double ghna = gH; //nS/pF
        double ghk = gH;  //nS/pF

        alphay = exp(-(vOld+78.91)/26.62);
        betay = exp((vOld+75.13)/21.25);

        yinf = alphay/(alphay+betay);   //vOld+64.0
        tauy = 1000/(alphay+betay);

        Gate.y = yinf-(yinf-Gate.y)*exp(-dt/tauy);

        iHna = ghna*Gate.y*Gate.y*(vOld-ena);
        iHk = ghk*Gate.y*Gate.y*(vOld-ek);
	iH = iHna+iHk;
};
//Uptake from myoplasm into NSR^M
void ControlSa::updateIup()
{
	double iUpbar = iupFactor*0.01;
        double kmup=0.0006; //0.0006 mM^M
        double kmcamk=0.15;
        double deltaiup=.75;
        double isofact;

        isofact = 0.0;//0.85*pow(isoConc,0.934)/(pow(isoConc,0.934)+0.0052);
        iUp=iUpbar*(1+isofact)*caI/(caI+kmup);  

};
//Transfer from NSR to JSR^M
void ControlSa::updateItr()
{       
	double tautr=60.0;  //60.0ms^M
        
        iTr=(caNsr-caJsr)/tautr;
};
//Ca2+ release from JSR into subspace
void ControlSa::updateIrel()
{       
	double iRelbar = irelFactor*1.25;
        double kmrel = 0.0012; //mM

        iRel = iRelbar*(caJsr-caR)/(1+(kmrel/caR)*(kmrel/caR));
        //iRel = iRelbar*0.5*(caJsr-caR);^M
};
//Ca2+ diffusion from subspace into bulk myoplasm
void ControlSa::updateIdiff()
{
	double tauDiff = 0.01;

        iDiff = (caR-caI)/tauDiff;
};
// Na concentration in myoplasm
void ControlSa::updateNai()
{
        double dnai;
        dnai=dt*(-iNat*ACap/((Vmyo+Vss)*FDAY));
        naI=naI+dnai;
};
// K concentration in myoplasm
void ControlSa::updateKi()
{
        double dki;
        dki=dt*(-iKt*ACap/((Vmyo+Vss)*FDAY));
        kI=kI+dki;
};
//Bulk Ca2+
void ControlSa::updateCai()
{
        double dcai,beta;
        double dcmdni,dtrpnca,dtrpnmg,dtrpnmgmg;
        double trpncabar = 0.031;   // mM Troponin-Ca2+ site
        double kmtrpnca = 0.0050225225;  // kb/kf
        double trpnmgbar = 0.062;   //0.062 mM Troponin-Mg2+ site
        double kmtrpnmg = 0.00003298199;
        double kmtrpnmgmg = 0.3298199;
        double cmdnbar = 0.045;  //0.045
        double kmcmdn = 0.002380325;

        dcai=dt*(-iCait*ACap/(Vmyo*2.0*FDAY)-(iUp)*Vnsr/Vmyo+iDiff*Vss/Vmyo);
        dcmdni=dt*(227.7*caI*(1-cmdnI)-0.542*cmdnI);
        dtrpnca=dt*(88.8*caI*(1-trpnCa)-0.446*trpnCa);
        dtrpnmg=dt*(227.7*caI*(1-trpnMg-trpnMgmg)-0.00751*trpnMg);
        dtrpnmgmg=dt*(2.277*2.5*(1-trpnMg-trpnMgmg)-0.751*trpnMgmg);

        caI=caI+dcai-cmdnbar*dcmdni-trpncabar*dtrpnca-trpnmgbar*dtrpnmg;
        cmdnI=cmdnI+dcmdni;
        trpnCa=trpnCa+dtrpnca;
        trpnMg=trpnMg+dtrpnmg;
        trpnMgmg=trpnMgmg+dtrpnmgmg;
};
//Subspace Ca2+
void ControlSa::updateCar()
{
        double beta,dcar;
        double dcmdnr;
        double kmcmdn = 0.002380325;  //mM
        double cmdnbar = 0.045;  //mM

        //Rapid buffer approximation - Wagner and Keizer Biophys J. 67:447-456; 1994.
        beta=1/(1+cmdnbar*kmcmdn/((kmcmdn+caR)*(kmcmdn+caR)));
        dcar=dt*(-(iCart)*ACap/(Vss*2.0*FDAY)+iRel*Vjsr/Vss-iDiff);

        caR=caR+dcar*beta;
        cmdnR=cmdnbar*(caR/(caR+kmcmdn));
};
void ControlSa::updateCasr()
{
        double dcajsr,dcansr,beta;
        double dcsqn;
        double kmcsqn=0.8333333;
        double csqnbar=10.0;

        //Rapid buffer approximation - Wagner and Keizer Biophys J. 67:447-456; 1994.
        beta=1/(1+csqnbar*kmcsqn/((kmcsqn+caJsr)*(kmcsqn+caJsr)));
        dcajsr=dt*(iTr-iRel);

        caJsr=caJsr+dcajsr*beta;

        csqn=csqnbar*(caJsr/(caJsr+kmcsqn));
        dcansr=dt*(iUp-iTr*Vjsr/Vnsr);
        caNsr=caNsr+dcansr;
};
void ControlSa::updateCurr()
{
   updateIst();   // Sustained inward current
   updateInak();   // Na-K pump
   updateInaca();  // Na-Ca exchanger
   updateInab();   // Background Ca current
   updateIcal();   // L-type Ca current
   updateIcatt();   // T-type Ca current
   updateIks();    // Slow delayed rectifier current
   updateIkr();    // Rapid delayed rectifier current
   updateI4ap();    // Transient outward K current
    updateItrek();    // Transient outward K current
   updateIkach();    // Transient outward K current
   updateIh();    // Transient outward K current

   iNat=iHna+iSt+iNab+3*iNak+3*iNaca;
   iCart=iCal+iCatt-2*iNaca;
   iCat=iCart;
   iKt=iTo+iKr+iKs+iHk+iKach+iSus+iTrek-2*iNak;

   iTotold=iTot;
   iTot=iNat+iCait+iKt+iCart;

};
void ControlSa::updateConc()
{
   updateIup();
   updateItr();
   updateIrel();
   updateIdiff();
   updateCai();
   updateCar();
   updateCasr();
   updateKi();
   updateNai();
};
// External stimulus.
int ControlSa::externalStim(double stimval)
{
    iKt = iKt + stimval;
    iTot = iTot + stimval;

    return 1;
  
};

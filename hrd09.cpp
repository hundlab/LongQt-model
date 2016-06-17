//######################################################
// This file contains all function definitions for
// version of Hund-Rudy dynamic model used in 
// Christensen et al. PLoS Comput Biol.2009;5:e1000583.
//
// Copyright (C) 2011 Thomas J. Hund. 
// Updated 11/21/2012
//######################################################

#include "hrd09.h"

//######################################################
// Constructor for control canine epicardial
// ventricular model.
//######################################################
HRD09Control::HRD09Control()
{
    this->Initialize();
};
HRD09Control::HRD09Control(const HRD09Control& toCopy) : Cell(toCopy) {
    this->Initialize();
    CellKernel::copyVarPar(toCopy);

}
//######################################################
// Destructor for control canine epicardial
// ventricular model.
//#####################################################
HRD09Control::~HRD09Control()
{
};

//##### Initialize variables ##################
void HRD09Control::Initialize() {
        type = "Canine Ventricular (Hund-Rudy 2009)";
        dVdt=dVdtmax=5.434230843e-10;//check
        Cm = 1.0; //uF/cm2  must be defined for fiber...default = 1.
	t=0.0;//check
        dt=dtmin = 0.005;
   	dtmed = 0.01;
   	dtmax = 0.1;
   	dvcut = 1.0;
        vOld = vNew = -87.21621716; //check
        tRel=10000.0;
        sponRelflag = 0;

        apTime = 0.0;

        naO = 140;
        caO = 1.8;
        kO = 5.4;
        clO = 100.0;

        kI = 142.4894672;//check
        naI = 9.889359792;//check
        caI = 9.22820165e-05;//check
        clI = 19.49938287;//check

        caNsr = 1.779020715;//check
        caJsr = 1.752822261;//check

        cellRadius=0.0011;   //must be defined for fiber...default = 0.001;
        cellLength=0.01;
	Rcg = 2.0;           //must be defined for fiber...default = 1.

        Vcell=1000*3.14*cellRadius*cellRadius*cellLength;
        AGeo=2*3.14*cellRadius*cellRadius+2*3.14*cellRadius*cellLength;
        ACap=AGeo*Rcg;
        Vmyo=Vcell*0.66;
        Vmito=Vcell*0.26;
        Vsr=Vcell*0.06;
        Vnsr=Vsr*0.92;
        Vjsr=Vsr*0.08;
        Vss=Vcell*0.02;

        //#### Initialize ion channel gatess ####
        Gate.m = 0.00106712815;//check
        Gate.h = 0.9907975703;//check
        Gate.j = 0.9939823939;//check
        Gate.ml = 0.0008371146773;//check
        Gate.hl = 0.3497149946;//check

        Gate.d = 1.325706938e-06;//check

        Gate.f = 0.9999686553;//check
        Gate.fca = 0.9733486742;//check
        Gate.fca2 = 0.9768246778;//check

        Gate.aa = 0.0006399001296;//check
        Gate.xs = 0.01877782697;//check
        Gate.xs2 = 0.01877782695;//check
        Gate.xr = 1.312795768e-08;//check

        Gate.a = 0.003631705908;//check
        Gate.i = Gate.i2 = 1.0;//check

        Gate.k1 = 0.5863265763;
        Gate.kp = 1.040365514E-7;

        ryRopen = 0.0001245604929;//check
        //#################################

        caR = 9.617454153e-05;//check

        cmdn = 0.001866332722;//check
        csqn = 6.866213476;//check
        trpn = 0.01090652928;//check

        caM = 0.0003647207545;//check
        fBlock = 0;//check
	fBound = 0.0004862831625;//check
	fI = 0.9995137085;//check
	fOx = 0;//check
	fOxP = 0;//check
	fPhos = 8.382592879e-09;//check
        
 	caTotal = caI+cmdn+trpn;

        iCab = -0.1545414915;//check
        iCa = -0.0002372516075;//check
        iClb = -0.01204268782;
        iDiff = 1.946262516e-05;//check
        iK1 = 0.06097535959;//check
        iKcl = -1.757431666E-5;
        iKp = 7.603990374e-11;//check
        iKr = 3.757346237e-11;//check
        iKs = -0.0001518875075;//check
        iLeak = 0.0007449665762;//check
        iNa = -3.02559356e-06;//check
        iNaca = -0.05704574832;//check
        iNacar = -0.01586413161;//check
        iNacl = 1.827578455E-5;
        iNak = 0.1690221704;//check
        iNal = -4.364711352e-10;//check
        iPca = 0.008958934766;//check
        iRel = 0.0002183204253;//check
        iTo = 1.415160573e-09;//check
        iTo2 = -0.001314601273;//check
        iTr = 0.0002183204519;//check
        iUp = 0.0007639509773;//check

        iCat = -4.844236345e-08;//check

        makemap();

}
//overriden deep copy funtion
HRD09Control* HRD09Control::clone(){
    return new HRD09Control(*this);
};
// L-type Ca current 
void HRD09Control::updateIlca()
{
    double taud,finf,fcainf,fcainf2,taufca,taufca2,tauf,dinf;//,taud2,d2inf;
	double maxIca;
	
	double gamcai=1.0;
	double gamcao=0.341;
	double pca=2.43E-4;  // Max permeability
	double kmca=0.002;   // Ca2+ half-saturation coeff. for Ca2+-dependent inactivation
	
	double deltatfca=10; // Maximal CaMKII-dependent increase in tfca
	double deltat=20;  
	double KMCAM=0.2;  // CaMKII half-saturation coefficient
	double cafac;
	double camfact;
	
   	cafac=1/(1.0+caR/kmca);   

	camfact=1/(1+pow((KMCAM/caM),2.0));

	dinf=(1/(1.0+exp(-(vOld-4.0)/6.74))); 
	taud=0.59+0.8*exp(0.052*(vOld+13))/(1+exp(0.132*(vOld+13)));
	
	finf=0.7/(1+exp((vOld+17.12)/7.0))+.3;
	tauf=22+280*exp(0.062*(vOld+28.3))/(1+exp(0.25*(vOld+28.3)))+deltat*camfact;
	
    	fcainf=0.3/(1-iCa/0.05)+0.55*cafac+0.15;  
    	taufca=0.5+deltatfca*camfact+1*cafac;
    	
	fcainf2=1/(1-iCa/0.01); 
    	taufca2=100/(1+exp((-iCa-0.175)/0.04))+100;
        
    	Gate.fca=fcainf-(fcainf-Gate.fca)*exp(-dt/taufca);
    	Gate.fca2=fcainf2-(fcainf2-Gate.fca2)*exp(-dt/taufca2);

	Gate.d=dinf-(dinf-Gate.d)*exp(-dt/taud);
    	Gate.f=finf-(finf-Gate.f)*exp(-dt/tauf);	
	
	maxIca=pca*4*(vOld)*(FDAY*FDAY/(RGAS*TEMP))*(gamcai*caR*exp(2*(vOld)*FDAY/(RGAS*TEMP))-gamcao*caO)/(exp(2*(vOld)*FDAY/(RGAS*TEMP))-1.0);
	
	iCa=Gate.d*Gate.f*Gate.fca*Gate.fca2*maxIca;
};

// Background Ca current  
void HRD09Control::updateIcab()
{
	double maxicab;//Ecan,
	double gamcai=1.0;
	double gamcao=0.341;
	double gcab=1.995084E-7;//2.25084E-7; different from HRd08
	
//	Ecan=(RGAS*TEMP/(2*FDAY))*log(caO/caI);	
	
	maxicab=4*(vOld)*(FDAY*FDAY/(RGAS*TEMP))*(gamcai*caI*exp(2*(vOld)*FDAY/(RGAS*TEMP))-gamcao*caO)/(exp(2*(vOld)*FDAY/(RGAS*TEMP))-1.0);

	iCab=gcab*maxicab; 
};

// Sarcolemmal Ca pump
void HRD09Control::updateIpca()
{
	double ipcabar=0.0575;//Different from HRd08 - 0.2675;   
	double kmpca=0.0005;
	
	iPca=ipcabar*caI/(kmpca+caI);  
};

// Background Cl current
void HRD09Control::updateIclb()
{
	double ecl;
	double gclb=0.000225;   
	
	ecl=-(RGAS*TEMP/(FDAY))*log(clO/clI);	
	iClb=gclb*(vOld-ecl);
	
};

// Slow delayed rectifier K current
void HRD09Control::updateIks()
{
	double Gks, Eks;
	double xsinf,tauxs1,tauxs2;
	double prnak = 0.01833;
	Gks=(0.028975*(1+.6/(1+pow((3.8E-5/caI),1.4))));
	Eks=(RGAS*TEMP/FDAY)*log((kO+prnak*naO)/(kI+prnak*naI));
	
	xsinf=1/(1+exp(-(vOld-10.5)/24.7));
	
	tauxs1=1/(5.0824E-5*(vOld+18.5172)/(1-exp(-0.064*(vOld+18.5172)))+4.8824E-4*(vOld+18.5172)/(exp(0.064*(vOld+18.5172))-1));
	tauxs2=2*tauxs1;
		
	Gate.xs=xsinf-(xsinf-Gate.xs)*exp(-dt/tauxs1);
	Gate.xs2=xsinf-(xsinf-Gate.xs2)*exp(-dt/tauxs2);
	
	iKs=Gks*Gate.xs*Gate.xs2*(vOld-Eks);		
};

// Rapid delayed rectifier K current
void HRD09Control::updateIkr()
{
	double Gkr, Ekr;
	double xrinf,tauxr;
	
	Gkr=0.0138542*sqrt(kO/5.4);
	Ekr=(RGAS*TEMP/FDAY)*log(kO/kI);
	
	xrinf=1/(1+exp(-(vOld+10.085)/4.25));  
	
	tauxr=1.0/(.6E-3*(vOld-1.7384)/(1-exp(-.136*(vOld-1.7384)))+3.0E-4*(vOld+38.3608)/(exp(.1522*(vOld+38.3608))-1));
	
	Gate.xr=xrinf-(xrinf-Gate.xr)*exp(-dt/tauxr);
	
	Gate.r=1/(1+exp((vOld+10.0)/15.4));
	
	iKr=Gkr*Gate.xr*Gate.r*(vOld-Ekr);			
};

// Plateau K current
void HRD09Control::updateIkp()
{
	double Gkp = 0.00276;
	double Ekp;

 	Ekp=(RGAS*TEMP/FDAY)*log(kO/kI);   
	Gate.kp=1/(1+exp((7.488-vOld)/5.98));
	
	iKp=Gkp*Gate.kp*(vOld-Ekp);	
};

// Time-independent K current
void HRD09Control::updateIk1()
{
	double Gk1, Ek1;
	double MaxGk1 =0.5;
        
	Ek1=(RGAS*TEMP/FDAY)*log(kO/kI);
	Gk1=MaxGk1*(sqrt(kO/5.4));
	
  	Rate.ak1=1.02/(1+exp(.2385*(vOld-Ek1-59.215)));
	Rate.bk1=(.49124*exp(.08032*(vOld-Ek1+5.476))+exp(.06175*(vOld-Ek1-594.31)))/(1+exp(-.5143*(vOld-Ek1+4.753)));

	Gate.k1=Rate.ak1/(Rate.ak1+Rate.bk1);
	iK1=Gk1*Gate.k1*(vOld-Ek1);
};

//K-Cl cotransporter
void HRD09Control::updateIkcl()
{
	double ecl, ek;
	double gkcl=0.0000177;   
	double deltanct=87.8251;
	
	ek=(RGAS*TEMP/FDAY)*log(kO/kI);
	ecl=-(RGAS*TEMP/(FDAY))*log(clO/clI);	
	iKcl=gkcl*(ek-ecl)/((ek-ecl)+deltanct); 
};

//Fast Na current
void HRD09Control::updateIna()
{
	double bj1a,bj2a,bj2b,aj1a,aj1b,aj1c;
	double ms,tm,hs,th,js,tj;
	double camfact;
	double KMCAM=0.3;  
	double vshift; 
	double deltaalpha = -.18;
	double MaxGNa, ENa;

	MaxGNa = 16.0;// different from HRd08 - 8.25;

	camfact= 1/(1+pow((KMCAM/caM),4.0));
	vshift = -3.25*camfact;

	ENa=(RGAS*TEMP/FDAY)*log(naO/naI);
	
	Rate.am=.32*(vOld+47.13)/(1-exp(-.1*(vOld+47.13)));  
	Rate.bm=.08*exp(-vOld/11.0);				 	

	if((vOld-vshift)>=-40.0)
	{
	        Rate.ah=0.0;	
		Rate.bh=1/(.13*(1+exp((vOld-vshift+10.66)/(-11.1))));  	
	}
	else
	{
	        Rate.ah=.135*exp((80.0+vOld-vshift)/(-6.8));
		Rate.bh=3.56*exp(.079*(vOld-vshift))+(3.1E5)*exp(.35*(vOld-vshift));
        }

	if((vOld-vshift)>=-40.0)
	{
		Rate.aj=0.0;	

		bj1a=exp((-2.535E-7)*(vOld-vshift));	
		Rate.bj=(.3)*bj1a/(exp(-.1*(vOld-vshift+32))+1);  
	}
	
	else
	{				 		
		aj1a=(-1.2714E5)*exp(.2444*(vOld-vshift));
		aj1b=(3.474E-5)*exp(-.04391*(vOld-vshift));
		aj1c=(vOld-vshift+37.78)/(1+exp(.311*(vOld-vshift+79.23)));
		
		Rate.aj=(1+camfact*deltaalpha)*(aj1a-aj1b)*aj1c;  

		bj2a=.1212*exp(-.01052*(vOld-vshift));
		bj2b=1+exp(-.1378*(vOld-vshift+40.14));
		Rate.bj=bj2a/bj2b;
	}
	
	ms=Rate.am/(Rate.am+Rate.bm);	
	tm=1/(Rate.am+Rate.bm);
	Gate.m=ms-(ms-Gate.m)*exp(-dt/tm);
	
	hs=Rate.ah/(Rate.ah+Rate.bh);
	th=1/(Rate.ah+Rate.bh);
	Gate.h=hs-(hs-Gate.h)*exp(-dt/th);
	
	js=Rate.aj/(Rate.aj+Rate.bj);
	tj=1/(Rate.aj+Rate.bj);
	Gate.j=js-(js-Gate.j)*exp(-dt/tj);

	iNa=(MaxGNa*Gate.m*Gate.m*Gate.m*Gate.h*Gate.j)*(vOld-ENa);


};

// Late Na current
void HRD09Control::updateInal()
{
	double ENa;
	double ms,tml,hlinf,thl;
	double camfact;
	double KMCAM=0.3;  
	double deltag = 0.0095;
		
	camfact=1/(1+pow((KMCAM/caM),4.0));

	ENa=(RGAS*TEMP/FDAY)*log(naO/naI);
	
	Rate.aml=.32*(vOld+47.13)/(1-exp(-.1*(vOld+47.13)));  
	Rate.bml=.08*exp(-vOld/11.0);				 
	
	hlinf = 1/(1+exp((vOld+91)/6.1));  

	ms=Rate.aml/(Rate.aml+Rate.bml);	
	tml=1/(Rate.aml+Rate.bml);
	Gate.ml=ms-(ms-Gate.ml)*exp(-dt/tml);
	
	thl=600;
	Gate.hl=hlinf-(hlinf-Gate.hl)*exp(-dt/thl);
		
	iNal=(0.0065+camfact*deltag)*Gate.ml*Gate.ml*Gate.ml*Gate.hl*(vOld-ENa); 	
};

//Na-Ca exchanger
void HRD09Control::updateInaca()
{
	double eta=.35; 
	double kmcaact=.000125;  
	double ksat = .27;  
	double vmax=5.3; 
	double kmnai=12.3;  
	double kmnao=87.5;
	double kmcai=.00136; 
	double kmcao=1.3;  
	double percr = 0.2;  // fraction of Na-Ca exchanger in subspace 
	
	//Na-Ca exchanger sensing Ca in subspace
	iNacar = percr*(1/(1+(kmcaact/(caR))*(kmcaact/(caR))))*(vmax*(naI*naI*naI*caO*exp(eta*vOld*FDAY/(RGAS*TEMP))-naO*naO*naO*caR*exp((eta-1)*vOld*FDAY/(RGAS*TEMP)))/((1+ksat*exp((eta-1)*vOld*FDAY/(RGAS*TEMP)))*(kmcao*naI*naI*naI+kmnao*kmnao*kmnao*caR+kmnai*kmnai*kmnai*caO*(1+caR/kmcai)+kmcai*naO*naO*naO*(1+naI*naI*naI/(kmnai*kmnai*kmnai))+naI*naI*naI*caO+naO*naO*naO*caR)));
	//Na-Ca exchanger sensing bulk myocplasmic Ca
	iNaca = (1-percr)*(1/(1+(kmcaact/(caI))*(kmcaact/(caI))))*(vmax*(naI*naI*naI*caO*exp(eta*vOld*FDAY/(RGAS*TEMP))-naO*naO*naO*caI*exp((eta-1)*vOld*FDAY/(RGAS*TEMP)))/((1+ksat*exp((eta-1)*vOld*FDAY/(RGAS*TEMP)))*(kmcao*naI*naI*naI+kmnao*kmnao*kmnao*caI+kmnai*kmnai*kmnai*caO*(1+caI/kmcai)+kmcai*naO*naO*naO*(1+naI*naI*naI/(kmnai*kmnai*kmnai))+naI*naI*naI*caO+naO*naO*naO*caI)));
};

//Na-Cl cotransporter
void HRD09Control::updateInacl()
{
	double ENa;
	double ecl;
	double gnacl=0.0000246108;
	double deltanct=87.8251;
	ENa=(RGAS*TEMP/FDAY)*log(naO/naI);
	
	ecl=-(RGAS*TEMP/(FDAY))*log(clO/clI);	
	iNacl=gnacl*(ENa-ecl)*(ENa-ecl)*(ENa-ecl)*(ENa-ecl)/((ENa-ecl)*(ENa-ecl)*(ENa-ecl)*(ENa-ecl)+deltanct*deltanct*deltanct*deltanct);
};

// Na-K ATPase
void HRD09Control::updateInak()
{
	double inakbar=0.93;
	double kmnai=10.0;
	double kmko=1.5;
 	
	double fnak,sigma;
	
	sigma=(exp(naO/67.3)-1.0)/7.0;
	fnak=1.0/(1.0+.1245*exp(-.1*vOld*FDAY/(RGAS*TEMP))+.0365*sigma*exp(-vOld*FDAY/(RGAS*TEMP)));
	
	iNak=inakbar*fnak*1/(1+(kmnai/naI)*(kmnai/naI))*kO/(kO+kmko); 
};

// Transient outward K current
void HRD09Control::updateIto()
{
    double tauzdv,zssdv,tauydv,tauy2dv,yssdv,yss2dv,rvdv;//azdv,bzdv,aydv,bydv,ay2dv,by2dv,
    double Ek;
    double MaxGto = 0.19; // different from HRd08 - 0.9;

   Ek=(RGAS*TEMP/FDAY)*log(kO/kI);
   rvdv = exp(vOld/300.0);
   
   tauzdv = 3.5*exp(-(vOld/30)*(vOld/30))+1.5;
   zssdv = 1.0/(1+exp((vOld+3.0)/(-15.0)));
	
   tauydv = 20/(1+exp((vOld+33.5)/10))+20;
   tauy2dv=3000/(1+exp((vOld+60)/10))+20;
   yssdv = 1/(1+exp(vOld+33.5)/10);
   yss2dv = 1/(1+exp(vOld+33.5)/10);

   Gate.a=zssdv-(zssdv-Gate.a)*exp(-dt/tauzdv);
   Gate.i=yssdv-(yssdv-Gate.i)*exp(-dt/tauydv);
   Gate.i2=yss2dv-(yss2dv-Gate.i2)*exp(-dt/tauy2dv);

   iTo=MaxGto*Gate.a*Gate.a*Gate.a*Gate.i*Gate.i2*rvdv*(vOld-Ek);


};

//Ca-dependent transient outward current
void HRD09Control::updateIto2()
{
	double tauaa,aainf;//eto2,
	double ibar;
	double pcl=0.0000004;
	double z=-1.0;
    
//	eto2 = -(RGAS*TEMP/FDAY)*log(clO/clI);  
	ibar=pcl*(vOld)*FDAY*FDAY/(RGAS*TEMP)*(clI-clO*exp(-z*FDAY*(vOld)/(RGAS*TEMP)))/(1-exp(-z*FDAY*(vOld)/(RGAS*TEMP)));

	aainf = 1/(1+0.1502/caR);    
	
	tauaa=1.0;  
		
	Gate.aa=aainf-(aainf-Gate.aa)*exp(-dt/tauaa);
	
	iTo2 = Gate.aa*ibar;
};

// Ca release from sarcoplasmic reticulum
void HRD09Control::updateIrel()
{
	double irelinf, taurel;
	double kappa=0.1;
	double betatau=5.25;
	double kmrelinf = 1.0;  	
	double deltakmrelinf = 0.35;
	double kmreltau = 0.0123; 
	double hrel=10.0;
  	double KMCAM=0.2; 
	double deltaalpharel = 1.0;  //Different from HRd08 - 0.0
	double alpharel;
//	double grel,grelbar;  //for spontaneous release
	double camkfact;

	camkfact=1/(1+pow((KMCAM/caM),5.0)); 
	alpharel = kappa*betatau*(1+deltaalpharel*camkfact);

	irelinf=alpharel*iCa/(1+(pow((kmrelinf-deltakmrelinf*camkfact)/caJsr,hrel)));
	taurel=betatau/(1+kmreltau/caJsr);
	
	//##### For spontaneous Ca release ######
	if(csqn>(10.0)){  
	  if(sponRelflag==0){
	    tRel=0.0;
	    sponRelflag=1;
	    cout << "Spontaneous release at t = " << t << endl;
	    ryRopen = irelinf = 6.0;
	  }
	  tRel=tRel+dt;
	}
	else if(sponRelflag==1)
	  taurel=10;
	if(tRel>100.0)
 	  sponRelflag=0;
	// ###################################

	ryRopen=ryRopen-dt*((irelinf+ryRopen)/taurel);
	iRel=ryRopen*(caJsr-caR);
};

// Ca transport into/out of sarcoplasmic reticulum
void HRD09Control::updateSrFlux()
{
	double iupbar=0.008375;   
	double kmup=0.00092;
	double nsrbar=15.0;
	double tautr=120.0;

	double plb;
	double deltakmup=0.00017;	
	double deltaileak=1.5;  
	double KMCAM=0.2;
	double KMCAMlk=0.3;
	double camfact;
	double camfactlk;

	camfact=1/(1+pow((KMCAM/caM),5.0));
        camfactlk=1/(1+pow((KMCAMlk/caM),5.0));
        
	plb=deltakmup*camfact;
	iUp=iupbar*caI/(caI+kmup-plb);   	
	iLeak=0.75*(iupbar/nsrbar)*(1+deltaileak*camfactlk)*caNsr;    
	
 	iTr=(caNsr-caJsr)/tautr;  
};

// Na concentration in myoplasm
void HRD09Control::updateNai()	
{
	double dnai;
	dnai=dt*(-iNat*ACap/(Vmyo*FDAY)+iNacl);
	naI=naI+dnai;
};

// K concentration in myoplasm
void HRD09Control::updateKi()	
{
	double dki;
	dki=dt*(-iKt*ACap/(Vmyo*FDAY)+iKcl);
	kI=kI+dki;
};

// Cl concentration in myoplasm
void HRD09Control::updateCli()   
{
	double dcli;
	dcli=dt*(iClt*ACap/(Vmyo*FDAY)+iNacl+iKcl);
	clI=clI+dcli;
};

// Ca concentration in myoplasm
void HRD09Control::updateCai()
{
	double dcai,cait,b,c,d;
	
	double trpnbar=0.07; 
	double kmtrpn=0.0005;  
	double cmdnbar=0.05;  
	double kmcmdn=0.00238;

        dcai=-dt*((iCab+iPca-2*iNaca)*ACap/(Vmyo*2.0*FDAY)+(iUp-iLeak)*Vnsr/Vmyo-iDiff*Vss/Vmyo);
	
        // Cytosolic buffer concentrations 
        trpn=trpnbar*(caI/(caI+kmtrpn));
	cmdn=cmdnbar*(caI/(caI+kmcmdn));
	
	cait=caI+trpn+cmdn+dcai;  // total [Ca] in cytosol
	
        // Analytical solution for new [Ca]i
	b=cmdnbar+trpnbar-cait+kmtrpn+kmcmdn;
	c=(kmcmdn*kmtrpn)-(cait*(kmtrpn+kmcmdn))+trpnbar*kmcmdn+cmdnbar*kmtrpn;
	d=-1*kmtrpn*kmcmdn*cait;
	
	caI=(2.0/3.0)*sqrt(b*b-3.0*c)*cos(acos((9.0*b*c-2*b*b*b-27*d)/(2.0*pow((b*b-3.0*c),1.5)))/3.0)-b/3.0;
		
	caTotal=caI+trpn+cmdn;
	
};

// Ca concentration in subspace
void HRD09Control::updateCaSub()
{
	double taudiff,dcar;//,bss;
	double bsrbar,kmbsr,bslbar,kmbsl;
    double b1,c1,d1,bsr,bsl,cart;//,carold;

	taudiff=.2; 
	bsrbar=0.047; 
	kmbsr=0.00087; 
	bslbar=1.124; 
	kmbsl=.0087;  
	
//	bss=1/(1+bsrbar*kmbsr/((kmbsr+caR)*(kmbsr+caR))+bslbar*kmbsl/((kmbsl+caR)*(kmbsl+caR))); 
	iDiff=(caR-caI)/taudiff;
	dcar=dt*(-(iCa-2*iNacar)*ACap/(Vss*2.0*FDAY)+iRel*Vjsr/Vss-iDiff); 
	
	bsr=bsrbar*(caR/(caR+kmbsr));
        bsl=bslbar*(caR/(caR+kmbsl));
        
       cart=caR+bsr+bsl+dcar;

        b1=bsrbar+bslbar-cart+kmbsr+kmbsl;
        c1=(kmbsr*kmbsl)-(cart*(kmbsr+kmbsl))+bsrbar*kmbsl+bslbar*kmbsr;
        d1=-1*kmbsr*kmbsl*cart;

       // Analytical solution for new value
       caR=(2.0/3.0)*sqrt(b1*b1-3.0*c1)*cos(acos((9.0*b1*c1-2*b1*b1*b1-27*d1)/(2.0*pow((b1*b1-3.0*c1),1.5)))/3.0)-b1/3.0;
	caRt = caR+bsr+bsl;   
};
 
// Ca concentration in sarcoplasmic reticulum
void HRD09Control::updateSr() 	
{
	double dcajsr,dcansr,b1,c1;//,dcsqn,csqnold
	double kmcsqn=0.8;
	double csqnbar=10.0;
		
	dcajsr=dt*(iTr-iRel);
	b1=csqnbar-csqn-dcajsr-caJsr+kmcsqn;	
	c1=kmcsqn*(csqn+dcajsr+caJsr);
	
	caJsr=(sqrt(b1*b1+4.0*c1)-b1)/2.0;
//	csqnold=csqn;
	csqn=csqnbar*(caJsr/(caJsr+kmcsqn));
//	dcsqn=(csqn-csqnold)/dt;

	caJsrtotal=caJsr+csqn;
	
	dcansr=dt*(iUp-iLeak-iTr*Vjsr/Vnsr);
	caNsr=caNsr+dcansr;
};

// CaMKII activity
void HRD09Control::updateCamk()
{
        double P=0.00003;          // rate of dephosphorylation .00003
        double calmodulin;         // free calmodulin bound to calcium.
        double calmtotal = .00006; // total free calmodulin = 60 nM.

        double va,va2,ka,tcamk;

        double ros = 0.0;          // concentration of H2O2, um
        double kib = 246.0;        // mM-1/ms-1
        double kbi = 0.0022;       // ms-1
        double kox = 0.0002909;    // ms-1  
        double kred = 0.0000228;   // um/ms
        double kt;  
//        double kbp = 0.0022;       // ms;
        double kn93 = 0.0;         // concentration of KN-93, um
        double kbli = 0.0022;      // ms-1
        double kibl = 0.0008536;   // um-1.ms-1

        calmodulin = calmtotal*(caR*caR*caR*caR/(caR*caR*caR*caR+0.005*0.005*0.005*0.005));
        tcamk=fBound+fPhos+fOx+fOxP;
        kt = kbi/kib/(1/tcamk-1);
        ka = kbi*kt/(kt+0.01851);
        va = ka*fBound;
        va2 = ka*fOx;

        fBound=dt*(kib*calmodulin*fI+P*fPhos+kred*fOx-(kbi+kox*ros)*fBound-va)+fBound;
        fPhos=dt*(va+kred*fOxP-(kox*ros+P)*fPhos)+fPhos;
        fOx=dt*(kox*ros*fBound+P*fOxP-kred*fOx-va2)+fOx;
        fOxP=dt*(va2-P*fOxP+kox*ros*fPhos-kred*fOxP)+fOxP;
        fBlock=dt*(kibl*kn93*fI-kbli*fBlock)+fBlock;
        fI=1-fBound-fPhos-fOx-fOxP-fBlock;
        caM=(0.75*fBound+fPhos+fOxP+.5*fOx);
};

void HRD09Control::updateCurr()
{
   updateIna();    // Fast Na current
   updateInal();   // Late Na current
   updateInak();   // Na-K pump
   updateInaca();  // Na-Ca exchanger
   updateIcab();   // Background Ca current
   updateIclb();   // Background Cl current
   updateInacl();  // Na-Cl cotransport
   updateIlca();   // L-type Ca current
   updateIpca();   // Sarcolemmal Ca pump
   updateIks();    // Slow delayed rectifier current
   updateIkr();    // Rapid delayed rectifier current
   updateIkcl();   // K-Cl cotransporter
   updateIk1();    // Time-independent K current
   updateIkp();    // Plateau K current
   updateIto();    // Transient outward K current
   updateIto2();   // Ca-dep transient outward Cl current

   iNat=iNa+3*iNak+3*iNaca+3*iNacar+iNal;
   iCat=iCa+iCab+iPca-2*iNaca-2*iNacar;
   iKt=iKs+iKr+iK1+iTo+iKp-2*iNak;
   iClt=iTo2+iClb;
   
   iTotold=iTot;
   iTot=iNat+iCat+iKt+iClt;

};

void HRD09Control::updateConc()
{
   updateSrFlux(); // SR Ca fluxes
   updateIrel(); // SR Ca release
   updateCaSub();  // [Ca]subspace
   updateNai();  // [Na]i
   updateKi();   // [K]i
   updateSr();   // [Ca]jsr and [Ca]nsr
   updateCli();  // [Cl]i
   updateCai();  // [Ca]i
   updateCamk();  // CaMKII activity

};


// External stimulus.
int HRD09Control::externalStim(double stimval)
{
    iKt = iKt + 0.5*stimval;
    iClt = iClt + 0.5*stimval;
    iTot = iTot + stimval;
  
  return 1;
};

// Create map for easy retrieval of variable values.
void HRD09Control::makemap()
{
  vars["vOld"]=&vOld;
  vars["t"]=&t;
  vars["dVdt"]=&dVdt;
  vars["naI"]=&naI;
  vars["kI"]=&kI;
  vars["clI"]=&clI;
  vars["caI"]=&caI;
  vars["caR"]=&caR;
  vars["caJsr"]=&caJsr;
  vars["caNsr"]=&caNsr;
  vars["trpn"]=&trpn;
  vars["cmdn"]=&cmdn;
  vars["csqn"]=&csqn;
  vars["caM"]=&caM;
  vars["fBound"]=&fBound;
  vars["fBlock"]=&fBlock;
  vars["fI"]=&fI;
  vars["fOx"]=&fOx;
  vars["fOxP"]=&fOxP;
  vars["fPhos"]=&fPhos;
  vars["iRel"]=&iRel;
  vars["ryRopen"]=&ryRopen;
  vars["iUp"]=&iUp;
  vars["iLeak"]=&iLeak;
  vars["iTr"]=&iTr;
  vars["iDiff"]=&iDiff;
  vars["iCa"]=&iCa;
  vars["Gate.d"]=&Gate.d;
  vars["Gate.f"]=&Gate.f;
  vars["Gate.fca"]=&Gate.fca;
  vars["Gate.fca2"]=&Gate.fca2;
  vars["iCab"]=&iCab;
  vars["iPca"]=&iPca;
  vars["iNa"]=&iNa;
  vars["Gate.m"]=&Gate.m;
  vars["Gate.h"]=&Gate.h;
  vars["Gate.j"]=&Gate.j;
  vars["iNal"]=&iNal;
  vars["Gate.ml"]=&Gate.ml;
  vars["Gate.hl"]=&Gate.hl;
  vars["iNak"]=&iNak;
  vars["iNaca"]=&iNaca;
  vars["iNacar"]=&iNacar;
  vars["iTo"]=&iTo;
  vars["Gate.a"]=&Gate.a;
  vars["Gate.i"]=&Gate.i;
  vars["Gate.i2"]=&Gate.i2;
  vars["iTo2"]=&iTo2;
  vars["Gate.aa"]=&Gate.aa;
  vars["iKs"]=&iKs;
  vars["Gate.xs"]=&Gate.xs;
  vars["Gate.xs2"]=&Gate.xs2;
  vars["iKr"]=&iKr;
  vars["Gate.xr"]=&Gate.xr;
  vars["iK1"]=&iK1;
  vars["iKp"]=&iKp;
  vars["iCat"]=&iCat;
  vars["iNat"]=&iNat; //not initialized
  vars["iKt"]=&iKt; //not initialized
  vars["iClt"]=&iClt; //not initialized

}


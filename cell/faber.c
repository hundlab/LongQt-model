/* The Luo-Rudy Dynamic (LRd) Model of the Mammalian Ventricular Myocyte */
/* Gregory Faber */
/* This code requires a C++ compiler */
/* Detailed list of equations and model description are provided in */

/*  Circ Res 1991;68:1501-1526      */ 
/*  Circ Res 1994;74:1071-1096      */
/*  Circ Res 1994;74:1097-1113      */
/*  Circ Res 1995;77:140-152        */
/*  Biophys J 1995;68:949-964       */
/*  Cardiovasc Res 1997;35:256-272  */
/*  Circulation 1999;99:2466-2474   */
/*  Cardiovas Res 1999;42:530-542   */
/*  Nature 1999;400:566-569         */
/*  Circulation 2000;101:1192-1198  */
/*  Biophy J 2000;78:2392-2404      */

 
/*#include <iostream.h>
#include <iomanip.h>*/
#include <math.h>
/*#include <fstream.h>*/
#include <stdlib.h>
#include <stdio.h>

#define bcl 300   // Basic Cycle Length (ms)
#define beats 20   // Number of Beats

/* List of variables and paramaters (this code uses all global variables) */

	/* Creation of Data File */
	FILE *ap;
	FILE *apchanged;
	FILE *fmaxs;
	FILE *fpara;
	void prttofile ();	
	int printdata;	
	int printval;	

	/* Cell Geometry */
	const double l = 0.01;       // Length of the cell (cm)
	const double a = 0.0011;     // Radius of the cell (cm)
	const double pi = 3.141592;  // Pi
	double vcell;   // Cell volume (uL)
	double ageo;    // Geometric membrane area (cm^2)
	double acap;    // Capacitive membrane area (cm^2)
	double vmyo;    // Myoplasm volume (uL)
	double vmito;   // Mitochondria volume (uL)
	double vsr;     // SR volume (uL)
	double vnsr;    // NSR volume (uL)
	double vjsr;    // JSR volume (uL)
	double vcleft;  // Cleft volume (uL)
	
	/* Voltage */
	double v;       // Membrane voltage (mV)
	double vnew;    // New Voltage (mV)
	double dvdt;    // Change in Voltage / Change in Time (mV/ms)
	double dvdtnew; // New dv/dt (mV/ms)
	double boolien; // Boolien condition to test for dvdtmax
	
	/* Time Step */
	double dt;      // Time step (ms)
	double t;       // Time (ms)
	double udt;     // Universal Time Step
	int utsc;       // Universal Time Step Counter
	int nxstep;     // Interval Between Calculating Ion Currents
	int steps;      // Number of Steps
	int increment;  // Loop Control Variable
	
	/* Action Potential Duration and Max. Info */
	double vmax[beats];           // Max. Voltage (mV)
	double dvdtmax[beats];        // Max. dv/dt (mV/ms)
	double apd[beats];            // Action Potential Duration
	double toneapd[beats];        // Time of dv/dt Max.
	double ttwoapd[beats];        // Time of 90% Repolarization
	double rmbp[beats];           // Resting Membrane Potential
	double nair[beats];           // Intracellular Na At Rest
	double cair[beats];           // Intracellular Ca At Rest
	double caimax[beats];	      // Peak Intracellular Ca
	int i;                        // Stimulus Counter
	
	/* Total Current and Stimulus */
	double st;       // Constant Stimulus (uA/cm^2)
	double tstim;    // Time Stimulus is Applied (ms)
	double stimtime; // Time period during which stimulus is applied (ms)
	double it;       // Total current (uA/cm^2)

	/* Terms for Solution of Conductance and Reversal Potential */
	const double R = 8314;      // Universal Gas Constant (J/kmol*K)
	const double frdy = 96485;  // Faraday's Constant (C/mol)
	const double temp = 310;    // Temperature (K)

	/* Ion Valences */
	const double zna = 1;  // Na valence
	const double zk = 1;   // K valence
	const double zca = 2;  // Ca valence

	/* Ion Concentrations */
	double nai;    // Intracellular Na Concentration (mM)
	double nao;    // Extracellular Na Concentration (mM)
	double nabm;   // Bulk Medium Na Concentration (mM)
	double dnao;   // Change in Cleft Na Concentration (mM)
	double ki;     // Intracellular K Concentration (mM)
 	double ko;     // Extracellular K Concentration (mM)
	double kbm;    // Bulk Medium K Concentration (mM)
 	double dko;    // Change in Cleft K Concentration (mM)
	double cai;    // Intracellular Ca Concentration (mM)
	double cao;    // Extracellular Ca Concentration (mM)
	double cabm;   // Bulk Medium Ca Concentration (mM)
	double dcao;   // Change in Cleft Ca Concentration (mM)
	double cmdn;   // Calmodulin Buffered Ca Concentration (mM)
	double trpn;   // Troponin Buffered Ca Concentration (mM)
	double nsr;    // NSR Ca Concentration (mM)
	double jsr;    // JSR Ca Concentration (mM)
	double csqn;   // Calsequestrin Buffered Ca Concentration (mM)
	const double taudiff = 1000; // Diffusion Constant for Ion Movement from Bulk Medium to Cleft Space

	/* Myoplasmic Na Ion Concentration Changes */
	double naiont;  // Total Na Ion Flow (uA/uF)
	double dnai;    // Change in Intracellular Na Concentration (mM)

	/* Myoplasmic K Ion Concentration Changes */
	double kiont; // Total K Ion Flow (uA/uF)
	double dki;   // Change in Intracellular K Concentration (mM)

	/* NSR Ca Ion Concentration Changes */
	double dnsr;   // Change in [Ca] in the NSR (mM)
	double iup;    // Ca uptake from myo. to NSR (mM/ms)
	double ileak;  // Ca leakage from NSR to myo. (mM/ms)
	double kleak;  // Rate constant of Ca leakage from NSR (ms^-1)
	const double kmup = 0.00092;    // Half-saturation concentration of iup (mM)
	const double iupbar = 0.00875;  // Max. current through iup channel (mM/ms)
	const double nsrbar = 15;       // Max. [Ca] in NSR (mM) 
	
	/* JSR Ca Ion Concentration Changes */
	double djsr;                    // Change in [Ca] in the JSR (mM)
	const double tauon = 2;         // Time constant of activation of Ca release from JSR (ms)
	const double tauoff = 2;        // Time constant of deactivation of Ca release from JSR (ms)
	double tcicr;                   // t=0 at time of CICR (ms)
	double irelcicr;                // Ca release from JSR to myo. due to CICR (mM/ms)
	const double csqnth = 8.75;     // Threshold for release of Ca from CSQN due to JSR overload (mM)
	const double gmaxrel = 150;     // Max. rate constant of Ca release from JSR due to overload (ms^-1)
	double grelbarjsrol;            // Rate constant of Ca release from JSR due to overload (ms^-1)
	double greljsrol;               // Rate constant of Ca release from JSR due to CICR (ms^-1)
	double tjsrol;                  // t=0 at time of JSR overload (ms)
	double ireljsrol;               // Ca release from JSR to myo. due to JSR overload (mM/ms)
	const double csqnbar = 10;      // Max. [Ca] buffered in CSQN (mM)
	const double kmcsqn = 0.8;      // Equalibrium constant of buffering for CSQN (mM)
	double bjsr;                    // b Variable for analytical computation of [Ca] in JSR (mM)
	double cjsr;                    // c Variable for analytical computation of [Ca] in JSR (mM)
	double on;                      // Time constant of activation of Ca release from JSR (ms)
	double off;                     // Time constant of deactivation of Ca release from JSR (ms)
	double magrel;                  // Magnitude of Ca release
	double dcaiont;                 // Rate of change of Ca entry
	double dcaiontnew;              // New rate of change of Ca entry
	double caiontold;               // Old rate of change of Ca entry

	/* Translocation of Ca Ions from NSR to JSR */
	double itr;                // Translocation current of Ca ions from NSR to JSR (mM/ms)
	const double tautr = 180;  // Time constant of Ca transfer from NSR to JSR (ms)
	
	/* Myoplasmic Ca Ion Concentration Changes */
	double caiont;  // Total Ca Ion Flow (uA/uF)
	double dcai;    // Change in myoplasmic Ca concentration (mM)
	double catotal; // Total myoplasmic Ca concentration (mM)
	double bmyo;    // b Variable for analytical computation of [Ca] in myoplasm (mM)
	double cmyo;    // c Variable for analytical computation of [Ca] in myoplasm (mM)
	double dmyo;    // d Variable for analytical computation of [Ca] in myoplasm (mM)
	double gpig;    // Tribute to all the guinea pigs killed for the advancement of knowledge
	const double cmdnbar = 0.050;   // Max. [Ca] buffered in CMDN (mM)
	const double trpnbar = 0.070;   // Max. [Ca] buffered in TRPN (mM)
	const double kmcmdn = 0.00238;  // Equalibrium constant of buffering for CMDN (mM)
	const double kmtrpn = 0.0005;   // Equalibrium constant of buffering for TRPN (mM)

	/* Fast Sodium Current (time dependant) */
	double ina;    // Fast Na Current (uA/uF)
	double gna;    // Max. Conductance of the Na Channel (mS/uF)
	double ena;    // Reversal Potential of Na (mV)
	double am;     // Na alpha-m rate constant (ms^-1)
	double bm;     // Na beta-m rate constant (ms^-1)
	double ah;     // Na alpha-h rate constant (ms^-1)
	double bh;     // Na beta-h rate constant (ms^-1)
	double aj;     // Na alpha-j rate constant (ms^-1)
	double bj;     // Na beta-j rate constant (ms^-1)
	double m;      // Na activation
	double h;      // Na inactivation
	double j;      // Na inactivation

	/* Current through L-type Ca Channel */
	double ilca;    // Ca current through L-type Ca channel (uA/uF)
	double ilcana;  // Na current through L-type Ca channel (uA/uF)
	double ilcak ;  // K current through L-type Ca channel (uA/uF)
	double ilcatot; // Total current through the L-type Ca channel (uA/uF)
	double ibarca;  // Max. Ca current through Ca channel (uA/uF)
	double ibarna;  // Max. Na current through Ca channel (uA/uF)
	double ibark;   // Max. K current through Ca channel (uA/uF)
	double d;       // Voltage dependant activation gate
	double dss;     // Steady-state value of activation gate d 
	double taud;    // Time constant of gate d (ms^-1)
	double f;       // Voltage dependant inactivation gate
	double fss;     // Steady-state value of inactivation gate f
	double tauf;    // Time constant of gate f (ms^-1)
	double fca;     // Ca dependant inactivation gate
	const double kmca = 0.0006;     // Half-saturation concentration of Ca channel (mM)
	const double pca = 0.00054;     // Permiability of membrane to Ca (cm/s)
	const double gacai = 1;         // Activity coefficient of Ca
	const double gacao = 0.341;     // Activity coefficient of Ca
	const double pna = 0.000000675; // Permiability of membrane to Na (cm/s)
	const double ganai = 0.75;      // Activity coefficient of Na
	const double ganao = 0.75;      // Activity coefficient of Na
	const double pk = 0.000000193;  // Permiability of membrane to K (cm/s)
	const double gaki = 0.75;       // Activity coefficient of K
	const double gako = 0.75;       // Activity coefficient of K

	/* Current through T-type Ca Channel */
	double icat;    // Ca current through T-type Ca channel (uA/uF)
	double gcat;    // Max. Conductance of the T-type Ca channel (mS/uF)
	double eca;     // Reversal Potential of the T-type Ca channel (mV)
	double b;       // Voltage dependant activation gate
	double bss;     // Steady-state value of activation gate b 
	double taub;    // Time constant of gate b (ms^-1)
	double g;       // Voltage dependant inactivation gate
	double gss;     // Steady-state value of inactivation gate g
	double taug;    // Time constant of gate g (ms^-1)

	/* Rapidly Activating Potassium Current */
	double ikr;   // Rapidly Activating K Current (uA/uF)
	double gkr;   // Channel Conductance of Rapidly Activating K Current (mS/uF)
	double ekr;   // Reversal Potential of Rapidly Activating K Current (mV)
	double xr;    // Rapidly Activating K time-dependant activation
	double xrss;  // Steady-state value of inactivation gate xr
	double tauxr; // Time constant of gate xr (ms^-1)
	double r;     // K time-independant inactivation
	
	/* Slowly Activating Potassium Current */
	double iks;   // Slowly Activating K Current (uA/uF)
	double gks;   // Channel Conductance of Slowly Activating K Current (mS/uF)
	double eks;   // Reversal Potential of Slowly Activating K Current (mV)
	double xs1;    // Slowly Activating K time-dependant activation
	double xs1ss;  // Steady-state value of inactivation gate xs1
	double tauxs1; // Time constant of gate xs1 (ms^-1)
	double xs2;    // Slowly Activating K time-dependant activation
	double xs2ss;  // Steady-state value of inactivation gate xs2
	double tauxs2; // Time constant of gate xs2 (ms^-1)
	const double prnak = 0.01833;  // Na/K Permiability Ratio
	
	/* Potassium Current (time-independant) */
	double iki;    // Time-independant K current (uA/uF)
	double gki;    // Channel Conductance of Time Independant K Current (mS/uF)
	double eki;    // Reversal Potential of Time Independant K Current (mV)
	double aki;    // K alpha-ki rate constant (ms^-1)
	double bki;    // K beta-ki rate constant (ms^-1)
	double kin;    // K inactivation

	/* Plateau Potassium Current */
	double ikp;    // Plateau K current (uA/uF)
	double gkp;    // Channel Conductance of Plateau K Current (mS/uF)
	double ekp;    // Reversal Potential of Plateau K Current (mV)
	double kp;     // K plateau factor	

	/* Na-Activated K Channel */
	double ikna;   // Na activated K channel
	double pona;   // Open probability dependant on Nai
	double pov;    // Open probability dependant on Voltage
	double ekna;   // Reversal potential
	const double gkna = 0.12848;   // Maximum conductance (mS/uF)
	const double nkna = 2.8;       // Hill coefficient for Na dependance
	const double kdkna = 66;       // Dissociation constant for Na dependance(mM)
	
	/* ATP-Sensitive K Channel */
	double ikatp;    // ATP-sensitive K current (uA/uF)
	double ekatp;    // K reversal potential (mV)
	double gkbaratp; // Conductance of the ATP-sensitive K channel (mS/uF)
	double gkatp;    // Maximum conductance of the ATP-sensitive K channel (mS/uF)
	double patp;     // Percentage availibility of open channels
	const double natp = 0.24;          // K dependence of ATP-sensitive K current
	const double nicholsarea = 0.00005; // Nichol's ares (cm^2)
	const double atpi = 3;             // Intracellular ATP concentraion (mM)
	const double hatp = 2;             // Hill coefficient
	const double katp = 0.250;         // Half-maximal saturation point of ATP-sensitive K current (mM)
	
	/* Ito Transient Outward Current (Dumaine et al. Circ Res 1999;85:803-809) */
	double ito;	  // Transient outward current
	double gitodv;	  // Maximum conductance of Ito
	double ekdv;	  // Reversal Potential of Ito
	double rvdv;      // Time independant voltage dependence of Ito
	double zdv;       // Ito activation
	double azdv;      // Ito alpha-z rate constant
	double bzdv;      // Ito beta-z rate constant
	double tauzdv;	  // Time constant of z gate
	double zssdv;     // Steady-state value of z gate
	double ydv;       // Ito inactivation
	double aydv;      // Ito alpha-y rate constant
	double bydv;      // Ito beta-y rate constant
	double tauydv;	  // Time constant of y gate
	double yssdv;     // Steady-state value of y gate
		
	/* Sodium-Calcium Exchanger V-S */
	double inaca;               // NaCa exchanger current (uA/uF)
	const double c1 = .00025;   // Scaling factor for inaca (uA/uF)
	const double c2 = 0.0001;   // Half-saturation concentration of NaCa exhanger (mM)
	const double gammas = .15;  // Position of energy barrier controlling voltage dependance of inaca

	/* Sodium-Potassium Pump */
	double inak;    // NaK pump current (uA/uF)
	double fnak;    // Voltage-dependance parameter of inak
	double sigma;   // [Na]o dependance factor of fnak
	const double ibarnak = 2.25;   // Max. current through Na-K pump (uA/uF)
	const double kmnai = 10;    // Half-saturation concentration of NaK pump (mM)
	const double kmko = 1.5;    // Half-saturation concentration of NaK pump (mM)
	
	/* Nonspecific Ca-activated Current */
	double insna;     // Non-specific Na current (uA/uF)
	double insk;      // Non-specific K current (uA/uF)
	double ibarnsna;  // Max. Na current through NSCa channel (uA/uF)
	double ibarnsk;   // Max. K current through NSCa channel (uA/uF)
	const double pnsca = 0.000000175;  // Permiability of channel to Na and K (cm/s)
	const double kmnsca = 0.0012;      // Half-saturation concentration of NSCa channel (mM)

	/* Sarcolemmal Ca Pump */
	double ipca;                 // Sarcolemmal Ca pump current (uA/uF)
	const double ibarpca = 1.15; // Max. Ca current through sarcolemmal Ca pump (uA/uF)
	const double kmpca = 0.0005; // Half-saturation concentration of sarcolemmal Ca pump (mM)
	
	/* Ca Background Current */
	double icab;  // Ca background current (uA/uF)
	double gcab;  // Max. conductance of Ca background (mS/uF)
	double ecan;  // Nernst potential for Ca (mV)

	/* Na Background Current */
	double inab;  // Na background current (uA/uF)
	double gnab;  // Max. conductance of Na background (mS/uF)
	double enan;  // Nernst potential for Na (mV)

	/* Ion Current Functions */	
	void comp_ina ();    // Calculates Fast Na Current
	void comp_ical ();   // Calculates Currents through L-Type Ca Channel
	void comp_icat ();   // Calculates Currents through T-Type Ca Channel
	void comp_ikr ();    // Calculates Rapidly Activating K Current
	void comp_iks ();    // Calculates Slowly Activating K Current
	void comp_iki ();    // Calculates Time-Independant K Current
	void comp_ikp ();    // Calculates Plateau K Current
	void comp_ikna ();   // Calculates Na-activated K Current
	void comp_ikatp ();  // Calculates ATP-Sensitive K Current
	void comp_ito ();    // Calculates Transient Outward Current
	void comp_inaca ();  // Calculates Na-Ca Exchanger Current
	void comp_inak ();   // Calculates Na-K Pump Current
	void comp_insca ();  // Calculates Non-Specific ca-Activated Current
	void comp_ipca ();   // Calculates Sarcolemmal Ca Pump Current
	void comp_icab ();   // Calculates Ca Background Current
	void comp_inab ();   // Calculates Na Background Current
	void comp_it ();     // Calculates Total Current

	/* Ion Concentration Functions */
	void conc_nai ();    // Calculates new myoplasmic Na ion concentration
	void conc_ki ();     // Calculates new myoplasmic K ion concentration
	void conc_nsr ();    // Calculates new NSR Ca ion concentration
	void conc_jsr ();    // Calculates new JSR Ca ion concentration
	void calc_itr ();    // Calculates Translocation of Ca from NSR to JSR
	void conc_cai ();    // Calculates new myoplasmic Ca ion concentration
	void conc_cleft ();  // Calculates new cleft ion concentrations

int main ()
{	
	/* Opening of Datafiles */
	ap = fopen("ap", "w");
	fpara = fopen("fpara", "w");
	fmaxs = fopen("fmaxs", "w");
	printdata = 60;
	
	/* Cell Geometry */
	vcell = 1000*pi*a*a*l;     //   3.801e-5 uL
	ageo = 2*pi*a*a+2*pi*a*l;  //   7.671e-5 cm^2
	acap = ageo*2;             //   1.534e-4 cm^2
	vmyo = vcell*0.68;
	vmito = vcell*0.26;
	vsr = vcell*0.06;
	vnsr = vcell*0.0552;
	vjsr = vcell*0.0048;
	vcleft = vcell*0.12/0.88;
	
	/* Time Loop Conditions */
	t = 0;           // Time (ms)
	udt = 0.002;     // Time step (ms)
	steps = (bcl*beats)/udt; // Number of ms
	st = -80;        // Stimulus
	tstim = 10;       // Time to begin stimulus
	stimtime = 10;   // Initial Condition for Stimulus
	v = -90;       // Initial Voltage (mv)

	/* Beginning Ion Concentrations */
	nai = 9;       // Initial Intracellular Na (mM)
	nao = 140;      // Initial Extracellular Na (mM)
	nabm = 140;     // Initial Bulk Medium Na (mM)
	ki = 141.2;       // Initial Intracellular K (mM)
	ko = 4.5;       // Initial Extracellular K (mM)
	kbm = 4.5;      // Initial Bulk Medium K (mM)
	cai = 0.00006;  // Initial Intracellular Ca (mM)
	cao = 1.8;      // Initial Extracellular Ca (mM)
	cabm = 1.8;     // Initial Bulk Medium Ca (mM)

	/* Initial Gate Conditions */
	m = 0.0008;
	h = 0.993771;
	j = 0.995727;
	d = 3.210618e-06;
	f = 0.999837;
	xs1 = 0.00445683;
	xs2 = 0.00445683;
	xr = 0.000124042;
	b = 0.000970231;
	g = 0.994305;
	zdv = 0.0120892;
	ydv = 0.999978;
	
	/* Initial Conditions */
	grelbarjsrol = 0;
	tjsrol = 25;
	tcicr = 25;
	jsr = 1.838;
	nsr = 1.838;
	trpn = 0.0143923;
	cmdn = 0.00257849;
	csqn = 6.97978;
	boolien = 1;
	dt = udt;
	utsc = 50;
	dcaiont = 0;
	i=-1;

	/* Beginning of Time Loop */
	for (increment = 0; increment < steps; increment++)
	{
		if(abs(dvdt)<0.25 && v<0)
			{nxstep = 500;}
		else
			{nxstep = 50;}
		
		if(utsc>=nxstep || dvdt>5 || irelcicr>0.01 || (t>=(tstim-udt) && t<=(tstim+udt)) || (stimtime>=0 && stimtime<0.5))
		{

		/* List of functions called for each timestep, currents commented out are only used when modeling pathological conditions */

		comp_ina ();
		comp_ical ();
		comp_icat ();
		comp_ikr ();
		comp_iks ();
		comp_iki ();
		comp_ikp ();
		//comp_ikna ();
		//comp_ikatp ();
		//comp_ito ();
		comp_inaca ();
		comp_inak ();
		comp_insca ();
		comp_ipca ();
		comp_icab ();
		comp_inab ();
		comp_it ();
		
		conc_nai ();
		conc_ki ();
		conc_nsr ();
		conc_jsr ();
		calc_itr ();
		conc_cai ();
		conc_cleft ();
		
		utsc = 0;
		dt = 0;

		}

			if(dvdt>3 || irelcicr>.01)
			{printval = 50;}
			else
			{printval = 750;}
			
			if(printdata>=printval) 
			{prttofile();	
			printdata = 0;}
			
			printdata = printdata+1;

	
	vnew = v-it*udt;
	dvdtnew = (vnew-v)/udt;
	
	if(i>=0)
		{
		if (vnew>vmax[i])
			vmax[i] = vnew;
		if (cai>caimax[i])
			caimax[i] = cai;
		if (dvdtnew>dvdtmax[i])
			{dvdtmax[i] = dvdtnew;
			toneapd[i] = t;}
		if (vnew>=(vmax[i]-0.9*(vmax[i]-rmbp[i])))
			ttwoapd[i] = t;
		}

	if(csqn>=csqnth && tjsrol>50)
		{grelbarjsrol = 4;
		tjsrol = 0;
		/*cout << "Spontaneous Release occured at time " << t << endl;*/
		}	


	v = vnew;
	dvdt = dvdtnew;
	caiontold = caiont;
	dcaiont = dcaiontnew;
	
	dt = dt+udt;
	utsc = utsc+1;
	
	t = t+udt;
	

	}

	fprintf(fpara,"%.3f\n%g\n%g\n%g\n%g\n%g\n%g\n%g\n%g\n%g\n%g\n%g\n%g\n%g\n%g\n%g\n%g\n%g\n%g\n%g\n%g\n%g\n", t, v, nai, ki, cai, jsr, nsr, nao, ko, cao, m, h, j, d, f, xs1, xs2, xr, b, g, ydv, zdv);
	for(i=0;i<beats;i++)
	{apd[i] = ttwoapd[i]-toneapd[i];
	fprintf(fmaxs, "%i\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\n", i, vmax[i], dvdtmax[i], apd[i], toneapd[i], ttwoapd[i], nair[i], cair[i], caimax[i], rmbp[i]);}

	/*cout << "\a\a";*/
}

/********************************************************/

/* Functions that describe the currents begin here */


void comp_ina ()
{
	gna = 16;
	ena = ((R*temp)/frdy)*log(nao/nai);

	am = 0.32*(v+47.13)/(1-exp(-0.1*(v+47.13)));
	bm = 0.08*exp(-v/11);
	
	if (v < -40)
	{ah = 0.135*exp((80+v)/-6.8);
	bh = 3.56*exp(0.079*v)+310000*exp(0.35*v);
	aj = (-127140*exp(0.2444*v)-0.00003474*exp(-0.04391*v))*((v+37.78)/(1+exp(0.311*(v+79.23))));
	bj = (0.1212*exp(-0.01052*v))/(1+exp(-0.1378*(v+40.14)));}

	else
	{ah = 0;
	bh = 1/(0.13*(1+exp((v+10.66)/-11.1)));
	aj = 0;
	bj = (0.3*exp(-0.0000002535*v))/(1+exp(-0.1*(v+32)));}
	
	h = ah/(ah+bh)-((ah/(ah+bh))-h)*exp(-dt/(1/(ah+bh)));
	j = aj/(aj+bj)-((aj/(aj+bj))-j)*exp(-dt/(1/(aj+bj)));
	m = am/(am+bm)-((am/(am+bm))-m)*exp(-dt/(1/(am+bm)));

	
	ina = gna*m*m*m*h*j*(v-ena);
}

void comp_ical ()
{
	dss = 1/(1+exp(-(v+10)/6.24));
	taud = dss*(1-exp(-(v+10)/6.24))/(0.035*(v+10));

	fss = (1/(1+exp((v+32)/8)))+(0.6/(1+exp((50-v)/20)));
	tauf = 1/(0.0197*exp(-pow(0.0337*(v+10),2))+0.02);

	d = dss-(dss-d)*exp(-dt/taud);
	f = fss-(fss-f)*exp(-dt/tauf);
	
	ibarca = pca*zca*zca*((v*frdy*frdy)/(R*temp))*((gacai*cai*exp((zca*v*frdy)/(R*temp))-gacao*cao)/(exp((zca*v*frdy)/(R*temp))-1));
	ibarna = pna*zna*zna*((v*frdy*frdy)/(R*temp))*((ganai*nai*exp((zna*v*frdy)/(R*temp))-ganao*nao)/(exp((zna*v*frdy)/(R*temp))-1));
	ibark = pk*zk*zk*((v*frdy*frdy)/(R*temp))*((gaki*ki*exp((zk*v*frdy)/(R*temp))-gako*ko)/(exp((zk*v*frdy)/(R*temp))-1));
		
	fca = 1/(1+cai/kmca);
	
	ilca = d*f*fca*ibarca;
	ilcana = d*f*fca*ibarna;
	ilcak = d*f*fca*ibark;
	
	ilcatot = ilca+ilcana+ilcak;
}

void comp_icat ()
{
	bss = 1/(1+exp(-(v+14)/10.8));
	taub = 3.7+6.1/(1+exp((v+25)/4.5));

	gss = 1/(1+exp((v+60)/5.6));
	if (v<=0)
	taug = -0.875*v+12;
	else
	taug = 12;

	b = bss-(bss-b)*exp(-dt/taub);
	g = gss-(gss-g)*exp(-dt/taug);
	
	gcat = 0.05;
	eca = (R*temp/(2*frdy))*log(cao/cai);
	
	icat = gcat*b*b*g*(v-eca);
}

void comp_ikr ()
{
	gkr = 0.02614*sqrt(ko/5.4);
	ekr = ((R*temp)/frdy)*log(ko/ki);

	xrss = 1/(1+exp(-(v+21.5)/7.5));
	tauxr = 1/(0.00138*(v+14.2)/(1-exp(-0.123*(v+14.2)))+0.00061*(v+38.9)/(exp(0.145*(v+38.9))-1));
		
	xr = xrss-(xrss-xr)*exp(-dt/tauxr);
	
	r = 1/(1+exp((v+9)/22.4));
		
	ikr = gkr*xr*r*(v-ekr);
}

void comp_iks ()
{
	gks = 0.433*(1+0.6/(1+pow((0.000038/cai),1.4)));
	eks = ((R*temp)/frdy)*log((ko+prnak*nao)/(ki+prnak*nai));

	xs1ss = 1/(1+exp(-(v-1.5)/16.7));
	xs2ss = xs1ss;
	tauxs1 = 1/(0.0000719*(v+30)/(1-exp(-0.148*(v+30)))+0.000131*(v+30)/(exp(0.0687*(v+30))-1));
	tauxs2 = 4*tauxs1;
			
	xs1 = xs1ss-(xs1ss-xs1)*exp(-dt/tauxs1);
	xs2 = xs2ss-(xs2ss-xs2)*exp(-dt/tauxs2);
	
	iks = gks*xs1*xs2*(v-eks);
}

void comp_iki ()
{
	gki = 0.75*(sqrt(ko/5.4));
	eki = ((R*temp)/frdy)*log(ko/ki);

	aki = 1.02/(1+exp(0.2385*(v-eki-59.215)));
	bki = (0.49124*exp(0.08032*(v-eki+5.476))+exp(0.06175*(v-eki-594.31)))/(1+exp(-0.5143*(v-eki+4.753)));
	
	kin = aki/(aki+bki);
	
	iki = gki*kin*(v-eki);
}

void comp_ikp ()
{
	gkp = 0.00552;
	ekp = eki;

	kp = 1/(1+exp((7.488-v)/5.98));	

	ikp = gkp*kp*(v-ekp);
}

void comp_ikna ()
{
	ekna = ((R*temp)/frdy)*log(ko/ki);
	pona = 0.85/(1+pow((kdkna/nai),2.8));
	pov = 0.8-0.65/(1+exp((v+125)/15));
	ikna = gkna*pona*pov*(v-ekna);	
}

void comp_ikatp ()
{
/* Note: If you wish to use this current in your simulations, there are additional       */
/* changes which must be made to the code as detailed in Cardiovasc Res 1997;35:256-272  */

	ekatp = ((R*temp)/frdy)*log(ko/ki);
	gkatp = 0.000195/nicholsarea;
	patp = 1/(1+(pow((atpi/katp),hatp)));
	gkbaratp = gkatp*patp*(pow((ko/4),natp));

	ikatp = gkbaratp*(v-ekatp);	
}

void comp_ito ()
{
	gitodv = 0.5;
	ekdv = ((R*temp)/frdy)*log((ko)/(ki));
	rvdv = exp(v/100);
	
	azdv = (10*exp((v-40)/25))/(1+exp((v-40)/25));
	bzdv = (10*exp(-(v+90)/25))/(1+exp(-(v+90)/25));
	tauzdv = 1/(azdv+bzdv);
	zssdv = azdv/(azdv+bzdv);
	zdv = zssdv-(zssdv-zdv)*exp(-dt/tauzdv);

	aydv = 0.015/(1+exp((v+60)/5));
	bydv = (0.1*exp((v+25)/5))/(1+exp((v+25)/5));
	tauydv = 1/(aydv+bydv);
	yssdv = aydv/(aydv+bydv);
	ydv = yssdv-(yssdv-ydv)*exp(-dt/tauydv);
	
	ito = gitodv*zdv*zdv*zdv*ydv*rvdv*(v-ekdv);
}

void comp_inaca ()
{
	inaca = c1*exp((gammas-1)*v*frdy/(R*temp))*((exp(v*frdy/(R*temp))*nai*nai*nai*cao-nao*nao*nao*cai)/(1+c2*exp((gammas-1)*v*frdy/(R*temp))*(exp(v*frdy/(R*temp))*nai*nai*nai*cao+nao*nao*nao*cai)));
}

void comp_inak ()
{
	sigma = (exp(nao/67.3)-1)/7;

	fnak = 1/(1+0.1245*exp((-0.1*v*frdy)/(R*temp))+0.0365*sigma*exp((-v*frdy)/(R*temp)));

	inak = ibarnak*fnak*(1/(1+pow(kmnai/nai,2)))*(ko/(ko+kmko));
}

void comp_insca ()
{
	ibarnsna = pnsca*zna*zna*((v*frdy*frdy)/(R*temp))*((ganai*nai*exp((zna*v*frdy)/(R*temp))-ganao*nao)/(exp((zna*v*frdy)/(R*temp))-1));
	ibarnsk = pnsca*zk*zk*((v*frdy*frdy)/(R*temp))*((gaki*ki*exp((zk*v*frdy)/(R*temp))-gako*ko)/(exp((zk*v*frdy)/(R*temp))-1));

	insna = ibarnsna/(1+pow(kmnsca/cai,3));
	insk = ibarnsk/(1+pow(kmnsca/cai,3));
}

void comp_ipca ()
{
	ipca = (ibarpca*cai)/(kmpca+cai);	
}

void comp_icab ()
{
	gcab = 0.003016;
	ecan = ((R*temp)/(2*frdy))*log(cao/cai);
		
	icab = gcab*(v-ecan);
}

void comp_inab ()
{
	gnab = 0.004;
	enan = ena;
	
	inab = gnab*(v-enan);
}

/* Total sum of currents is calculated here, if the time is between stimtime = 0 and stimtime = 0.5, a stimulus is applied */


void comp_it ()
{
	naiont = ina+inab+ilcana/*+insna*/+3*inak+3*inaca;
	kiont = ikr+iks+iki+ikp+ilcak/*+insk*/-2*inak+ito+ikna+ikatp;
	caiont = ilca+icab+ipca-2*inaca+icat;
	
	if (t>=tstim && t<(tstim+dt))
	{stimtime = 0;
	tstim = tstim+bcl;
	boolien = 0;
	i = i+1;
	rmbp[i]=v;
	nair[i] = nai;
	cair[i] = cai;}
	
	if(stimtime>=0 &&  stimtime<0.5)
	{it = st+naiont+kiont+caiont;}
	else
	{it = naiont+kiont+caiont;}

	stimtime = stimtime+dt;
}


/* Functions that calculate intracellular ion concentrations begins here */


void conc_nai ()
{
// The units of dnai is in mM.  Note that naiont should be multiplied by the
// cell capacitance to get the correct units.  Since cell capacitance = 1 uF/cm^2,
// it doesn't explicitly appear in the equation below.
// This holds true for the calculation of dki and dcai. */

	dnai = -dt*(naiont*acap)/(vmyo*zna*frdy);
	nai = dnai + nai;
}

void conc_ki ()
{
	if(stimtime>=0 &&  stimtime<0.5)
	{dki = -dt*((kiont+st)*acap)/(vmyo*zk*frdy);}
	else
	{dki = -dt*(kiont*acap)/(vmyo*zk*frdy);}

	ki = dki + ki;
}

void conc_nsr ()
{
	kleak = iupbar/nsrbar;
	ileak = kleak*nsr;

	iup = iupbar*cai/(cai+kmup);

	dnsr = dt*(iup-ileak-itr*vjsr/vnsr);
	nsr = nsr+dnsr;
}

void conc_jsr ()
{
	dcaiontnew = (caiont-caiontold)/dt;

	if(v>-35 && dcaiontnew<dcaiont && boolien==0)
		{boolien = 1;
		tcicr = 0;}
	
	on = 1/(1+exp((-tcicr+4)/.5));
	off = (1-1/(1+exp((-tcicr+4)/.5)));
	magrel = 1/(1+exp(((ilca+icab+ipca-2*inaca+icat)+5)/0.9));
	
	irelcicr = gmaxrel*on*off*magrel*(jsr-cai);

	tcicr = tcicr+dt;

	greljsrol = grelbarjsrol*(1-exp(-tjsrol/tauon))*exp(-tjsrol/tauoff);
	ireljsrol = greljsrol*(jsr-cai);
	tjsrol = tjsrol+dt;

	csqn = csqnbar*(jsr/(jsr+kmcsqn));
	djsr = dt*(itr-irelcicr-ireljsrol);
	bjsr = csqnbar-csqn-djsr-jsr+kmcsqn;
	cjsr = kmcsqn*(csqn+djsr+jsr);

	jsr = (sqrt(bjsr*bjsr+4*cjsr)-bjsr)/2;
}

void calc_itr ()
{
	itr = (nsr-jsr)/tautr;
}

void conc_cai ()
{
	dcai = -dt*(((caiont*acap)/(vmyo*zca*frdy))+((iup-ileak)*vnsr/vmyo)-(irelcicr*vjsr/vmyo)-(ireljsrol*vjsr/vmyo));
	trpn = trpnbar*(cai/(cai+kmtrpn));
	cmdn = cmdnbar*(cai/(cai+kmcmdn));

	catotal = trpn+cmdn+dcai+cai;
	bmyo = cmdnbar+trpnbar-catotal+kmtrpn+kmcmdn;
	cmyo = (kmcmdn*kmtrpn)-(catotal*(kmtrpn+kmcmdn))+(trpnbar*kmcmdn)+(cmdnbar*kmtrpn);
	dmyo = -kmtrpn*kmcmdn*catotal;

	gpig = sqrt(bmyo*bmyo-3*cmyo);

	cai = (2*gpig/3)*cos(acos((9*bmyo*cmyo-2*bmyo*bmyo*bmyo-27*dmyo)/(2*pow((bmyo*bmyo-3*cmyo),1.5)))/3)-(bmyo/3);
}

void conc_cleft()
{
	dnao = dt*((nabm-nao)/taudiff+naiont*acap/(vcleft*frdy));
	nao = dnao+nao;
	dko = dt*((kbm-ko)/taudiff+kiont*acap/(vcleft*frdy));
	ko = dko+ko;
	dcao = dt*((cabm-cao)/taudiff+caiont*acap/(vcleft*frdy*2));
	cao = dcao+cao;
}

/* Values are printed to a file called ap. The voltage and currents can be plotted versus time using graphing software. */

void prttofile()
{
if(t>(300) && t<(600))
	{fprintf(apchanged,"%.3f\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\n", t, v, insna, insk, ilca, ilcana, ilcak, inaca, nai, ki, caiont);}
}

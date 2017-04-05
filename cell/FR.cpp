//
//  main.cpp
//  /* The Luo-Rudy Dynamic (LRd) Model of the Mammalian Ventricular Myocyte */
/* Gregory Faber */
//
//  Created by Shana Bee on 2/17/17.
//  Copyright © 2017 Shana Bee. All rights reserved.
//

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "FR.h"

//Constructor
FR::FR()
{
    this->Initialize();
};
FR::FR(FR& toCopy) : Cell(toCopy){
    this->Initialize();
    CellKernel::copyVarPar(toCopy);
}

void FR::Initialize(){
    dt=dtmin = 0.005;
    dtmed = 0.05;
    dtmax = 0.05;
    dvcut = 1.0;
    apTime = 0.0;

    dVdt=dVdtmax=0.0;
    t=0.0;
    vOld = vNew = -58.600291137693;

    nao = 140;
    cao = 2.0;
    ko = 5.4;
    type = "Faber Rudy Model";

    
    /* Cell Geometry */
    cellLength = 0.01;       // Length of the cell (cm)
    cellRadius = 0.0011;     // Radius of the cell (cm)
    Vcell = 1000*pi*a*a*l;     //   3.801e-5 uL
    AGeo = 2*pi*a*a+2*pi*a*l;  //   7.671e-5 cm^2
    ACap = AGeo*2;             //   1.534e-4 cm^2
    Vmyo = Vcell*0.68;
    vmito = Vcell*0.26;
    vsr = Vcell*0.06;
    Vnsr = Vcell*0.0552;
    Vjsr = Vcell*0.0048;
    vcleft = Vcell*0.12/0.88;
    
    /* Terms for Solution of Conductance and Reversal Potential */
    RGAS = 8314;      // Universal Gas Constant (J/kmol*K)
    FDAY = 96485;  // Faraday's Constant (C/mol)
    TEMP = 310;    // Temperature (K)
    
    /* Ion Valences */
    zna = 1;  // Na valence
    zk = 1;   // K valence
    zca = 2;  // Ca valence

    gacai = 1;
    gacao = 1;
    icalFactor = 1;
    icattFactor = 1;
    ikrFactor = 1;
    iksFactor = 1;
    itoFactor = 1;
    isusFactor = 1;
    ikiFactor = 1;
    //ikachFactor = 1;
    istFactor = 1;
    inabFactor = 1;
    inakFactor = 1;
    inacaFactor = 1;
    //ihFactor = 1;
    iupFactor = 1;
    irelFactor = 1;



    /* Ion Concentrations */
    taudiff = 1000; // Diffusion Constant for Ion Movement from Bulk Medium to Cleft Space
    
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
//    dt = udt;
    utsc = 50;
    dcaiont = 0;
    i=-1;

    /* NSR Ca Ion Concentration Changes */
    kmup = 0.00092;    // Half-saturation concentration of iup (mM)
    iupbar = iupFactor*0.00875;  // Max. current through iup channel (mM/ms)
    nsrbar = 15;       // Max. [Ca] in NSR (mM)
    
    /* JSR Ca Ion Concentration Changes */
    tauon = 2;         // Time constant of activation of Ca release from JSR (ms)
    tauoff = 2;        // Time constant of deactivation of Ca release from JSR (ms)
    csqnth = 8.75;     // Threshold for release of Ca from CSQN due to JSR overload (mM)
    gmaxrel = 150;     // Max. rate constant of Ca release from JSR due to overload (ms^-1)
    csqnbar = 10;      // Max. [Ca] buffered in CSQN (mM)
    kmcsqn = 0.8;      // Equalibrium constant of buffering for CSQN (mM)
    
    /* Translocation of Ca Ions from NSR to JSR */
    tautr = 180;  // Time constant of Ca transfer from NSR to JSR (ms)
    
    /* Myoplasmic Ca Ion Concentration Changes */
    cmdnbar = 0.050;   // Max. [Ca] buffered in CMDN (mM)
    trpnbar = 0.070;   // Max. [Ca] buffered in TRPN (mM)
    kmcmdn = 0.00238;  // Equalibrium constant of buffering for CMDN (mM)
    kmtrpn = 0.0005;   // Equalibrium constant of buffering for TRPN (mM)
    
    /* Current through L-type Ca Channel */
    kmca = 0.0006;     // Half-saturation concentration of Ca channel (mM)
    pca = 0.00054;     // Permiability of membrane to Ca (cm/s)
    gacai = 1;         // Activity coefficient of Ca
    gacao = 0.341;     // Activity coefficient of Ca
    pna = 0.000000675; // Permiability of membrane to Na (cm/s)
    ganai = 0.75;      // Activity coefficient of Na
    ganao = 0.75;      // Activity coefficient of Na
    pk = 0.000000193;  // Permiability of membrane to K (cm/s)
    gaki = 0.75;       // Activity coefficient of K
    gako = 0.75;       // Activity coefficient of K
    
    /* Slowly Activating Potassium Current */
    prnak = 0.01833;  // Na/K Permiability Ratio
    
    /* Na-Activated K Channel */
    gkna = 0.12848;   // Maximum conductance (mS/uF)
    nkna = 2.8;       // Hill coefficient for Na dependance
    kdkna = 66;       // Dissociation constant for Na dependance(mM)

    /* ATP-Sensitive K Channel */
    natp = 0.24;          // K dependence of ATP-sensitive K current
    nicholsarea = 0.00005; // Nichol's ares (cm^2)
    atpi = 3;             // Intracellular ATP concentraion (mM)
    hatp = 2;             // Hill coefficient
    katp = 0.250;         // Half-maximal saturation point of ATP-sensitive K current (mM)

    
    /* Sodium-Calcium Exchanger V-S */

    c1 = .00025;   // Scaling factor for inaca (uA/uF)
    c2 = 0.0001;   // Half-saturation concentration of NaCa exhanger (mM)
    gammas = .15;  // Position of energy barrier controlling voltage dependance of inaca

    /* Sodium-Potassium Pump */
    ibarnak = 2.25;   // Max. current through Na-K pump (uA/uF)
    kmnai = 10;    // Half-saturation concentration of NaK pump (mM)
    kmko = 1.5;    // Half-saturation concentration of NaK pump (mM)

    /* Nonspecific Ca-activated Current */
    pnsca = 0.000000175;  // Permiability of channel to Na and K (cm/s)
    kmnsca = 0.0012;      // Half-saturation concentration of NSCa channel (mM)

        /* Sarcolemmal Ca Pump */
    ibarpca = 1.15; // Max. Ca current through sarcolemmal Ca pump (uA/uF)
    kmpca = 0.0005; // Half-saturation concentration of sarcolemmal Ca pump (mM)

    makemap();


    //make variable map so user can alter it
 //   this->makemap();

};


FR* FR::clone(){
    return new FR(*this);
}

//Destructor

FR::~FR()
{
    
};

//* Functions that describe the currents begin here */


void FR::comp_ina ()
{
    gna = 16;
    ena = ((RGAS*TEMP)/FDAY)*log(nao/nai);
    
    am = 0.32*(vOld+47.13)/(1-exp(-0.1*(vOld+47.13)));
    bm = 0.08*exp(-vOld/11);
    
    if (vOld < -40)
    {ah = 0.135*exp((80+vOld)/-6.8);
        bh = 3.56*exp(0.079*vOld)+310000*exp(0.35*vOld);
        aj = (-127140*exp(0.2444*vOld)-0.00003474*exp(-0.04391*vOld))*((vOld+37.78)/(1+exp(0.311*(vOld+79.23))));
        bj = (0.1212*exp(-0.01052*vOld))/(1+exp(-0.1378*(vOld+40.14)));}
    
    else
    {ah = 0;
        bh = 1/(0.13*(1+exp((vOld+10.66)/-11.1)));
        aj = 0;
        bj = (0.3*exp(-0.0000002535*vOld))/(1+exp(-0.1*(vOld+32)));}
    
    h = ah/(ah+bh)-((ah/(ah+bh))-h)*exp(-dt/(1/(ah+bh)));
    j = aj/(aj+bj)-((aj/(aj+bj))-j)*exp(-dt/(1/(aj+bj)));
    m = am/(am+bm)-((am/(am+bm))-m)*exp(-dt/(1/(am+bm)));
    
    
    ina = gna*m*m*m*h*j*(vOld-ena);
}

void FR::comp_ical ()
{
    dss = 1/(1+exp(-(vOld+10)/6.24));
    taud = dss*(1-exp(-(vOld+10)/6.24))/(0.035*(vOld+10));
    
    fss = (1/(1+exp((vOld+32)/8)))+(0.6/(1+exp((50-vOld)/20)));
    tauf = 1/(0.0197*exp(-pow(0.0337*(vOld+10),2))+0.02);
    
    d = dss-(dss-d)*exp(-dt/taud);
    f = fss-(fss-f)*exp(-dt/tauf);
    
    ibarca = pca*zca*zca*((vOld*FDAY*FDAY)/(RGAS*TEMP))*((gacai*cai*exp((zca*vOld*FDAY)/(RGAS*TEMP))-gacao*cao)/(exp((zca*vOld*FDAY)/(RGAS*TEMP))-1));
    ibarna = pna*zna*zna*((vOld*FDAY*FDAY)/(RGAS*TEMP))*((ganai*nai*exp((zna*vOld*FDAY)/(RGAS*TEMP))-ganao*nao)/(exp((zna*vOld*FDAY)/(RGAS*TEMP))-1));
    ibark = pk*zk*zk*((vOld*FDAY*FDAY)/(RGAS*TEMP))*((gaki*ki*exp((zk*vOld*FDAY)/(RGAS*TEMP))-gako*ko)/(exp((zk*vOld*FDAY)/(RGAS*TEMP))-1));
    
    fca = 1/(1+cai/kmca);
    
    ilca = icalFactor*d*f*fca*ibarca;
    ilcana = d*f*fca*ibarna;
    ilcak = d*f*fca*ibark;
    
    ilcatot = ilca+ilcana+ilcak;
}

void FR::comp_icat ()
{
    bss = 1/(1+exp(-(vOld+14)/10.8));
    taub = 3.7+6.1/(1+exp((vOld+25)/4.5));
    
    gss = 1/(1+exp((vOld+60)/5.6));
    if (vOld<=0)
        taug = -0.875*vOld+12;
    else
        taug = 12;
    
    b = bss-(bss-b)*exp(-dt/taub);
    g = gss-(gss-g)*exp(-dt/taug);
    
    //gcat = 0.05;
    gcat = icattFactor*0.05;
    eca = (RGAS*TEMP/(2*FDAY))*log(cao/cai);
    
    icat = gcat*b*b*g*(vOld-eca);
}

void FR::comp_ikr ()
{
    //gkr = 0.02614*sqrt(ko/5.4);
    gkr = ikrFactor*0.02614*sqrt(ko/5.4);
    ekr = ((RGAS*TEMP)/FDAY)*log(ko/ki);
    
    xrss = 1/(1+exp(-(vOld+21.5)/7.5));
    tauxr = 1/(0.00138*(vOld+14.2)/(1-exp(-0.123*(vOld+14.2)))+0.00061*(vOld+38.9)/(exp(0.145*(vOld+38.9))-1));
    
    xr = xrss-(xrss-xr)*exp(-dt/tauxr);
    
    r = 1/(1+exp((vOld+9)/22.4));
    
    ikr = gkr*xr*r*(vOld-ekr);
}

void FR::comp_iks ()
{
    gks = iksFactor*0.433*(1+0.6/(1+pow((0.000038/cai),1.4)));
    eks = ((RGAS*TEMP)/FDAY)*log((ko+prnak*nao)/(ki+prnak*nai));
    
    xs1ss = 1/(1+exp(-(vOld-1.5)/16.7));
    xs2ss = xs1ss;
    tauxs1 = 1/(0.0000719*(vOld+30)/(1-exp(-0.148*(vOld+30)))+0.000131*(vOld+30)/(exp(0.0687*(vOld+30))-1));
    tauxs2 = 4*tauxs1;
    
    xs1 = xs1ss-(xs1ss-xs1)*exp(-dt/tauxs1);
    xs2 = xs2ss-(xs2ss-xs2)*exp(-dt/tauxs2);
    
    iks = gks*xs1*xs2*(vOld-eks);
}

void FR::comp_iki ()
{
    //gki = 0.75*(sqrt(ko/5.4));
    gki = ikiFactor*0.75*(sqrt(ko/5.4));
    eki = ((RGAS*TEMP)/FDAY)*log(ko/ki);
    
    aki = 1.02/(1+exp(0.2385*(vOld-eki-59.215)));
    bki = (0.49124*exp(0.08032*(vOld-eki+5.476))+exp(0.06175*(vOld-eki-594.31)))/(1+exp(-0.5143*(vOld-eki+4.753)));
    
    kin = aki/(aki+bki);

    iki = gki*kin*(vOld-eki);
}

void FR::comp_ikp ()
{
    gkp = 0.00552;
    //gkp = isusFactor * 0.00552;
    ekp = eki;
    
    kp = 1/(1+exp((7.488-vOld)/5.98));
    
    ikp = gkp*kp*(vOld-ekp);
}

void FR::comp_ikna ()
{
    ekna = ((RGAS*TEMP)/FDAY)*log(ko/ki);
    pona = 0.85/(1+pow((kdkna/nai),2.8));
    pov = 0.8-0.65/(1+exp((vOld+125)/15));
    ikna = gkna*pona*pov*(vOld-ekna);
}

void FR::comp_ikatp ()
{
    /* Note: If you wish to use this current in your simulations, there are additional       */
    /* changes which must be made to the code as detailed in Cardiovasc Res 1997;35:256-272  */
    
    ekatp = ((RGAS*TEMP)/FDAY)*log(ko/ki);
    //gkatp = 0.000195/nicholsarea;
    gkatp = isusFactor * 0.000195/nicholsarea;
    patp = 1/(1+(pow((atpi/katp),hatp)));
    gkbaratp = gkatp*patp*(pow((ko/4),natp));
    
    ikatp = gkbaratp*(vOld-ekatp);
}

void FR::comp_ito ()
{
    //gitodv = 0.5;
    gitodv = itoFactor * 0.5;
    ekdv = ((RGAS*TEMP)/FDAY)*log((ko)/(ki));
    rvdv = exp(vOld/100);
    
    azdv = (10*exp((vOld-40)/25))/(1+exp((vOld-40)/25));
    bzdv = (10*exp(-(vOld+90)/25))/(1+exp(-(vOld+90)/25));
    tauzdv = 1/(azdv+bzdv);
    zssdv = azdv/(azdv+bzdv);
    zdv = zssdv-(zssdv-zdv)*exp(-dt/tauzdv);
    
    aydv = 0.015/(1+exp((vOld+60)/5));
    bydv = (0.1*exp((vOld+25)/5))/(1+exp((vOld+25)/5));
    tauydv = 1/(aydv+bydv);
    yssdv = aydv/(aydv+bydv);
    ydv = yssdv-(yssdv-ydv)*exp(-dt/tauydv);
    
    ito = gitodv*zdv*zdv*zdv*ydv*rvdv*(vOld-ekdv);
}

void FR::comp_inaca ()
{
    inaca = inacaFactor*c1*exp((gammas-1)*vOld*FDAY/(RGAS*TEMP))*((exp(vOld*FDAY/(RGAS*TEMP))*nai*nai*nai*cao-nao*nao*nao*cai)/(1+c2*exp((gammas-1)*vOld*FDAY/(RGAS*TEMP))*(exp(vOld*FDAY/(RGAS*TEMP))*nai*nai*nai*cao+nao*nao*nao*cai)));
}

void FR::comp_inak ()
{
    sigma = (exp(nao/67.3)-1)/7;
    
    fnak = 1/(1+0.1245*exp((-0.1*vOld*FDAY)/(RGAS*TEMP))+0.0365*sigma*exp((-vOld*FDAY)/(RGAS*TEMP)));
    //
    inak = inakFactor*ibarnak*fnak*(1/(1+pow(kmnai/nai,2)))*(ko/(ko+kmko));
}

void FR::comp_insca ()
{
    ibarnsna = pnsca*zna*zna*((vOld*FDAY*FDAY)/(RGAS*TEMP))*((ganai*nai*exp((zna*vOld*FDAY)/(RGAS*TEMP))-ganao*nao)/(exp((zna*vOld*FDAY)/(RGAS*TEMP))-1));
    ibarnsk = pnsca*zk*zk*((vOld*FDAY*FDAY)/(RGAS*TEMP))*((gaki*ki*exp((zk*vOld*FDAY)/(RGAS*TEMP))-gako*ko)/(exp((zk*vOld*FDAY)/(RGAS*TEMP))-1));
    
    insna = ibarnsna/(1+pow(kmnsca/cai,3));
    insk = ibarnsk/(1+pow(kmnsca/cai,3));
}

void FR::comp_ipca ()
{
    ipca = (ibarpca*cai)/(kmpca+cai);
}

void FR::comp_icab ()
{
    gcab = 0.003016;
    ecan = ((RGAS*TEMP)/(2*FDAY))*log(cao/cai);
    
    icab = gcab*(vOld-ecan);
}

void FR::comp_inab ()
{
    //gnab = 0.004;
    gnab = inabFactor * 0.004;
    enan = ena;
    
    inab = gnab*(vOld-enan);
}

/* Total sum of currents is calculated here, if the time is between stimtime = 0 and stimtime = 0.5, a stimulus is applied */


void FR::comp_it ()
{
    iNat = ina+inab+ilcana+3*inak+3*inaca;
    iKt = ikr+iks+iki+ikp+ilcak-2*inak+ito+ikna+ikatp;
    iCat = ilca+icab+ipca-2*inaca+icat;

 /*
    if (t>=stimt && t<(stimt+dt))
    {stimtime = 0;
        //stimt = stimt+bcl;
        boolien = 0;
        i = i+1;
        rmbp[i]=vOld;
        nair[i] = nai;
        cair[i] = cai;}

    if(stimtime>=0 &&  stimtime<0.5)
   {iTot = stimval+iNat+iKt+iCat;}
    else
*/
    iTot = iNat+iKt+iCat;
    //stimtime = stimtime+dt;
}



/* Functions that calculate intracellular ion concentrations begins here */


void FR::conc_nai ()
{
    // The units of dnai is in mM.  Note that naiont should be multiplied by the
    // cell capacitance to get the correct units.  Since cell capacitance = 1 uF/cm^2,
    // it doesn't explicitly appear in the equation below.
    // This holds true for the calculation of dki and dcai. */
    
    dnai = -dt*(iNat*ACap)/(Vmyo*zna*FDAY);
    nai = dnai + nai;
}

void FR::conc_ki ()
{
    /*if(stimtime>=0 &&  stimtime<0.5)
    {dki = -dt*((iKt+stimval)*ACap)/(Vmyo*zk*FDAY);}
    else
    */
    dki = -dt*(iKt*ACap)/(Vmyo*zk*FDAY);
    
    ki = dki + ki;

}

void FR::conc_nsr ()
{
    kleak = iupbar/nsrbar;
    ileak = kleak*nsr;
    
    iup = iupbar*cai/(cai+kmup);
    
    dnsr = dt*(iup-ileak-itr*Vjsr/Vnsr);
    nsr = nsr+dnsr;
}

void FR::conc_jsr ()
{
    dcaiontnew = (iCat-caiontold)/dt;
    
    if(vOld>-35 && dcaiontnew<dcaiont && boolien==0)
    {boolien = 1;
        tcicr = 0;}
    
    on = 1/(1+exp((-tcicr+4)/.5));
    off = (1-1/(1+exp((-tcicr+4)/.5)));
    magrel = 1/(1+exp(((ilca+icab+ipca-2*inaca+icat)+5)/0.9));
    
    irelcicr = gmaxrel*on*off*magrel*(jsr-cai);
    
    tcicr = tcicr+dt;
    
    greljsrol = irelFactor*grelbarjsrol*(1-exp(-tjsrol/tauon))*exp(-tjsrol/tauoff);
    ireljsrol = greljsrol*(jsr-cai);
    tjsrol = tjsrol+dt;
    
    csqn = csqnbar*(jsr/(jsr+kmcsqn));
    djsr = dt*(itr-irelcicr-ireljsrol);
    bjsr = csqnbar-csqn-djsr-jsr+kmcsqn;
    cjsr = kmcsqn*(csqn+djsr+jsr);
    
    jsr = (sqrt(bjsr*bjsr+4*cjsr)-bjsr)/2;
}

void FR::calc_itr ()
{
    itr = (nsr-jsr)/tautr;
}

void FR::conc_cai ()
{
    dcai = -dt*(((iCat*ACap)/(Vmyo*zca*FDAY))+((iup-ileak)*Vnsr/Vmyo)-(irelcicr*Vjsr/Vmyo)-(ireljsrol*Vjsr/Vmyo));
    trpn = trpnbar*(cai/(cai+kmtrpn));
    cmdn = cmdnbar*(cai/(cai+kmcmdn));
    
    catotal = trpn+cmdn+dcai+cai;
    bmyo = cmdnbar+trpnbar-catotal+kmtrpn+kmcmdn;
    cmyo = (kmcmdn*kmtrpn)-(catotal*(kmtrpn+kmcmdn))+(trpnbar*kmcmdn)+(cmdnbar*kmtrpn);
    dmyo = -kmtrpn*kmcmdn*catotal;
    
    gpig = sqrt(bmyo*bmyo-3*cmyo);
    
    cai = (2*gpig/3)*cos(acos((9*bmyo*cmyo-2*bmyo*bmyo*bmyo-27*dmyo)/(2*pow((bmyo*bmyo-3*cmyo),1.5)))/3)-(bmyo/3);
}

void FR::conc_cleft()
{
    dnao = dt*((nabm-nao)/taudiff+iNat*ACap/(vcleft*FDAY));
    nao = dnao+nao;
    dko = dt*((kbm-ko)/taudiff+iKt*ACap/(vcleft*FDAY));
    ko = dko+ko;
    dcao = dt*((cabm-cao)/taudiff+iCat*ACap/(vcleft*FDAY*2));
    cao = dcao+cao;
}


void FR::updateCurr()
{
    comp_ina();    // Calculates Fast Na Current
    comp_ical();   // Calculates Currents through L-Type Ca Channel (output is ilcatot)
    comp_icat();   // Calculates Currents through T-Type Ca Channel
    comp_ikr();    // Calculates Rapidly Activating K Current
    comp_iks();    // Calculates Slowly Activating K Current
    comp_iki();    // Calculates Time-Independant K Current
    comp_ikp();    // Calculates Plateau K Current
    //comp_ikna();   // Calculates Na-activated K Current
    //comp_ikatp();  // Calculates ATP-Sensitive K Current
    //comp_ito();    // Calculates Transient Outward Current
    comp_inaca();  // Calculates Na-Ca Exchanger Current
    comp_inak();   // Calculates Na-K Pump Current
    comp_insca();  // Calculates Non-Specific ca-Activated Current
    comp_ipca();   // Calculates Sarcolemmal Ca Pump Current
    comp_icab();   // Calculates Ca Background Current
    comp_inab();   // Calculates Na Background Current

    comp_it();     // Calculates Total Current
    std::cout << //inaca << "," << inak << "," << inab << "," << insna << "," << insk << "," << ipca << "," << icab << std::endl;
                 //ikp << "," << ikna << "," << ikatp << "," << ito << std::endl;
                 //ina << "," << ilcatot << "," << icat << "," << ikr << "," << iks << "," << iki << std::endl;

                 iCat << "," << iKt << "," << iNat << "," << iTot << "," << vOld << std::endl;
                 //insna << "," << insk << "," << ilca <<"," << ilcana << "," << ilcak << "," << cai << "," << iCat << std::endl;
}

void FR::updateConc()
{
    conc_nai();    // Calculates new myoplasmic Na ion concentration
    conc_ki();     // Calculates new myoplasmic K ion concentration
    conc_nsr();    // Calculates new NSR Ca ion concentration
    conc_jsr();    // Calculates new JSR Ca ion concentration
    calc_itr();    // Calculates Translocation of Ca from NSR to JSR
    conc_cai();    // Calculates new myoplasmic Ca ion concentration
    conc_cleft();  // Calculates new cleft ion concentrations

//    std::cout << dnao << "," << dko << "," << dcao << std::endl;
    //std::cout << nai << "," << ki << "," << nsr << "," << jsr << "," << itr << "," << cai << std::endl;
}




int FR::externalStim(double stimval){
    iTot = iTot + stimval;
    return 1;
}



void FR::makemap(){
    vars["iTot"]=&iTot;

    //ion concentrations
    vars["nai"]=&nai;
 //   vars["nao"]=&nao;
 //   vars["nabm"]=&nabm;
 //   vars["dnao"]=&dnao;
    vars["ki"]=&ki;
 //   vars["ko"]=&ko;
 //   vars["kbm"]=&kbm;
 //   vars["dko"]=&dko;
    vars["cai"]=&cai;
 //   vars["cao"]=&cao;
 //   vars["cabm"]=&cabm;
 //   vars["dcao"]=&dcao;
    vars["cmdn"]=&cmdn;
    vars["trpn"]=&trpn;
    vars["nsr"]=&nsr;
    vars["jsr"]=&jsr;
    vars["csqn"]=&csqn;
 //   vars["taudiff"] = &taudiff;
 //   vars["dnsr"]=&dnsr; // Change in [Ca] in the NSR (mM)
    vars["iup"]=&iup; // Ca uptake from myo. to NSR (mM/ms)
//    vars["ileak"]=&ileak; // Ca leakage from NSR to myo. (mM/ms)
//    vars["kleak"]=&kleak; // Rate constant of Ca leakage from NSR (ms^-1)
//    vars["kmup"]=&kmup; // Half-saturation concentration of iup (mM)
//    vars["iupbar"]=&iupbar; // Max. current through iup channel (mM/ms)
    vars["nsrbar"]=&nsrbar; // Max. [Ca] in NSR (mM)

    //sodium current
    vars["ina"]=&ina;    // Fast Na Current (uA/uF)
 //   vars["gna"]=&gna;    // Max. Conductance of the Na Channel (mS/uF)
//    vars["ena"]=&ena;    // Reversal Potential of Na (mV)
    vars["am"]=&am;     // Na alpha-m rate constant (ms^-1)
    vars["bm"]=&bm;     // Na beta-m rate constant (ms^-1)
    vars["ah"]=&ah;     // Na alpha-h rate constant (ms^-1)
    vars["bh"]=&bh;     // Na beta-h rate constant (ms^-1)
    vars["aj"]=&aj;     // Na alpha-j rate constant (ms^-1)
    vars["bj"]=&bj;     // Na beta-j rate constant (ms^-1)
    vars["m"]=&m;      // Na activation
    vars["h"]=&h;      // Na inactivation
    vars["j"]=&j;      // Na inactivation

    //l-type ca
    vars["ilca"]=&ilca; // Ca current through L-type Ca channel (uA/uF)
    vars["ilcana"]=&ilcana; // Na current through L-type Ca channel (uA/uF)
    vars["ilcak"]=&ilcak;
    vars["ilcatot"]=&ilcatot;
 //   vars["ibarca"]=&ibarca;  // Max. Ca current through Ca channel (uA/uF)
 //   vars["ibarna"]=&ibarna;  // Max. Na current through Ca channel (uA/uF)
  //  vars["ibark"]=&ibark;   // Max. K current through Ca channel (uA/uF)
    vars["d"]=&d;       // Voltage dependant activation gate
 //   vars["dss"]=&dss;     // Steady-state value of activation gate d
    vars["taud"]=&taud;    // Time constant of gate d (ms^-1)
    vars["f"]=&f;       // Voltage dependant inactivation gate
 //   vars["fss"]=&fss;     // Steady-state value of inactivation gate f
    vars["tauf"]=&tauf;    // Time constant of gate f (ms^-1)
    vars["fca"]=&fca;     // Ca dependant inactivation gate
 //   vars["kmca"]=&kmca;     // Half-saturation concentration of Ca channel (mM)
 //   vars["pca"]=&pca;     // Permiability of membrane to Ca (cm/s)
    vars["gacai"]=&gacai;         // Activity coefficient of Ca
    vars["gacao"]=&gacao;     // Activity coefficient of Ca
 //   vars["pna"]=&pna; // Permiability of membrane to Na (cm/s)
 //   vars["ganai"]=&ganai;      // Activity coefficient of Na
 //   vars["ganao"]=&ganao;      // Activity coefficient of Na
  //  vars["pk"]=&pk;  // Permiability of membrane to K (cm/s)
 //   vars["gaki"]=&gaki;       // Activity coefficient of K
 //   vars["gako"]=&gako;       // Activity coefficient of K

    /* Current through T-type Ca Channel */
    vars["icat"]=&icat;    // Ca current through T-type Ca channel (uA/uF)
 //   vars["gcat"]=&gcat;    // Max. Conductance of the T-type Ca channel (mS/uF)
 //   vars["eca"]=&eca;     // Reversal Potential of the T-type Ca channel (mV)
    vars["b"]=&b;       // Voltage dependant activation gate
 //   vars["bss"]=&bss;     // Steady-state value of activation gate b
    vars["taub"]=&taub;    // Time constant of gate b (ms^-1)
    vars["g"]=&g;       // Voltage dependant inactivation gate
 //   vars["gss"]=&gss;     // Steady-state value of inactivation gate g
    vars["taug"]=&taug;    // Time constant of gate g (ms^-1)

    /* Rapidly Activating Potassium Current */
    vars["ikr"]=&ikr;   // Rapidly Activating K Current (uA/uF)
    vars["gkr"]=&gkr;   // Channel Conductance of Rapidly Activating K Current (mS/uF)
    vars["ekr"]=&ekr;   // Reversal Potential of Rapidly Activating K Current (mV)
    vars["xr"]=&xr;    // Rapidly Activating K time-dependant activation
 //   vars["xrss"]=&xrss;  // Steady-state value of inactivation gate xr
    vars["tauxr"]=&tauxr; // Time constant of gate xr (ms^-1)
    vars["r"]=&r;     // K time-independant inactivation

    /* Slowly Activating Potassium Current */
    vars["iks"]=&iks;   // Slowly Activating K Current (uA/uF)
 //   vars["gks"]=&gks;   // Channel Conductance of Slowly Activating K Current (mS/uF)
 //   vars["eks"]=&eks;   // Reversal Potential of Slowly Activating K Current (mV)
    vars["xs1"]=&xs1;    // Slowly Activating K time-dependant activation
 //   vars["xs1ss"]=&xs1ss;  // Steady-state value of inactivation gate xs1
    vars["tauxs1"]=&tauxs1; // Time constant of gate xs1 (ms^-1)
    vars["xs2"]=&xs2;    // Slowly Activating K time-dependant activation
//    vars["xs2ss"]=&xs2ss;  // Steady-state value of inactivation gate xs2
    vars["tauxs2"]=&tauxs2; // Time constant of gate xs2 (ms^-1)
//    vars["prnak"]=&prnak;  // Na/K Permiability Ratio

    /* Potassium Current (time-independant) */
    vars["iki"]=&iki;    // Time-independant K current (uA/uF)
  //  vars["gki"]=&gki;    // Channel Conductance of Time Independant K Current (mS/uF)
 //   vars["eki"]=&eki;    // Reversal Potential of Time Independant K Current (mV)
    vars["aki"]=&aki;    // K alpha-ki rate constant (ms^-1)
    vars["bki"]=&bki;    // K beta-ki rate constant (ms^-1)
    vars["kin"]=&kin;    // K inactivation

    /* Plateau Potassium Current */
    vars["ikp"]=&ikp;    // Plateau K current (uA/uF)
 //   vars["gkp"]=&gkp;    // Channel Conductance of Plateau K Current (mS/uF)
  //  vars["ekp"]=&ekp;    // Reversal Potential of Plateau K Current (mV)
    vars["kp"]=&kp;     // K plateau factor

    /* Na-Activated K Channel */
    vars["ikna"]=&ikna;   // Na activated K channel
  //  vars["pona"]=&pona;   // Open probability dependant on Nai
  //  vars["pov"]=&pov;    // Open probability dependant on Voltage
 //   vars["ekna"]=&ekna;   // Reversal potential
  //  vars["gkna"]=&gkna;   // Maximum conductance (mS/uF)
 //   vars["nkna"]=&nkna;       // Hill coefficient for Na dependance
 //   vars["kdkna"]=&kdkna;       // Dissociation constant for Na dependance(mM)

    /* ATP-Sensitive K Channel */
    vars["ikatp"]=&ikatp;    // ATP-sensitive K current (uA/uF)
 //   vars["ekatp"]=&ekatp;    // K reversal potential (mV)
 //   vars["gkbaratp"]=&gkbaratp; // Conductance of the ATP-sensitive K channel (mS/uF)
 //   vars["gkatp"]=&gkatp;    // Maximum conductance of the ATP-sensitive K channel (mS/uF)
 //   vars["patp"]=&patp;     // Percentage availibility of open channels
 //   vars["natp"]=&natp;          // K dependence of ATP-sensitive K current
    vars["atpi"]=&atpi;             // Intracellular ATP concentraion (mM)
    vars["hatp"]=&hatp;             // Hill coefficient
 //   vars["katp"]=&katp;         // Half-maximal saturation point of ATP-sensitive K current (mM)

    /* Ito Transient Outward Current (Dumaine et al. Circ Res 1999;85:803-809) */
    vars["ito"]=&ito;	  // Transient outward current
 //   vars["gitodv"]=&gitodv;	  // Maximum conductance of Ito
 //   vars["ekdv"]=&ekdv;	  // Reversal Potential of Ito
    vars["rvdv"]=&rvdv;      // Time independant voltage dependence of Ito
    vars["zdv"]=&zdv;       // Ito activation
    vars["azdv"]=&azdv;      // Ito alpha-z rate constant
    vars["bzdv"]=&bzdv;      // Ito beta-z rate constant
    vars["tauzdv"]=&tauzdv;	  // Time constant of z gate
 //   vars["zssdv"]=&zssdv;     // Steady-state value of z gate
    vars["ydv"]=&ydv;       // Ito inactivation
    vars["aydv"]=&aydv;      // Ito alpha-y rate constant
    vars["bydv"]=&bydv;      // Ito beta-y rate constant
    vars["tauydv"]=&tauydv;	  // Time constant of y gate
 //   vars["yssdv"]=&yssdv;     // Steady-state value of y gate

    /* Sodium-Calcium Exchanger V-S */
    vars["inaca"]=&inaca;               // NaCa exchanger current (uA/uF)
 //   vars["c1"]=&c1;   // Scaling factor for inaca (uA/uF)
 //   vars["c2"]=&c2;   // Half-saturation concentration of NaCa exhanger (mM)
 //   vars["gammas"]=&gammas;  // Position of energy barrier controlling voltage dependance of inaca

    /* Sodium-Potassium Pump */
    vars["inak"]=&inak;    // NaK pump current (uA/uF)
  //  vars["fnak"]=&fnak;    // Voltage-dependance parameter of inak
  //  vars["sigma"]=&sigma;   // [Na]o dependance factor of fnak
   // vars["ibarnak"]=&ibarnak;   // Max. current through Na-K pump (uA/uF)
  //  vars["kmnai"]=&kmnai;    // Half-saturation concentration of NaK pump (mM)
  //  vars["kmko"]=&kmko;    // Half-saturation concentration of NaK pump (mM)

    /* Nonspecific Ca-activated Current */
    vars["insna"]=&insna;     // Non-specific Na current (uA/uF)
    vars["insk"]=&insk;      // Non-specific K current (uA/uF)
  //  vars["ibarnsna"]=&ibarnsna;  // Max. Na current through NSCa channel (uA/uF)
  //  vars["ibarnsk"]=&ibarnsk;   // Max. K current through NSCa channel (uA/uF)
  //  vars["pnsca"]=&pnsca;  // Permiability of channel to Na and K (cm/s)
  //  vars["kmnsca"]=&kmnsca;      // Half-saturation concentration of NSCa channel (mM)

    /* Sarcolemmal Ca Pump */
    vars["ipca"]=&ipca;                 // Sarcolemmal Ca pump current (uA/uF)
  //  vars["ibarpca"]=&ibarpca; // Max. Ca current through sarcolemmal Ca pump (uA/uF)
  //  vars["kmpca"]=&kmpca; // Half-saturation concentration of sarcolemmal Ca pump (mM)

    /* Ca Background Current */
    vars["icab"]=&icab;  // Ca background current (uA/uF)
  //  vars["gcab"]=&gcab;  // Max. conductance of Ca background (mS/uF)
  //  vars["ecan"]=&ecan;  // Nernst potential for Ca (mV)

    /* Na Background Current */
    vars["inab"]=&inab;  // Na background current (uA/uF)
 //   vars["gnab"]=&gnab;  // Max. conductance of Na background (mS/uF)
//    vars["enan"]=&enan;  // Nernst potential for Na (mV)

    pars["Vnsr"] = &Vnsr;
    pars["Vjsr"] = &Vjsr;
    pars["zk"] = &zk;
    pars["zna"]= &zna;
    pars["zca"] = &zca;
    pars["nao"] = &nao;
    pars["ko"] = &ko;
    pars["cao"] = &cao;
    pars["gacai"] = &gacai;
    pars["gacao"] = &gacao;
    pars["icalFactor"] = &icalFactor;
    pars["icattFactor"] = &icattFactor;
    pars["ikrFactor"] = &ikrFactor;
    pars["iksFactor"] = &iksFactor;
    pars["itoFactor"] = &itoFactor;
    pars["isusFactor"] = &isusFactor; //atp sensitive potassium current
    //pars["ikachFactor"] = &ikachFactor;
   // pars["istFactor"] = &istFactor;
    pars["inabFactor"] = &inabFactor;
    pars["inakFactor"] = &inakFactor;
    pars["inacaFactor"] = &inacaFactor;
    pars["ikiFactor"] = &ikiFactor;
 //   pars["ihFactor"] = &ihFactor;
    pars["iupFactor"] = &iupFactor;
    pars["irelFactor"] = &irelFactor;

    };


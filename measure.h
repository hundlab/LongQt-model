//################################################
// This header file
// declares Measure class used to track properties
// (e.g. peak, min, duration) of specified state variable.
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015
// Email thomas.hund@osumc.edu
//#################################################

#ifndef MEASURE_H
#define MEASURE_H

#include <string>
#include <map>
#include <set>
#include <limits>

using namespace std;

class Measure
{
    public:
        Measure(set<string> selected = {}, double percrepol = 50);
        Measure(const Measure& toCopy) = default;
        Measure(Measure&& toCopy) = default;
        ~Measure() = default;

        //		Measure& operator=(const Measure& toCopy);

        bool measure(double time,double var);  //measures props related to var; returns 1 when ready for output.
        void reset();   //resets params to init vals

        //		string varname;

        set<string> variables();
        map<string,double> variablesMap();

        void percrepol(double val);
        double percrepol() const;

        set<string> selection();
        void selection(set<string> new_selection);
        //		void restoreLast();
        string getNameString(string name);
        string getValueString();
    protected:
        const map<string, double* const> varmap = // map for refing properties that can be measured.
        {{"cl",&cl},{"peak",&peak},{"min",&min},{"amp",&amp},{"ddr",&ddr},
            {"maxderiv",&maxderiv},{"dur",&dur},{"durtime1",&durtime1},
            {"vartakeoff",&vartakeoff},{"mint",&mint},{"derivt",&derivt},
            {"deriv2ndt",&deriv2ndt}};

        //		map<string, double> lastMap;

        double varold = 100;
        double vartakeoff=-100; //var value at point of max deflection.
        double told = std::numeric_limits<double>::min();
        double mint = std::numeric_limits<double>::min();    //time of min value.
        double maxt = std::numeric_limits<double>::min();    //time of max value.
        double durtime1;
        double derivold = 0; //dv/dt from prev. time step
        double derivt = 0;   // time of max deriv.
        double derivt1 = 0;  // time of prev. cycle max deriv.
        double derivt2 = 0;
        double deriv2ndt = 0;  // time of max 2nd deriv.
        double peak = std::numeric_limits<double>::min();      // max var value
        double min = std::numeric_limits<double>::max();       // min var value
        double amp = 70;
        double ddr;      //diastolic depol. rate
        double maxderiv = 0;
        //		double maxderiv1;
        //		double maxderiv2;
        double maxderiv2nd = 0;
        double cl = 0;
        double dur;   //duration
        double __percrepol = 50;   //specify percent repolarization
        double repol = -25;           // repol var val for duration measure.
        bool minflag = false;
        bool maxflag = false;
        bool durflag = false;    //1 while measuring duration.
        bool ampflag = false;
        bool ddrflag = false;
        bool returnflag = false;

    private:
        set<string> __selection; // map for refing properties that will be output.
        //		void copy(const Measure& toCopy);
};

#endif

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
#include <cmath>

using namespace std;

class Measure
{
    public:
        Measure(set<string> selected = {});
        Measure(const Measure& toCopy);
        Measure(Measure&& toCopy);
        virtual ~Measure() = default;

        Measure& operator=(const Measure& toCopy) = delete;

        virtual bool measure(double time,double var);  //measures props related to var; returns 1 when ready for output.
        virtual void reset();   //resets params to init vals

        //		string varname;

        set<string> variables();
        map<string,double> variablesMap();

        set<string> selection();
        void selection(set<string> new_selection);
        //		void restoreLast();
        virtual string getNameString(string name);
        virtual string getValueString();
    protected:
        virtual void calcMeasure(double time, double var);
        virtual void updateOld(double time, double var);

        map<string, double* const> varmap = // map for refing properties that can be measured.
        {{"peak",&peak},{"min",&min},{"maxderiv",&maxderiv},
        {"mint",&mint},{"derivt",&derivt}};

        //		map<string, double> lastMap;

        double varold = NAN;
//        double vartakeoff=-100; //var value at point of max deflection.
        double told = NAN;
//        double durtime1;
        double derivold = 0; //dv/dt from prev. time step
        double derivt = 0;   // time of max deriv.
        double peak = std::numeric_limits<double>::min();      // max var value
        double maxt = NAN;    //time of max value.
        double min = std::numeric_limits<double>::max();       // min var value
        double mint = NAN;    //time of min value.
        double maxderiv = 0;
        double deriv = 0;
        //		double maxderiv1;
        //		double maxderiv2;
//        double maxderiv2nd = 0;
//        double cl = 0;
//        double dur;   //duration
//        double __percrepol = 50;   //specify percent repolarization
//        double repol = -25;           // repol var val for duration measure.
//        bool durflag = false;    //1 while measuring duration.
//        bool ampflag = false;
//        bool ddrflag = false;
        bool returnflag = false;

        set<string> __selection; // map for refing properties that will be output.
    private:
        void copy(const Measure& toCopy);
};

#endif

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

#define INF std::numeric_limits<double>::infinity()
#define Q_NAN std::numeric_limits<double>::quiet_NaN()

namespace LongQt {

class Measure
{
    public:
        Measure(std::set<std::string> selected = {});
        Measure(const Measure& toCopy);
        Measure(Measure&& toCopy);
        virtual ~Measure() = default;

        Measure& operator=(const Measure& toCopy) = delete;

        virtual bool measure(double time,double var);  //measures props related to var; returns 1 when ready for output.
        virtual void reset();   //resets params to init vals

        //		string varname;

        std::set<std::string> variables();
        std::map<std::string,double> variablesMap();

        std::set<std::string> selection();
        void selection(std::set<std::string> new_selection);
        //		void restoreLast();
        virtual std::string getNameString(std::string name) const;
        virtual std::string getValueString() const;
    protected:
        virtual void calcMeasure(double time, double var);
        virtual void updateOld(double time, double var);

        std::map<std::string, double* const> varmap = // map for refing properties that can be measured.
        {{"peak",&max.second},{"min",&min.second},{"maxderiv",&maxderiv.second},
        {"mint",&min.first},{"derivt",&maxderiv.first},{"maxt",&max.first}};

        double varold = Q_NAN;
        double told = Q_NAN;
        double derivold = Q_NAN; //dv/dt from prev. time step

        double deriv = 0;
        std::pair<double,double> maxderiv = {Q_NAN,-INF};   // time of max deriv.
        std::pair<double,double> max = {Q_NAN,-INF};      // max var value
        std::pair<double,double> min = {Q_NAN,INF};       // min var value

        bool resetflag = false;

        std::set<std::string> __selection; // map for refing properties that will be output.
        //        double vartakeoff=-100; //var value at point of max deflection.
        //        double durtime1;
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
    private:
        void copy(const Measure& toCopy);
};
} //LongQt
#undef INF
#undef Q_NAN
#endif

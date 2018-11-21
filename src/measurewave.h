//################################################
// This header file
// declares Measure class used to track properties
// (e.g. peak, min, duration) of specified state variable.
//
// Copyright (C) 2015 Thomas J. Hund.
// Updated 07/2015
// Email thomas.hund@osumc.edu
//#################################################

#ifndef MEASUREWAVE_H
#define MEASUREWAVE_H

#include <string>
#include <map>
#include <set>
#include <limits>
#include "measure.h"
#define INF std::numeric_limits<double>::infinity()
#define Q_NAN std::numeric_limits<double>::quiet_NaN()

namespace LongQt {

class MeasureWave: public Measure
{
    public:
        MeasureWave(std::set<std::string> selected = {}, double percrepol = 50);
        MeasureWave(const MeasureWave& toCopy);
        MeasureWave(MeasureWave&& toCopy);
        virtual ~MeasureWave() = default;

        MeasureWave& operator=(const MeasureWave& toCopy) = delete;

        void reset();   //resets params to init vals

        //		string varname;

        std::set<std::string> variables();
        std::map<std::string,double> variablesMap();

        void percrepol(double val);
        double percrepol() const;

    protected:

        virtual void calcMeasure(double time,double var);  //measures props related to var; returns 1 when ready for output.
        double vartakeoff = Q_NAN; //var value at point of max deflection.
        double durtime1 = Q_NAN;
        double amp = Q_NAN;
        double ddr = Q_NAN;      //diastolic depol. rate
        //		double maxderiv1;
        //		double maxderiv2;
        double maxderiv_prev = Q_NAN;  // time of prev. cycle max deriv.
        std::pair<double,double> maxderiv2d = {Q_NAN,-INF};
        double cl = 0;
        double dur = Q_NAN;   //duration
        double __percrepol = 50;   //specify percent repolarization
        double repol = -25;           // repol var val for duration measure.
        bool minfound = false;
        bool maxfound = false;
        bool inAP = false;    //1 while measuring duration.
        bool ampfound = false;
        bool ddrfound = false;

    private:
        void copy(const MeasureWave& toCopy);
};
} //LongQt
#undef INF
#undef Q_NAN
#endif

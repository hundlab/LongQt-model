#ifndef CELLPVARS_H
#define CELLPVARS_H
#include <map>
#include <random>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

using namespace std;

class CellPvars {
    public:
        //Types
        enum Distribution {
            none = 0,
            normal = 1,
            lognormal = 2
        };

        struct IonChanParam {
            Distribution dist;
            double val[2];
            /*  what these values are depends on dist
             *  for none: val[0] = starting value, val[1] = increment amount
             *  for normal & lognormal: val[0] = mean, val[1] = standard deviation
             */
            virtual string str(string name);
        };

        typedef std::map<string,IonChanParam*>::const_iterator const_iterator;

        //Functions
        virtual CellPvars* clone() = 0;

        virtual void setIonChanParams() = 0;
        virtual void calcIonChanParams() = 0;

        virtual void writePvars(QXmlStreamWriter& xml) = 0;
        virtual void readPvars(QXmlStreamReader& xml) = 0;

        virtual void insert(string,IonChanParam) = 0;
        virtual void erase(string);
        virtual void clear();
        virtual IonChanParam* at(string);
        virtual const_iterator begin() const;
        virtual const_iterator end() const;

    protected:
        default_random_engine generator;
        map<string,IonChanParam*> pvars;
};
#endif

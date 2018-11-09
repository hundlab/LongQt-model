#ifndef CELLPVARS_H
#define CELLPVARS_H
#include <map>
#include <random>
#include <array>
#include <memory>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

namespace LongQt {

class PvarsCell {
    public:
        //Types
        enum Distribution {
            none = 0,
            normal = 1,
            lognormal = 2
        };

        struct IonChanParam {
            IonChanParam(Distribution dist = Distribution::none,
                double val1 = 0, double val2 = 0) {
                this->dist = dist;
                this->val[0] = val1;
                this->val[1] = val2;
            }
            IonChanParam(const IonChanParam&) = default;
            virtual ~IonChanParam() {}
            Distribution dist;
            std::array<double,2> val;
            /*  what these values are depends on dist
             *  for none: val[0] = starting value, val[1] = increment amount
             *  for normal & lognormal: val[0] = mean, val[1] = standard deviation
             */
            virtual std::string str(std::string name);
        };

        virtual ~PvarsCell();
        typedef std::map<std::string,IonChanParam*>::const_iterator const_iterator;

        //Functions
        virtual void setIonChanParams() = 0;
        virtual void calcIonChanParams() = 0;

        virtual void writePvars(QXmlStreamWriter& xml) = 0;
        virtual void readPvars(QXmlStreamReader& xml) = 0;

        virtual void insert(std::string,IonChanParam) = 0;
        virtual void erase(std::string);
        virtual int size() const;
        virtual void clear();
        virtual IonChanParam* at(std::string);
        virtual const_iterator begin() const;
        virtual const_iterator end() const;

    protected:
        std::default_random_engine generator =
                std::default_random_engine(std::random_device{}());
        std::map<std::string,IonChanParam*> pvars;
};
} //LongQt

#endif

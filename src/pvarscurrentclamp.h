#ifndef PVARSCURRENTCLAMP_H
#define PVARSCURRENTCLAMP_H

#include "protocol.h"
#include "pvarscell.h"
namespace LongQt {

class PvarsCurrentClamp : public PvarsCell {
    public:
        //Types
        struct TIonChanParam : IonChanParam {
            std::vector<double> trials; //index -> value
            virtual std::string str(std::string name) override;
            TIonChanParam() = default;
            TIonChanParam(const IonChanParam& other): IonChanParam(other) {};
            virtual ~TIonChanParam() = default;
        };

        //Functions
        PvarsCurrentClamp(Protocol* proto);
        PvarsCurrentClamp(const PvarsCurrentClamp&, Protocol *proto);
        virtual ~PvarsCurrentClamp() = default;

        virtual void setIonChanParams();
        virtual void calcIonChanParams();

        virtual void writePvars(QXmlStreamWriter& xml);
        virtual void readPvars(QXmlStreamReader& xml);

        virtual void insert(std::string,IonChanParam);

    private:
        void handlePvars(QXmlStreamReader& xml);
        void handlePvar(QXmlStreamReader& xml);
        std::pair<int,double> handleTrial(QXmlStreamReader& xml);

        void calcIonChanParam(TIonChanParam* param);

        std::map<std::string,TIonChanParam*>* __pvars =
            (reinterpret_cast<std::map<std::string,TIonChanParam*>*>(&this->pvars));
        Protocol* proto;
};
} //LongQt

#endif

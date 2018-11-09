#ifndef PVARSVOLTAGECLAMP_H
#define PVARSVOLTAGECLAMP_H

#include "protocol.h"
#include "pvarscell.h"
namespace LongQt {

class PvarsVoltageClamp : public PvarsCell {
    public:
        //Types
        struct SIonChanParam : IonChanParam {
            double paramVal; // value for that cell
            virtual std::string str(std::string name) override;
            SIonChanParam() = default;
            SIonChanParam(const IonChanParam& other): IonChanParam(other) {};
            virtual ~SIonChanParam() = default;
        };

        //Functions
        PvarsVoltageClamp(Protocol* proto);
        PvarsVoltageClamp(const PvarsVoltageClamp&, Protocol* proto);
        virtual ~PvarsVoltageClamp() = default;

        virtual void setIonChanParams();
        virtual void calcIonChanParams();
        void calcIonChanParam(SIonChanParam* param);

        virtual void writePvars(QXmlStreamWriter& xml);
        virtual void readPvars(QXmlStreamReader& xml);
        void handlePvars(QXmlStreamReader& xml);
        void handlePvar(QXmlStreamReader& xml);

        virtual void insert(std::string,IonChanParam);

    private:
        std::map<std::string,SIonChanParam*>* __pvars =
            (reinterpret_cast<std::map<std::string,SIonChanParam*>*>(&this->pvars));
        Protocol* proto;
};
} //LongQt

#endif

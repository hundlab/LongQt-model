#ifndef PVARSVOLTAGECLAMP_H
#define PVARSVOLTAGECLAMP_H

#include "protocol.h"
#include "cellpvars.h"

class PvarsVoltageClamp : public CellPvars {
    public:
        //Types
        struct SIonChanParam : IonChanParam {
            double paramVal; // value for that cell
            virtual string str(string name) override;
            SIonChanParam() = default;
            SIonChanParam(const IonChanParam& other): IonChanParam(other) {};
        };

        //Functions
        PvarsVoltageClamp(Protocol* proto);
        PvarsVoltageClamp(const PvarsVoltageClamp&) = default;
        virtual ~PvarsVoltageClamp() = default;
        virtual CellPvars* clone();
        void protocol(Protocol *proto);

        virtual void setIonChanParams();
        virtual void calcIonChanParams();
        void calcIonChanParam(SIonChanParam* param);

        virtual void writePvars(QXmlStreamWriter& xml);
        virtual void readPvars(QXmlStreamReader& xml);
        void handlePvars(QXmlStreamReader& xml);
        void handlePvar(QXmlStreamReader& xml);

        virtual void insert(string,IonChanParam);

    private:
        map<string,SIonChanParam*>* __pvars =
            reinterpret_cast<map<string,SIonChanParam*>*>(&this->pvars);
        Protocol* proto;
};
#endif

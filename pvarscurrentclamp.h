#ifndef PVARSCURRENTCLAMP_H
#define PVARSCURRENTCLAMP_H

#include "protocol.h"
#include "cellpvars.h"

class PvarsCurrentClamp : public CellPvars {
    public:
        //Types
        struct TIonChanParam : IonChanParam {
            vector<double> trials; //index -> value
            virtual string str(string name) override;
            TIonChanParam() = default;
            TIonChanParam(const IonChanParam& other): IonChanParam(other) {};
            virtual ~TIonChanParam() = default;
        };

        //Functions
        PvarsCurrentClamp(Protocol* proto);
        PvarsCurrentClamp(const PvarsCurrentClamp&);
        virtual ~PvarsCurrentClamp() = default;
        virtual CellPvars* clone();
        void protocol(Protocol *proto);

        virtual void setIonChanParams();
        virtual void calcIonChanParams();

        virtual void writePvars(QXmlStreamWriter& xml);
        virtual void readPvars(QXmlStreamReader& xml);
        void handlePvars(QXmlStreamReader& xml);
        void handlePvar(QXmlStreamReader& xml);
        pair<int,double> handleTrial(QXmlStreamReader& xml);

        virtual void insert(string,IonChanParam);

    private:
        void calcIonChanParam(TIonChanParam* param);

        map<string,TIonChanParam*>* __pvars =
            reinterpret_cast<map<string,TIonChanParam*>*>(&this->pvars);
        Protocol* proto;
};
#endif

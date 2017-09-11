#ifndef PVARSGRID_H
#define PVARSGRID_H

#include "grid.h"
#include "cellpvars.h"
#include <map>
#include <set>

class PvarsGrid : public CellPvars {
    public:
        //Types
        struct MIonChanParam : CellPvars::IonChanParam {
            int maxDist = 0;
            double maxVal = 100;
            set<pair<int,int>> startCells; //x,y pos of start cells
            map<pair<int,int>,double> cells; //map from x,y pos -> value

            virtual string str(string name) override;
            MIonChanParam() = default;
            MIonChanParam(const IonChanParam& other): IonChanParam(other) {};
            virtual ~MIonChanParam() = default;
        };

        //Functions
        PvarsGrid(Grid* grid);
        PvarsGrid(const PvarsGrid&);
        virtual ~PvarsGrid() = default;
        virtual CellPvars* clone();
        void setGrid(Grid* grid);

        virtual void setIonChanParams();
        virtual void calcIonChanParams();

        virtual void writePvars(QXmlStreamWriter& xml);
        virtual void readPvars(QXmlStreamReader& xml);
        void handlePvars(QXmlStreamReader& xml);
        void handlePvar(QXmlStreamReader& xml);
        pair<pair<int,int>,double> handleCell(QXmlStreamReader& xml, bool& start);

        virtual void insert(string,IonChanParam);
        void setMaxDistAndVal(string varname, int maxDist, double maxVal);
        void setStartCells(string varname, set<pair<int,int>> startCells);

    private:
        map<string,MIonChanParam*>* __pvars =
            reinterpret_cast<map<string,MIonChanParam*>*>(&this->pvars);
        Grid* grid;

        void calcIonChanParam(MIonChanParam* param);
        void getNext();
        void add(pair<int,int> e, set<pair<int,int>>& next);

        set<pair<int,int>> current;
        set<pair<int,int>> visited;
};
#endif

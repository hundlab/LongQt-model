#ifndef MEASUREMANAGER_H
#define MEASUREMANAGER_H

#include "measure.h"
#include "measurewave.h"
#include "cell.h"
#include <map>
#include <set>
#include <functional>
#include <string>
#include <memory>
#include <QFile>

class MeasureManager {
    public:
        MeasureManager(Cell* cell);
        virtual ~MeasureManager();
        virtual MeasureManager* clone();

        virtual bool writeMVarsFile(QXmlStreamWriter& xml);
        virtual bool readMvarsFile(QXmlStreamReader& xml);

        Cell* cell();
        void cell(Cell* cell);
        map<string,set<string>> selection();
        void selection(map<string,set<string>> sel);
        double percrepol();
        void percrepol(double percrepol);
        shared_ptr<Measure> getMeasure(string varname, set<string> selection);

        virtual void addMeasure(string var);
        virtual void removeMeasure(string var);
        virtual void setupMeasures(string filename);
        virtual void measure(double time);
        virtual void write(QFile* file = 0);
        virtual void writeLast(string filename);
        virtual void clear();
        virtual void close();
        virtual void resetMeasures();

        const map<string,string> varsMeas = {
            {"vOld","MeasureWave"},
            {"caI","MeasureWave"}
        };
        const map<string,function<Measure*(set<string> selection)>> varMeasCreator =
            {{"MeasureWave", [this](set<string> selection)
                {return (Measure*)new MeasureWave(selection,this->__percrepol);}}
            };

    protected:
        MeasureManager(const MeasureManager&);

        map<string,set<string>> variableSelection;

    private:
        void removeBad();
        void copy(const MeasureManager& other);

        Cell* __cell = 0;
        string last = "";
        double __percrepol = 50;
        unique_ptr<QFile> ofile;
        map<string,shared_ptr<Measure>> measures;
};
#endif

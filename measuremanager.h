#ifndef MEASUREMANAGER_H
#define MEASUREMANAGER_H

#include "measure.h"
#include "cell.h"
#include <map>
#include <set>
#include <string>
#include <QFile>

class MeasureManager {
    public:
        MeasureManager(Cell* cell);
        MeasureManager(const MeasureManager&) = default;
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

        virtual void addMeasure(string var);
        virtual void removeMeasure(string var);
        virtual void setupMeasures(string filename);
        virtual void measure(double time);
        virtual void write(QFile* file = 0);
        virtual void writeLast(string filename);
        virtual void clear();
        virtual void close();
        virtual void resetMeasures();

    protected:
        Cell* __cell = 0;
        map<string,set<string>> variableSelection;
        double __percrepol = 50;
        QFile* ofile = 0;
        string last = "";

    private:
        void removeBad();

        map<string,Measure> measures;
};
#endif

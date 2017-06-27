#ifndef GRIDMEASUREMANAGER_H
#define GRIDMEASUREMANAGER_H

#include "measuremanager.h"
#include "measure.h"
#include "gridCell.h"
#include "grid.h"
#include <QFile>

#include <map>
#include <set>
#include <string>
#include <utility>
#include <memory>

class GridMeasureManager: public MeasureManager {
    public:
        GridMeasureManager(GridCell* cell);
        virtual ~GridMeasureManager() = default;
        virtual GridMeasureManager* clone();

        void dataNodes(set<pair<int,int>> nodes);
        set<pair<int,int>> dataNodes();

        virtual bool writeMVarsFile(QXmlStreamWriter& xml);
        virtual bool readMvarsFile(QXmlStreamReader& xml);

        virtual void setupMeasures(string filename);
        virtual void measure(double time);
        virtual void write(QFile* file = 0);
        virtual void writeLast(string filename);
        virtual std::string nameString();
        virtual void clear();
        virtual void resetMeasures();

    private:
        GridMeasureManager(const GridMeasureManager&);
        set<pair<int,int>> __dataNodes;
        map<pair<string,pair<int,int>>,shared_ptr<Measure>> measures;
        Grid* grid = 0;
};
#endif

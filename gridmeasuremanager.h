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
        void cell(GridCell* cell);

        virtual bool writeMVarsFile(QXmlStreamWriter& xml);
        virtual bool readMvarsFile(QXmlStreamReader& xml);

        virtual void setupMeasures(string filename);
        virtual void measure(double time);
        virtual void write(QFile* file = 0);
        virtual void writeSingleCell(pair<int,int> node, QFile* file = 0);
        virtual void writeLast(string filename);
        virtual std::string nameString(pair<int,int> node) const;
        virtual void close();
        virtual void clear();
        virtual void resetMeasures(pair<int,int> node);

    private:
        GridMeasureManager(const GridMeasureManager&);
        set<pair<int,int>> __dataNodes;
        map<pair<int,int>,shared_ptr<QFile>> ofiles;
        map<pair<int,int>,string> lasts;
        map<pair<int,int>,map<string,shared_ptr<Measure>>> measures;
        Grid* grid = 0;
};
#endif

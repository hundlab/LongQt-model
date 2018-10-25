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
namespace LongQt {

class GridMeasureManager: public MeasureManager {
    public:
        GridMeasureManager(std::shared_ptr<GridCell> cell);
        virtual ~GridMeasureManager() = default;
        virtual GridMeasureManager* clone();

        void dataNodes(std::set<std::pair<int,int>> nodes);
        std::set<std::pair<int,int>> dataNodes();
        void cell(std::shared_ptr<GridCell> cell);

        virtual bool writeMVarsFile(QXmlStreamWriter& xml);
        virtual bool readMvarsFile(QXmlStreamReader& xml);

        virtual void setupMeasures(std::string filename);
        virtual void measure(double time);
        virtual void write(QFile* file = 0);
        virtual void writeSingleCell(std::pair<int,int> node, QFile* file = 0);
        virtual void writeLast(std::string filename);
        virtual std::string nameString(std::pair<int,int> node) const;
        virtual void close();
        virtual void clear();
        virtual void resetMeasures(std::pair<int,int> node);

    private:
        GridMeasureManager(const GridMeasureManager&);
        std::set<std::pair<int,int>> __dataNodes;
        std::map<std::pair<int,int>,std::shared_ptr<QFile>> ofiles;
        std::map<std::pair<int,int>,std::string> lasts;
        std::map<std::pair<int,int>,std::map<std::string,std::shared_ptr<Measure>>> measures;
        Grid* grid = 0;
};
} //LongQt

#endif

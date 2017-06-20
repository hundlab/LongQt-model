// 5/10/2016
//################################################
//cell type for a grid, used by grid protocol to
// run a simulaiton on a grid of cells
// class declarations
//################################################

#ifndef GRIDCELL_H
#define GRIDCELL_H

#include "grid.h"
#include "cell.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include <set>

class GridCell: public Cell {
    public:
        GridCell();
        GridCell(GridCell& toCopy);
        ~GridCell();

        void Initialize();
        GridCell* clone();
        Grid* getGrid();

        virtual void updateConc();
        virtual void updateCurr();
        virtual double updateV();
        virtual int externalStim(double stimval); //stimulates every cell
        virtual double tstep(double stimt);
        virtual set<string> getVariables();
        virtual set<string> getConstants();

        //cell io functions
        virtual void setGridfile(string name);
        virtual string gridfile();
        virtual bool writeGridfile(QXmlStreamWriter& xml);
        virtual bool writeGridfile(string fileName ="");
        virtual bool readGridfile(QXmlStreamReader& xml);
        bool handleNode(QXmlStreamReader& xml, set<CellInfo*>& cells, CellInfo* info);
        bool handleRow(QXmlStreamReader& xml, set<CellInfo*>& cells, CellInfo* info);
        bool handleGrid(QXmlStreamReader& xml);
        virtual bool readGridfile(string filename);
        virtual bool setOutputfileConstants(string filename);
        virtual bool setOuputfileVariables(string filename);
        virtual bool setConstantSelection(set<string> new_selection);
        virtual bool setVariableSelection(set<string> new_selection);
        virtual void writeConstants();
        virtual void writeVariables();
        virtual void closeFiles();
        virtual bool readCellState(string filename);
        virtual bool writeCellState(string filename);

    private:
        Grid grid;
        char buffer[500];
        double dx; //0.01
        double dy;
        double np; //1
        int tcount;//0
        string gridfileName;

        void makeMap();
};
#endif

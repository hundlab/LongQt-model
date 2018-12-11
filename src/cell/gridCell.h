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

#include <list>
#include <set>
namespace LongQt {

class GridCell: public Cell {
    public:
        GridCell();
        GridCell(GridCell& toCopy);
        ~GridCell();

        GridCell* clone() override;
        Grid* getGrid();

        virtual void updateConc();
        virtual void updateCurr();
        virtual double updateV();
        virtual void externalStim(double stimval); //stimulates every cell
        virtual double tstep(double stimt);
        virtual std::set<std::string> vars();
        virtual std::set<std::string> pars();

        //cell io functions
        virtual void setGridfile(std::string name);
        virtual std::string gridfile();
        virtual bool writeGridfile(QXmlStreamWriter& xml);
        virtual bool writeGridfile(std::string fileName ="");
        virtual bool readGridfile(QXmlStreamReader& xml);
        virtual bool readGridfile(std::string filename);
        virtual bool setOutputfileConstants(std::string filename);
        virtual bool setOuputfileVariables(std::string filename);
        virtual bool setConstantSelection(std::set<std::string> new_selection);
        virtual bool setVariableSelection(std::set<std::string> new_selection);
        virtual void writeConstants();
        virtual void writeVariables();
        virtual void closeFiles();
        virtual bool readCellState(std::string filename);
        virtual bool writeCellState(std::string filename);
        virtual const char* type() const;

    private:
        bool handleNode(QXmlStreamReader& xml, std::list<CellInfo>& cells, CellInfo& info);
        bool handleRow(QXmlStreamReader& xml, std::list<CellInfo>& cells, CellInfo& info);
        bool handleGrid(QXmlStreamReader& xml);

        Grid grid;
        double dx; //0.01
        double dy;
        double np; //1
        int tcount;//0
        std::string gridfileName;

        void makeMap();
    protected:
        virtual void Initialize();
};
} //LongQt

#endif

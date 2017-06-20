#include "gridmeasuremanager.h"

GridMeasureManager::GridMeasureManager(GridCell* cell):
MeasureManager(cell) {
    this->grid = cell->getGrid();
}

GridMeasureManager* GridMeasureManager::clone() {
    return new GridMeasureManager(*this);
}

void GridMeasureManager::dataNodes(set<pair<int,int>> nodes) {
    this->__dataNodes = nodes;
}

set<pair<int,int>> GridMeasureManager::dataNodes() {
    return this->__dataNodes;
}

bool GridMeasureManager::writeMVarsFile(QXmlStreamWriter& xml) {
    xml.writeStartElement("gridMvars");
    MeasureManager::writeMVarsFile(xml);
    xml.writeStartElement("dataNodes");
    for(auto& node: this->__dataNodes) {
        xml.writeStartElement("node");
        xml.writeAttribute("x",QString::number(node.first));
        xml.writeAttribute("y",QString::number(node.second));
        xml.writeEndElement();
    }
    xml.writeEndElement();
    xml.writeEndElement();
    return true;
}

bool GridMeasureManager::readMvarsFile(QXmlStreamReader& xml) {
    if(!CellUtils::readNext(xml, "gridMvars")) return false;
    MeasureManager::readMvarsFile(xml);
    if(xml.readNextStartElement() && xml.name()=="dataNodes") {
        while(!xml.atEnd() && xml.readNextStartElement() && xml.name()=="node"){
            auto attrib = xml.attributes();
            int x = attrib.value("x").toInt();
            int y = attrib.value("y").toInt();
            this->__dataNodes.insert({x,y});
            xml.skipCurrentElement();
        }
    }
    return true;
}

void GridMeasureManager::setupMeasures(string filename) {
    this->measures.clear();
    if(variableSelection.count("vOld") == 0) {
        variableSelection.insert({"vOld",{}});
    }
    this->ofile = new QFile(filename.c_str());
    if(!ofile->open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("MeasureManager: File could not be opened for writing.");
    }
    for(auto& sel: variableSelection) {
        for(auto& node: this->__dataNodes) {
            if(this->grid->findNode(node)!=NULL
                &&this->grid->findNode(node)->cell->vars.count(sel.first) > 0)
                {
                measures.insert({{sel.first,{node.first,node.second}},
                    Measure(sel.second,__percrepol)}).first;
            }
        }
    }
    if(ofile->write((this->nameString()+"\n").c_str())==-1) {
        qWarning("MeasureManager: File cound not be written to");
    }
}

std::string GridMeasureManager::nameString() {
    string nameStr = "";
    for(auto& meas: this->measures) {
        nameStr += meas.second.getNameString(
            "cell"+to_string(meas.first.second.first)+
            "_"+to_string(meas.first.second.second)+
            "/"+meas.first.first+"/");
    }
    return nameStr;
}

void GridMeasureManager::measure(double time) {
    bool write = false;
    for(auto& m: this->measures) {
        double* val = this->grid->findNode(m.first.second)->
            cell->vars.at(m.first.first);
        bool varWrite = m.second.measure(time, *val);
        if(m.first.first=="vOld"&&varWrite) {
            write = true;
        }
    }
    if(write) {
        this->write();
        this->resetMeasures();
    }
}

void GridMeasureManager::write(QFile* file) {
    QFile* ofile;
    if(file == 0) {
        ofile = this->ofile;
    } else {
        ofile  = file;
    }
    if(!ofile->isOpen()) {
        qWarning("MeasureManager: Measure file must be open to be written");
        return;
    }
    this->last = "";
    for(auto& meas: measures) {
        string valStr = meas.second.getValueString();
        last += valStr;
        if(ofile->write(valStr.c_str())==-1) {
            qWarning("MeasureManager: File cound not be written to");
        }
    }
    last += "\n";
    if(ofile->write("\n")==-1) {
        qWarning("MeasureManager: File cound not be written to");
    }
}

void GridMeasureManager::writeLast(string filename) {
    QFile lastFile(filename.c_str());
    if (!lastFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("MeasureManager: File could not be opened for writing.");
    }
    if(lastFile.write((this->nameString()+"\n").c_str())==-1) {
        qWarning("MeasureManager: Last file cound not be written to");
    }
    if(this->last == "") {
        this->write(&lastFile);
    } else {
        if(lastFile.write(last.c_str())==-1) {
            qWarning("MeasureManager: Last file cound not be written to");
        }
    }
    lastFile.close();
}

void GridMeasureManager::clear() {
    __percrepol = 50;
    variableSelection.clear();
    measures.clear();
}

void GridMeasureManager::resetMeasures() {
    for(auto& meas: this->measures) {
        meas.second.reset();
    }
}


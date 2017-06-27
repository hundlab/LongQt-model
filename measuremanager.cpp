#include "measuremanager.h"
#include "cellutils.h"
#include <QDebug>

MeasureManager::MeasureManager(Cell* cell): __cell(cell) {};

MeasureManager::~MeasureManager() {
    if(this->ofile) {
        ofile->close();
    }
}

MeasureManager::MeasureManager(const MeasureManager& o) {
    this->copy(o);
}

MeasureManager* MeasureManager::clone() {
    return new MeasureManager(*this);
}

Cell* MeasureManager::cell() {
    return this->__cell;
}
void MeasureManager::copy(const MeasureManager& o) {
    __cell = o.__cell;
    variableSelection = o.variableSelection;
    __percrepol = o.__percrepol;
    last = o.last;
}

void MeasureManager::cell(Cell* cell) {
    if(cell->type != __cell->type) {
        this->variableSelection.clear();
        this->measures.clear();
    }
    this->__cell = cell;
}

map<string,set<string>> MeasureManager::selection() {
    return this->variableSelection;
}

void MeasureManager::selection(map<string,set<string>> sel) {
    this->variableSelection = sel;
}

double MeasureManager::percrepol() {
    return this->__percrepol;
}

void MeasureManager::percrepol(double percrepol) {
    this->__percrepol = percrepol;
}

shared_ptr<Measure> MeasureManager::getMeasure(string varname, set<string> selection) {
    if(varsMeas.count(varname)>0) {
        string measName = varsMeas.at(varname);
        return shared_ptr<Measure>(varMeasCreator.at(measName)(selection));
    }
    return shared_ptr<Measure>(new Measure(selection));
}

void MeasureManager::addMeasure(string var) {
    variableSelection.insert({var,{}});
}

void MeasureManager::removeMeasure(string var) {
    measures.erase(var);
}

void MeasureManager::setupMeasures(string filename) {
    this->measures.clear();
    if(variableSelection.count("vOld") == 0) {
        variableSelection.insert({"vOld",{}});
    }
    this->removeBad();
    this->ofile.reset(new QFile(filename.c_str()));
    if (!ofile->open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("MeasureManager: File could not be opened for writing.");
    }
    for(auto& sel: variableSelection) {
        auto it = measures
            .insert({sel.first,this->getMeasure(sel.first,sel.second)}).first;
        string nameStr = it->second->getNameString(sel.first);
        if(ofile->write(nameStr.c_str())==-1) {
            qWarning("MeasureManager: File cound not be written to");
        }
    }
    if(ofile->write("\n")==-1) {
        qWarning("MeasureManager: File cound not be written to");
    }
}

void MeasureManager::measure(double time) {
    bool write = false;
    for(auto& m: this->measures) {
        bool varWrite = m.second->measure(time, *__cell->vars.at(m.first));
        if(m.first=="vOld"&&varWrite) {
            write = true;
        }
    }
    if(write) {
        this->write();
        this->resetMeasures();
    }
}

void MeasureManager::writeLast(string filename) {
    QFile lastFile(filename.c_str());
    if (!lastFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("MeasureManager: File could not be opened for writing.");
    }
    for(auto& meas: this->measures) {
        string nameStr = meas.second->getNameString(meas.first);
        if(lastFile.write(nameStr.c_str())==-1) {
            qWarning("MeasureManager: Last file cound not be written to");
        }
    }
    if(lastFile.write("\n")==-1) {
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

void MeasureManager::write(QFile* file) {
    QFile& ofile = file? *file: *this->ofile;
    if(!ofile.isOpen()) {
        qWarning("MeasureManager: Measure file must be open to be written");
        return;
    }
    this->last = "";
    for(auto& meas: measures) {
        string valStr = meas.second->getValueString();
        last += valStr;
        if(ofile.write(valStr.c_str())==-1) {
            qWarning("MeasureManager: File cound not be written to");
        }
    }
    last += "\n";
    if(ofile.write("\n")==-1) {
        qWarning("MeasureManager: File cound not be written to");
    }
}

void MeasureManager::clear() {
    __percrepol = 50;
    variableSelection.clear();
    measures.clear();
}

void MeasureManager::close() {
    if(this->ofile->isOpen()) {
        ofile->close();
    }
}

void MeasureManager::resetMeasures() {
    for(auto& meas: this->measures) {
        meas.second->reset();
    }
}

//#############################################################
//read and mvars style file and use it to set measures list
//where varname is the first word on the line
//and all sequential words are properties to measure (the selection)
//#############################################################
bool MeasureManager::readMvarsFile(QXmlStreamReader& xml)
{
    variableSelection.clear();
    set<string> possible_vars = __cell->getVariables();
    if(!CellUtils::readNext(xml, "mvars")) return false;
    if(xml.readNextStartElement() && xml.name()=="percrepol") {
        xml.readNext();
        __percrepol = xml.text().toDouble();
        xml.skipCurrentElement();
    } else {
        return true;
    }
    while(!xml.atEnd() && xml.readNextStartElement() && xml.name()=="mvar"){
        string varname = xml.attributes().value("name").toString().toStdString();
        set<string> selection;
        while(!xml.atEnd() && xml.readNextStartElement() && xml.name()=="property"){
            xml.readNext();
            string propName = xml.text().toString().toStdString();
            selection.insert(propName);
            xml.readNext();
        }
        if(possible_vars.count(varname) == 1) {
            variableSelection.insert({varname,selection});
        }
        xml.skipCurrentElement();
    }
    return true;
};

bool MeasureManager::writeMVarsFile(QXmlStreamWriter& xml) {
    xml.writeStartElement("mvars");
    xml.writeTextElement("percrepol",QString::number(__percrepol));
    for(auto& sel: this->variableSelection){
        xml.writeStartElement("mvar");
        xml.writeAttribute("name",sel.first.c_str());
        for(auto& prop: sel.second){
            xml.writeTextElement("property",prop.c_str());
        }
        xml.writeEndElement();
    }
    xml.writeEndElement();
    return 0;
}

void MeasureManager::removeBad() {
    list<map<string,set<string>>::iterator> bad;
    auto vars = __cell->getVariables();
    for(auto it = this->variableSelection.begin(); it != variableSelection.end(); ++it) {
        if(vars.count(it->first) != 1) {
            bad.push_back(it);
        }
    }
    for(auto& b: bad) {
        variableSelection.erase(b);
    }
}

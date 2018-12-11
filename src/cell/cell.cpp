//####################################################
// This file contains general function definitions 
// for simulation of excitable cell activity read and
// write functions.
//
// Copyright (C) 2011 Thomas J. Hund.
//####################################################

#include "cell.h"
#include "cellutils.h"
#include "logger.h"
#include <sstream>
#include <QFile>
using namespace LongQt;
using namespace std;

bool Cell::setOuputfileVariables(string filename) {
    return setOutputfile(filename, this->varsSelection, &varsofile);
};

bool Cell::setOutputfileConstants(string filename) {
    return setOutputfile(filename, this->parsSelection, &parsofile);
};

void Cell::writeVariables() {
    write(varsSelection, this->__vars, &varsofile);
};

void Cell::writeConstants() {
    write(parsSelection, this->__pars, &parsofile);
};

bool Cell::setConstantSelection(set<string> selection) {
    return setSelection(this->__pars, &this->parsSelection, selection, &parsofile);
};

bool Cell::setVariableSelection(set<string> selection) {
    return setSelection(this->__vars, &this->varsSelection, selection, &varsofile);
};

set<string> Cell::getConstantSelection() {
    return parsSelection;
};

set<string> Cell::getVariableSelection() {
    return varsSelection;
};

bool Cell::setSelection(map<string, double*> map, set<string>* old_selection, set<string> new_selection, ofstream* ofile) {
    bool toReturn = true;
    set<string>::iterator set_it;
    
    *old_selection = set<string>();

    for(set_it = new_selection.begin(); set_it != new_selection.end(); set_it++) {
        if(map.find(*set_it) != map.end()) {
            old_selection->insert(*set_it);
        } else {
            toReturn = false;
        }
    }
    if(!ofile->good()) {
        return toReturn;
    }
    ofile->seekp(0);
    for(set<string>::iterator it = old_selection->begin(); it != old_selection->end(); it++) {
        *ofile << *it << "\t";
    }
    *ofile << endl;
    
    return toReturn;
};

void Cell::closeFiles() {
    IOBase::closeFile(&parsofile);
    IOBase::closeFile(&varsofile);
}
bool Cell::writeCellState(string file) {
	QFile ofile(file.c_str());
	string name;

	if(!ofile.open(QIODevice::WriteOnly|QIODevice::Text)){
        Logger::getInstance()->write<std::runtime_error>("Cell: Error opening {}", file);
		return 1;
	}
	QXmlStreamWriter xml(&ofile);
	xml.setAutoFormatting(true);
	xml.writeStartDocument();
	bool success = this->writeCellState(xml);
	xml.writeEndDocument();
    ofile.close();
    return success;
}
bool Cell::writeCellState(QXmlStreamWriter& xml) {
	xml.writeStartElement("cell");
	xml.writeAttribute("type", this->type());

	xml.writeStartElement("pars");
	for(auto& par : this->__pars){
		xml.writeStartElement("par");
		xml.writeAttribute("name",par.first.c_str());
		xml.writeCharacters(QString::number(*par.second));
		xml.writeEndElement();
	}
	xml.writeEndElement();

	xml.writeStartElement("vars");
	for(auto& var : this->__vars){
		xml.writeStartElement("var");
		xml.writeAttribute("name",var.first.c_str());
		xml.writeCharacters(QString::number(*var.second));
		xml.writeEndElement();
	}
	xml.writeEndElement();

	xml.writeEndElement();
    return true;
}
bool Cell::readCellState(string file) {
	QFile ifile(file.c_str());

	if(!ifile.open(QIODevice::ReadOnly)){
       Logger::getInstance()->write<std::runtime_error>("Cell: Error opening {}", file);
		return false;
	}
	QXmlStreamReader xml(&ifile);
	this->readCellState(xml);
	ifile.close();
	return true;
}
bool Cell::readCellState(QXmlStreamReader& xml) {
	string name = "";
	if(!CellUtils::readNext(xml, "cell")) return false;
	QString type = xml.attributes().value("type").toString();
	if(type != this->type()) {
        Logger::getInstance()->write<std::logic_error>("Cell: cell state file is for {} but cell is of type {}"
                ,type.toStdString(),this->type());
	}
	if(!CellUtils::readNext(xml, "pars")) return false;
	while(!xml.atEnd() && xml.readNextStartElement()){
		name = xml.attributes().value("name").toString().toStdString();
		xml.readNext();
		bool ok = true;
		double val = xml.text().toDouble(&ok);
		if(ok) {
			try {
				*(this->__pars.at(name)) = val;
            } catch (const std::out_of_range&) {
                Logger::getInstance()->write("Cell: {} not in cell pars",name);
			}
		}
		xml.readNext();
	}
	if(!CellUtils::readNext(xml, "vars")) return false;
	while(!xml.atEnd() && xml.readNextStartElement()){
		name = xml.attributes().value("name").toString().toStdString();
		xml.readNext();
		bool ok = true;
		double val = xml.text().toDouble(&ok);
		if(ok) {
			try {
				*(this->__vars.at(name)) = val;
            } catch (const std::out_of_range&) {
                Logger::getInstance()->write("Cell: {} not in cell vars",name);
			}
		}
		xml.readNext();
	}
	return true;
}

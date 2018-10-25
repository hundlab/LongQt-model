#include "settingsIO.h"
#include "cellutils.h"
#include <QFile>
#include <QDebug>
#include <QDir>
using namespace LongQt;
using namespace std;

SettingsIO* SettingsIO::__instance = 0;

SettingsIO* SettingsIO::getInstance() {
    if(!__instance) {
        __instance = new SettingsIO();
    }
    return __instance;
}

void SettingsIO::write(shared_ptr<Protocol> proto, QXmlStreamWriter& xml) {
    xml.writeStartDocument();
    xml.writeStartElement("file");

    xml.writeTextElement("protocolType",proto->type());
    proto->writepars(xml);
    proto->measureMgr().writeMVarsFile(xml);
    this->writedvars(proto, xml);
    proto->pvars().writePvars(xml);

    xml.writeEndElement();
}

void SettingsIO::writeSettings(QString filename, shared_ptr<Protocol> proto) {
    QFile ofile(filename);
    string name;

    if(!ofile.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate)){
        qCritical() << "SettingsIO: Error opening " << filename;
        return;
    }
    QXmlStreamWriter xml(&ofile);
    xml.setAutoFormatting(true);
    this->write(proto, xml);
}

void SettingsIO::writeSettingsStr(QString* text, shared_ptr<Protocol> proto) {
    QXmlStreamWriter xml(text);
    this->write(proto, xml);
}

bool SettingsIO::readProtoType(shared_ptr<Protocol>& proto,  QXmlStreamReader& xml) {
    if(CellUtils::readNext(xml, "protocolType")) {
        xml.readNext();
        QString type = xml.text().toString();
        if(!proto|| proto->type() != type) {
            if(!allowProtoChange) {
                qWarning("SettingsIO: Changing protocol type is disabled");
                throw std::invalid_argument("SettingsIO: Changing protocol type is disabled");
                return false;
            }
            try {
                QString datadir = proto ? proto->getDataDir().c_str() : "";
                proto = CellUtils::protoMap.at(type.toStdString())();
                proto->setDataDir(datadir.toStdString());
                emit ProtocolChanged(proto);
            } catch (const std::out_of_range&) {
                qWarning("SettingsIO: %s not in protocol map", type.toStdString().c_str());
                return true;
            }
        }
    } else {
        qWarning("SettingsIO: Settings file does not contain protocol type");
        return false;
    }
    return true;
}

void SettingsIO::read(shared_ptr<Protocol> proto, QXmlStreamReader& xml) {
    try {
        this->readProtoType(proto,xml);
        proto->readpars(xml);
        /*
           try {
           if(proto->pars.at("writeCellState").get() == "true") {
           QDir fileDir(fileName);
           fileDir.cdUp();
           proto->pars.at("cellStateDir").set(fileDir.path().toStdString());
           proto->pars.at("readCellState").set("true");
           proto->pars.at("writeCellState").set("false");
           }
           } catch(const std::out_of_range& e) {
           qDebug("SimvarMenu: par not in proto: %s", e.what());
           }*/
        //    proto->datadir = working_dir.absolutePath().toStdString();
        proto->measureMgr().readMvarsFile(xml);
        this->readdvars(proto, xml);
        proto->pvars().readPvars(xml);
    } catch (const std::invalid_argument&) {
        return;
    }

    this->lastProto = proto;
}

void SettingsIO::readSettings(QString filename, shared_ptr<Protocol> proto) {
    QFile ifile(filename);

    if(!ifile.open(QIODevice::ReadOnly)){
        qCritical() << "SettingsIO: Error opening " << filename;
        return;
    }
    QXmlStreamReader xml(&ifile);

    this->read(proto, xml);

    ifile.close();
}
void SettingsIO::readSettingsStr(QString text, shared_ptr<Protocol> proto) {
    QXmlStreamReader xml(text);
    this->read(proto, xml);
}
void SettingsIO::writedvars(shared_ptr<Protocol> proto, QXmlStreamWriter& xml) {
    set<string> selection = proto->cell()->getVariableSelection();

    xml.writeStartElement("dvars");
    for(auto it = selection.begin(); it != selection.end(); it++){
        xml.writeTextElement("dvar",it->c_str());
    }
    xml.writeEndElement();
}
void SettingsIO::readdvars(shared_ptr<Protocol> proto, QXmlStreamReader& xml) {
    set<string> selection;

    if(!CellUtils::readNext(xml, "dvars")) return;
    while(!xml.atEnd() && xml.readNextStartElement()){
        xml.readNext();
        selection.insert(xml.text().toString().toStdString());
        xml.readNext();
    }
    if(!proto->cell()->setVariableSelection(selection)) {
        qWarning("SettingsIO: Some dvars were not vars in cell");
    }
}

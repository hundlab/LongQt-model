#include "settingsIO.h"
#include "cellutils.h"
#include <QFile>
#include <QDebug>

SettingsIO* SettingsIO::__instance = 0;

SettingsIO* SettingsIO::getInstance() {
    if(!__instance) {
        __instance = new SettingsIO();
    }
    return __instance;
}

void SettingsIO::writeSettings(shared_ptr<Protocol> proto, QString filename) {
    QFile ofile(filename);
    string name;

    if(!ofile.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate)){
        qCritical() << "SettingsIO: Error opening " << filename;
        return;
    }
    QXmlStreamWriter xml(&ofile);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("file");

    xml.writeTextElement("protocolType",proto->type);
    proto->writepars(xml);
    proto->measureMgr().writeMVarsFile(xml);
    this->writedvars(proto, xml);
    proto->pvars().writePvars(xml);

    xml.writeEndElement();

}

bool SettingsIO::readProtoType(shared_ptr<Protocol>& proto,  QXmlStreamReader& xml) {
    if(CellUtils::readNext(xml, "protocolType")) {
        xml.readNext();
        QString type = xml.text().toString();
        if(proto->type != type) {
            if(!allowProtoChange) {
                qWarning("SettingsIO: Changing protocol type is disabled");
                throw std::invalid_argument("SettingsIO: Changing protocol type is disabled");
                return false;
            }
            try {
                string datadir = proto->datadir;
                proto.reset(CellUtils::protoMap.at(type.toStdString())());
                proto->datadir = datadir;
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

void SettingsIO::readSettings(shared_ptr<Protocol> proto, QString filename) {
    QFile ifile(filename);

    if(!ifile.open(QIODevice::ReadOnly)){
        qCritical() << "SettingsIO: Error opening " << filename;
        return;
    }
    QXmlStreamReader xml(&ifile);

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
    ifile.close();
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

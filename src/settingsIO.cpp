#include "settingsIO.h"
#include <QDir>
#include <QFile>
#include "cellutils.h"
#include "logger.h"
using namespace LongQt;
using namespace std;

SettingsIO* SettingsIO::__instance = 0;

SettingsIO* SettingsIO::getInstance() {
  if (!__instance) {
    __instance = new SettingsIO();
  }
  return __instance;
}

void SettingsIO::write(shared_ptr<Protocol> proto, QXmlStreamWriter& xml) {
  xml.writeStartDocument();
  xml.writeStartElement("file");

  xml.writeTextElement("protocolType", proto->type());
  proto->writepars(xml);
  proto->measureMgr().writeMVarsFile(xml);
  this->writedvars(proto, xml);
  proto->pvars().writePvars(xml);

  xml.writeEndElement();
}

void SettingsIO::writeSettings(QString filename, shared_ptr<Protocol> proto) {
  QFile ofile(filename);
  string name;

  if (!ofile.open(QIODevice::WriteOnly | QIODevice::Text |
                  QIODevice::Truncate)) {
    Logger::getInstance()->write<std::runtime_error>(
        "SettingsIO: Error opening {}", filename.toStdString());
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

bool SettingsIO::protoChanged() const { return __protoChanged; }

shared_ptr<Protocol> SettingsIO::readProtoType(shared_ptr<Protocol> proto,
                                               QXmlStreamReader& xml) {
  shared_ptr<Protocol> new_proto = proto;
  __protoChanged = false;
  if (CellUtils::readNext(xml, "protocolType")) {
    xml.readNext();
    QString type = xml.text().toString();
    if (!proto || proto->type() != type) {
      try {
        string datadir = proto ? proto->getDataDir() : "";
        new_proto = CellUtils::protoMap.at(type.toStdString())();
        new_proto->setDataDir(datadir);
        __protoChanged = true;
      } catch (const std::out_of_range&) {
        Logger::getInstance()->write<std::out_of_range>(
            "SettingsIO: {} not in protocol map", type.toStdString());
        return 0;
      }
    }
  } else {
    Logger::getInstance()->write(
        "SettingsIO: Settings file does not contain protocol type");
  }
  return new_proto;
}

shared_ptr<Protocol> SettingsIO::read(shared_ptr<Protocol> proto,
                                      QXmlStreamReader& xml) {
  try {
    auto new_proto = this->readProtoType(proto, xml);
    if (!new_proto) {
      return 0;
    }
    new_proto->readpars(xml);
    new_proto->measureMgr().readMvarsFile(xml);
    this->readdvars(new_proto, xml);
    new_proto->pvars().readPvars(xml);
    return new_proto;
  } catch (const std::invalid_argument&) {
    return 0;
  }
}

shared_ptr<Protocol> SettingsIO::readSettings(QString filename,
                                              shared_ptr<Protocol> proto) {
  QFile ifile(filename);

  if (!ifile.open(QIODevice::ReadOnly)) {
    Logger::getInstance()->write<std::runtime_error>(
        "SettingsIO: Error opening {}", filename.toStdString());
    return proto;
  }
  QXmlStreamReader xml(&ifile);

  auto new_proto = this->read(proto, xml);

  ifile.close();
  return new_proto;
}
shared_ptr<Protocol> SettingsIO::readSettingsStr(QString text,
                                                 shared_ptr<Protocol> proto) {
  QXmlStreamReader xml(text);
  return this->read(proto, xml);
}
void SettingsIO::writedvars(shared_ptr<Protocol> proto, QXmlStreamWriter& xml) {
  set<string> selection = proto->cell()->getVariableSelection();

  xml.writeStartElement("dvars");
  for (auto it = selection.begin(); it != selection.end(); it++) {
    xml.writeTextElement("dvar", it->c_str());
  }
  xml.writeEndElement();
}
void SettingsIO::readdvars(shared_ptr<Protocol> proto, QXmlStreamReader& xml) {
  set<string> selection;

  if (!CellUtils::readNext(xml, "dvars")) return;
  while (!xml.atEnd() && xml.readNextStartElement()) {
    xml.readNext();
    selection.insert(xml.text().toString().toStdString());
    xml.readNext();
  }
  proto->cell()->setVariableSelection(selection);
}

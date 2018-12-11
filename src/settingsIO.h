#ifndef SETTINGSIO_H
#define SETTINGSIO_H

#include <QObject>
#include "protocol.h"
namespace LongQt {

class SettingsIO {
 public:
  static SettingsIO* getInstance();
  void writedvars(std::shared_ptr<Protocol> proto,
                  QXmlStreamWriter& xml);  // write varmap keys to a file
  void readdvars(std::shared_ptr<Protocol> proto, QXmlStreamReader& xml);

  std::shared_ptr<Protocol> readSettings(
      QString filename, std::shared_ptr<Protocol> proto = NULL);
  std::shared_ptr<Protocol> readSettingsStr(
      QString text, std::shared_ptr<Protocol> proto = NULL);
  void writeSettings(QString filename, std::shared_ptr<Protocol> proto);
  void writeSettingsStr(QString* text, std::shared_ptr<Protocol> proto);
  bool protoChanged() const;

 private:
  std::shared_ptr<Protocol> readProtoType(std::shared_ptr<Protocol> proto,
                                          QXmlStreamReader& xml);
  std::shared_ptr<Protocol> read(std::shared_ptr<Protocol> proto,
                                 QXmlStreamReader& xml);
  void write(std::shared_ptr<Protocol> proto, QXmlStreamWriter& xml);

  bool __protoChanged = false;
  static SettingsIO* __instance;
  SettingsIO() = default;
};
}  // namespace LongQt

#endif

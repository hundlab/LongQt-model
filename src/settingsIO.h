#ifndef SETTINGSIO_H
#define SETTINGSIO_H

#include <QObject>
#include "protocol.h"
namespace LongQt {

class SettingsIO : public QObject {
    Q_OBJECT
    public:
        static SettingsIO* getInstance();
        void writedvars(std::shared_ptr<Protocol> proto, QXmlStreamWriter& xml); //write varmap keys to a file
        void readdvars(std::shared_ptr<Protocol> proto, QXmlStreamReader& xml);

        std::shared_ptr<Protocol> lastProto;
        bool allowProtoChange = true;
    private:
        bool readProtoType(std::shared_ptr<Protocol>& proto, QXmlStreamReader& xml);
        void read(std::shared_ptr<Protocol> proto, QXmlStreamReader& xml);
        void write(std::shared_ptr<Protocol> proto, QXmlStreamWriter& xml);

        static SettingsIO* __instance;
        SettingsIO() = default;
    public slots:
        void readSettings(QString filename, std::shared_ptr<Protocol> proto = NULL);
        void readSettingsStr(QString text, std::shared_ptr<Protocol> proto = NULL);
        void writeSettings(QString filename, std::shared_ptr<Protocol> proto);
        void writeSettingsStr(QString* text, std::shared_ptr<Protocol> proto);
    signals:
        void ProtocolChanged(std::shared_ptr<Protocol>);
        void CellChanged(std::shared_ptr<Cell>);
        void settingsRead();
};
} //LongQt

#endif

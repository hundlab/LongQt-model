#ifndef SETTINGSIO_H
#define SETTINGSIO_H

#include <QObject>
#include "protocol.h"

class SettingsIO : public QObject {
    Q_OBJECT
    public:
        static SettingsIO* getInstance();
        void writedvars(shared_ptr<Protocol> proto, QXmlStreamWriter& xml); //write varmap keys to a file
        void readdvars(shared_ptr<Protocol> proto, QXmlStreamReader& xml);

        shared_ptr<Protocol> lastProto;
        bool allowProtoChange = true;
    private:
        bool readProtoType(shared_ptr<Protocol>& proto, QXmlStreamReader& xml);

        static SettingsIO* __instance;
        SettingsIO() = default;
    public slots:
        void readSettings(shared_ptr<Protocol> proto, QString filename);
        void writeSettings(shared_ptr<Protocol> proto, QString filename);
    signals:
        void ProtocolChanged(shared_ptr<Protocol>);
        void CellChanged(shared_ptr<Cell>);
        void settingsRead();
};

#endif

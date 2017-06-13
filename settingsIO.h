#ifndef SETTINGSIO_H
#define SETTINGSIO_H

#include <QObject>
#include "protocol.h"

class SettingsIO : public QObject {
Q_OBJECT
	public:
		static SettingsIO* getInstance();
		bool readProtoType(Protocol** proto, QXmlStreamReader& xml);
		bool allowProtoChange = true;
		Protocol* lastProto = 0;
		void writedvars(Protocol* proto, QXmlStreamWriter& xml); //write varmap keys to a file
		void readdvars(Protocol* proto, QXmlStreamReader& xml);
	private:
		static SettingsIO* __instance;
		SettingsIO() = default;
	public slots:
		void readSettings(Protocol* proto, QString filename);
		void writeSettings(Protocol* proto, QString filename);
	signals:
		void ProtocolChanged(Protocol*);
		void CellChanged(Cell*);
		void settingsRead();
};

#endif

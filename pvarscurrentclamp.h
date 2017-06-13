#ifndef PVARSGRID_H
#define PVARSGRID_H

#include "protocol.h"
#include "cellpvars.h"

class PvarsCurrentClamp : public CellPvars {
public:
	//Types
	struct TIonChanParam : IonChanParam {
		vector<double> trials; //index -> value
		virtual string str(string name) override;
		TIonChanParam() = default;
		TIonChanParam(const IonChanParam& other): IonChanParam(other) {};
	};

	//Functions
	PvarsCurrentClamp(Protocol* proto);
	PvarsCurrentClamp(const PvarsCurrentClamp&) = default;
	virtual ~PvarsCurrentClamp() = default;
	virtual CellPvars* clone();

	virtual void setIonChanParams();
	virtual void calcIonChanParams();

	virtual void writePvars(QXmlStreamWriter& xml);
	virtual void readPvars(QXmlStreamReader& xml);
	void handlePvars(QXmlStreamReader& xml);
	void handlePvar(QXmlStreamReader& xml);
	pair<int,double> handleTrial(QXmlStreamReader& xml);

	virtual void insert(string,IonChanParam);

private:
	void calcIonChanParam(TIonChanParam* param);

	map<string,TIonChanParam*>* __pvars =
		reinterpret_cast<map<string,TIonChanParam*>*>(&this->pvars);
	Protocol* proto;
};
#endif

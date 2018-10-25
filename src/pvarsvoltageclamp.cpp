#include "pvarsvoltageclamp.h"
using namespace LongQt;
using namespace std;

PvarsVoltageClamp::PvarsVoltageClamp(Protocol *proto): proto(proto) {}

PvarsVoltageClamp::PvarsVoltageClamp(const PvarsVoltageClamp& o) {
    this->proto = o.proto;
    for(auto pvar: *(o.__pvars)) {
        this->__pvars->insert({pvar.first,new SIonChanParam(*pvar.second)});
    }
}

PvarsCell* PvarsVoltageClamp::clone() {
    return new PvarsVoltageClamp(*this);
}
void PvarsVoltageClamp::protocol(Protocol* proto) {
    this->proto = proto;
}
void PvarsVoltageClamp::setIonChanParams() {
    for(auto& pvar : *this->__pvars) {
        proto->cell()->setPar(pvar.first, pvar.second->paramVal);
    }
}
void PvarsVoltageClamp::calcIonChanParams() {
    for(auto& pvar : *this->__pvars) {
        this->calcIonChanParam(pvar.second);
    }
}
void PvarsVoltageClamp::calcIonChanParam(SIonChanParam* param) {
    double val = 0;
    switch(param->dist) {
        case Distribution::none:
            val = param->val[0];
            break;
        case Distribution::normal:
            {
                normal_distribution<double> distribution(param->val[0]
                        ,param->val[1]);
                val = distribution(this->generator);
                break;
            }
        case Distribution::lognormal:
            {
                lognormal_distribution<double> logdistribution(
                        param->val[0], param->val[1]);
                val = logdistribution(this->generator);
                break;
            }
    }
    param->paramVal = val;
}
void PvarsVoltageClamp::writePvars(QXmlStreamWriter& xml) {
    xml.writeStartElement("pvars");
    for(auto& pvar : *this->__pvars) {
        xml.writeStartElement("pvar");
        xml.writeAttribute("name", pvar.first.c_str());
        xml.writeTextElement("distribution_type", QString::number(pvar.second->dist));
        xml.writeTextElement("value0", QString::number(pvar.second->val[0]));
        xml.writeTextElement("value1", QString::number(pvar.second->val[1]));
        xml.writeTextElement("cell",QString::number(pvar.second->paramVal));
        xml.writeEndElement();
    }
    xml.writeEndElement();
}

void PvarsVoltageClamp::readPvars(QXmlStreamReader& xml) {
    this->clear();
    while(!xml.atEnd() && xml.name() != "pvars") {
        xml.readNext();
    }
    this->handlePvars(xml);
}

void PvarsVoltageClamp::handlePvars(QXmlStreamReader& xml) {
    if(xml.atEnd()) return;
    while(xml.readNextStartElement() && xml.name()=="pvar"){
        this->handlePvar(xml);
    }
    xml.skipCurrentElement();
}

void PvarsVoltageClamp::handlePvar(QXmlStreamReader& xml) {
    if(xml.atEnd()) return;
    pair<string,SIonChanParam*> pvar;
    pvar.second = new SIonChanParam;
    pvar.first = xml.attributes().value("name").toString().toStdString();
    while(xml.readNextStartElement()) {
        if(xml.name()=="distribution_type") {
            xml.readNext();
            pvar.second->dist = static_cast<Distribution>(xml.text().toInt());
            xml.skipCurrentElement();
        } else if(xml.name()=="value0") {
            xml.readNext();
            pvar.second->val[0] = xml.text().toDouble();
            xml.skipCurrentElement();
        } else if(xml.name()=="value1") {
            xml.readNext();
            pvar.second->val[1] = xml.text().toDouble();
            xml.skipCurrentElement();
        } else if(xml.name()=="cell") {
            xml.readNext();
            pvar.second->paramVal = xml.text().toDouble();
        }
        else {
            xml.skipCurrentElement();
        }
    }
    (*this->__pvars)[pvar.first] = pvar.second;
}
string PvarsVoltageClamp::SIonChanParam::str(string name) {
    string text = name+"\n";
    text += "value = "+std::to_string(paramVal)+"\n";
    return text;
}
void PvarsVoltageClamp::insert(string name,IonChanParam param) {
    SIonChanParam* nparam = new SIonChanParam(const_cast<IonChanParam&>(param));
    this->calcIonChanParam(nparam);
    (*this->__pvars)[name] = nparam;
}

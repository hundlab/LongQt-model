#include "pvarscurrentclamp.h"
using namespace LongQt;
using namespace std;

PvarsCurrentClamp::PvarsCurrentClamp(Protocol* proto) : proto(proto) {}

PvarsCurrentClamp::PvarsCurrentClamp(const PvarsCurrentClamp& o,
                                     Protocol* proto) {
  this->proto = proto;
  for (auto pvar : *(o.__pvars)) {
    this->__pvars->insert({pvar.first, new TIonChanParam(*pvar.second)});
  }
}

void PvarsCurrentClamp::setIonChanParams() {
  int trial = proto->trial();
  for (auto& pvar : *this->__pvars) {
    if (trial >= pvar.second->trials.size()) {
      Logger::getInstance()->write(
          "PvarsCurrentClamp: too few trials calculated {} > {}, recalculating",
          trial, pvar.second->trials.size());
      this->calcIonChanParams();
      break;
    }
  }
  for (auto& pvar : *this->__pvars) {
    try {
      proto->cell()->setPar(pvar.first, pvar.second->trials.at(trial));
    } catch (std::out_of_range&) {
      Logger::getInstance()->write(
          "PvarsCurrentClamp: {} trial {} not calculated", pvar.first, trial);
    }
  }
}
void PvarsCurrentClamp::calcIonChanParams() {
  for (auto& pvar : *this->__pvars) {
    this->calcIonChanParam(pvar.second);
  }
}
void PvarsCurrentClamp::calcIonChanParam(TIonChanParam* param) {
  param->trials.clear();
  int numtrials = this->proto->numtrials();
  for (int i = 0; i < numtrials; i++) {
    double val = 0;
    switch (param->dist) {
      case Distribution::none:
        val = param->val[0] + param->val[1] * i;
        break;
      case Distribution::normal: {
        normal_distribution<double> distribution(param->val[0], param->val[1]);
        val = distribution(this->generator);
        break;
      }
      case Distribution::lognormal: {
        lognormal_distribution<double> logdistribution(param->val[0],
                                                       param->val[1]);
        val = logdistribution(this->generator);
        break;
      }
    }
    param->trials.push_back(val);
  }
}
void PvarsCurrentClamp::writePvars(QXmlStreamWriter& xml) {
  xml.writeStartElement("pvars");
  for (auto& pvar : *this->__pvars) {
    xml.writeStartElement("pvar");
    xml.writeAttribute("name", pvar.first.c_str());
    xml.writeTextElement("distribution_type",
                         QString::number(pvar.second->dist));
    xml.writeTextElement("value0", QString::number(pvar.second->val[0]));
    xml.writeTextElement("value1", QString::number(pvar.second->val[1]));
    xml.writeStartElement("trials");
    int i = 0;
    for (auto& trial : pvar.second->trials) {
      xml.writeStartElement("trial");
      xml.writeAttribute("number", QString::number(i));
      xml.writeCharacters(QString::number(trial));
      xml.writeEndElement();
      i++;
    }
    xml.writeEndElement();
    xml.writeEndElement();
  }
  xml.writeEndElement();
}

void PvarsCurrentClamp::readPvars(QXmlStreamReader& xml) {
  this->clear();
  if (!CellUtils::readNext(xml, "pvars")) return;
  this->handlePvars(xml);
}

void PvarsCurrentClamp::handlePvars(QXmlStreamReader& xml) {
  if (xml.atEnd()) return;
  while (xml.readNextStartElement() && xml.name() == "pvar") {
    this->handlePvar(xml);
  }
  xml.skipCurrentElement();
}

void PvarsCurrentClamp::handlePvar(QXmlStreamReader& xml) {
  if (xml.atEnd()) return;
  pair<string, TIonChanParam*> pvar;
  pvar.second = new TIonChanParam;
  pvar.first = xml.attributes().value("name").toString().toStdString();
  while (xml.readNextStartElement()) {
    if (xml.name() == "distribution_type") {
      xml.readNext();
      pvar.second->dist = static_cast<Distribution>(xml.text().toInt());
      xml.skipCurrentElement();
    } else if (xml.name() == "value0") {
      xml.readNext();
      pvar.second->val[0] = xml.text().toDouble();
      xml.skipCurrentElement();
    } else if (xml.name() == "value1") {
      xml.readNext();
      pvar.second->val[1] = xml.text().toDouble();
      xml.skipCurrentElement();
    } else if (xml.name() == "trials") {
      while (xml.readNextStartElement() && xml.name() == "trial") {
        pair<int, double> p = this->handleTrial(xml);
        if (pvar.second->trials.size() == (unsigned int)p.first) {
          pvar.second->trials.push_back(p.second);
        } else {
          Logger::getInstance()->write(
              "CurrentClamp: Trials in xml are not in order");
        }
      }
    } else {
      xml.skipCurrentElement();
    }
  }
  this->pvars[pvar.first] = pvar.second;
}

pair<int, double> PvarsCurrentClamp::handleTrial(QXmlStreamReader& xml) {
  pair<int, double> c;
  if (xml.atEnd()) return c;
  c.first = xml.attributes().value("number").toInt();
  xml.readNext();
  c.second = xml.text().toDouble();
  xml.skipCurrentElement();
  return c;
}
string PvarsCurrentClamp::TIonChanParam::str(string name) {
  string text = name + "\n";
  int i = 0;
  for (auto& trial : this->trials) {
    text +=
        "trial[" + std::to_string(i) + "] = " + std::to_string(trial) + "\n";
    i++;
  }
  return text;
}
void PvarsCurrentClamp::insert(string name, IonChanParam param) {
  TIonChanParam* nparam = new TIonChanParam(const_cast<IonChanParam&>(param));
  this->calcIonChanParam(nparam);
  (*this->__pvars)[name] = nparam;
}

int PvarsCurrentClamp::numtrials() const
{
  int numtrials = 0;
  bool first = true;
  for (auto& pvar : *this->__pvars) {
    int trial_size = pvar.second->trials.size();
    if(first) {
        numtrials = trial_size;
        first = false;
    } else {
      if(numtrials != trial_size) {
          Logger::getInstance()->write(
              "CurrentClamp: Parameter {}"
              " has a different number of trials "
              "({}) than the other parameters",
                      pvar.first, trial_size);
          return this->proto->numtrials();
      }
    }
  }
  return numtrials;
}

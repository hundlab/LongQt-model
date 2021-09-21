#include "cellutils.h"

#include "FR.h"
#include "OHaraRudyEndo.h"
#include "OHaraRudyEpi.h"
#include "OHaraRudyM.h"
#include "atrial.h"
#include "br04.h"
#include "gpbatrial.h"
#include "gpbatrialonal17.h"
#include "gpbhuman.h"
#include "gridCell.h"
#include "hrd09.h"
#include "ksan.h"
#include "kurata08.h"
#include "tnnp04.h"
#include "Koivumaki.h"
#include "gpbatrial_different_ina.h"

#include "coupledinexcitablecell.h"

#include "currentClampProtocol.h"
#include "gridProtocol.h"
#include "voltageClampProtocol.h"

#include "logger.h"
#include "protocol.h"

#include <stdarg.h>
#include <stdio.h>
using namespace LongQt;
using namespace std;

/*
 * cell map this is how new instances of cells are created
 * if you are adding a new cell to longqt add it here
 * and also add it to the protocolCellDefualts map below
 */
const map<string, CellUtils::CellInitializer> CellUtils::cellMap = {
    {Kurata08().type(), []() { return make_shared<Kurata08>(); }},
    {GpbAtrial().type(), []() { return make_shared<GpbAtrial>(); }},
    {HRD09Control().type(), []() { return make_shared<HRD09Control>(); }},
    {HRD09BorderZone().type(), []() { return make_shared<HRD09BorderZone>(); }},
    {TNNP04Control().type(), []() { return make_shared<TNNP04Control>(); }},
    {OHaraRudyEndo().type(), []() { return make_shared<OHaraRudyEndo>(); }},
    {OHaraRudyEpi().type(), []() { return make_shared<OHaraRudyEpi>(); }},
    {OHaraRudyM().type(), []() { return make_shared<OHaraRudyM>(); }},
    {GpbAtrialOnal17().type(), []() { return make_shared<GpbAtrialOnal17>(); }},
    {FR().type(), []() { return make_shared<FR>(); }},
    {Ksan().type(), []() { return make_shared<Ksan>(); }},
    {Koivumaki().type(), []() { return make_shared<Koivumaki>(); }},
//    {GpbAtrialdiffINa().type(), []() { return make_shared<GpbAtrialdiffINa>(); }},
    {GpbVent().type(), []() { return make_shared<GpbVent>(); }},
    {Br04().type(), []() { return make_shared<Br04>(); }},
    {Courtemanche98().type(), []() { return make_shared<Courtemanche98>(); }},
    {CoupledInexcitableCell().type(),
     []() { return make_shared<CoupledInexcitableCell>(); }},
};

/*
 * this map is used to setup default simulations in longqt add your new cell to
 * this map to give it a meaningful default simulation. We typically pace to
 * study-state ~500,000 ms and output values for the last 5,000 ms
 */
const map<pair<string, string>, map<string, string>>
    CellUtils::protocolCellDefaults = {
        {{CurrentClamp::name, ""},
         {{"paceflag", "true"},
          {"stimval", "-60"},
          {"stimdur", "5"},
          {"tMax", "500000"},
          {"writetime", "495000"},
          {"bcl", "1000"},
          {"numstims", "500"}}},
        {{CurrentClamp::name, Kurata08().type()},
         {{"paceflag", "false"}, {"stimdur", "1"}}},
        {{CurrentClamp::name, HRD09Control().type()},
         {{"stimval", "-80"}, {"stimdur", "0.5"}}},
        {{CurrentClamp::name, GpbAtrial().type()}, {{"stimval", "-12.5"}}},
        {{CurrentClamp::name, HRD09BorderZone().type()},
         {{"stimval", "-80"}, {"stimdur", "0.5"}}},
        {{CurrentClamp::name, TNNP04Control().type()}, {{"stimdur", "1"}}},
        {{CurrentClamp::name, OHaraRudyEndo().type()},
         {{"stimval", "-80"}, {"stimdur", "0.5"}}},
        {{CurrentClamp::name, OHaraRudyEpi().type()},
         {{"stimval", "-80"}, {"stimdur", "0.5"}}},
        {{CurrentClamp::name, OHaraRudyM().type()},
         {{"stimval", "-80"}, {"stimdur", "0.5"}}},
        {{CurrentClamp::name, GpbAtrialOnal17().type()},
         {{"stimval", "-12.5"}}},
        {{CurrentClamp::name, FR().type()},
         {{"stimval", "-80"}, {"stimdur", "0.5"}}},
        {{CurrentClamp::name, Br04().type()},
         {{"stimval", "-60"}, {"stimdur", "0.5"}}},
        {{CurrentClamp::name, Ksan().type()},
         {{"stimval", "-10"}, {"paceflag", "false"}}},
        {{CurrentClamp::name, Koivumaki().type()},
         {{"stimval", "-5.6"}, {"stimdur", "6"}}},

        {{VoltageClamp::name, ""}, {{"writetime", "0"}, {"writeint", "20"}}},

        {{GridProtocol::name, ""},
         {{"stimval", "-60"},
          {"stimdur", "5"},
          {"tMax", "5000"},
          {"writetime", "4000"},
          {"bcl", "1000"},
          {"numstims", "500"},
          {"stimval", "-100"}}},
        {{GridProtocol::name, GridCell().type()}, {}},
};

void CellUtils::set_default_vals(Protocol& proto) {
  const string& proto_type = proto.type();
  const string& cell_type = proto.cell()->type();
  map<string, string> defaults;
  map<string, string> vals;
  try {
    defaults = CellUtils::protocolCellDefaults.at({proto_type, ""});
  } catch (out_of_range&) {
    Logger::getInstance()->write(
        "CellUtils: no entry for ({}, Default) in cell defaults", proto_type);
  }
  try {
    vals = CellUtils::protocolCellDefaults.at({proto_type, cell_type});
  } catch (out_of_range&) {
    Logger::getInstance()->write(
        "CellUtils: no entry for ({}, {}) in cell defaults", proto_type,
        cell_type);
  }
  vals.insert(defaults.begin(), defaults.end());
  for (auto& val : vals) {
    try {
      proto.parsStr(val.first, val.second);
    } catch (out_of_range&) {
      Logger::getInstance()->write("CellUtils: default {} not in {} pars",
                                   val.first, proto_type);
    };
  }
}

/*
 * map of known protocols used to create new instances of protocols in
 * longqt
 */
const map<string, CellUtils::ProtocolInitializer> CellUtils::protoMap = {
    {CurrentClamp::name, []() { return make_shared<CurrentClamp>(); }},
    {VoltageClamp::name, []() { return make_shared<VoltageClamp>(); }},
    {GridProtocol::name, []() { return make_shared<GridProtocol>(); }}};

CellUtils::Side CellUtils::flipSide(CellUtils::Side s) {
  switch (s) {
    case Side::top:
      return Side::bottom;
    case Side::bottom:
      return Side::top;
    case Side::left:
      return Side::right;
    case Side::right:
      return Side::left;
  }
}

vector<string> CellUtils::split(string s, char delim, bool keepEmpty) {
  vector<string> v;
  size_t prev_pos = 0;
  size_t pos = s.find(delim, prev_pos);
  std::string token;
  while (pos != std::string::npos) {
    token = s.substr(prev_pos, pos - prev_pos);
    if (keepEmpty || token != "") {
      v.push_back(token);
    }
    prev_pos = pos + 1;
    pos = s.find(delim, prev_pos);
  }
  token = s.substr(prev_pos);
  if (token != "") {
    v.push_back(token);
  }
  return v;
}

std::map<std::string, bool> CellUtils::strToFlag(std::string opts,
                                                 char seperator) {
  std::map<std::string, bool> out;
  auto splits = CellUtils::split(opts, seperator);
  for (auto& sp : splits) {
    out[sp] = true;
  }
  return out;
}

std::string CellUtils::flagToStr(std::map<std::string, bool> optsMap,
                                 char seperator) {
  std::string str = "";
  bool first = true;
  for (auto& opt : optsMap) {
    if (!opt.second) continue;
    if (first) {
      str = opt.first;
      first = false;
    } else {
      str += seperator + opt.first;
    }
  }
  return str;
}

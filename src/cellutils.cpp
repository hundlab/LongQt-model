#include "cellutils.h"

#include "gpbatrial.h"
#include "gridCell.h"
#include "hrd09.h"
#include "tnnp04.h"
#include "kurata08.h"
#include "gridCell.h"
#include "gpbhuman.h"
#include "ksan.h"
#include "gpbatrialonal17.h"
#include "atrial.h"
#include "br04.h"
#include "OHaraRudyEndo.h"
#include "OHaraRudyEpi.h"
#include "OHaraRudyM.h"
#include "FR.h"

#include "gridProtocol.h"
#include "voltageClampProtocol.h"
#include "currentClampProtocol.h"

#include "protocol.h"

#include <QFile>
#include <QDebug>
#include <stdio.h>
#include <stdarg.h>
using namespace LongQt;
using namespace std;

/*
 * cell map this is how new instances of cells are created
 * if you are adding a new cell to longqt add it here
 * and also add it to the protocolCellDefualts map below
 */
const map<string, CellUtils::CellInitializer> CellUtils::cellMap = {
    { ControlSa().type(), [] () {return make_shared<ControlSa>(); }},
    { GpbAtrial().type(), [] () {return make_shared<GpbAtrial>();}},
    { HRD09Control().type(), [] () {return make_shared<HRD09Control>();}},
    { HRD09BorderZone().type(), [] () {return make_shared<HRD09BorderZone>();}},
    { TNNP04Control().type(), [] () {return make_shared<TNNP04Control>();}},
    { OHaraRudyEndo().type(), [] () {return make_shared<OHaraRudyEndo>();}},
    { OHaraRudyEpi().type(), [] () {return make_shared<OHaraRudyEpi>();}},
    { OHaraRudyM().type(), [] () {return make_shared<OHaraRudyM>();}},
    { GpbAtrialOnal17().type(), [] () {return make_shared<GpbAtrialOnal17>();}},
    { FR().type(), [] () {return make_shared<FR>();}},
    { Ksan().type(), [] () {return make_shared<Ksan>();}},

            { GpbVent().type(), [] () {return make_shared<GpbVent>();}},
            { Br04().type(), [] () {return make_shared<Br04>();}},
            { Courtemanche98().type(), [] () {return make_shared<Courtemanche98>();}},
//            { GpbAtrialWT().type(), [] () {return make_shared<GpbAtrialWT>();}},
//            { GpbAtrialSE().type(), [] () {return make_shared<GpbAtrialSE>();}},
};

/*
 * this map is used to setup default simulations in longqt add your new cell to 
 * this map to give it a meaningful default simulation. We typically pace to 
 * study-state ~500,000 ms and output values for the last 5,000 ms
 */
const map<pair<string,string>, map<string,string>> CellUtils::protocolCellDefaults = {
    { {CurrentClamp::name,""}, {{"paceflag","true"},{"stimval","-60"},{"stimdur","5"},
      {"tMax","500000"},{"writetime","495000"},{"bcl","1000"}, {"numstims","500"}}
    },
    { {CurrentClamp::name,ControlSa().type()}, {{"paceflag","false"},{"stimdur","1"}}},
    { {CurrentClamp::name,HRD09Control().type()}, {{"stimval","-80"},{"stimdur","0.5"}}},
    { {CurrentClamp::name,GpbAtrial().type()}, {{"stimval","-12.5"}}},
    { {CurrentClamp::name,HRD09BorderZone().type()}, {{"stimval","-80"},{"stimdur","0.5"}}},
    { {CurrentClamp::name,TNNP04Control().type()}, {{"stimdur","1"}}},
    { {CurrentClamp::name,OHaraRudyEndo().type()}, {{"stimval","-80"},{"stimdur","0.5"}}},
    { {CurrentClamp::name,OHaraRudyEpi().type()}, {{"stimval","-80"},{"stimdur","0.5"}}},
    { {CurrentClamp::name,OHaraRudyM().type()}, {{"stimval","-80"},{"stimdur","0.5"}}},
    { {CurrentClamp::name,GpbAtrialOnal17().type()}, {{"stimval","-12.5"}}},
    { {CurrentClamp::name,FR().type()}, {{"stimval","-80"},{"stimdur","0.5"}}},

    { {VoltageClamp::name,""}, {{"writetime","0"},{"writeint","20"}}},

    { {GridProtocol::name,""}, {{"stimval","-60"},{"stimdur","5"},
      {"tMax","500000"},{"writetime","495000"},{"bcl","1000"}, {"numstims","500"},{"stimval","-80"}}},
    { {GridProtocol::name,GridCell().type()}, {}},
};

void CellUtils::set_default_vals(Protocol &proto) {
    const string& proto_type = proto.type();
    const string& cell_type = proto.cell()->type();
    map<string,string> defaults;
    map<string,string> vals;
    try {
        defaults = CellUtils::protocolCellDefaults.at({proto_type,""});
    } catch(out_of_range) {
        qDebug("CellUtils: no entry for (%s,%s) in cell defaults", proto_type.c_str(), "Default");
    }
    try {
        vals = CellUtils::protocolCellDefaults.at({proto_type,cell_type});
        vals.insert(defaults.begin(),defaults.end());
    } catch(out_of_range) {
        qDebug("CellUtils: no entry for (%s,%s) in cell defaults", proto_type.c_str(), cell_type.c_str());
    }
    for(auto& val :vals) {
        try {
            proto.parsStr(val.first,val.second);
        } catch(out_of_range) {
            qDebug("CellUtils: default %s not in %s pars", val.first.c_str(), proto_type.c_str());
        };
    }
}

/*
 * map of known protocols used to create new instances of protocols in
 * longqt
 */
const map<string, CellUtils::ProtocolInitializer> CellUtils::protoMap = {
    {CurrentClamp::name, [] () {return make_shared<CurrentClamp>();}},
    {VoltageClamp::name, [] () {return make_shared<VoltageClamp>();}},
    {GridProtocol::name, [] () {return make_shared<GridProtocol>();}}
};

std::string CellUtils::strprintf(const char * format, ...) {
    va_list argsLen,args;
    va_start(args, format);
    va_copy(argsLen,args);
    int numbytes = vsnprintf((char*)NULL, 0, format, argsLen);
    char* cstr = new char[(numbytes+1)*sizeof(char)];
    vsnprintf(cstr, numbytes+1, format, args);
    std::string str(cstr);
    va_end(args);
    delete[] cstr;
    return str;
}


CellUtils::Side CellUtils::flipSide(CellUtils::Side s)
{
    switch(s) {
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

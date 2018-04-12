#include "cellutils.h"

#include "gpbatrial.h"
#include "gridCell.h"
#include "hrd09.h"
#include "tnnp04.h"
#include "gpbatrialRyr.h"
#include "kurata08.h"
#include "gridCell.h"
#include "gpbhuman.h"
#include "ksan.h"
#include "gpbatrialWT.h"
#include "gpbatrialSE.h"
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
    { Ksan().type(), [] () {return make_shared<Ksan>();}}

    /*		{ GpbVent().type, [] () {return (Cell*) new GpbVent;}},
            { Br04().type, [] () {return (Cell*) new Br04;}},
            { Courtemanche98().type, [] () {return (Cell*) new Courtemanche98;}},
            { GpbAtrialWT().type, [] () {return (Cell*) new GpbAtrialWT;}},
            { GpbAtrialSE().type, [] () {return (Cell*) new GpbAtrialSE;}}*/
};

/*
 * this map is used to setup default simulations in longqt add your new cell to 
 * this map to give it a meaningful default simulation. We typically pace to 
 * study-state ~500,000 ms and output values for the last 5,000 ms
 */
const map<string, list<pair<string,string>>> CellUtils::protocolCellDefaults = {
    { ControlSa().type(), {{"paceflag","false"},{"stimval","-60"},{"stimdur","1"},
                            {"tMax","500000"},{"writetime","495000"},{"bcl","1000"},
                            {"numstims","500"}}},
    { HRD09Control().type(), {{"paceflag","true"},{"stimval","-80"},
                               {"stimdur","0.5"},{"tMax","500000"},{"writetime","495000"},
                               {"bcl","1000"},{"numstims","500"}}},
    { GpbAtrial().type(), {{"paceflag","true"},{"stimval","-12.5"},{"stimdur","5"},
                            {"tMax","500000"},{"writetime","495000"},{"bcl","1000"},
                            {"numstims","500"}}},
    { GridCell().type(), {{"paceflag","true"},{"stimval","-12.5"},{"stimdur","5"},
                           {"tMax","500000"},{"writetime","495000"},{"bcl","1000"},
                           {"numstims","500"}}},
    { HRD09BorderZone().type(), {{"paceflag","true"},{"stimval","-80"},
                                  {"stimdur","0.5"},{"tMax","500000"},{"writetime","495000"},
                                  {"bcl","1000"},{"numstims","500"}}},
    { TNNP04Control().type(), {{"paceflag","true"},{"stimval","-60"},
                                {"stimdur","1"},{"tMax","500000"},{"writetime","495000"},{"bcl","1000"},
                                {"numstims","500"}}},
    { OHaraRudyEndo().type(), {{"paceflag","true"},{"stimval","-80"},
                                {"stimdur","0.5"},{"tMax","500000"},{"writetime","495000"},{"bcl","1000"},
                                {"numstims","500"}}},
    { OHaraRudyEpi().type(), {{"paceflag","true"},{"stimval","-80"},
                               {"stimdur","0.5"},{"tMax","500000"},{"writetime","495000"},{"bcl","1000"},
                               {"numstims","500"}}},
    { OHaraRudyM().type(), {{"paceflag","true"},{"stimval","-80"},
                             {"stimdur","0.5"},{"tMax","500000"},{"writetime","495000"},{"bcl","1000"},
                             {"numstims","500"}}},
    { GpbAtrialOnal17().type(), {{"paceflag","true"},{"stimval","-12.5"},{"stimdur","5"},{"tMax","500000"},{"writetime","495000"},{"bcl","1000"},{"numstims","500"}}},
    { FR().type(), {{"paceflag","true"},{"stimval","-80"},{"stimdur","0.5"},{"tMax","1000"},{"writetime","0"},{"bcl","300"},{"numstims","20"}}}
};

void CellUtils::set_default_vals(Protocol &proto) {
    const string& name = proto.cell()->type();
    try {
        const auto& vals = CellUtils::protocolCellDefaults.at(name);
        for(auto& val :vals) {
            try {
                proto.pars.at(val.first).set(val.second);
            } catch(bad_function_call) {
            } catch(out_of_range) {
                qDebug("CellUtils: default %s not in proto pars", val.first.c_str());
            };
        }
    } catch(out_of_range) {}
}

/*
 * map of known protocols used to create new instances of protocols in
 * longqt
 */
const map<string, CellUtils::ProtocolInitializer> CellUtils::protoMap = {
    {CurrentClamp().type(), [] () {return make_shared<CurrentClamp>();}},
    {VoltageClamp().type(), [] () {return make_shared<VoltageClamp>();}},
    {GridProtocol().type(), [] () {return make_shared<GridProtocol>();}}
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
    case CellUtils::Side::top:
        return CellUtils::Side::bottom;
    case CellUtils::Side::bottom:
        return CellUtils::Side::top;
    case CellUtils::Side::left:
        return CellUtils::Side::right;
    case CellUtils::Side::right:
        return CellUtils::Side::left;
    }
}

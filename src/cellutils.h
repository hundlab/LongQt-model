/*
 * general purpose functions used by mostly by protocols 
 * This is the place to add a new cell type
 */
#ifndef CELLUTILS_H
#define CELLUTILS_H

#include <map>
#include <list>
#include <utility>
#include <string>
#include <stdarg.h>
#include <memory>
#include <functional>
#include <sstream>

#include <QXmlStreamReader>
#include <QString>

#include <cellutils_core.h>

namespace LongQt {

class Protocol;
class Cell;
/*
 * This is equivalent to a simple static class which holds functions
 * and variables which are not class specific
 */
namespace CellUtils {
    //declare CellInitializer function type
    typedef std::function<std::shared_ptr<Cell>(void)> CellInitializer;

    /*
     * cell map this is how new instances of cells are created
     * if you are adding a new cell to longqt add it to 
     * cellutils.cpp and add it to protocolCellDefualts map
     * declaration found below definition found in cellutils.cpp
     */
    extern const std::map<std::string, CellInitializer> cellMap;

    /*
     * this map is used to setup default simulations in longqt add your new cell to 
     * this map to give it a meaningful default simulation. We typically pace to 
     * study-state ~500,000 ms and output values for the last 5,000 ms
     * (protocol name, cell name) -> {(property name: property value)}
     */
    extern const std::map<std::pair<std::string,std::string>, std::map<std::string,std::string>> protocolCellDefaults;

    //declare the ProtocolIntializer type
    typedef std::function<std::shared_ptr<Protocol>(void)> ProtocolInitializer;

    /*
     * map of known protocols used to create new instances of protocols in
     * longqt
     */
    extern const std::map<std::string, ProtocolInitializer> protoMap;

    void set_default_vals(Protocol& proto);
} //CellUtils
} //LongQt

#endif

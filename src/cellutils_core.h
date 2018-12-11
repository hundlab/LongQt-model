/*
 * CellUtils which do not depend on any internal LongQt classes and thus can be included
 * anywhere definittions may be found in cellutils.cpp
 */
#ifndef CELLUTILS_CORE_H
#define CELLUTILS_CORE_H

#include <cmath>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <memory>

#include <QXmlStreamReader>
#include <QString>

#if defined(_WIN32) || defined(_WIN64)
  #define snprintf _snprintf
  #define vsnprintf _vsnprintf
  #define strcasecmp _stricmp
  #define strncasecmp _strnicmp
#endif
namespace LongQt {

namespace CellUtils {

/*
 * Side provides a consistent system for numbering sides in 2D grids
 */
enum Side {
    top = 0,
    right = 1,
    bottom = 2,
    left = 3
};

Side flipSide(Side s);

/*reads in until the next StartElement with name name
 *returns:
 *	True if it is found
 *	False if eof or error
 */
inline bool readNext(QXmlStreamReader& xml, QString name);

/*
 * reads until xml tree is one level higher
 * returns:
 *	True if it is found
 *	False if eof or error
 */
inline bool readUpLevel(QXmlStreamReader& xml);

//trim whitespace from beginning and end of a string
inline std::string trim(std::string str);

/* maps a bool to string
 * b:true -> "true"
 * b:false-> "false"
 */
inline std::string to_string(const bool& b) {
    return b ? "true" : "false";
}

/* maps a string to a bool
 * s:"true" -> true
 * s:"false" -> false
 */
inline bool stob(const std::string& s) {
    return (strcasecmp("true",trim(s).c_str()) == 0);
}

/*
 * splits s on delim into a list of strings not including delim
 * does not keep empty strings
 */
std::vector<std::string> split(std::string s, char delim);

/*
 * Create formated string, similar to python with {}
 * denoting placeholder
 */
template<typename... Args>
inline std::string strprintf(const std::string& format, Args... args);

/* Helper functions
 */
namespace detials {

inline void args_to_strings(std::vector<std::string>& values) {}

template<typename T>
inline void args_to_strings(std::vector<std::string>& values, T v);

template<typename T, typename... Args>
inline void args_to_strings(std::vector<std::string>& values, T v, Args... args);

inline void split_format(const std::string& format, /*ret*/std::vector<std::string>& texts, /*ret*/std::vector<std::string>& formats);

inline std::string build_string(const std::vector<std::string>& values, const std::vector<std::string>& texts, const std::vector<std::string>& formats);
} //detials

} //CellUtils
} //LongQt
#include "cellutils_core.hpp"

#endif // CELLUTILS_CORE_H

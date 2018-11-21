#ifndef CELLUTILS_HPP
#define CELLUTILS_HPP

namespace LongQt {
namespace CellUtils {

inline bool readNext(QXmlStreamReader& xml, QString name) {
    if(xml.name() == name && xml.tokenType() == QXmlStreamReader::StartElement) {
        return true;
    }
    while(!xml.atEnd()) {
        QXmlStreamReader::TokenType t = xml.readNext();
        if(xml.name() == name && t == QXmlStreamReader::StartElement) {
            return true;
        } else if(xml.name() == name && t == QXmlStreamReader::EndElement) {
            return false;
        }
    }
    return false;
}

inline bool readUpLevel(QXmlStreamReader& xml) {
    int depth = 1;
    while(!xml.atEnd()) {
        QXmlStreamReader::TokenType t = xml.readNext();
        if(t == QXmlStreamReader::StartElement) {
            ++depth;
        } else if(t == QXmlStreamReader::EndElement) {
            --depth;
        }
        if(depth < 1) {
            return true;
        }
    }
    return false;
}

inline std::string trim(std::string str)
{
    std::string toFind = " \t\n\v\f\r";
    str.erase(0, str.find_first_not_of(toFind));
    str.erase(str.find_last_not_of(toFind)+1);
    return str;
}

template<typename T>
inline std::string strprintf(const std::string& format, T v) {
    std::stringstream ss;
    detials::strprintf_helper(format, 0, ss, v);
    return ss.str();
}

template<typename... Args>
inline std::string strprintf(const std::string& format, Args... args) {
    std::stringstream ss;
    detials::strprintf_helper(format, 0, ss, args...);
    return ss.str();
}

namespace detials {

template<typename T>
inline void strprintf_helper(const std::string &format, size_t pos, std::stringstream &ss, T v) {
    size_t posBegin = format.find("{",pos);
    size_t posEnd = format.find("}",posBegin)+1;
    if(posBegin == std::string::npos) {
        ss << format.substr(pos);
        return;
    }
//    std::string formatSpecif = format.substr(posBegin,posEnd);
    auto temp = format.substr(pos,posBegin-pos);
    ss << temp;
    ss << v;
    temp = format.substr(posEnd);
    ss << temp;
}

template<typename T, typename... Args>
inline void strprintf_helper(const std::string &format, size_t pos, std::stringstream &ss, T v, Args... args) {
    size_t posBegin = format.find("{",pos);
    size_t posEnd = format.find("}",posBegin)+1;
    if(posBegin == std::string::npos) {
        ss << format.substr(pos);
        return;
    }
//    std::string formatSpecif = format.substr(posBegin,posEnd);
    auto temp =format.substr(pos,posBegin-pos);
    ss << temp;
    ss << v;
    strprintf_helper(format, posEnd, ss, args...);
}
} //details
} //CellUtils
} //LongQt
#endif // CELLUTILS_HPP

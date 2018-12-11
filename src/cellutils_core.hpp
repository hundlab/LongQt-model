#ifndef CELLUTILS_HPP
#define CELLUTILS_HPP

#include <sstream>

namespace LongQt {
namespace CellUtils {

inline bool readNext(QXmlStreamReader& xml, QString name) {
  if (xml.name() == name && xml.tokenType() == QXmlStreamReader::StartElement) {
    return true;
  }
  while (!xml.atEnd()) {
    QXmlStreamReader::TokenType t = xml.readNext();
    if (xml.name() == name && t == QXmlStreamReader::StartElement) {
      return true;
    } else if (xml.name() == name && t == QXmlStreamReader::EndElement) {
      return false;
    }
  }
  return false;
}

inline bool readUpLevel(QXmlStreamReader& xml) {
  int depth = 1;
  while (!xml.atEnd()) {
    QXmlStreamReader::TokenType t = xml.readNext();
    if (t == QXmlStreamReader::StartElement) {
      ++depth;
    } else if (t == QXmlStreamReader::EndElement) {
      --depth;
    }
    if (depth < 1) {
      return true;
    }
  }
  return false;
}

inline std::string trim(std::string str) {
  std::string toFind = " \t\n\v\f\r";
  str.erase(0, str.find_first_not_of(toFind));
  str.erase(str.find_last_not_of(toFind) + 1);
  return str;
}

// template<typename T>
// inline std::string strprintf(const std::string& format, T v) {
//    std::vector<std::string> texts;
//    std::vector<std::string> formats;
//    std::vector<std::string> values;
//    detials::split_format(format, texts, formats);
//    detials::strprintf_helper(values, v);
//    return detials::build_string(values, texts, formats);
//}

template <typename... Args>
inline std::string strprintf(const std::string& format, Args... args) {
  std::vector<std::string> texts;
  std::vector<std::string> formats;
  std::vector<std::string> values;
  detials::split_format(format, texts, formats);
  detials::args_to_strings(values, args...);
  return detials::build_string(values, texts, formats);
}

namespace detials {

template <typename T>
inline void args_to_strings(std::vector<std::string>& values, T v) {
  std::stringstream ss;
  ss << v;
  values.push_back(ss.str());
}

template <typename T, typename... Args>
inline void args_to_strings(std::vector<std::string>& values, T v,
                            Args... args) {
  std::stringstream ss;
  ss << v;
  values.push_back(ss.str());
  args_to_strings(values, args...);
}
inline void split_format(const std::string& format,
                         /*ret*/ std::vector<std::string>& texts,
                         /*ret*/ std::vector<std::string>& formats) {
  size_t posBegin = 0;
  size_t posEnd = 0;
  size_t posPrev = 0;
  std::string text;

  while (posBegin != std::string::npos) {
    posBegin = format.find("{", posEnd);
    //        posBegin = posBegin == std::string::npos? posBegin: posBegin+1;
    posEnd = format.find("}", posBegin);
    posEnd = posEnd == std::string::npos ? posEnd : posEnd + 1;
    std::string formatSpecif = "";
    if (posBegin == std::string::npos) {
      text += format.substr(posPrev, posBegin);
      continue;
    } else {
      text += format.substr(posPrev, posBegin - posPrev);
      formatSpecif = format.substr(posBegin + 1, (posEnd - posBegin) - 2);
    }
    if (formatSpecif.size() > 0 && formatSpecif[0] == '{') {
      posEnd += 1;
      formatSpecif = format.substr(posBegin + 1, (posEnd - posBegin) - 2);
      text += formatSpecif;
      posPrev = posEnd;
      continue;
    }
    texts.push_back(text);
    formats.push_back(formatSpecif);
    posPrev = posEnd;
    text = "";
  }
  texts.push_back(text);
}

inline std::string build_string(const std::vector<std::string>& values,
                                const std::vector<std::string>& texts,
                                const std::vector<std::string>& formats) {
  std::vector<std::string> finalValues;
  int pos = 0;
  while (pos < formats.size()) {
    size_t endNum = formats[pos].find(":", 0);
    int num = pos;
    try {
      num = std::stoi(formats[pos].substr(0, endNum));
    } catch (std::invalid_argument&) {
    } catch (std::out_of_range&) {
    }
    finalValues.push_back(values[num]);
    pos++;
  }
  std::string out = "";
  int i = 0;
  for (i = 0; i < texts.size() && i < finalValues.size(); i++) {
    out += texts[i] + finalValues[i];
  }
  out += texts[i];
  return out;
}

}  // namespace detials
}  // namespace CellUtils
}  // namespace LongQt
#endif  // CELLUTILS_HPP

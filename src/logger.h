#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
#include "cellutils_core.h"

namespace LongQt {
class Logger {
  std::ofstream* out = 0;
  std::ostream* stdOut = 0;
  static Logger* __instance;
  Logger() = default;
  ~Logger();

 public:
  bool exceptionEnabled = false;

  static Logger* getInstance();
  void setFile(std::string filename);
  void delFile();
  void STDOut(std::ostream* stdOut);
  void delSTDOut();

  template <typename... Args>
  void write(const char* format, Args... args) {
    std::string s = CellUtils::strprintf(format, args...);
    if (this->out && this->out->good()) {
      *out << s;
    }
    if (this->stdOut && this->stdOut->good()) {
      *stdOut << s;
    }
  }

  template <typename exception, typename... Args>
  void write(const std::string& format, Args... args) {
    std::string s = CellUtils::strprintf(format, args...);
    if (this->out && this->out->good()) {
      *out << s;
    }
    if (this->stdOut && this->stdOut->good()) {
      *stdOut << s << std::endl;
    }
    if (this->exceptionEnabled) {
      throw exception(s);
      //            std::is_constructible<exception, std::string>{};
      //            std::is_constructible<exception>{};
    }
  }
};
}  // namespace LongQt
#endif  // LOGGER_H

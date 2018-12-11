#include "logger.h"
#include <fstream>
#include "cellutils.h"

using namespace LongQt;

Logger* Logger::__instance = 0;

Logger::~Logger() { delFile(); }

Logger* Logger::getInstance() {
  if (!__instance) {
    __instance = new Logger();
  }
  return __instance;
}

void Logger::setFile(std::string filename) {
  this->out = new std::ofstream(filename, std::ofstream::app);
}

void Logger::delFile() {
  if (this->out->is_open()) {
    this->out->close();
    delete this->out;
  }
  this->out = 0;
}

void Logger::STDOut(std::ostream* stdOut) { this->stdOut = stdOut; }

void Logger::delSTDOut() { this->stdOut = 0; }

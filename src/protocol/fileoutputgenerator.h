#ifndef FILEOUTPUTGENERATOR_H
#define FILEOUTPUTGENERATOR_H

#include <atomic>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <thread>
#include "cellutils_core.h"

namespace LongQt {

class FileOutputHandler {
  struct Props {
    std::atomic<int> buffsize = 0;
    std::atomic_flag writing = ATOMIC_FLAG_INIT;
    std::ofstream stream;
    std::atomic<Props*> next = 0;
    std::atomic<bool> open = false;
  } prop;

 public:
  FileOutputHandler();
  FileOutputHandler(std::string filename);
  ~FileOutputHandler();
  static int max_fileHandlers;
  static int buffmax;
  std::string getFileName() { return this->filename; }
  void setFileName(std::string filename) {
    if (!prop.open) this->filename = filename;
  }

  template <typename... Args>
  void write(const std::string& format, Args... args) {
    std::string s = CellUtils::strprintf(format, args...);
    buffer.push_back(s);
    prop.buffsize.store(buffer.size());

    if (buffer.size() > buffmax) {
      if (!this->openStream()) return;
      this->writeAll();
    }
  }

  void clear();
  void close();

 private:
  static std::atomic<Props*> head;
  static std::atomic<int> numUsed;
  std::string filename;
  std::list<std::string> buffer;

  bool openStream();
  void writeAll();
  void closeStream(Props*);  // set open = false

  void push_front(Props*);
  void remove(Props*);  // req writing lock
  Props* find_min();
};
}  // namespace LongQt

#endif  // FILEOUTPUTGENERATOR_H

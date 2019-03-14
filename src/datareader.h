#ifndef DATAREADER_H
#define DATAREADER_H

#include <string>
#include <vector>

namespace LongQt {

class DataReader {
 public:
  struct TSVData {
    std::vector<std::string> header;
    std::vector<std::vector<double>> data;
    int trial = -1;
  };

  static TSVData readFile(std::string &filename);
};
}  // namespace LongQt

#endif  // DATAREADER_H

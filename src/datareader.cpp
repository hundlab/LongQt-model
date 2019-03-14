#include "datareader.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <regex>
#include "cellutils.h"
#include "logger.h"
#include "settingsIO.h"

using namespace LongQt;

DataReader::TSVData DataReader::readFile(std::string& filename) {
  std::ifstream file;
  TSVData data;
  file.open(filename);
  if (!file.is_open()) {
    file.close();
    return data;
  }
  std::string line;
  std::vector<std::string> stringVals;

  try {
    std::string name = CellUtils::split(filename, '/').back();
    std::smatch matches;
    std::regex regx("\d+");
    if (std::regex_search(name, matches, regx)) {
      data.trial = std::stoi(matches[0]);
    }
  } catch (std::exception&) {
  }

  std::getline(file, line);
  data.header = CellUtils::split(line, '\t');

  while (std::getline(file, line)) {
    stringVals = CellUtils::split(line, '\t');
    std::vector<double> doubleVals(stringVals.size());
    std::transform(stringVals.begin(), stringVals.end(), doubleVals.begin(),
                   [](auto& s) { return std::stod(s); });
    data.data.push_back(doubleVals);
    if (doubleVals.size() != data.header.size()) {
      Logger::getInstance()->write(
          "DataReader: file '{}', line {} is length {}, but header is {}",
          filename, data.data.size(), doubleVals.size(), data.header.size());
    }
  }
}

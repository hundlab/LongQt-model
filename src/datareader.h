#ifndef DATAREADER_H
#define DATAREADER_H

#include <filesystem>
#include <regex>
#include <set>
#include <string>
#include <vector>
#include "cellutils.h"
#include "logger.h"

namespace LongQt {

class DataReader {
 public:
  struct TSVData {
    std::vector<std::string> header;
    std::vector<std::vector<double>> data;
    int trial = -1;
  };
  struct MeasHeader {
    std::string cell_info;
    std::vector<int> cell_info_parsed;
    std::string var_name;
    std::string prop_name;
  };
  struct TraceHeader {
    std::string cell_info;
    std::vector<int> cell_info_parsed;
    std::string var_name;
  };
  template <class Header>
  struct TrialData {
    TrialData() {}
    TrialData(TSVData& data);
    ~TrialData() {}
    std::vector<Header> header;
    std::vector<std::vector<double>> data;
  };
  struct SimData {
    std::vector<TrialData<TraceHeader>> trace;
    std::vector<TrialData<MeasHeader>> meas;
  };

  static TSVData readFile(const std::filesystem::path& file,
                          const std::set<int>& exclude = {});
  static SimData readDir(const std::filesystem::path& dir,
                         const std::set<int>& excludeTrials = {});
  static std::set<int> getTrialNums(
      const std::filesystem::path& dir);

 private:
  template <class Header>
  static void setCellInfoParsed(Header& trial, const std::string& cell_info) {
    std::smatch matches;
    std::string text = cell_info;
    std::vector<int> parsed;
    while (std::regex_search(text, matches, std::regex("\\D*(\\d+)\\D*"))) {
      try {
        parsed.push_back(std::stoi(matches[1]));
      } catch (std::invalid_argument&) {
      }
      text = matches.suffix();
    }
    trial.cell_info_parsed = parsed;
  }

  static void tryLegacy(SimData data,
                        const std::filesystem::path& file,
                        const std::set<int>& exclude);

  static int getTrialNum(std::string filename);
};
}  // namespace LongQt

#endif  // DATAREADER_H

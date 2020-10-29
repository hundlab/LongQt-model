#include "datareader.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <regex>
#include "cellutils.h"
#include "logger.h"
#include "settingsIO.h"

using namespace LongQt;
namespace fs = std::filesystem;
using dlim = std::numeric_limits<double>;

DataReader::TSVData DataReader::readFile(const fs::path& file,
                                         const std::set<int>& exclude) {
  std::ifstream fileStrm;
  TSVData data;
  fileStrm.open(file);
  if (!fileStrm.is_open()) {
    fileStrm.close();
    return data;
  }
  std::string line;
  std::vector<std::string> stringVals;

  data.trial = DataReader::getTrialNum(file.filename().string());
  if (exclude.count(data.trial) > 0) {
    fileStrm.close();
    return data;
  }

  std::getline(fileStrm, line);
  data.header = CellUtils::split(line, '\t');

  data.data.resize(data.header.size());
  int lineNum = 1;
  while (std::getline(fileStrm, line)) {
    stringVals = CellUtils::split(line, '\t', true);
    int stringValsSize = stringVals.size();
    if (stringVals.size() > data.header.size()) {
      Logger::getInstance()->write(
          "DataReader: tsv line {} in '{}' is too long (trimming down)", lineNum, file);
      stringValsSize = data.header.size();
    }
    int i;
    for (i = 0; i < stringValsSize; ++i) {
      try {
        data.data[i].push_back(std::stod(stringVals[i]));
      } catch (std::invalid_argument&) {
        Logger::getInstance()->write("DataReader: '{}' not a valid number",
                                     stringVals[i]);
      }
    }
    if(i < data.header.size()) {
      Logger::getInstance()->write(
          "DataReader: tsv line {} in '{}' is too short (filling with NaNs)", lineNum, file);
      for(; i < data.header.size(); ++i) {
          data.data[i].push_back(nanf(""));
      }
    }
    ++lineNum;
  }
  fileStrm.close();
  return data;
}

DataReader::SimData DataReader::readDir(const fs::path& dir,
                                        const std::set<int>& exclude) {
  SimData simdat;
  for (auto& file : fs::directory_iterator(dir)) {
    const auto& filePath = file.path();
    auto filename = filePath.filename().string();
    std::smatch matches;
    if (std::regex_match(filename, matches, std::regex("meas\\d+\\.tsv"))) {
      TSVData dat = DataReader::readFile(file, exclude);
      if (dat.trial == -1) continue;
      simdat.meas.resize(std::max(simdat.meas.size(),
                                  static_cast<std::size_t>(dat.trial + 1)));
      simdat.meas[dat.trial] = TrialData<MeasHeader>(dat);
    } else if (std::regex_match(filename, matches,
                                std::regex("trace\\d+\\.tsv"))) {
      TSVData dat = DataReader::readFile(file, exclude);
      if (dat.trial == -1) continue;
      simdat.trace.resize(std::max(simdat.trace.size(),
                                   static_cast<std::size_t>(dat.trial + 1)));
      simdat.trace[dat.trial] = TrialData<TraceHeader>(dat);
    } else {
      DataReader::tryLegacy(simdat, filePath, exclude);
    }
  }
  return simdat;
}

std::set<int> DataReader::getTrialNums(
    const fs::path& dir) {
  std::set<int> nums;
  for (auto& file : fs::directory_iterator(dir)) {
    const auto& filePath = file.path();
    auto filename = filePath.filename().string();
    std::smatch matches;
    if (std::regex_match(filename, matches, std::regex("meas\\d+\\.tsv"))) {
      nums.insert(DataReader::getTrialNum(filename));
    } else if (std::regex_match(filename, matches,
                                std::regex("trace\\d+\\.tsv"))) {
      nums.insert(DataReader::getTrialNum(filename));
    } else if (std::regex_search(filename, matches,
                                 std::regex("dt\\d+\\.tsv"))) {
      nums.insert(DataReader::getTrialNum(filename));
    } else if (std::regex_search(
                   filename, matches,
                   std::regex("(cell_(\\d+)_(\\d+)_)?dt\\d+_dvars\\.tsv"))) {
      nums.insert(DataReader::getTrialNum(filename));
    }
  }
  return nums;
}

void DataReader::tryLegacy(DataReader::SimData simdat, const fs::path& file,
                           const std::set<int>& exclude) {
  std::smatch matches;
  auto filename = file.filename().string();
  if (std::regex_search(filename, matches, std::regex("dt\\d+\\.tsv"))) {
    TSVData dat = DataReader::readFile(file, exclude);
    if (dat.trial == -1) return;
    simdat.meas.resize(
        std::max(simdat.meas.size(), static_cast<std::size_t>(dat.trial + 1)));
    TrialData<MeasHeader>& trialDat = simdat.meas[dat.trial];
    trialDat.data.insert(trialDat.data.end(), dat.data.begin(), dat.data.end());
    for (auto& info : dat.header) {
      MeasHeader header;
      auto split = CellUtils::split(info, '/');
      if (split.size() < 2) continue;
      header.prop_name = split.back();
      header.var_name = split.size() == 2 ? split.front() : split[1];
      std::string cell_info = split.size() == 3 ? split.front() : "";
      header.cell_info = cell_info;
      DataReader::setCellInfoParsed(header, cell_info);
      trialDat.header.push_back(header);
    }
  } else if (std::regex_search(
                 filename, matches,
                 std::regex("(cell_(\\d+)_(\\d+)_)?dt\\d+_dvars\\.tsv"))) {
    TSVData dat = DataReader::readFile(file, exclude);
    if (dat.trial == -1) return;
    simdat.trace.resize(
        std::max(simdat.trace.size(), static_cast<std::size_t>(dat.trial + 1)));
    TrialData<TraceHeader>& trialDat = simdat.trace[dat.trial];
    trialDat.data.insert(trialDat.data.end(), dat.data.begin(), dat.data.end());
    for (auto& info : dat.header) {
      TraceHeader header;
      header.var_name = info;
      if (matches.size() > 3) {
        header.cell_info = matches[1];
        header.cell_info_parsed = {std::stoi(matches[2]),
                                   std::stoi(matches[3])};
      } else {
        header.cell_info = "";
      }
      trialDat.header.push_back(header);
    }
  }
}

int DataReader::getTrialNum(std::string filename) {
  try {
    std::smatch matches;
    if (std::regex_search(filename, matches,
                          std::regex("(?:\\D*(\\d+)\\D*)+"))) {
      return std::stoi(matches[1]);
    }
  } catch (std::exception&) {
  }
  Logger::getInstance()->write(
      "DataReader: trial number is not a part of file name for '{}'", filename);
  return -1;
}

template <>
DataReader::TrialData<DataReader::MeasHeader>::TrialData(
    DataReader::TSVData& dat) {
  this->data = std::move(dat.data);
  for (auto& info : dat.header) {
    MeasHeader header;
    auto split = CellUtils::split(info, '/');
    if (split.size() < 2) continue;
    header.prop_name = split.back();
    header.var_name = split.size() == 2 ? split.front() : split[1];
    std::string cell_info = split.size() == 3 ? split.front() : "";
    header.cell_info = cell_info;
    DataReader::setCellInfoParsed(header, cell_info);
    this->header.push_back(header);
  }
}

template <>
DataReader::TrialData<DataReader::TraceHeader>::TrialData(
    DataReader::TSVData& dat) {
  this->data = std::move(dat.data);
  for (auto& info : dat.header) {
    TraceHeader header;
    auto split = CellUtils::split(info, '/');
    if (split.size() < 1) continue;
    header.var_name = split.back();
    std::string cell_info = split.size() == 2 ? split.front() : "";
    header.cell_info = cell_info;
    DataReader::setCellInfoParsed(header, cell_info);
    this->header.push_back(header);
  }
}

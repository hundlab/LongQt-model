#ifndef MEASURE_TEST_H
#define MEASURE_TEST_H
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>
#include "measure.h"
#include "measuredefault.h"
#include "measurevoltage.h"

#define INF std::numeric_limits<double>::infinity()
#define Q_NAN std::numeric_limits<double>::quiet_NaN()

using namespace testing;
using namespace LongQt;

TEST(measure, measure) {
  std::set<std::string> ans = {"avg", "stdev", "min", "amp"};
  auto meas = MeasureDefault(ans);
  auto res = meas.selection();
  EXPECT_EQ(ans, res);
}

TEST(measure, getNameSring_normal) {
  std::set<std::string> sel = {"avg", "stdev", "min", "amp"};
  auto meas = MeasureDefault(sel);

  std::string res = meas.getNameString("asdf");
  std::sort(res.begin(), res.end());
  std::string ans = "asdf/amp\tasdf/avg\tasdf/min\tasdf/stdev";
  std::sort(ans.begin(), ans.end());
  EXPECT_EQ(ans, res);
}

TEST(measure, getNameSring_empty) {
  std::set<std::string> sel;
  auto meas = MeasureDefault(sel);

  std::string res = meas.getNameString("asdf");
  std::sort(res.begin(), res.end());
  std::string ans = "";
  std::sort(ans.begin(), ans.end());
  EXPECT_EQ(ans, res);
}

TEST(measure, getNameSring_no_name) {
  std::set<std::string> sel = {"avg", "stdev", "min"};
  auto meas = MeasureDefault(sel);

  std::string res = meas.getNameString("");
  std::sort(res.begin(), res.end());
  std::string ans = "/avg\t/min\t/stdev";
  std::sort(ans.begin(), ans.end());
  EXPECT_EQ(ans, res);
}

TEST(measure, getValueString_normal) {
  std::set<std::string> sel = {"avg", "stdev", "min", "amp"};
  auto meas = MeasureDefault(sel);

  std::string res = meas.getValueString();
  std::sort(res.begin(), res.end());
  std::string ans = "nan\t0.000000e+00\tinf\t0.000000e+00";
  std::sort(ans.begin(), ans.end());
  EXPECT_EQ(ans, res);
}

TEST(measure, getValueString_empty) {
  std::set<std::string> sel;
  auto meas = MeasureDefault(sel);

  std::string res = meas.getValueString();
  std::sort(res.begin(), res.end());
  std::string ans = "";
  std::sort(ans.begin(), ans.end());
  EXPECT_EQ(ans, res);
}

inline void compareMap(std::map<std::string, double> ans,
                       std::map<std::string, double> res) {
  //    EXPECT_EQ(res,ans);

  // write result
  /*
      std::cout << "{";
      for(auto& pair: ans) {
          std::stringstream result;
          if(std::isnan(res[pair.first])) {
              result << "Q_NAN";
          } else if(std::isinf(res[pair.first])) {
              result << "INF";
          } else {
              result << res[pair.first];
          }
          std::cout << "{\"" << pair.first << "\", " << result.str() << "}, ";
      }
      std::cout << "}" << std::endl;*/

  for (auto& pair : ans) {
    if (std::isnan(pair.second)) {
      EXPECT_TRUE(std::isnan(pair.second)) << "Name: " << pair.first;
    } else if (std::isinf(pair.second)) {
      EXPECT_TRUE(std::isinf(res[pair.first])) << "Name: " << pair.first;
    } else {
      EXPECT_NO_THROW(EXPECT_NEAR(res[pair.first], pair.second, 0.1)
                      << "Name: " << pair.first);
    }
  }
}

TEST(measure_default, measure_square_wave) {
  std::ifstream ist("./tests/time_square_wave.csv");
  std::istream_iterator<double> startt(ist), end;
  std::vector<double> times(startt, end);
  std::ifstream isv("./tests/value_square_wave.csv");
  std::istream_iterator<double> startv(isv);
  std::vector<double> values(startv, end);

  ASSERT_NE(values.size(), 0) << "Values not found."
                              << " working dir should be project dir";
  ASSERT_NE(times.size(), 0) << "Times not found."
                             << " working dir should be project dir";

  std::set<std::string> sel = {"peak", "min", "maxderiv", "mint", "derivt",
                               "maxt", "amp", "avg",      "stdev"};
  auto meas = MeasureDefault(sel);
  for (int i = 0; i < values.size() && i < times.size(); i++) {
    meas.measure(times[i], values[i]);
  }
  auto res = meas.variablesMap();
  std::map<std::string, double> ans = {
      {"amp", 200},        {"avg", -50}, {"derivt", 20},
      {"maxderiv", 20000}, {"maxt", 0},  {"min", -150},
      {"mint", 10},        {"peak", 50}, {"stdev", 100}};
  compareMap(ans, res);
}

TEST(measure_default, measure_hrd09) {
  std::ifstream st("./tests/hrd09.tsv");
  char const row_delim = '\n';
  char const field_delim = '\t';
  std::vector<std::vector<double>> values;
  for (std::string row; std::getline(st, row, row_delim);) {
    std::istringstream ss(row);
    double a, b, c, d;
    ss >> a >> b >> c >> d;
    values.push_back({a, b, c, d});
  }

  ASSERT_NE(values.size(), 0) << "Values not found."
                              << " working dir should be project dir";

  std::set<std::string> sel = {"peak", "min", "maxderiv", "mint", "derivt",
                               "maxt", "amp", "avg",      "stdev"};
  std::vector<std::map<std::string, double>> ans = {
      {
          {"amp", 0.000316823},
          {"avg", 0.000191251},
          {"derivt", 10},
          {"maxderiv", 3.22966e-05},
          {"maxt", 57.8},
          {"min", 9.2282e-05},
          {"mint", 0.005},
          {"peak", 0.000409105},
          {"stdev", 0.000107748},
      },
      {
          {"amp", 3.84475},
          {"avg", -0.786769},
          {"derivt", 10},
          {"maxderiv", 0.515181},
          {"maxt", 242.5},
          {"min", -3.00662},
          {"mint", 4.75},
          {"peak", 0.838121},
          {"stdev", 1.17585},
      }};

  for (int col = 0; col < 2; col++) {
    auto meas = MeasureDefault(sel);
    for (int i = 0; i < values.size() && values[i][2] <= 1000; i++) {
      meas.measure(values[i][2], values[i][col]);
    }
    auto res = meas.variablesMap();

    compareMap(ans[col], res);
  }
}

TEST(measure_voltage, measure50_square_wave) {
  std::ifstream ist("./tests/time_square_wave.csv");
  std::istream_iterator<double> startt(ist), end;
  std::vector<double> times(startt, end);
  std::ifstream isv("./tests/value_square_wave.csv");
  std::istream_iterator<double> startv(isv);
  std::vector<double> values(startv, end);

  ASSERT_NE(values.size(), 0) << "Values not found."
                              << " working dir should be project dir";
  ASSERT_NE(times.size(), 0) << "Times not found."
                             << " working dir should be project dir";

  std::set<std::string> sel = {
      "peak", "min", "maxderiv", "mint",     "derivt",     "maxt",     "cl",
      "amp",  "ddr", "dur",      "durtime1", "vartakeoff", "deriv2ndt"};
  std::vector<std::map<std::string, double>> ans = {{{"amp", Q_NAN},
                                                     {"cl", 0},
                                                     {"ddr", Q_NAN},
                                                     {"deriv2ndt", Q_NAN},
                                                     {"derivt", 0.01},
                                                     {"dur", 9.99},
                                                     {"durtime1", 0.01},
                                                     {"maxderiv", 0},
                                                     {"maxt", Q_NAN},
                                                     {"min", -150},
                                                     {"mint", 10},
                                                     {"peak", -INF},
                                                     {"vartakeoff", Q_NAN}},
                                                    {{"amp", 200},
                                                     {"cl", Q_NAN},
                                                     {"ddr", 20.02},
                                                     {"deriv2ndt", 20},
                                                     {"derivt", 20},
                                                     {"dur", 10},
                                                     {"durtime1", 20},
                                                     {"maxderiv", 20000},
                                                     {"maxt", 20.01},
                                                     {"min", -150},
                                                     {"mint", 10.01},
                                                     {"peak", 50},
                                                     {"vartakeoff", 50}},
                                                    {{"amp", 200},
                                                     {"cl", 20},
                                                     {"ddr", 20.02},
                                                     {"deriv2ndt", 40},
                                                     {"derivt", 40},
                                                     {"dur", 10},
                                                     {"durtime1", 40},
                                                     {"maxderiv", 20000},
                                                     {"maxt", 40.01},
                                                     {"min", -150},
                                                     {"mint", 30.01},
                                                     {"peak", 50},
                                                     {"vartakeoff", 50}},
                                                    {{"amp", 200},
                                                     {"cl", 20},
                                                     {"ddr", 20.02},
                                                     {"deriv2ndt", 60},
                                                     {"derivt", 60},
                                                     {"dur", 10},
                                                     {"durtime1", 60},
                                                     {"maxderiv", 20000},
                                                     {"maxt", 60.01},
                                                     {"min", -150},
                                                     {"mint", 50.01},
                                                     {"peak", 50},
                                                     {"vartakeoff", 50}},
                                                    {{"amp", 200},
                                                     {"cl", 20},
                                                     {"ddr", 20.02},
                                                     {"deriv2ndt", 80},
                                                     {"derivt", 80},
                                                     {"dur", 10},
                                                     {"durtime1", 80},
                                                     {"maxderiv", 20000},
                                                     {"maxt", 80.01},
                                                     {"min", -150},
                                                     {"mint", 70.01},
                                                     {"peak", 50},
                                                     {"vartakeoff", 50}},
                                                    {{"amp", -INF},
                                                     {"cl", 20},
                                                     {"ddr", 20.02},
                                                     {"deriv2ndt", 80},
                                                     {"derivt", 90},
                                                     {"dur", 10},
                                                     {"durtime1", 80},
                                                     {"maxderiv", 0},
                                                     {"maxt", 80.01},
                                                     {"min", -150},
                                                     {"mint", 90.01},
                                                     {"peak", -INF},
                                                     {"vartakeoff", 50}}};

  int count = 0;
  auto meas = MeasureVoltage(sel);
  meas.percrepol(50);
  for (int i = 0; i < values.size() && i < times.size(); i++) {
    bool reset = meas.measure(times[i], values[i]);
    if (reset) {
      auto res = meas.variablesMap();
      //            EXPECT_EQ(res,ans);
      compareMap(ans[count], res);
      count++;
      meas.reset();
    }
  }
  auto res = meas.variablesMap();
  ASSERT_EQ(count, 5);
  EXPECT_NO_THROW(compareMap(ans[count], res));
}

TEST(measure_voltage, measure90_square_wave) {
  std::ifstream ist("./tests/time_square_wave.csv");
  std::istream_iterator<double> startt(ist), end;
  std::vector<double> times(startt, end);
  std::ifstream isv("./tests/value_square_wave.csv");
  std::istream_iterator<double> startv(isv);
  std::vector<double> values(startv, end);

  ASSERT_NE(values.size(), 0) << "Values not found."
                              << " working dir should be project dir";
  ASSERT_NE(times.size(), 0) << "Times not found."
                             << " working dir should be project dir";

  std::set<std::string> sel = {
      "peak", "min", "maxderiv", "mint",     "derivt",     "maxt",     "cl",
      "amp",  "ddr", "dur",      "durtime1", "vartakeoff", "deriv2ndt"};
  std::vector<std::map<std::string, double>> ans = {{{"amp", Q_NAN},
                                                     {"cl", 0},
                                                     {"ddr", Q_NAN},
                                                     {"deriv2ndt", Q_NAN},
                                                     {"derivt", 0.01},
                                                     {"dur", 9.99},
                                                     {"durtime1", 0.01},
                                                     {"maxderiv", 0},
                                                     {"maxt", Q_NAN},
                                                     {"min", -150},
                                                     {"mint", 10},
                                                     {"peak", -INF},
                                                     {"vartakeoff", Q_NAN}},
                                                    {{"amp", 200},
                                                     {"cl", Q_NAN},
                                                     {"ddr", 20.02},
                                                     {"deriv2ndt", 20},
                                                     {"derivt", 20},
                                                     {"dur", 10},
                                                     {"durtime1", 20},
                                                     {"maxderiv", 20000},
                                                     {"maxt", 20.01},
                                                     {"min", -150},
                                                     {"mint", 10.01},
                                                     {"peak", 50},
                                                     {"vartakeoff", 50}},
                                                    {{"amp", 200},
                                                     {"cl", 20},
                                                     {"ddr", 20.02},
                                                     {"deriv2ndt", 40},
                                                     {"derivt", 40},
                                                     {"dur", 10},
                                                     {"durtime1", 40},
                                                     {"maxderiv", 20000},
                                                     {"maxt", 40.01},
                                                     {"min", -150},
                                                     {"mint", 30.01},
                                                     {"peak", 50},
                                                     {"vartakeoff", 50}},
                                                    {{"amp", 200},
                                                     {"cl", 20},
                                                     {"ddr", 20.02},
                                                     {"deriv2ndt", 60},
                                                     {"derivt", 60},
                                                     {"dur", 10},
                                                     {"durtime1", 60},
                                                     {"maxderiv", 20000},
                                                     {"maxt", 60.01},
                                                     {"min", -150},
                                                     {"mint", 50.01},
                                                     {"peak", 50},
                                                     {"vartakeoff", 50}},
                                                    {{"amp", 200},
                                                     {"cl", 20},
                                                     {"ddr", 20.02},
                                                     {"deriv2ndt", 80},
                                                     {"derivt", 80},
                                                     {"dur", 10},
                                                     {"durtime1", 80},
                                                     {"maxderiv", 20000},
                                                     {"maxt", 80.01},
                                                     {"min", -150},
                                                     {"mint", 70.01},
                                                     {"peak", 50},
                                                     {"vartakeoff", 50}},
                                                    {{"amp", -INF},
                                                     {"cl", 20},
                                                     {"ddr", 20.02},
                                                     {"deriv2ndt", 80},
                                                     {"derivt", 90},
                                                     {"dur", 10},
                                                     {"durtime1", 80},
                                                     {"maxderiv", 0},
                                                     {"maxt", 80.01},
                                                     {"min", -150},
                                                     {"mint", 90.01},
                                                     {"peak", -INF},
                                                     {"vartakeoff", 50}}};

  int count = 0;
  auto meas = MeasureVoltage(sel);
  meas.percrepol(90);
  for (int i = 0; i < values.size() && i < times.size(); i++) {
    bool reset = meas.measure(times[i], values[i]);
    if (reset) {
      auto res = meas.variablesMap();
      //            EXPECT_EQ(res,ans);
      compareMap(ans[count], res);
      count++;
      meas.reset();
    }
  }
  auto res = meas.variablesMap();
  ASSERT_EQ(count, 5);
  compareMap(ans[count], res);
}

TEST(measure_voltage, measure90_hrd09) {
  std::ifstream st("./tests/hrd09.tsv");
  char const row_delim = '\n';
  char const field_delim = '\t';
  std::vector<std::vector<double>> values;
  for (std::string row; std::getline(st, row, row_delim);) {
    std::istringstream ss(row);
    double a, b, c, d;
    ss >> a >> b >> c >> d;
    values.push_back({a, b, c, d});
  }

  ASSERT_NE(values.size(), 0) << "Values not found."
                              << " working dir should be project dir";

  std::set<std::string> sel = {
      "peak", "min", "maxderiv", "mint",     "derivt",     "maxt",     "cl",
      "amp",  "ddr", "dur",      "durtime1", "vartakeoff", "deriv2ndt"};
  std::vector<std::map<std::string, double>> ans = {
      {
          {"amp", 136.201},
          {"cl", Q_NAN},
          {"ddr", 79.885},
          {"deriv2ndt", 0.02},
          {"derivt", 0.845},
          {"dur", 235.225},
          {"durtime1", 0.775},
          {"maxderiv", 391.725},
          {"maxt", 1.605},
          {"min", -87.2162},
          {"mint", 0.005},
          {"peak", 48.9851},
          {"vartakeoff", -86.0179},
      },
      {
          {"amp", 136.353},
          {"cl", 999.98},
          {"ddr", 79.9999},
          {"deriv2ndt", 1000.1},
          {"derivt", 1000.83},
          {"dur", 227.73},
          {"durtime1", 1000.17},
          {"maxderiv", 391.902},
          {"maxt", 1001.8},
          {"min", -87.1904},
          {"mint", 1000},
          {"peak", 49.1625},
          {"vartakeoff", -79.1904},
      },
      {
          {"amp", 136.328},
          {"cl", 1000},
          {"ddr", 79.9999},
          {"deriv2ndt", 2000.11},
          {"derivt", 2000.83},
          {"dur", 228.33},
          {"durtime1", 2000.18},
          {"maxderiv", 391.871},
          {"maxt", 2001.8},
          {"min", -87.1795},
          {"mint", 2000.01},
          {"peak", 49.1486},
          {"vartakeoff", -79.1795},
      },
      {
          {"amp", 136.288},
          {"cl", 1000.01},
          {"ddr", 79.9999},
          {"deriv2ndt", 3000.11},
          {"derivt", 3000.84},
          {"dur", 229.025},
          {"durtime1", 3000.18},
          {"maxderiv", 391.672},
          {"maxt", 3001.81},
          {"min", -87.169},
          {"mint", 3000.01},
          {"peak", 49.1188},
          {"vartakeoff", -79.169},
      },
      {
          {"amp", 136.263},
          {"cl", 1000},
          {"ddr", 79.9999},
          {"deriv2ndt", 4000.11},
          {"derivt", 4000.84},
          {"dur", 229.625},
          {"durtime1", 4000.18},
          {"maxderiv", 391.587},
          {"maxt", 4001.8},
          {"min", -87.1593},
          {"mint", 4000.01},
          {"peak", 49.1041},
          {"vartakeoff", -79.1593},
      },
      {
          {"amp", 136.263},
          {"cl", 1000},
          {"ddr", 79.9999},
          {"deriv2ndt", 4000.11},
          {"derivt", 5000.01},
          {"dur", 229.625},
          {"durtime1", 4000.18},
          {"maxderiv", -6.34e-05},
          {"maxt", 4001.8},
          {"min", -87.1504},
          {"mint", 5000.01},
          {"peak", INF},
          {"vartakeoff", -79.1593},
      }};

  int count = 0;
  auto meas = MeasureVoltage(sel);
  meas.percrepol(90);
  for (int i = 0; i < values.size(); i++) {
    bool reset = meas.measure(values[i][2], values[i][3]);
    if (reset) {
      auto res = meas.variablesMap();
      compareMap(ans[count], res);
      count++;
      meas.reset();
    }
  }
  auto res = meas.variablesMap();
  ASSERT_EQ(count, 5);
  compareMap(ans[count], res);
}
#undef INF
#undef Q_NAN
#endif  // MEASURE_TEST_H

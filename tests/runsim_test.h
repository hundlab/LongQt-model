#ifndef RUNSIM_TEST_H
#define RUNSIM_TEST_H
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <inexcitablecell.h>
#include <utility>
#include "currentClampProtocol.h"
#include "protocol.h"
#include "runsim.h"

using namespace testing;
using namespace LongQt;

TEST(runsim, build_empty) {
  RunSim runner;
  EXPECT_TRUE(runner.finished());
  auto range = runner.progressRange();
  EXPECT_EQ(range.first, 0);
  EXPECT_EQ(range.second, 0);
  EXPECT_EQ(runner.progress(), 0);
}

TEST(runsim, build_single) {
  auto proto = std::make_shared<CurrentClamp>();
  proto->tMax = 500;
  RunSim runner(proto);
  EXPECT_TRUE(runner.finished());
  auto range = runner.progressRange();
  EXPECT_EQ(range.first, 0);
  EXPECT_EQ(range.second, 500);
  EXPECT_EQ(runner.progress(), 0);
}

TEST(runsim, build_multi) {
  std::vector<std::shared_ptr<Protocol>> protos;
  int tMax = 100;
  int tMax_total = 0;
  for (int i = 0; i < 10; ++i) {
    auto proto = std::make_shared<CurrentClamp>();
    proto->tMax = tMax;
    protos.push_back(proto);
    tMax_total += tMax;
  }

  RunSim runner(protos);
  EXPECT_TRUE(runner.finished());
  auto range = runner.progressRange();
  EXPECT_EQ(range.first, 0);
  EXPECT_EQ(range.second, tMax_total);
  EXPECT_EQ(runner.progress(), 0);
}

TEST(runsim, set_single) {
  auto proto = std::make_shared<CurrentClamp>();
  RunSim runner;
  proto->tMax = 500;
  runner.setSims(proto);
  EXPECT_TRUE(runner.finished());
  auto range = runner.progressRange();
  EXPECT_EQ(range.first, 0);
  EXPECT_EQ(range.second, 500);
  EXPECT_EQ(runner.progress(), 0);
}

TEST(runsim, set_single_mult_X) {
  auto proto = std::make_shared<CurrentClamp>();
  RunSim runner;
  proto->tMax = 500;
  runner.setSims(proto);
  runner.setSims(proto);
  runner.setSims(proto);
  EXPECT_TRUE(runner.finished());
  auto range = runner.progressRange();
  EXPECT_EQ(range.first, 0);
  EXPECT_EQ(range.second, 500);
  EXPECT_EQ(runner.progress(), 0);
}

TEST(runsim, set_single_mult_trials) {
  auto proto = std::make_shared<CurrentClamp>();
  RunSim runner;
  proto->tMax = 500;
  proto->numtrials = 5;
  runner.setSims(proto);
  runner.setSims(proto);
  runner.setSims(proto);
  EXPECT_TRUE(runner.finished());
  auto range = runner.progressRange();
  EXPECT_EQ(range.first, 0);
  EXPECT_EQ(range.second, 5 * 500);
  EXPECT_EQ(runner.progress(), 0);
}

TEST(runsim, set_multi) {
  std::vector<std::shared_ptr<Protocol>> protos;
  RunSim runner;
  int tMax = 100;
  int tMax_total = 0;
  for (int i = 0; i < 10; ++i) {
    auto proto = std::make_shared<CurrentClamp>();
    proto->tMax = tMax;
    protos.push_back(proto);
    tMax_total += tMax;
    tMax += tMax;
  }

  runner.setSims(protos);
  EXPECT_TRUE(runner.finished());
  auto range = runner.progressRange();
  EXPECT_EQ(range.first, 0);
  EXPECT_EQ(range.second, tMax_total);
  EXPECT_EQ(runner.progress(), 0);
}

TEST(runsim, set_multi_mult_X) {
  std::vector<std::shared_ptr<Protocol>> protos;
  RunSim runner;
  int tMax = 100;
  int tMax_total = 0;
  for (int i = 0; i < 10; ++i) {
    auto proto = std::make_shared<CurrentClamp>();
    proto->tMax = tMax;
    protos.push_back(proto);
    tMax_total += tMax;
    tMax += tMax;
  }

  runner.setSims(protos);
  runner.setSims(protos);
  runner.setSims(protos);
  EXPECT_TRUE(runner.finished());
  auto range = runner.progressRange();
  EXPECT_EQ(range.first, 0);
  EXPECT_EQ(range.second, tMax_total);
  EXPECT_EQ(runner.progress(), 0);
}

TEST(runsim, append_single) {
  std::vector<std::shared_ptr<Protocol>> protos;
  RunSim runner;
  int tMax = 100;
  int tMax_2 = 100;
  int tMax_total = 0;
  for (int i = 0; i < 10; ++i) {
    auto proto = std::make_shared<CurrentClamp>();
    proto->tMax = tMax;
    protos.push_back(proto);
    tMax_total += tMax;
    tMax += tMax;
  }
  runner.setSims(protos);
  auto proto = std::make_shared<CurrentClamp>();
  proto->tMax = tMax_2;
  runner.appendSims(proto);
  EXPECT_TRUE(runner.finished());
  auto range = runner.progressRange();
  EXPECT_EQ(range.first, 0);
  EXPECT_EQ(range.second, tMax_total + tMax_2);
  EXPECT_EQ(runner.progress(), 0);
}

TEST(runsim, append_single_mult_trials) {
  std::vector<std::shared_ptr<Protocol>> protos;
  RunSim runner;
  int tMax = 100;
  int tMax_2 = 100;
  int tMax_total = 0;
  for (int i = 0; i < 10; ++i) {
    auto proto = std::make_shared<CurrentClamp>();
    proto->tMax = tMax;
    protos.push_back(proto);
    tMax_total += tMax;
    tMax += tMax;
  }
  runner.setSims(protos);
  auto proto = std::make_shared<CurrentClamp>();
  proto->tMax = tMax_2;
  proto->numtrials = 5;
  runner.appendSims(proto);
  EXPECT_TRUE(runner.finished());
  auto range = runner.progressRange();
  EXPECT_EQ(range.first, 0);
  EXPECT_EQ(range.second, tMax_total + 5 * tMax_2);
  EXPECT_EQ(runner.progress(), 0);
}

TEST(runsim, append_multi) {
  std::vector<std::shared_ptr<Protocol>> protos;
  RunSim runner;
  int tMax = 100;
  int tMax_total = 0;
  for (int i = 0; i < 10; ++i) {
    auto proto = std::make_shared<CurrentClamp>();
    proto->tMax = tMax;
    protos.push_back(proto);
    tMax_total += tMax;
    tMax += tMax;
  }
  runner.setSims(protos);
  runner.appendSims(protos);
  EXPECT_TRUE(runner.finished());
  auto range = runner.progressRange();
  EXPECT_EQ(range.first, 0);
  EXPECT_EQ(range.second, 2 * tMax_total);
  EXPECT_EQ(runner.progress(), 0);
}

TEST(runsim, clear) {
  std::vector<std::shared_ptr<Protocol>> protos;
  RunSim runner;
  int tMax = 100;
  int tMax_total = 0;
  for (int i = 0; i < 10; ++i) {
    auto proto = std::make_shared<CurrentClamp>();
    proto->tMax = tMax;
    protos.push_back(proto);
    tMax_total += tMax;
    tMax += tMax;
  }
  runner.setSims(protos);
  runner.clear();
  EXPECT_TRUE(runner.finished());
  auto range = runner.progressRange();
  EXPECT_EQ(range.first, 0);
  EXPECT_EQ(range.second, 0);
  EXPECT_EQ(runner.progress(), 0);
}

TEST(runsim, clear_empty) {
  RunSim runner;
  runner.clear();
  EXPECT_TRUE(runner.finished());
  auto range = runner.progressRange();
  EXPECT_EQ(range.first, 0);
  EXPECT_EQ(range.second, 0);
  EXPECT_EQ(runner.progress(), 0);
}

TEST(runsim, wait_none) {
  RunSim runner;
  runner.run();
  runner.wait();
  EXPECT_TRUE(runner.finished());
  auto range = runner.progressRange();
  EXPECT_EQ(range.first, 0);
  EXPECT_EQ(range.second, 0);
  EXPECT_GE(runner.progress(), 0);
}

TEST(runsim, run_single) {
  auto proto = std::make_shared<CurrentClamp>();
  proto->cell(std::make_shared<InexcitableCell>());
  proto->tMax = 500;
  RunSim runner(proto);
  runner.run();
  EXPECT_FALSE(runner.finished());
  runner.wait();
  EXPECT_TRUE(runner.finished());
  auto range = runner.progressRange();
  EXPECT_EQ(range.first, 0);
  EXPECT_EQ(range.second, 500);
  EXPECT_GE(runner.progress(), range.second);
  EXPECT_LT(runner.progress(), range.second + 1);
}

TEST(runsim, run_multi) {
  std::vector<std::shared_ptr<Protocol>> protos;
  int tMax = 100;
  int tMax_total = 0;
  for (int i = 0; i < 10; ++i) {
    auto proto = std::make_shared<CurrentClamp>();
    proto->tMax = tMax;
    proto->cell(std::make_shared<InexcitableCell>());
    protos.push_back(proto);
    tMax_total += tMax;
  }

  RunSim runner(protos);
  runner.run();
  EXPECT_FALSE(runner.finished());
  runner.wait();
  EXPECT_TRUE(runner.finished());
  auto range = runner.progressRange();
  EXPECT_EQ(range.first, 0);
  EXPECT_EQ(range.second, tMax_total);
  EXPECT_GE(runner.progress(), range.second);
  EXPECT_LT(runner.progress(), range.second + 1);
}

TEST(runsim, finishedCallback) {
  std::vector<std::shared_ptr<Protocol>> protos;
  int tMax = 100;
  int tMax_total = 0;
  std::atomic<int> fin = 0;
  for (int i = 0; i < 10; ++i) {
    auto proto = std::make_shared<CurrentClamp>();
    proto->tMax = tMax;
    proto->cell(std::make_shared<InexcitableCell>());
    protos.push_back(proto);
    tMax_total += tMax;
  }
  RunSim runner(protos);
  runner.finishedCallback([&fin]() { ++fin; });
  runner.run();
  EXPECT_FALSE(runner.finished());
  runner.wait();
  EXPECT_TRUE(runner.finished());
  auto range = runner.progressRange();
  EXPECT_EQ(range.first, 0);
  EXPECT_EQ(range.second, tMax_total);
  EXPECT_GE(runner.progress(), range.second);
  EXPECT_LT(runner.progress(), range.second + 1);
  EXPECT_EQ(fin, 1);
}

TEST(runsim, startCallback) {
  std::vector<std::shared_ptr<Protocol>> protos;
  int tMax = 100;
  int tMax_total = 0;
  std::atomic<int> start = 0;
  for (int i = 0; i < 10; ++i) {
    auto proto = std::make_shared<CurrentClamp>();
    proto->tMax = tMax;
    proto->cell(std::make_shared<InexcitableCell>());
    protos.push_back(proto);
    tMax_total += tMax;
  }
  RunSim runner(protos);
  runner.startCallback([&start]() { ++start; });
  runner.run();
  EXPECT_EQ(start, 1);
  EXPECT_FALSE(runner.finished());
  runner.wait();
  EXPECT_TRUE(runner.finished());
  auto range = runner.progressRange();
  EXPECT_EQ(range.first, 0);
  EXPECT_EQ(range.second, tMax_total);
  EXPECT_GE(runner.progress(), range.second);
  EXPECT_LT(runner.progress(), range.second + 1);
}

TEST(runsim, cancel) {
  std::vector<std::shared_ptr<Protocol>> protos;
  int tMax = 1000;
  int tMax_total = 0;
  for (int i = 0; i < 10; ++i) {
    auto proto = std::make_shared<CurrentClamp>();
    proto->tMax = tMax;
    proto->cell(std::make_shared<InexcitableCell>());
    protos.push_back(proto);
    tMax_total += tMax;
  }
  RunSim runner(protos);
  runner.run();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  runner.cancel();
  EXPECT_FALSE(runner.finished());
  runner.wait();
  EXPECT_TRUE(runner.finished());
  auto range = runner.progressRange();
  EXPECT_EQ(range.first, 0);
  EXPECT_EQ(range.second, tMax_total);
  EXPECT_GE(runner.progress(), range.first);
  EXPECT_LT(runner.progress(), range.second);
}

#endif  // RUNSIM_TEST_H

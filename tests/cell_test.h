#ifndef CELL_TEST
#define CELL_TEST
#include <gmock/gmock-matchers.h>
#include <gpbatrialonal17.h>
#include <gtest/gtest.h>
#include <inexcitablecell.h>

using namespace testing;
using namespace LongQt;

TEST(cellkernel, updateV) {
  auto cell = InexcitableCell();
  cell.vOld = 100;
  cell.iTot = 50;
  cell.dt = 1. / 5;
  double v = cell.updateV();
  EXPECT_EQ(v, 90.);
  EXPECT_EQ(cell.dVdt, -50.);
}

TEST(cellkernel, setV) {
  auto cell = InexcitableCell();
  cell.setV(100);
  EXPECT_DOUBLE_EQ(cell.vOld, 100);
}

TEST(cellkernel, tstep) {
  auto cell = InexcitableCell();
  double t = cell.t;
  double dt = cell.dt;
  double t_new = cell.tstep(100);
  EXPECT_EQ(t_new, t + dt);
}

TEST(cellkernel, vars) {
  auto cell = InexcitableCell();
  auto vars = cell.vars();
  for (auto var : vars) {
    EXPECT_TRUE(cell.hasVar(var));
    EXPECT_TRUE(cell.setVar(var, -1000));
    EXPECT_EQ(cell.var(var), -1000);
  }
}

TEST(cellkernel, pars) {
  auto cell = InexcitableCell();
  auto pars = cell.pars();
  for (auto par : pars) {
    EXPECT_TRUE(cell.hasPar(par));
    EXPECT_TRUE(cell.setPar(par, -1000));
    EXPECT_EQ(cell.par(par), -1000);
  }
}

TEST(cellkernel, options) {
  auto* cell = new GpbAtrialOnal17;
  EXPECT_FALSE(cell->option("ISO"));
  EXPECT_FALSE(cell->option("S571E"));
  EXPECT_FALSE(cell->option("S571A"));

  auto optsMap = cell->optionsMap();
  EXPECT_EQ(optsMap["ISO"], cell->option("ISO"));

  cell->setOption("S571E", true);
  EXPECT_TRUE(cell->option("S571E"));

  cell->setOption("S571A", true);
  EXPECT_TRUE(cell->option("S571A"));
  EXPECT_FALSE(cell->option("S571E"));

  cell->setOption("S571E", true);
  cell->setOption("S571A", false);
  EXPECT_FALSE(cell->option("S571A"));
  EXPECT_TRUE(cell->option("S571E"));
  delete cell;
}

TEST(cell, ConstantSelection) {
  auto cell = InexcitableCell();
  auto selection = cell.getConstantSelection();
  EXPECT_EQ(selection.size(), 0);
  std::set<std::string> new_selection = {"dtmin", "RGAS", "TEMP"};
  cell.setConstantSelection(new_selection);
  selection = cell.getConstantSelection();
  EXPECT_EQ(selection.size(), 3);
  std::set<std::string> test;
  test.insert(new_selection.begin(), new_selection.end());
  selection = cell.getConstantSelection();
  test.insert(selection.begin(), selection.end());
  EXPECT_EQ(test.size(), 3);
}

TEST(cell, ConstantSelection_extra) {
  auto cell = InexcitableCell();
  std::set<std::string> new_selection = {"dtmin", "RGAS", "TEMP", "asdf",
                                         "vOld"};
  cell.setConstantSelection(new_selection);
  std::vector<std::string> test(10);
  auto selection = cell.getConstantSelection();
  auto it =
      std::set_intersection(new_selection.begin(), new_selection.end(),
                            selection.begin(), selection.end(), test.begin());
  test.resize(it - test.begin());
  EXPECT_EQ(test.size(), 3);
}

TEST(cell, VariableSelection) {
  auto cell = InexcitableCell();
  auto selection = cell.getVariableSelection();
  EXPECT_EQ(selection.size(), 2);
  std::set<std::string> new_selection = {"iTot", "iKt", "iCat"};
  cell.setVariableSelection(new_selection);
  selection = cell.getVariableSelection();
  EXPECT_EQ(selection.size(), 3);
  std::set<std::string> test;
  test.insert(new_selection.begin(), new_selection.end());
  selection = cell.getVariableSelection();
  test.insert(selection.begin(), selection.end());
  EXPECT_EQ(test.size(), 3);
}

TEST(cell, VariableSelection_extra) {
  auto cell = InexcitableCell();
  std::set<std::string> new_selection = {"dtmin", "RGAS", "TEMP",
                                         "asdf",  "vOld", "iKt"};
  cell.setVariableSelection(new_selection);
  std::vector<std::string> test(10);
  auto selection = cell.getVariableSelection();
  auto it =
      std::set_intersection(new_selection.begin(), new_selection.end(),
                            selection.begin(), selection.end(), test.begin());
  test.resize(it - test.begin());
  EXPECT_EQ(test.size(), 2);
}
#endif

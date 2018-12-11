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
  auto inital_opt = cell->optionStr();
  EXPECT_EQ(inital_opt, "WT");
  auto optsMap = cell->optionsMap();
  EXPECT_EQ(optsMap["WT"], 0);
  cell->setOption(GpbAtrialOnal17::Options::S571E);
  EXPECT_EQ(optsMap["S571E"], cell->option());
  EXPECT_EQ("S571E", cell->optionStr());
  cell->setOption(GpbAtrialOnal17::Options::S571E |
                  GpbAtrialOnal17::Options::S571A);
  EXPECT_EQ(optsMap["S571E"], cell->option());
  cell->setOption(GpbAtrialOnal17::Options::S571A |
                  GpbAtrialOnal17::Options::S2814D);
  EXPECT_EQ(optsMap["S571A"] | optsMap["S2814D"], cell->option());
  delete cell;
}
#endif

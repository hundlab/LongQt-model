#ifndef PROTOCOL_TEST
#define PROTOCOL_TEST
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <currentClampProtocol.h>
#include <gridProtocol.h>
#include <inexcitablecell.h>
#include <voltageClampProtocol.h>

using namespace testing;
using namespace LongQt;
using namespace std;
//----------------------------------- Protocol

class Mock_Protocol : public Protocol {
 public:
  MOCK_METHOD0(clone, Protocol*());
  MOCK_METHOD0(runTrial, bool());
  virtual bool cell(const std::string& type) override {
    return Protocol::cell(type);
  }
  MOCK_METHOD1(cell, void(std::shared_ptr<Cell>));
  MOCK_CONST_METHOD0(cell, std::shared_ptr<Cell>());
  MOCK_METHOD0(pvars, PvarsCell&());
  MOCK_METHOD0(measureMgr, MeasureManager&());
  MOCK_CONST_METHOD0(type, const char*());
};

TEST(protocol, trial) {
  Mock_Protocol proto;
  EXPECT_FALSE(proto.trial(5));
  EXPECT_EQ(proto.trial(), 0);
  proto.numtrials(5);
  EXPECT_TRUE(proto.trial(4));
  EXPECT_EQ(proto.trial(), 4);
}

TEST(protocol, runSim_false) {
  Mock_Protocol proto;
  proto.numtrials(5);
  EXPECT_CALL(proto, runTrial()).Times(5).WillRepeatedly(Return(false));
  EXPECT_FALSE(proto.runSim());
}

TEST(protocol, runSim_true) {
  Mock_Protocol proto;
  proto.numtrials(5);
  EXPECT_CALL(proto, runTrial()).Times(5).WillRepeatedly(Return(true));
  EXPECT_TRUE(proto.runSim());
}

TEST(protocol, runSim_mixed) {
  Mock_Protocol proto;
  proto.numtrials(5);
  EXPECT_CALL(proto, runTrial())
      .Times(5)
      .WillOnce(Return(true))
      .WillOnce(Return(true))
      .WillOnce(Return(false))
      .WillRepeatedly(Return(true));
  EXPECT_FALSE(proto.runSim());
}

TEST(protocol, cell_fake) {
  Mock_Protocol proto;
  try {
    EXPECT_FALSE(proto.cell("NOT A CELL NAME"));
  } catch (std::out_of_range&) {
    SUCCEED();
  }
}

TEST(protocol, cell_real) {
  Mock_Protocol proto;
  std::string name = (++CellUtils::cellMap.begin())->first;
  EXPECT_TRUE(proto.cell(name));
  //  EXPECT_CALL(proto, cell(std::shared_ptr<LongQt::Cell>)).Times(1);
}

// TEST(protocol, stopTrial) {}

TEST(protocol, DataDir) {
  Mock_Protocol proto;
  proto.setDataDir("", "D:", "_endText");
  QString datadir = proto.datadir.absolutePath();
  EXPECT_TRUE(datadir == proto.getDataDir().c_str());
  EXPECT_TRUE(datadir.contains("D:/"));
  EXPECT_TRUE(datadir.contains("data"));
  EXPECT_TRUE(datadir.contains("_endText"));
}

//----------------------------------- CurrentClamp
//----------------------------------- VoltageClamp

//----------------------------------- Grid
TEST(gridprotocol, cell) {
  auto protocol = GridProtocol();
  auto cell_ref = protocol.cell();
  EXPECT_STREQ(protocol.cell()->type(), "gridCell");
  protocol.cell("Rabbit Sinus Node (Kurata 2008)");
  EXPECT_STREQ(protocol.cell()->type(), "gridCell");
  protocol.cell(shared_ptr<Cell>());
  EXPECT_STREQ(protocol.cell()->type(), "gridCell");

  auto cell1 = make_shared<InexcitableCell>();
  protocol.cell(cell1);
  EXPECT_STREQ(protocol.cell()->type(), "gridCell");

  auto cell2 = make_shared<GridCell>();
  EXPECT_TRUE(cell_ref == protocol.cell());
  protocol.cell(cell2);
  EXPECT_FALSE(cell_ref == protocol.cell());
  EXPECT_TRUE(cell2 == protocol.cell());
  EXPECT_EQ(cell_ref.use_count(), 1);
}
TEST(gridprotocol, misc) {
  auto protocol = GridProtocol();
  auto grid = protocol.getGrid();
}
#endif

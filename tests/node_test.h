#ifndef NODE_TEST_H
#define NODE_TEST_H
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <hrd09.h>
#include <inexcitablecell.h>
#include <node.h>
#include <grid.h>

using namespace testing;
using namespace LongQt;

TEST(node, parent_null) {
  Node node = Node();
  auto parent = node.getParent();
  EXPECT_EQ(parent, (Grid*)NULL);
  node.setParent(0, 1, 6);
  parent = node.getParent();
  EXPECT_EQ(parent, (Grid*)NULL);
  EXPECT_EQ(node.row, -1);
  EXPECT_EQ(node.col, -1);
}

TEST(node, set_parent) {
    Node node = Node();
    Grid grid = Grid();
    node.setParent(&grid, 1, 6);
    auto parent = node.getParent();
    EXPECT_EQ(parent, &grid);
    EXPECT_EQ(node.row, 1);
    EXPECT_EQ(node.col, 6);
}

TEST(node, set_position) {
    Node node = Node();
    node.setPosition(6, 4);
    EXPECT_EQ(node.row, 6);
    EXPECT_EQ(node.col, 4);
}

//TEST(node, set_CondConst_Inex) {
//  Node node = Node();
//  Grid grid = Grid();
//  node.setParent(&grid, 0, 0);
//  EXPECT_EQ(node.getCondConst(CellUtils::bottom), 0);
//  EXPECT_EQ(node.getCondConst(CellUtils::top), 0);
//  EXPECT_EQ(node.getCondConst(CellUtils::right), 0);
//  EXPECT_EQ(node.getCondConst(CellUtils::left), 0);

//  node.setCondConst(0.01, CellUtils::bottom, false, 100);
//  EXPECT_EQ(node.getCondConst(CellUtils::bottom), 0);
//  node.setCondConst(0.01, CellUtils::top, false, 100);
//  EXPECT_EQ(node.getCondConst(CellUtils::top), 0);
//  node.setCondConst(0.01, CellUtils::right, false, 100);
//  EXPECT_EQ(node.getCondConst(CellUtils::right), 0);
//  node.setCondConst(0.01, CellUtils::left, false, 100);
//  EXPECT_EQ(node.getCondConst(CellUtils::left), 0);
//}

#endif  // NODE_TEST_H

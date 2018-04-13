#ifndef CELL_TEST
#define CELL_TEST
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <inexcitablecell.h>

using namespace testing;

TEST(cellkernel, setV)
{
    auto cell = InexcitableCell();
    EXPECT_DOUBLE_EQ(cell.vOld,-88);
    cell.setV(100);
    EXPECT_DOUBLE_EQ(cell.vOld,100);
}


#endif

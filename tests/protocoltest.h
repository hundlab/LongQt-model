#ifndef PROTOCOL_TEST
#define PROTOCOL_TEST
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <gridProtocol.h>
#include <currentClampProtocol.h>
#include <voltageClampProtocol.h>
#include <inexcitablecell.h>

using namespace testing;

TEST(gridprotocol, cell)
{
    auto protocol = GridProtocol();
    auto cell_ref = protocol.cell();
    EXPECT_STREQ(protocol.cell()->type(),"gridCell");
    protocol.cell("Rabbit Sinus Node (Kurata 2008)");
    EXPECT_STREQ(protocol.cell()->type(),"gridCell");
    protocol.cell(shared_ptr<Cell>());
    EXPECT_STREQ(protocol.cell()->type(),"gridCell");

    auto cell1 =  make_shared<InexcitableCell>();
    protocol.cell(cell1);
    EXPECT_STREQ(protocol.cell()->type(),"gridCell");

    auto cell2 = make_shared<GridCell>();
    EXPECT_TRUE(cell_ref == protocol.cell());
    protocol.cell(cell2);
    EXPECT_FALSE(cell_ref == protocol.cell());
    EXPECT_TRUE(cell2 == protocol.cell());
    EXPECT_EQ(cell_ref.use_count(),1);
}
#endif

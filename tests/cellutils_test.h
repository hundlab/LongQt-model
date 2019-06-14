#ifndef CELLUTILS_TEST_H
#define CELLUTILS_TEST_H
#include <cellutils.h>
#include <currentClampProtocol.h>
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <kurata08.h>

using namespace testing;
using namespace LongQt;

TEST(flipSide, all) {
  CellUtils::Side s;
  s = CellUtils::Side::bottom;
  auto ret = CellUtils::flipSide(s);
  EXPECT_EQ(CellUtils::Side::top, ret);
  ret = CellUtils::flipSide(ret);
  EXPECT_EQ(s, ret);
  s = CellUtils::Side::left;
  ret = CellUtils::flipSide(s);
  EXPECT_EQ(CellUtils::Side::right, ret);
  ret = CellUtils::flipSide(ret);
  EXPECT_EQ(s, ret);
}

TEST(set_default_vals, all) {
  CurrentClamp* proto = new CurrentClamp;
  proto->cell(std::make_shared<Kurata08>());
  CellUtils::set_default_vals(*proto);
  EXPECT_EQ(proto->stimval, -60);
  EXPECT_EQ(proto->paceflag, false);
}

TEST(str_to_flag, empty) {
  auto flagMap = CellUtils::strToFlag("");
  EXPECT_TRUE(flagMap.size() == 0);
}

TEST(str_to_flag, one) {
  auto flagMap = CellUtils::strToFlag("TEST1");
  EXPECT_TRUE(flagMap.size() == 1);
  EXPECT_EQ(flagMap["TEST1"], true);
}

TEST(str_to_flag, many) {
  auto flagMap = CellUtils::strToFlag("TEST1|&^*&^*&|sadfasfd");
  EXPECT_TRUE(flagMap.size() == 3);
  EXPECT_EQ(flagMap["TEST1"], true);
  EXPECT_EQ(flagMap["&^*&^*&"], true);
  EXPECT_EQ(flagMap["sadfasfd"], true);
}

TEST(flag_to_str, empty) {
  auto str = CellUtils::flagToStr({});
  EXPECT_EQ(str, "");
}

TEST(flag_to_str, many) {
  auto str =
      CellUtils::flagToStr({{"ASDF", true}, {"%^&", true}, {"IDL", false}});
  EXPECT_EQ(str, "%^&|ASDF|IDL");
}
/*
 * Hopefully I will remove the dependance on QXml and
 * so these functions will become irrelevant
 * readNext
 * readUpLevel
 */

TEST(trim, all) {
  EXPECT_STREQ(CellUtils::trim(" asdf      \t  \r   ").c_str(), "asdf");
  EXPECT_STREQ(CellUtils::trim("\v asdf   \f  \n      ").c_str(), "asdf");
  EXPECT_STREQ(CellUtils::trim("asdf").c_str(), "asdf");
  EXPECT_STREQ(CellUtils::trim("").c_str(), "");
}

TEST(string_bool, all) {
  EXPECT_STREQ("true", CellUtils::to_string(true).c_str());
  EXPECT_STREQ("false", CellUtils::to_string(false).c_str());
  EXPECT_EQ(true, CellUtils::stob(" TRUe"));
  EXPECT_EQ(false, CellUtils::stob("  falSe    "));
  EXPECT_EQ(false, CellUtils::stob(""));
}

TEST(split, base) {
  std::vector<std::string> v;
  EXPECT_EQ(v, CellUtils::split("", '*'));
}

TEST(split, no_text) {
  std::vector<std::string> v;
  EXPECT_EQ(v, CellUtils::split("d", 'd'));
  v = {"a"};
  EXPECT_EQ(v, CellUtils::split("ad", 'd'));
  EXPECT_EQ(v, CellUtils::split("da", 'd'));
}

TEST(split, multiple_splits) {
  std::vector<std::string> v = {"the", "other", "day", "i"};
  EXPECT_EQ(v, CellUtils::split("the&other&day&i", '&'));
}

TEST(strprintf, base) {
  std::string instring = "";
  std::string outstring = "";
  std::string res = CellUtils::strprintf(instring);
  EXPECT_STREQ(outstring.c_str(), res.c_str());
}

TEST(strprintf, base1) {
  std::string instring = "342";
  std::string outstring = "342";
  std::string res = CellUtils::strprintf(instring, 0);
  EXPECT_STREQ(outstring.c_str(), res.c_str());
}

TEST(strprintf, unnamed_pos) {
  std::string instring = "{} {}{}";
  std::string outstring = "0 112";
  std::string res = CellUtils::strprintf(instring, 0, 11, 2);
  EXPECT_STREQ(outstring.c_str(), res.c_str());
}

TEST(strprintf, named_pos) {
  std::string instring = "{0}{1}{0} {4} {3} ";
  std::string outstring = "010 44 3 ";
  std::string res = CellUtils::strprintf(instring, 0, 1, 2, 3, 44, 5);
  EXPECT_STREQ(outstring.c_str(), res.c_str());
}

TEST(strprintf, double_bracket) {
  std::string instring = "{{}} {} {{}} ";
  std::string outstring = "{} 0 {} ";
  std::string res = CellUtils::strprintf(instring, 0);
  EXPECT_STREQ(outstring.c_str(), res.c_str());
}

TEST(strprintf, other_text) {
  std::string instring = "{}a{}b:{};!@#$%^& ";
  std::string outstring = "0a1b:22;!@#$%^& ";
  std::string res = CellUtils::strprintf(instring, 0, 1, 22);
  EXPECT_STREQ(outstring.c_str(), res.c_str());
}

TEST(strprintf, colon) {
  std::string instring = "a{{2:}} asd{0:} {2}463";
  std::string outstring = "a{2:} asdaba cca463";
  std::string res = CellUtils::strprintf(instring, "aba", "bba", "cca");
  EXPECT_STREQ(outstring.c_str(), res.c_str());
}
#endif  // CELLUTILS_TEST_H

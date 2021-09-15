#ifndef LOGGER_TEST_H
#define LOGGER_TEST_H
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <logger.h>

using namespace testing;
using namespace LongQt;

TEST(logger, no_throw_multi_arg) {
  Logger::getInstance()->exceptionEnabled = true;
  EXPECT_NO_THROW(
      Logger::getInstance()->write("TEST: test {}", "no_throw_mult"));
}

TEST(logger, throw_multi_arg) {
  Logger::getInstance()->exceptionEnabled = true;
  try {
    Logger::getInstance()->write<std::runtime_error>("TEST: test {}",
                                                     "throw_mult");
  } catch (std::runtime_error& e) {
    EXPECT_EQ(e.what(), std::string("TEST: test throw_mult"));
  } catch (...) {
    FAIL() << "expected std::runtime_error";
  }
}
#endif  // LOGGER_TEST_H

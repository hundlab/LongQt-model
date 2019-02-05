#include "cell_test.h"
#include "cellutils_test.h"
#include "measure_test.h"
#include "protocol_test.h"
#include "threadpool_test.h"
#include "runsim_test.h"
#include "node_test.h"

#include <gtest/gtest.h>

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#include "mon_confile.h"

#include <memory>

#include "gtest/gtest.h"
#include "test/test_runtime.h"

namespace Monitor {
TEST(MonConfileTest, Init) {
  using namespace Monitor;

  Singleton<MonConfile>::Get().Init(TEST_DEFAULT_RUN_DIR
                                    "/test/resources/mon_confile_init.json");
}
}  // namespace Monitor
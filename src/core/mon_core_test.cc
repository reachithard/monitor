#include "mon_core.h"

#include <memory>

#include "gtest/gtest.h"
#include "mon_confile.h"
#include "test/test_runtime.h"

TEST(MonCoreTest, Init) {
  using namespace Monitor;
  std::unique_ptr<MonCore> ptr = std::make_unique<MonCore>();
  ptr->Init(TEST_DEFAULT_RUN_DIR "/test/resources/mon_confile_init.json");
  ptr->Update();
}

#include "mon_timer.h"

#include <memory>

#include "gtest/gtest.h"
#include "utils/logger.hpp"

namespace Monitor {
TEST(MonTimerTest, addTimer) {
  using namespace Monitor;
  LOG_DEBUG("time start");
  std::unique_ptr<Monitor::MonTimer> ptr =
      std::make_unique<Monitor::MonTimer>();
  ptr->AddTimer([](const std::error_code&) { LOG_DEBUG("time out"); }, 5000,
                true);
  ptr->Run();
  while (true) {
    sleep(1);
  }
}
}  // namespace Monitor
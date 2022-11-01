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
  ptr->AddTimer(
      [](const std::error_code&) {
        LOG_DEBUG("time out");
        sleep(4);  // 定时任务里面不能有太耗时的任务 否则应该提交到子线程
      },
      5000, false);

  std::string timer1 = ptr->AddTimer(
      [](const std::error_code&) { LOG_DEBUG("time1 out"); }, 1000, true);
  ptr->Run();
  int i = 0;
  while (true) {
    sleep(1);
    i++;
    if (i == 20) {
      ptr->ClearTimer(timer1);
      ptr->Stop();
      break;
    }
  }
}
}  // namespace Monitor
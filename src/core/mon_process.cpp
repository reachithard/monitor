#include "mon_process.h"

#include <cstring>

#include "export/monitor_errors.h"
#include "utils/logger.hpp"

namespace Monitor {
MonProcess::MonProcess() {}
MonProcess::~MonProcess() {}

int32_t MonProcess::Init(const ConfigItem_t* config) {
  if (config == nullptr) {
    return MON_CORE_PARAM;
  }

  asio::signal_set signals(context, SIGINT, SIGTERM);
  signals.async_wait(
      [&](std::error_code /*ec*/, int /*signo*/) { context.stop(); });
  context.notify_fork(asio::io_context::fork_prepare);

  // 父进程不退出
  int32_t ret = Parse(config);

  return ret;
}

int32_t MonProcess::Parse(const ConfigItem_t* config) {
  // 解析两次 第一次解析基本的配置 第二次解析process配置
  int32_t ret = 0;
  for (uint32_t idx = 0; idx < config->childCnt; idx++) {
    ConfigItem_t* cur = &config->children[idx];
    if (cur == nullptr) {
      LOG_INFO("get a unformat config, please check it");
      continue;
    }
    if (cur->childCnt == 0 && cur->children == nullptr) {
      // 基础配置
    } else if (strcasecmp(cur->key, "process") != 0) {
      // 复杂配置
    }
  }

  // 进程配置
}
}  // namespace Monitor
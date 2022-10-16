#include "mon_confile.h"

#include "export/mon_config.h"
#include "export/monitor_errors.h"
#include "utils/logger.hpp"

namespace Monitor {

MonConfile::MonConfile() : config(nullptr) {}

MonConfile::~MonConfile() {
  if (config != nullptr) {
    ConfigFree(config);
    config = nullptr;
  }
}

int32_t MonConfile::Init(const std::string& file) {
  if (config != nullptr) {
    ConfigFree(config);
    config = nullptr;
  }

  config = ConfigParseFile(file.c_str());
  if (config == nullptr) {
    LOG_DEBUG("parse config error:{}", file);
    return MON_CORE_PARAM;
  }

  // 进行配置文件的解析
  int32_t ret = 0;
  for (uint32_t idx = 0; idx < config->childCnt; idx++) {
    ConfigItem_t* cur = &config->children[idx];
    if (cur->children == nullptr) {
      // 说明是普通的配置 key value
      // 只要有一个配置有问题 就让程序跑不起来
      if (DispatchValue(cur) != 0) {
        LOG_DEBUG("get conf error:{}", cur->key);
        ret = MON_CORE_CONF;
      }
    } else {
      // 说明是复杂的配置 key tree
      if (DispatchBlock(cur) != 0) {
        LOG_DEBUG("get conf error:{}", cur->key);
        ret = MON_CORE_CONF;
      }
    }
  }

  // 进行释放
  ConfigFree(config);
  config = nullptr;
  return ret;
}

int32_t MonConfile::DispatchValue(const ConfigItem_t* conf) {}

int32_t MonConfile::DispatchBlock(const ConfigItem_t* conf) {}
}  // namespace Monitor
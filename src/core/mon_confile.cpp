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
  // 只调用一次 之后变成编译期间
  std::call_once(mapFlag, &MonConfile::InitConfMaps, this);
  if (config != nullptr) {
    ConfigFree(config);
    config = nullptr;
  }

  config = ConfigParseFile(file.c_str());
  if (config == nullptr) {
    LOG_DEBUG("parse config error:{}", file);
    return MON_CORE_PARAM;
  }

  // 进行配置文件的解析 遍历两次 第一次加载key value 第二次加载block
  // 防止全局配置写在block后面
  int32_t ret = 0;
  for (uint32_t idx = 0; idx < config->childCnt; idx++) {
    ConfigItem_t* cur = &config->children[idx];
    if (cur->children == nullptr) {
      // 说明是普通的配置 key value
      // 只要有一个配置有问题 就让程序跑不起来 将配置问题扼杀在启动阶段
      if (DispatchConf(cur) != 0) {
        LOG_DEBUG("get conf error:{}", cur->key);
        ret = MON_CORE_CONF;
      }
    }
  }

  for (uint32_t idx = 0; idx < config->childCnt; idx++) {
    ConfigItem_t* cur = &config->children[idx];
    if (cur->children != nullptr) {
      // 说明是复杂配置 key tree
      // 只要有一个配置有问题 就让程序跑不起来 将配置问题扼杀在启动阶段
      if (DispatchConf(cur) != 0) {
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

int32_t MonConfile::DispatchConf(const ConfigItem_t* conf) {
  int32_t ret = 0;
  if (conf->key == nullptr) {
    LOG_DEBUG("get a error conf");
    return MON_CORE_CONF;
  }

  auto it = confMaps.find(std::string(conf->key));
  if (it == confMaps.end()) {
    return MON_CORE_CONF;
  }
  if (it->second != nullptr) {
    ret = it->second(conf);
  } else {
    LOG_DEBUG("unsupport conf");
    return MON_CORE_UNSUPPORT;
  }
  return ret;
}

void MonConfile::InitConfMaps() {
  // TODO
  confMaps["pluginDir"] = nullptr;
  confMaps["baseDir"] = nullptr;
  confMaps["pidFile"] = nullptr;
  confMaps["interval"] = nullptr;
  confMaps["readThreads"] = nullptr;
  confMaps["writeThreads"] = nullptr;
  confMaps["timeout"] = nullptr;
  confMaps["maxReadInterval"] = nullptr;
  confMaps["plugins"] = nullptr;
}
}  // namespace Monitor
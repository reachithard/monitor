#include "mon_confile.h"

#include "export/mon_config.h"
#include "export/mon_config_utils.h"
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
  std::call_once(mapFlag, &MonConfile::InitDispatchMaps, this);
  if (config != nullptr) {
    ConfigFree(config);
    config = nullptr;
  }

  config = ConfigParseFile(file.c_str());
  if (config == nullptr) {
    LOG_DEBUG("parse config error:{}", file);
    return ERR_CORE_PARAM;
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
        LOG_DEBUG("get normal conf error:{}", cur->key);
        ret = ERR_CORE_CONF;
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
        ret = ERR_CORE_CONF;
      }
    }
  }

  // 进行释放
  LOG_DEBUG("free config");
  ConfigFree(config);
  config = nullptr;
  return ret;
}

int32_t MonConfile::DispatchConf(const ConfigItem_t* conf) {
  int32_t ret = 0;
  if (conf->key == nullptr) {
    LOG_DEBUG("get a error conf");
    return ERR_CORE_CONF;
  }

  // 先转换为小写 防止大小写不敏感
  std::string temp = std::string(conf->key);
  std::transform(temp.begin(), temp.end(), temp.begin(),
                 [](unsigned char c) { return std::tolower(c); }  // correct
  );
  auto it = confMaps.find(temp);
  if (it == confMaps.end()) {
    return ERR_CORE_CONF;
  }
  if (it->second != nullptr) {
    ret = it->second(conf);
  } else {
    LOG_DEBUG("unsupport conf");
    return ERR_CORE_UNSUPPORT;
  }
  return ret;
}

int32_t MonConfile::DispatchBaseConf(const ConfigItem_t* conf) {
  LOG_DEBUG("called DispatchBaseConf");
  // TODO 之后优化
  assert(conf != nullptr);

  if (strcasecmp(conf->key, "plugindir") == 0) {
    char* temp = MonConfigGetStr(conf);
    if (temp == nullptr) {
      return ERR_CORE_CONF;
    }
    globalConf.pluginDir = std::string(temp);
  } else if (strcasecmp(conf->key, "basedir") == 0) {
    char* temp = MonConfigGetStr(conf);
    if (temp == nullptr) {
      return ERR_CORE_CONF;
    }
    globalConf.baseDir = std::string(temp);
  } else if (strcasecmp(conf->key, "pidfile") == 0) {
    char* temp = MonConfigGetStr(conf);
    if (temp == nullptr) {
      return ERR_CORE_CONF;
    }
    globalConf.pidfile = std::string(temp);
  } else if (strcasecmp(conf->key, "interval") == 0) {
    if (!MonConfigGetU64(conf, &globalConf.internal)) {
      return ERR_CORE_CONF;
    }
  } else if (strcasecmp(conf->key, "readthreads") == 0) {
    if (!MonConfigGetU64(conf, &globalConf.readThreads)) {
      return ERR_CORE_CONF;
    }
  } else if (strcasecmp(conf->key, "writethreads") == 0) {
    if (!MonConfigGetU64(conf, &globalConf.writeThreads)) {
      return ERR_CORE_CONF;
    }
  } else if (strcasecmp(conf->key, "timeout") == 0) {
    if (!MonConfigGetU64(conf, &globalConf.timeout)) {
      return ERR_CORE_CONF;
    }
  } else if (strcasecmp(conf->key, "maxreadinterval") == 0) {
    if (!MonConfigGetU64(conf, &globalConf.timeout)) {
      return ERR_CORE_CONF;
    }
  } else {
    return ERR_CORE_UNSUPPORT;
  }
  return 0;
}

int32_t MonConfile::DispatchPluginConf(const ConfigItem_t* conf) {
  // 这里去加载动态库之类的
}

void MonConfile::InitDispatchMaps() {
  // 全部改成小写 传入进来的先转换小写 然后再查找
  confMaps["plugindir"] =
      std::bind(&MonConfile::DispatchBaseConf, this, std::placeholders::_1);
  confMaps["basedir"] =
      std::bind(&MonConfile::DispatchBaseConf, this, std::placeholders::_1);
  confMaps["pidfile"] =
      std::bind(&MonConfile::DispatchBaseConf, this, std::placeholders::_1);
  confMaps["interval"] =
      std::bind(&MonConfile::DispatchBaseConf, this, std::placeholders::_1);
  confMaps["readthreads"] =
      std::bind(&MonConfile::DispatchBaseConf, this, std::placeholders::_1);
  confMaps["writethreads"] =
      std::bind(&MonConfile::DispatchBaseConf, this, std::placeholders::_1);
  confMaps["timeout"] =
      std::bind(&MonConfile::DispatchBaseConf, this, std::placeholders::_1);
  confMaps["maxreadinterval"] =
      std::bind(&MonConfile::DispatchBaseConf, this, std::placeholders::_1);
  confMaps["plugins"] = nullptr;
}

}  // namespace Monitor
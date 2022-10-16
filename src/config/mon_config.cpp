#include "export/mon_config.h"

#include <memory>

#include "mon_json.h"
#include "utils/logger.hpp"

ConfigItem_t* ConfigParseFile(const char* file) {
  std::unique_ptr<Monitor::MonJson> ptr = std::make_unique<Monitor::MonJson>();
  ConfigItem_t* config = nullptr;
  yyjson_read_flag flg =
      YYJSON_READ_ALLOW_COMMENTS | YYJSON_READ_ALLOW_TRAILING_COMMAS;
  int32_t ret = ptr->Decode(&config, flg, file);
  if (ret != 0) {
    LOG_DEBUG("get error:{}", ptr->GetError().c_str());
    ConfigFree(config);
    return nullptr;
  }
  return config;
}

ConfigItem_t* ConfigParseBuffer(const char* buffer, uint32_t size) {
  std::unique_ptr<Monitor::MonJson> ptr = std::make_unique<Monitor::MonJson>();
  ConfigItem_t* config = nullptr;
  int32_t ret = ptr->Decode(&config, buffer, size, 0);
  if (ret != 0) {
    LOG_DEBUG("get error:{}", ptr->GetError().c_str());
    ConfigFree(config);
    return nullptr;
  }
  return config;
}

void ConfigFree(ConfigItem_t* config) {
  // int32_t ret = Monitor::MonJson::FreeItem(config);
  // if (ret != 0) {
  //   LOG_DEBUG("free config error, may be resource leak");
  // }
}
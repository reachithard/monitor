#ifndef _MON_CONFILE_H_
#define _MON_CONFILE_H_

#include <atomic>
#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>

#include "export/mon_config.h"
#include "utils/singleton.h"

namespace Monitor {
class MonConfile : Singleton<MonConfile> {
 public:
  MonConfile();

  ~MonConfile();

  int32_t Init(const std::string& file);

 protected:
  int32_t DispatchConf(const ConfigItem_t* conf);

  void InitConfMaps();

 private:
  ConfigItem_t* config = nullptr;
  std::unordered_map<std::string,
                     std::function<int32_t(const ConfigItem_t* conf)>>
      confMaps;
  std::once_flag mapFlag;
};
}  // namespace Monitor

#endif  // _MON_CONFILE_H_
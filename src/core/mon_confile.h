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
typedef struct MonGlobalConf_s {
  std::string pluginDir;
  std::string baseDir;
  std::string pidfile;
  uint64_t internal;
  uint64_t readThreads;
  uint64_t writeThreads;
  uint64_t timeout;
  uint64_t maxReadInternal;
} MonGlobalConf_t;

class MonConfile : Singleton<MonConfile> {
 public:
  MonConfile();

  ~MonConfile();

  int32_t Init(const std::string& file);

 protected:
  int32_t DispatchConf(const ConfigItem_t* conf);

  int32_t DispatchBaseConf(const ConfigItem_t* conf);

  int32_t DispatchPluginConf(const ConfigItem_t* conf);

  void InitDispatchMaps();

 private:
  ConfigItem_t* config = nullptr;
  std::unordered_map<std::string,
                     std::function<int32_t(const ConfigItem_t* conf)>>
      confMaps;
  std::once_flag mapFlag;

  // 之后改为存cache里面
  MonGlobalConf_t globalConf;
};
}  // namespace Monitor

#endif  // _MON_CONFILE_H_
#ifndef _MON_MODULE_H_
#define _MON_MODULE_H_

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "export/mon_config.h"
#include "export/mon_plugin.h"
#include "utils/singleton.h"

namespace Monitor {

typedef struct PluginCtx_s {
  uint64_t interval;
  uint64_t flushInterval;
  uint64_t flushTimeout;
} PluginCtx_t;

typedef struct ConfCallback_s {
  PluginCtx_t ctx;
  std::function<int32_t(const ConfigItem_t*)> callback;
} ConfCallback_t;

typedef struct InitCallback_s {
  PluginCtx_t ctx;
  std::function<int32_t(void*)> callback;
} InitCallback_t;

class MonModule : public Singleton<MonModule> {
 public:
  int32_t LoadPlugin(const std::string& dir, const std::string& module,
                     bool global);

  int32_t RegisterModule(const std::string& name,
                         const PluginCallbacks_t* callback);

  int32_t UnregisterModule(const std::string& name);

  int32_t DispatchConf(const std::string& plugin, const ConfigItem_t* item);

 private:
  int32_t LoadFile(const std::string& filename, bool global);

  std::unordered_map<std::string, std::unique_ptr<ConfCallback_t>> cfCallbacks;
  std::unordered_map<std::string, std::unique_ptr<InitCallback_t>>
      initCallbacks;
};
}  // namespace Monitor
#endif  // _MON_MODULE_H_
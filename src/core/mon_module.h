#ifndef _MON_MODULE_H_
#define _MON_MODULE_H_

#include <string>

#include "export/mon_plugin.h"
#include "utils/singleton.h"

namespace Monitor {
class MonModule : public Singleton<MonModule> {
 public:
  int32_t LoadPlugin(const std::string& dir, const std::string& module,
                     bool global);

  int32_t RegisterModule(const std::string& name,
                         const PluginCallbacks_t* callback);

  int32_t UnregisterModule(const std::string& name);

 private:
  int32_t LoadFile(const std::string& filename, bool global);
};
}  // namespace Monitor
#endif  // _MON_MODULE_H_
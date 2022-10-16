#ifndef _MON_CONFILE_H_
#define _MON_CONFILE_H_

#include <cstdint>
#include <functional>
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
  int32_t DispatchValue(const ConfigItem_t* conf);

  int32_t DispatchBlock(const ConfigItem_t* conf);

 private:
  ConfigItem_t* config = nullptr;
};
}  // namespace Monitor

#endif  // _MON_CONFILE_H_
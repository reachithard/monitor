#ifndef _MON_PROCESS_H_
#define _MON_PROCESS_H_

#include <asio.hpp>
#include <cstdint>

#include "export/mon_config.h"

namespace Monitor {
class MonProcess {
 public:
  MonProcess();
  ~MonProcess();

  int32_t Init(const ConfigItem_t* config);

 protected:
  int32_t Parse(const ConfigItem_t* config);

  int32_t ParseCommonConfig(const ConfigItem_t* config);

  int32_t ParseComplexConfig(const ConfigItem_t* config);

  int32_t ParseProcessConfig(const ConfigItem_t* config);

 private:
  asio::io_context context;
};
}  // namespace Monitor

#endif  // _MON_PROCESS_H_
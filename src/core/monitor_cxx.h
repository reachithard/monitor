#ifndef _MONITOR_CXX_H_
#define _MONITOR_CXX_H_

#include "export/mon_config.h"

namespace Monitor {
class MonitorCxx {
 public:
  MonitorCxx();

  ~MonitorCxx();

  int32_t Init(int32_t argc, char *argv);
};
}  // namespace Monitor
#endif  // _MONITOR_CXX_H_

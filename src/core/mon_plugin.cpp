#include "export/mon_plugin.h"

#include <cstdio>
#include <stdarg.h>

#include "mon_module.h"
#include "utils/logger.hpp"

void PluginLog(PlguinLogLevel level, const char* format, ...) {
  char msg[1024];
  va_list ap;

  va_start(ap, format);
  vsnprintf(msg, sizeof(msg), format, ap);
  msg[sizeof(msg) - 1] = '\0';
  va_end(ap);
  switch (level) {
    case LVL_TRACE:
      LOG_TRACE(msg);
      break;
    case LVL_INFO:
      LOG_INFO(msg);
      break;
    case LVL_WARN:
      LOG_WARN(msg);
      break;
    case LVL_ERROR:
      LOG_ERROR(msg);
      break;
    case LVL_FATAL:
      LOG_FATAL(msg);
      break;
    default:
      LOG_DEBUG(msg);
      break;
  }
}

// 插件主动调用这个接口 注册回调函数 往MonModule里面注册进去 供他管理
int32_t ModuleRegister(const char* name, const PluginCallbacks_t* callback) {
  LOG_DEBUG("register a module");
}
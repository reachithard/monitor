#ifndef _MON_PLUGIN_H_
#define _MON_PLUGIN_H_

#include <stdint.h>

#include "mon_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PluginCtx_s {
  char* name;  // remeber free
  uint64_t interval;
  uint64_t flushInterval;
  uint64_t flushTimeout;
} PluginCtx_t;

typedef struct PluginUser_s {
  void* data;
  void (*gc)(void*);
} PluginUser_t;

// TODO 增加更多回调
typedef struct PluginCallbacks_s {
  char* name;  // remeber free
  int32_t (*config)(const ConfigItem_t* cf);
  int32_t (*init)(PluginUser_s* cf);
  int32_t (*read)(PluginUser_s* cf);
  int32_t (*write)(PluginUser_s* cf);     // 还需要有数据
  int32_t (*shutdown)(PluginUser_s* cf);  // 还需要有数据
} PluginCallbacks_t;

// 插件主动调用这个接口 注册回调函数
int32_t ModuleRegister(const PluginCallbacks_t* callback);

// TODO 一个线程接口

// 写入数据接口

enum PlguinLogLevel {
  LVL_TRACE,
  LVL_DEBUG,
  LVL_INFO,
  LVL_WARN,
  LVL_ERROR,
  LVL_FATAL
};

void PluginLog(PlguinLogLevel level, const char* format, ...)
    __attribute__((format(printf, 2, 3)));

#define PLUGIN_TRACE(...) PluginLog(LVL_TRACE, __VA_ARGS__)
#define PLUGIN_DEBUG(...) PluginLog(LVL_DEBUG, __VA_ARGS__)
#define PLUGIN_INFO(...) PluginLog(LVL_INFO, __VA_ARGS__)
#define PLUGIN_ERROR(...) PluginLog(LVL_ERROR, __VA_ARGS__)
#define PLUGIN_FATAL(...) PluginLog(LVL_FATAL, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif  // _MON_PLUGIN_H_
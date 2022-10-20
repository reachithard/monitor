#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include "export/mon_config.h"
#include "export/mon_config_utils.h"
#include "export/mon_plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

// 插件实现该接口
void PluginRegister();

#ifdef __cplusplus
}
#endif

#endif  // _PLUGIN_H_
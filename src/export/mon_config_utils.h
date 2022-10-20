#ifndef _MON_CONFIG_UTILS_H_
#define _MON_CONFIG_UTILS_H_

#include "mon_config.h"

#ifdef __cplusplus
extern "C" {
#endif

bool MonConfigIsStr(const ConfigItem_t* config);

bool MonConfigIsU64(const ConfigItem_t* config);

bool MonConfigIsI64(const ConfigItem_t* config);

bool MonConfigIsF64(const ConfigItem_t* config);

bool MonConfigIsBool(const ConfigItem_t* config);

bool MonConfigIsNull(const ConfigItem_t* config);

char* MonConfigGetStr(const ConfigItem_t* config);

bool MonConfigGetU64(const ConfigItem_t* config, uint64_t* u64);

bool MonConfigGetI64(const ConfigItem_t* config, int64_t* i64);

bool MonConfigGetF64(const ConfigItem_t* config, double* f64);

bool MonConfigGetBool(const ConfigItem_t* config, bool* bo);

char* MonConfigGetNull(const ConfigItem_t* config);

#ifdef __cplusplus
}
#endif

#endif  // _MON_CONFIG_UTILS_H_
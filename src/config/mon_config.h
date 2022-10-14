#ifndef _MON_CONFIG_H_
#define _MON_CONFIG_H_

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// CONFIG_NULL对应json的空类型
enum ConfigValueType { CONFIG_STRING, CONFIG_NUMBER, CONFIG_BOOL, CONFIG_NULL };

typedef struct MonConfigValue_s {
  union {
    /* data */
    char *str;   // 对应json的字符串类型
    double num;  // 对应json的数值类型
    bool bo;     // 对应json的bool类型
  } value;
  ConfigValueType type;
} MonConfigValue_t;

typedef struct ConfigItem_s {
  char *key;
  MonConfigValue_t *values;
  uint32_t valuesCnt;

  struct ConfigItem_s *children;
  uint32_t childCnt;
} ConfigItem_t;

ConfigItem_t *ConfigParseFile(const char *file);

ConfigItem_t *ConfigParseBuffer(const char *file);

void ConfigFree(ConfigItem_t *config);

#ifdef __cplusplus
}
#endif

#endif  // _MON_CONFIG_H_
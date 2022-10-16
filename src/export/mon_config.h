#ifndef _MON_CONFIG_H_
#define _MON_CONFIG_H_

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// CONFIG_NULL对应json的空类型
enum ConfigValueType {
  CONFIG_DEFAULT,
  CONFIG_STRING,
  CONFIG_U64,
  CONFIG_I64,
  CONFIG_F64,
  CONFIG_BOOL,
  CONFIG_NULL
};

typedef struct ConfigValue_s {
  union {
    /* data */
    char *str;  // 对应json的字符串类型
    uint64_t u64;
    int64_t i64;
    double f64;
    bool bo;  // 对应json的bool类型
  } value;
  ConfigValueType type;
} ConfigValue_t;

typedef struct ConfigItem_s ConfigItem_t;
struct ConfigItem_s {
  char *key;
  ConfigValue_t *values;
  uint32_t valuesCnt;

  ConfigItem_t *children;
  uint32_t childCnt;
};

ConfigItem_t *ConfigParseFile(const char *file);

ConfigItem_t *ConfigParseBuffer(const char *buffer, uint64_t size);

void ConfigFree(ConfigItem_t *config);

// TODO 更多的config的接口

#ifdef __cplusplus
}
#endif

#endif  // _MON_CONFIG_H_
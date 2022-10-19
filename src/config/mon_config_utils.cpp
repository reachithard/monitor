#include "export/mon_config_utils.h"

static bool MonConfigIsType(const ConfigItem_t* config, ConfigValueType type) {
  if (config == nullptr || config->itemType != CONFIG_VALUE ||
      config->valuesCnt != 1 || config->values->type != type) {
    return false;
  }

  return true;
}

bool MonConfigIsStr(const ConfigItem_t* config) {
  return MonConfigIsType(config, CONFIG_STRING);
}

bool MonConfigIsU64(const ConfigItem_t* config) {
  return MonConfigIsType(config, CONFIG_U64);
}

bool MonConfigIsI64(const ConfigItem_t* config) {
  return MonConfigIsType(config, CONFIG_I64);
}

bool MonConfigIsF64(const ConfigItem_t* config) {
  return MonConfigIsType(config, CONFIG_F64);
}

bool MonConfigIsBool(const ConfigItem_t* config) {
  return MonConfigIsType(config, CONFIG_BOOL);
}

bool MonConfigIsNull(const ConfigItem_t* config) {
  return MonConfigIsType(config, CONFIG_NULL);
}

char* MonConfigGetStr(const ConfigItem_t* config) {
  if (MonConfigIsStr(config)) {
    return config->values[0].value.str;
  }
  return nullptr;
}

bool MonConfigGetU64(const ConfigItem_t* config, uint64_t* u64) {
  if (MonConfigIsU64(config)) {
    *u64 = config->values[0].value.u64;
    return true;
  }
  return false;
}

bool MonConfigGetI64(const ConfigItem_t* config, int64_t* i64) {
  if (MonConfigIsI64(config)) {
    *i64 = config->values[0].value.i64;
    return true;
  }
  return false;
}

bool MonConfigGetF64(const ConfigItem_t* config, double* f64) {
  if (MonConfigIsF64(config)) {
    *f64 = config->values[0].value.f64;
    return true;
  }
  return false;
}

bool MonConfigGetBool(const ConfigItem_t* config, bool* bo) {
  if (MonConfigIsBool(config)) {
    *bo = config->values[0].value.bo;
    return true;
  }
  return false;
}

char* MonConfigGetNull(const ConfigItem_t* config) {
  if (MonConfigIsNull(config)) {
    return config->values[0].value.str;
  }
  return nullptr;
}
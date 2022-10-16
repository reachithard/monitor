#include "mon_json.h"

#include <cstdio>
#include <string>
#include <vector>

#include "export/monitor_errors.h"
#include "utils/logger.hpp"

namespace Monitor {
MonJson::MonJson() : doc(nullptr) {}

MonJson::~MonJson() {
  if (doc) {
    yyjson_doc_free(doc);
    doc = nullptr;
  }
}

int32_t MonJson::Decode(ConfigItem_t** item, uint32_t flag, const char* file) {
  if (item == nullptr || file == nullptr) {
    return ERR_CONFIG_PARAM;
  }

  int32_t ret = 0;
  ret = ParseJsonFile(flag, file);
  if (ret != 0) {
    return ret;
  }

  ret = ValidJsonFile();
  if (ret != 0) {
    return ret;
  }

  ConfigItem_t* config = (ConfigItem_t*)malloc(sizeof(ConfigItem_t));
  memset(config, 0, sizeof(ConfigItem_t));
  config->key = strdup("root");  // 第一个是root
  yyjson_val* root = yyjson_doc_get_root(doc);
  if (TraverseJsonDom(config, root) != 0) {
    MonJson::FreeJson(config);
    return ERR_CONFIG_TRANSLATE;
  }
  *item = config;
  return ret;
}

int32_t MonJson::Decode(ConfigItem_t** item, const char* buffer, uint64_t size,
                        uint32_t flag) {}

int32_t MonJson::Encode(ConfigItem_t* item, const char* file) {}

int32_t MonJson::Encode(ConfigItem_t* item, const char* buffer, uint64_t size) {
}

void MonJson::FreeJson(ConfigItem_t* item) {}

int32_t MonJson::ParseJsonFile(uint32_t flag, const char* file) {
  if (doc) {
    yyjson_doc_free(doc);
    doc = nullptr;
  }

  yyjson_read_err err;
  doc = yyjson_read_file(file, flag, nullptr, &err);
  if (doc == nullptr) {
    return ERR_CONFIG_READ;
  }
  return 0;
}

int32_t MonJson::ValidJsonFile() {
  if (doc == nullptr) {
    return ERR_CONFIG_READ;
  }

  yyjson_val* obj = yyjson_doc_get_root(doc);
  // 第一个必须为obj对象
  if (yyjson_get_type(obj) != YYJSON_TYPE_OBJ) {
    return ERR_CONFIG_FORMAT;
  }
  return 0;
}

int32_t MonJson::TraverseJsonDom(ConfigItem_t* item, yyjson_val* cur) {
  //   typedef uint8_t yyjson_type;
  // #define YYJSON_TYPE_NONE        ((uint8_t)0)        /* _____000 */
  // #define YYJSON_TYPE_RAW         ((uint8_t)1)        /* _____001 */
  // #define YYJSON_TYPE_NULL        ((uint8_t)2)        /* _____010 */
  // #define YYJSON_TYPE_BOOL        ((uint8_t)3)        /* _____011 */
  // #define YYJSON_TYPE_NUM         ((uint8_t)4)        /* _____100 */
  // #define YYJSON_TYPE_STR         ((uint8_t)5)        /* _____101 */
  // #define YYJSON_TYPE_ARR         ((uint8_t)6)        /* _____110 */
  // #define YYJSON_TYPE_OBJ         ((uint8_t)7)        /* _____111 */

  // /** Subtype of JSON value (2 bit). */
  // typedef uint8_t yyjson_subtype;
  // #define YYJSON_SUBTYPE_NONE     ((uint8_t)(0 << 3)) /* ___00___ */
  // #define YYJSON_SUBTYPE_FALSE    ((uint8_t)(0 << 3)) /* ___00___ */
  // #define YYJSON_SUBTYPE_TRUE     ((uint8_t)(1 << 3)) /* ___01___ */
  // #define YYJSON_SUBTYPE_UINT     ((uint8_t)(0 << 3)) /* ___00___ */
  // #define YYJSON_SUBTYPE_SINT     ((uint8_t)(1 << 3)) /* ___01___ */
  // #define YYJSON_SUBTYPE_REAL     ((uint8_t)(2 << 3)) /* ___10___ */
  int ret = 0;
  yyjson_obj_iter iter;
  yyjson_obj_iter_init(cur, &iter);
  yyjson_val* key = nullptr;
  yyjson_val* value = nullptr;
  // 这里利用vector的动态增长 防止重复realloc 必须保证pod类型
  std::vector<ConfigItem_t> temps;
  while ((key = yyjson_obj_iter_next(&iter))) {
    value = yyjson_obj_iter_get_val(key);
    switch (yyjson_get_type(value)) {
      case YYJSON_TYPE_BOOL: {
        TraverseJsonBool(key, value, temps);
        break;
      }
      case YYJSON_TYPE_NUM: {
        TraverseJsonNum(key, value, temps);
        break;
      }
      case YYJSON_TYPE_STR: {
        TraverseJsonString(key, value, temps);
        break;
      }
      case YYJSON_TYPE_ARR: {
        LOG_DEBUG("get YYJSON_TYPE_ARR:{}", yyjson_get_str(key));
        // array 里面不能有array 以及不能有数组等多维数组
        TraverseJsonArray(key, value, temps);
        break;
      }
      case YYJSON_TYPE_OBJ: {
        LOG_DEBUG("get YYJSON_TYPE_OBJ:{} ", yyjson_get_str(key));
        ConfigItem_t temp;
        memset(&temp, 0, sizeof(ConfigItem_t));
        temp.key = strdup(yyjson_get_str(key));

        TraverseJsonDom(&temp, value);
        temps.push_back(temp);
        break;
      }
      default: {
        TraverseJsonOther(key, value, temps);
        break;
      }
    }

    if (!temps.empty()) {
      ConfigItem_t* children =
          (ConfigItem_t*)malloc(sizeof(ConfigItem_t) * temps.size());
      memset(children, 0, sizeof(ConfigItem_t) * temps.size());
      // pod类型这样玩
      memcpy(children, temps.data(), sizeof(ConfigItem_t) * temps.size());
      item->children = children;
      item->childCnt = temps.size();
    }
  }
  return ret;
}

int32_t MonJson::TraverseJsonBool(yyjson_val* key, yyjson_val* value,
                                  std::vector<ConfigItem_t>& temps) {
  LOG_DEBUG("get YYJSON_TYPE_BOOL:{} value:{}", yyjson_get_str(key),
            yyjson_get_bool(value));
  ConfigItem_t temp;
  memset(&temp, 0, sizeof(ConfigItem_t));
  temp.key = strdup(yyjson_get_str(key));
  temp.valuesCnt = 1;
  temp.values = (ConfigValue_t*)malloc(1 * sizeof(ConfigValue_t));
  temp.values[0].value.bo = yyjson_get_bool(value);
  temp.values[0].type = CONFIG_BOOL;
  temps.push_back(temp);
}

int32_t MonJson::TraverseJsonNum(yyjson_val* key, yyjson_val* value,
                                 std::vector<ConfigItem_t>& temps) {
  ConfigItem_t temp;
  memset(&temp, 0, sizeof(ConfigItem_t));
  temp.key = strdup(yyjson_get_str(key));
  temp.valuesCnt = 1;
  temp.values = (ConfigValue_t*)malloc(1 * sizeof(ConfigValue_t));
  switch (yyjson_get_subtype(value)) {
    case YYJSON_SUBTYPE_UINT: {
      temp.values[0].type = CONFIG_U64;
      temp.values[0].value.u64 = yyjson_get_uint(value);
      break;
    }
    case YYJSON_SUBTYPE_SINT: {
      temp.values[0].type = CONFIG_I64;
      temp.values[0].value.i64 = yyjson_get_int(value);
      break;
    }
    case YYJSON_SUBTYPE_REAL: {
      temp.values[0].type = CONFIG_I64;
      temp.values[0].value.f64 = yyjson_get_real(value);
      break;
    }
    default: {
      temp.values[0].type = CONFIG_NULL;
      temp.values[0].value.str = strdup("none");
      break;
    }
  }
  temps.push_back(temp);
}

int32_t MonJson::TraverseJsonString(yyjson_val* key, yyjson_val* value,
                                    std::vector<ConfigItem_t>& temps) {
  LOG_DEBUG("get YYJSON_TYPE_STR:{} value:{}", yyjson_get_str(key),
            yyjson_get_str(value));
  ConfigItem_t temp;
  memset(&temp, 0, sizeof(ConfigItem_t));
  temp.key = strdup(yyjson_get_str(key));
  temp.valuesCnt = 1;
  temp.values = (ConfigValue_t*)malloc(1 * sizeof(ConfigValue_t));
  temp.values[0].value.str = strdup(yyjson_get_str(value));
  temp.values[0].type = CONFIG_STRING;
  temps.push_back(temp);
}

int32_t MonJson::TraverseJsonOther(yyjson_val* key, yyjson_val* value,
                                   std::vector<ConfigItem_t>& temps) {
  ConfigItem_t temp;
  memset(&temp, 0, sizeof(ConfigItem_t));
  temp.key = strdup(yyjson_get_str(key));
  temp.valuesCnt = 1;
  temp.values = (ConfigValue_t*)malloc(1 * sizeof(ConfigValue_t));
  temp.values[0].value.str = strdup("none");
  temp.values[0].type = CONFIG_NULL;
  temps.push_back(temp);
}

int32_t MonJson::TraverseJsonArray(yyjson_val* key, yyjson_val* values,
                                   std::vector<ConfigItem_t>& temps) {
  ConfigItem_t temp;
  memset(&temp, 0, sizeof(ConfigItem_t));
  temp.key = strdup(yyjson_get_str(key));
  temp.valuesCnt = yyjson_arr_size(values);
  temp.values = (ConfigValue_t*)malloc(temp.valuesCnt * sizeof(ConfigValue_t));

  bool normal = false;
  bool complex = false;
  uint64_t idx = 0;
  uint64_t max = 0;
  yyjson_val* value = nullptr;
  std::vector<ConfigItem_t> arrayObjs;
  // TODO temp也要这样玩 防止各种类型
  yyjson_arr_foreach(values, idx, max, value) {
    switch (yyjson_get_type(value)) {
      case YYJSON_TYPE_BOOL: {
        temp.values[idx].value.bo = yyjson_get_bool(value);
        temp.values[idx].type = CONFIG_BOOL;
        normal = true;
        break;
      }
      case YYJSON_TYPE_NUM: {
        switch (yyjson_get_subtype(value)) {
          case YYJSON_SUBTYPE_UINT: {
            temp.values[idx].type = CONFIG_U64;
            temp.values[idx].value.u64 = yyjson_get_uint(value);
            break;
          }
          case YYJSON_SUBTYPE_SINT: {
            temp.values[idx].type = CONFIG_I64;
            temp.values[idx].value.i64 = yyjson_get_int(value);
            break;
          }
          case YYJSON_SUBTYPE_REAL: {
            temp.values[idx].type = CONFIG_I64;
            temp.values[idx].value.f64 = yyjson_get_real(value);
            break;
          }
          default: {
            temp.values[idx].type = CONFIG_NULL;
            temp.values[idx].value.str = strdup("none");
            break;
          }
        }
        normal = true;
        break;
      }
      case YYJSON_TYPE_STR: {
        temp.values[idx].value.str = strdup(yyjson_get_str(value));
        temp.values[idx].type = CONFIG_STRING;
        normal = true;
        break;
      }
      case YYJSON_TYPE_ARR: {
        // TODO
        LOG_ERROR("unsupport mutilple array");
        break;
      }
      case YYJSON_TYPE_OBJ: {
        // TODO 这个是要支持的
        LOG_DEBUG("get array YYJSON_TYPE_OBJ:{} ", yyjson_get_str(key));
        ConfigItem_t objs;
        memset(&objs, 0, sizeof(ConfigItem_t));
        objs.key = strdup(yyjson_get_str(key));

        TraverseJsonDom(&objs, value);
        arrayObjs.push_back(objs);
        complex = true;
        break;
      }
      default: {
        temp.values[idx].value.str = strdup("none");
        temp.values[idx].type = CONFIG_NULL;
        normal = true;
        break;
      }
    }
  }

  // 直接不支持基本类型和数组类型
  if (normal && complex) {
    // 不支持
    LOG_ERROR("unsupport this config");
  } else if (complex) {
    temp.valuesCnt = 0;
    free(temp.values);
    temp.values = nullptr;
  }

  if (!arrayObjs.empty()) {
    ConfigItem_t* children =
        (ConfigItem_t*)malloc(sizeof(ConfigItem_t) * arrayObjs.size());
    memset(children, 0, sizeof(ConfigItem_t) * arrayObjs.size());
    // pod类型这样玩
    memcpy(children, arrayObjs.data(), sizeof(ConfigItem_t) * arrayObjs.size());
    temp.children = children;
    temp.childCnt = arrayObjs.size();
  }

  temps.push_back(temp);
}
}  // namespace Monitor
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
    return MON_CONFIG_PARAM;
  }
  if (doc) {
    yyjson_doc_free(doc);
    doc = nullptr;
  }
  yyjson_read_err err;
  // TODO 如果有jemalloc需求 把nullptr进行修改
  doc = yyjson_read_file(file, flag, nullptr, &err);
  if (doc) {
    yyjson_val* obj = yyjson_doc_get_root(doc);
    // 第一个必须为obj对象
    if (yyjson_get_type(obj) != YYJSON_TYPE_OBJ) {
      errorInfo = "fist must be object";
      return MON_CONFIG_FORMAT;
    }

    ConfigItem_t* root = (ConfigItem_t*)malloc(sizeof(ConfigItem_t));
    memset(root, 0, sizeof(ConfigItem_t));
    int32_t ret = Parse(&root->children, obj, root->childCnt);
    if (ret != 0) {
      // 这里直接清空 内存回收
      MonJson::FreeItem(root);
      return ret;
    }
    *item = root;
  } else {
    errorInfo = "read error " + std::to_string(err.code) + " " +
                std::string(err.msg) +
                " at position:" + std::to_string(err.pos);
    return MON_CONFIG_PARSE;
  }

  return 0;
}

int32_t MonJson::Decode(ConfigItem_t** item, const char* buffer, uint64_t size,
                        uint32_t flag) {
  if (item == nullptr || buffer == nullptr) {
    return MON_CONFIG_PARAM;
  }

  if (doc) {
    yyjson_doc_free(doc);
    doc = nullptr;
  }

  // TODO 如果有jemalloc需求 把nullptr进行修改
  doc = yyjson_read(buffer, size, flag);
  if (doc) {
    yyjson_val* obj = yyjson_doc_get_root(doc);
    // 第一个必须为obj对象
    if (yyjson_get_type(obj) != YYJSON_TYPE_OBJ) {
      errorInfo = "fist must be object";
      return MON_CONFIG_FORMAT;
    }

    ConfigItem_t* root = (ConfigItem_t*)malloc(sizeof(ConfigItem_t));
    memset(item, 0, sizeof(ConfigItem_t));
    int32_t ret = Parse(&root->children, obj, root->childCnt);
    if (ret != 0) {
      // 这里直接清空 内存回收
      MonJson::FreeItem(root);
      return ret;
    }
    *item = root;
  } else {
    errorInfo = "read json buffer error";
    return MON_CONFIG_PARSE;
  }

  return 0;
}

int32_t MonJson::Encode(ConfigItem_t* item, const char* file) {}

int32_t MonJson::Encode(ConfigItem_t* item, const char* buffer, uint64_t size) {
}

int32_t MonJson::Parse(ConfigItem_t** item, yyjson_val* obj,
                       uint32_t& childCnt) {
  yyjson_obj_iter iter;
  yyjson_obj_iter_init(obj, &iter);
  yyjson_val* key = nullptr;
  yyjson_val* val = nullptr;
  // 这里利用vector的动态增长 防止重复realloc 必须保证pod类型
  std::vector<ConfigItem_t> temps;
  while ((key = yyjson_obj_iter_next(&iter))) {
    // #define YYJSON_TYPE_NONE        ((uint8_t)0)        /* _____000 */
    // #define YYJSON_TYPE_RAW         ((uint8_t)1)        /* _____001 */
    // #define YYJSON_TYPE_NULL        ((uint8_t)2)        /* _____010 */
    // #define YYJSON_TYPE_BOOL        ((uint8_t)3)        /* _____011 */
    // #define YYJSON_TYPE_NUM         ((uint8_t)4)        /* _____100 */
    // #define YYJSON_TYPE_STR         ((uint8_t)5)        /* _____101 */
    // #define YYJSON_TYPE_ARR         ((uint8_t)6)        /* _____110 */
    // #define YYJSON_TYPE_OBJ         ((uint8_t)7)        /* _____111 */

    val = yyjson_obj_iter_get_val(key);
    if (val == nullptr) {
      return MON_CONFIG_TRANSLATE;
    }
    switch (yyjson_get_type(val)) {
      case YYJSON_TYPE_BOOL:
        /* code */
        LOG_DEBUG("get bool:{} {}", yyjson_get_bool(key),
                  yyjson_get_type_desc(val));
        ConfigItem_t temp;
        memset(&temp, 0, sizeof(ConfigItem_t));
        temp.key = strdup(yyjson_get_str(key));
        temp.valuesCnt = 1;
        temp.values =
            (ConfigValue_t*)malloc(temp.valuesCnt * sizeof(ConfigItem_t));
        temp.values[0].type = CONFIG_BOOL;
        temp.values[0].value.bo = yyjson_get_bool(val);
        temps.push_back(temp);
        break;
      case YYJSON_TYPE_NUM: {
        /* code */
        LOG_DEBUG("get num:{} {}", yyjson_get_sint(key),
                  yyjson_get_type_desc(val));
        // #define YYJSON_SUBTYPE_NONE     ((uint8_t)(0 << 3)) /* ___00___ */
        // #define YYJSON_SUBTYPE_FALSE    ((uint8_t)(0 << 3)) /* ___00___ */
        // #define YYJSON_SUBTYPE_TRUE     ((uint8_t)(1 << 3)) /* ___01___ */
        // #define YYJSON_SUBTYPE_UINT     ((uint8_t)(0 << 3)) /* ___00___ */
        // #define YYJSON_SUBTYPE_SINT     ((uint8_t)(1 << 3)) /* ___01___ */
        // #define YYJSON_SUBTYPE_REAL     ((uint8_t)(2 << 3)) /* ___10___ */
        ConfigItem_t temp;
        memset(&temp, 0, sizeof(ConfigItem_t));
        temp.key = strdup(yyjson_get_str(key));
        temp.valuesCnt = 1;
        temp.values =
            (ConfigValue_t*)malloc(temp.valuesCnt * sizeof(ConfigValue_t));
        switch (yyjson_get_subtype(val)) {
          case YYJSON_SUBTYPE_UINT: {
            temp.values[0].type = CONFIG_U64;
            temp.values[0].value.u64 = yyjson_get_uint(val);
            break;
          }
          case YYJSON_SUBTYPE_SINT: {
            temp.values[0].type = CONFIG_I64;
            temp.values[0].value.i64 = yyjson_get_int(val);
            break;
          }
          case YYJSON_SUBTYPE_REAL: {
            temp.values[0].type = CONFIG_I64;
            temp.values[0].value.f64 = yyjson_get_real(val);
            break;
          }
          default: {
            temp.values[0].type = CONFIG_NULL;
            temp.values[0].value.str = strdup("none");
            break;
          }
        }
        temps.push_back(temp);
        break;
      }

      case YYJSON_TYPE_STR: {
        /* code */
        LOG_DEBUG("get str:{} {}", yyjson_get_str(key),
                  yyjson_get_type_desc(val));
        ConfigItem_t temp;
        memset(&temp, 0, sizeof(ConfigItem_t));
        temp.key = strdup(yyjson_get_str(key));
        temp.valuesCnt = 1;
        temp.values =
            (ConfigValue_t*)malloc(temp.valuesCnt * sizeof(ConfigValue_t));
        temp.values[0].type = CONFIG_STRING;
        temp.values[0].value.str = strdup(yyjson_get_str(val));
        temps.push_back(temp);
        break;
      }

      case YYJSON_TYPE_ARR: {
        /* code */
        // 需要先取一个 判断类型 因为基本类型和对象类型没放一起
        yyjson_val* arr = yyjson_arr_get_first(val);

        if (arr != nullptr) {
          yyjson_type type = yyjson_get_type(arr);
          yyjson_subtype subtype = yyjson_get_subtype(arr);
          uint64_t idx = 0;
          uint64_t max = 0;

          temp.key = strdup(yyjson_get_str(key));
          if (type != YYJSON_TYPE_OBJ) {
            ConfigItem_t temp;
            memset(&temp, 0, sizeof(ConfigItem_t));
            temp.valuesCnt = yyjson_arr_size(val);
            temp.values =
                (ConfigValue_t*)malloc(temp.valuesCnt * sizeof(ConfigValue_t));
            yyjson_arr_foreach(val, idx, max, obj) {
              switch (yyjson_get_type(obj)) {
                case YYJSON_TYPE_BOOL: {
                  temp.values[idx].type = CONFIG_BOOL;
                  temp.values[idx].value.bo = yyjson_get_bool(obj);
                  break;
                }
                case YYJSON_TYPE_NUM: {
                  switch (yyjson_get_subtype(obj)) {
                    case YYJSON_SUBTYPE_UINT: {
                      temp.values[idx].type = CONFIG_U64;
                      temp.values[idx].value.u64 = yyjson_get_uint(obj);
                      break;
                    }
                    case YYJSON_SUBTYPE_SINT: {
                      temp.values[idx].type = CONFIG_I64;
                      temp.values[idx].value.i64 = yyjson_get_int(obj);
                      break;
                    }
                    case YYJSON_SUBTYPE_REAL: {
                      temp.values[idx].type = CONFIG_I64;
                      temp.values[idx].value.f64 = yyjson_get_real(obj);
                      break;
                    }
                    default: {
                      temp.values[idx].type = CONFIG_NULL;
                      temp.values[idx].value.str = strdup("none");
                      break;
                    }
                  }
                  break;
                }
                case YYJSON_TYPE_STR: {
                  temp.values[idx].type = CONFIG_STRING;
                  temp.values[idx].value.str = strdup(yyjson_get_str(obj));
                  break;
                }
                case YYJSON_TYPE_ARR: {
                  LOG_DEBUG("not support this format, so sorry");
                  return MON_CONFIG_TRANSLATE;
                }
                case YYJSON_TYPE_OBJ: {
                  LOG_DEBUG("not support this format, so sorry");
                  return MON_CONFIG_TRANSLATE;
                }
                default:
                  temp.values[idx].type = CONFIG_NULL;
                  temp.values[idx].value.str = strdup("null");
                  break;
              }
            }
            temps.push_back(temp);
          } else {
            ConfigItem_t temp;
            memset(&temp, 0, sizeof(ConfigItem_t));
            temp.valuesCnt = 0;
            temp.childCnt = yyjson_arr_size(val);
            temp.children =
                (ConfigItem_t*)malloc(temp.childCnt * sizeof(ConfigItem_t));
            yyjson_arr_foreach(val, idx, max, obj) {
              LOG_DEBUG("get obj:{}", yyjson_get_type_desc(obj));
              temp.key = strdup(yyjson_get_str(key));
              temp.valuesCnt = 0;
              if (Parse(&temp.children + idx, obj,
                        (temp.children + idx)->childCnt) != 0) {
                LOG_ERROR("change json to native error");
                return MON_CONFIG_TRANSLATE;
              }
            }
            temps.push_back(temp);
          }
        } else {
          LOG_DEBUG("get a error form yyjson array");
          return MON_CONFIG_TRANSLATE;
        }
        temps.push_back(temp);
        break;
      }
      case YYJSON_TYPE_OBJ: {
        /* code */
        ConfigItem_t temp;
        memset(&temp, 0, sizeof(ConfigItem_t));
        LOG_DEBUG("get obj:{}", yyjson_get_type_desc(val));
        temp.key = strdup(yyjson_get_str(key));
        temp.valuesCnt = 0;
        if (Parse(&temp.children, val, temp.childCnt) != 0) {
          LOG_ERROR("change json to native error");
          return MON_CONFIG_TRANSLATE;
        }
        temps.push_back(temp);
        break;
      }
      default: {
        LOG_DEBUG("get null:{} {}", yyjson_get_str(key),
                  yyjson_get_type_desc(val));
        ConfigItem_t temp;
        memset(&temp, 0, sizeof(ConfigItem_t));
        temp.key = strdup(yyjson_get_str(key));
        temp.valuesCnt = 1;
        temp.values = (ConfigValue_t*)malloc(temp.valuesCnt);
        temp.values[0].type = CONFIG_NULL;
        temp.values[0].value.str = strdup("null");
        temps.push_back(temp);
        break;
      }
    }
  }

  // 映射建立完毕 必须保证是pod类型 才能这样操作
  if (!temps.empty()) {
    ConfigItem_t* config =
        (ConfigItem_t*)malloc(sizeof(ConfigItem_t) * temps.size());
    memset(config, 0, sizeof(ConfigItem_t) * temps.size());
    // pod类型这样玩
    memcpy(config, temps.data(), sizeof(ConfigItem_t) * temps.size());
    *item = config;  // 传出去
    childCnt = temps.size();
  }
  return 0;
}

int32_t MonJson::FreeItem(ConfigItem_t* item) {
  if (item == nullptr) {
    return MON_CONFIG_PARAM;
  }

  // TODO 代码优化
  int32_t ret = 0;
  ret = MonJson::FreeItemImpl(item);
  free(item);
  return 0;
}

int32_t MonJson::FreeItemImpl(ConfigItem_t* item) {
  if (item == nullptr) {
    return MON_CONFIG_PARAM;
  }

  if (item && item->values) {
    switch (item->values->type) {
      case CONFIG_DEFAULT: {
        if (item->key) {
          LOG_DEBUG("key:root default value");
        }
        break;
      }
      case CONFIG_STRING: {
        if (item->key) {
          LOG_DEBUG("key:{} string value:{}", item->key,
                    item->values[0].value.str);
        }

        for (uint32_t idx = 0; idx < item->valuesCnt; idx++) {
          if (item->values[idx].value.str) {
            free(item->values[idx].value.str);
            item->values[idx].value.str = nullptr;
          }
        }

        if (item->values) {
          free(item->values);
          item->values = nullptr;
          item->valuesCnt = 0;
        }
        break;
      }
      case CONFIG_NULL: {
        if (item->key) {
          LOG_DEBUG("key:{} null value:{}", item->key,
                    item->values[0].value.str);
        }

        for (uint32_t idx = 0; idx < item->valuesCnt; idx++) {
          if (item->values[idx].value.str) {
            free(item->values[idx].value.str);
            item->values[idx].value.str = nullptr;
          }
        }

        if (item->values) {
          free(item->values);
          item->values = nullptr;
          item->valuesCnt = 0;
        }
        break;
      }

      default: {
        if (item->values) {
          if (item->key) {
            LOG_DEBUG("key:{} normal value", item->key);
          }
          free(item->values);
          item->values = nullptr;
          item->valuesCnt = 0;
        }
        break;
      }
    }
  }

  if (item->key) {
    free(item->key);
    item->key = nullptr;
  }

  for (uint32_t idx = 0; idx < item->childCnt; idx++) {
    FreeItemImpl(&item->children[idx]);
  }

  if (item->children) {
    free(item->children);
    item->children = nullptr;
    item->childCnt = 0;
  }

  return 0;
}
}  // namespace Monitor
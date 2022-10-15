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
    ConfigItem_t temp;
    memset(&temp, 0, sizeof(ConfigItem_t));
    val = yyjson_obj_iter_get_val(key);
    switch (yyjson_get_type(val)) {
      case YYJSON_TYPE_BOOL:
        /* code */
        LOG_DEBUG("get bool:{} {}", yyjson_get_bool(key),
                  yyjson_get_type_desc(val));
        temp.key = strdup(yyjson_get_str(key));
        temp.valuesCnt = 1;
        temp.values = (ConfigValue_t*)malloc(temp.valuesCnt);
        temp.values[0].type = CONFIG_BOOL;
        temp.values[0].value.bo = yyjson_get_bool(val);
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
        switch (yyjson_get_subtype(val)) {
          case YYJSON_SUBTYPE_UINT: {
            /* code */
            temp.key = strdup(yyjson_get_str(key));
            temp.valuesCnt = 1;
            temp.values =
                (ConfigValue_t*)malloc(temp.valuesCnt * sizeof(ConfigValue_t));
            temp.values[0].type = CONFIG_U64;
            temp.values[0].value.u64 = yyjson_get_uint(val);
            break;
          }
          case YYJSON_SUBTYPE_SINT: {
            /* code */
            temp.key = strdup(yyjson_get_str(key));
            temp.valuesCnt = 1;
            temp.values =
                (ConfigValue_t*)malloc(temp.valuesCnt * sizeof(ConfigValue_t));
            temp.values[0].type = CONFIG_I64;
            temp.values[0].value.i64 = yyjson_get_int(val);
            break;
          }
          case YYJSON_SUBTYPE_REAL: {
            /* code */
            temp.key = strdup(yyjson_get_str(key));
            temp.valuesCnt = 1;
            temp.values =
                (ConfigValue_t*)malloc(temp.valuesCnt * sizeof(ConfigValue_t));
            temp.values[0].type = CONFIG_I64;
            temp.values[0].value.f64 = yyjson_get_real(val);
            break;
          }
          default: {
            temp.key = strdup(yyjson_get_str(key));
            temp.valuesCnt = 1;
            temp.values =
                (ConfigValue_t*)malloc(temp.valuesCnt * sizeof(ConfigValue_t));
            temp.values[0].type = CONFIG_NULL;
            temp.values[0].value.str = strdup("null");
            break;
          }
        }
        break;
      }

      case YYJSON_TYPE_STR: {
        /* code */
        LOG_DEBUG("get str:{} {}", yyjson_get_str(key),
                  yyjson_get_type_desc(val));
        temp.key = strdup(yyjson_get_str(key));
        temp.valuesCnt = 1;
        temp.values =
            (ConfigValue_t*)malloc(temp.valuesCnt * sizeof(ConfigValue_t));
        temp.values[0].type = CONFIG_STRING;
        temp.values[0].value.str = strdup(yyjson_get_str(val));
        break;
      }

      case YYJSON_TYPE_ARR: {
        /* code */
        // 进行数组迭代
        size_t idx, max;
        yyjson_arr_foreach(key, idx, max, val) {
          //   printf("hit%d: %d\n", (int)idx, (int)yyjson_get_int(hit));
          // TODO 这里还得考虑嵌套数组的情况
        }
        break;
      }
      case YYJSON_TYPE_OBJ: {
        /* code */
        LOG_DEBUG("get obj:{}", yyjson_get_type_desc(val));
        temp.key = strdup(yyjson_get_str(key));
        temp.valuesCnt = 0;
        if (Parse(&temp.children, val, temp.childCnt) != 0) {
          LOG_ERROR("change json to native error");
          return MON_CONFIG_TRANSLATE;
        }
        break;
      }
      default: {
        LOG_DEBUG("get null:{} {}", yyjson_get_str(key),
                  yyjson_get_type_desc(val));
        temp.key = strdup(yyjson_get_str(key));
        temp.valuesCnt = 1;
        temp.values = (ConfigValue_t*)malloc(temp.valuesCnt);
        temp.values[0].type = CONFIG_NULL;
        temp.values[0].value.str = strdup("null");
        break;
      }
    }
    temps.push_back(temp);
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
  int32_t ret = 0;
  ret = MonJson::FreeItemImpl(item);
  free(item);
  return 0;
}

int32_t MonJson::FreeItemImpl(ConfigItem_t* item) {
  if (item == nullptr) {
    return MON_CONFIG_PARAM;
  }

  if (item->values) {
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
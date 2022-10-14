#include "mon_json.h"

#include <cstdio>
#include <string>

#include "../export/monitor_errors.h"
#include "../utils/logger.hpp"

namespace Monintor {
MonJson::MonJson() {}

MonJson::~MonJson() {
  if (doc) {
    yyjson_doc_free(doc);
    doc = nullptr;
  }
}

int32_t MonJson::Decode(ConfigItem_t** item, uint32_t flag, const char* file) {
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
    Parse(obj);
  } else {
    errorInfo = "read error " + std::to_string(err.code) + " " +
                std::string(err.msg) +
                " at position:" + std::to_string(err.pos);
    return MON_CONFIG_PARSE;
  }
  return 0;
}

int32_t MonJson::Decode(ConfigItem_t** item, const char* buffer,
                        uint64_t size) {}

int32_t MonJson::Encode(ConfigItem_t* item, const char* file) {}

int32_t MonJson::Encode(ConfigItem_t** item, const char* buffer,
                        uint64_t size) {}

int32_t MonJson::Parse(yyjson_val* obj) {
  yyjson_obj_iter iter;
  yyjson_obj_iter_init(obj, &iter);
  yyjson_val *key, *val;
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
    switch (yyjson_get_type(key)) {
      case YYJSON_TYPE_RAW:
        /* code */
        LOG_DEBUG("get raw:{} {}", yyjson_get_raw(key),
                  yyjson_get_type_desc(val));
        break;
      case YYJSON_TYPE_BOOL:
        /* code */
        LOG_DEBUG("get bool:{} {}", yyjson_get_bool(key),
                  yyjson_get_type_desc(val));
        break;
      case YYJSON_TYPE_NUM:
        /* code */
        LOG_DEBUG("get num:{} {}", yyjson_get_sint(key),
                  yyjson_get_type_desc(val));
        break;
      case YYJSON_TYPE_STR:
        /* code */
        LOG_DEBUG("get str:{} {}", yyjson_get_str(key),
                  yyjson_get_type_desc(val));
        break;
      case YYJSON_TYPE_ARR: {
        /* code */
        // 进行数组迭代
        size_t idx, max;
        yyjson_val* hit;
        yyjson_arr_foreach(key, idx, max, val) {
          //   printf("hit%d: %d\n", (int)idx, (int)yyjson_get_int(hit));
        }
        break;
      }
      case YYJSON_TYPE_OBJ:
        /* code */
        LOG_DEBUG("get obj:{}", yyjson_get_type_desc(val));
        if (Parse(key) != 0) {
          return MON_CONFIG_TRANSLATE;
        }
        break;
      default:
        // 这里全是默认null
        break;
    }
  }
  return 0;
}
}  // namespace Monintor
#include "mon_json.h"

#include <cstdio>
#include <string>
#include <vector>

#include "export/monitor_errors.h"
#include "utils/logger.hpp"

namespace Monitor {
MonJson::MonJson() : doc(nullptr), dom(nullptr) {}

MonJson::~MonJson() {
  if (doc) {
    yyjson_doc_free(doc);
    doc = nullptr;
  }

  if (dom) {
    yyjson_mut_doc_free(dom);
    dom = nullptr;
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
                        uint32_t flag) {
  if (item == nullptr || buffer == nullptr) {
    return ERR_CONFIG_PARAM;
  }

  int32_t ret = 0;
  ret = ParseJsonFile(flag, buffer, size);
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

int32_t MonJson::Encode(ConfigItem_t* item, const char* file) {
  if (item == nullptr || file == nullptr) {
    return ERR_CONFIG_PARAM;
  }

  if (dom != nullptr) {
    yyjson_mut_doc_free(dom);
    dom = nullptr;
  }

  dom = yyjson_mut_doc_new(NULL);
  yyjson_mut_val* root = yyjson_mut_obj(dom);
  yyjson_mut_doc_set_root(dom, root);

  WriteJsonDom(item, root);
  // To string, minified
  yyjson_write_flag flg = YYJSON_WRITE_PRETTY | YYJSON_WRITE_ESCAPE_UNICODE;
  yyjson_write_err err;
  yyjson_mut_write_file(file, dom, flg, NULL, &err);
  if (err.code) {
    LOG_ERROR("yyjson write error:{} msg:{}", err.code, err.msg);
    return ERR_CONFIG_SERIAL;
  }
  return 0;
}

int32_t MonJson::Encode(ConfigItem_t* item, const char* buffer, uint64_t size) {
  if (item == nullptr || buffer == nullptr) {
    return ERR_CONFIG_PARAM;
  }

  if (dom != nullptr) {
    yyjson_mut_doc_free(dom);
    dom = nullptr;
  }

  dom = yyjson_mut_doc_new(NULL);
  yyjson_mut_val* root = yyjson_mut_obj(dom);
  yyjson_mut_doc_set_root(dom, root);

  WriteJsonDom(item, root);
  // To string, minified
  const char* json = yyjson_mut_write(dom, 0, NULL);
  if (json) {
    LOG_DEBUG("get json:{}", json);
    free((void*)json);
  }
  return 0;
}

// int32_t MonJson::WriteJsonDom(ConfigItem_t* cur, yyjson_mut_val* node) {
//   if (cur == nullptr || node == nullptr) {
//     return 0;
//   }
//   if (cur->childCnt != 0 && cur->valuesCnt != 0) {
//     return ERR_CONFIG_FORMAT;
//   }

//   if (cur->childCnt != 0) {
//     // 对象类型 但对象还是数组 要看一下
//     bool value = false;
//     bool block = false;
//     for (uint32_t idx = 0; idx < cur->childCnt; idx++) {
//       if (cur->children[idx].childCnt == 0) {
//         value = true;
//       } else {
//         block = true;
//       }
//     }

//     if (value && block) {
//       // 普通类型 块类型混杂
//       for (uint32_t idx = 0; idx < cur->childCnt; idx++) {
//         if (cur->children[idx].childCnt == 0) {
//           WriteJsonDom(&cur->children[idx], node);
//         } else {
//           yyjson_mut_val* arr = yyjson_mut_arr(dom);
//           yyjson_mut_obj_add_val(dom, node, cur->children->key, arr);
//           for (uint32_t childIdx = 0; childIdx < cur->children[idx].childCnt;
//                childIdx++) {
//             yyjson_mut_val* obj = yyjson_mut_obj(dom);
//             yyjson_mut_arr_insert(arr, obj, childIdx);
//             WriteJsonDom(&cur->children[idx].children[childIdx], obj);
//           }
//         }
//       }
//     } else if (value && !block) {
//       // 只有普通类型
//       yyjson_mut_val* obj = yyjson_mut_obj(dom);
//       yyjson_mut_obj_add_val(dom, node, cur->key, obj);
//       for (uint32_t idx = 0; idx < cur->childCnt; idx++) {
//         WriteJsonDom(&cur->children[idx], obj);
//       }
//     } else {
//       // 只有块类型
//       yyjson_mut_val* arr = yyjson_mut_arr(dom);
//       yyjson_mut_obj_add_val(dom, node, cur->key, arr);
//       for (uint32_t idx = 0; idx < cur->childCnt; idx++) {
//         yyjson_mut_val* obj = yyjson_mut_obj(dom);
//         yyjson_mut_arr_insert(arr, obj, idx);
//         WriteJsonDom(&cur->children[idx], obj);
//       }
//     }
//   }

//   if (cur->valuesCnt != 0) {
//     // 数组或者基本类型
//     if (cur->valuesCnt > 1) {
//       // 数组类型
//       // // CONFIG_NULL对应json的空类型
//       // enum ConfigValueType {
//       //   CONFIG_DEFAULT,
//       //   CONFIG_STRING,
//       //   CONFIG_U64,
//       //   CONFIG_I64,
//       //   CONFIG_F64,
//       //   CONFIG_BOOL,
//       //   CONFIG_NULL
//       // };

//       yyjson_mut_val* arr = yyjson_mut_arr(dom);
//       yyjson_mut_obj_add_val(dom, node, cur->key, arr);
//       for (uint32_t idx = 0; idx < cur->valuesCnt; idx++) {
//         switch (cur->values[idx].type) {
//           case CONFIG_STRING: {
//             yyjson_mut_val* str =
//                 yyjson_mut_str(dom, cur->values[idx].value.str);
//             yyjson_mut_arr_insert(arr, str, idx);
//             break;
//           }
//           case CONFIG_U64: {
//             yyjson_mut_val* u64 =
//                 yyjson_mut_uint(dom, cur->values[idx].value.u64);
//             yyjson_mut_arr_insert(arr, u64, idx);
//             break;
//           }
//           case CONFIG_I64: {
//             yyjson_mut_val* i64 =
//                 yyjson_mut_int(dom, cur->values[idx].value.i64);
//             yyjson_mut_arr_insert(arr, i64, idx);
//             break;
//           }
//           case CONFIG_F64: {
//             yyjson_mut_val* f64 =
//                 yyjson_mut_real(dom, cur->values[idx].value.f64);
//             yyjson_mut_arr_insert(arr, f64, idx);
//             break;
//           }
//           case CONFIG_BOOL: {
//             yyjson_mut_val* bo =
//                 yyjson_mut_bool(dom, cur->values[idx].value.bo);
//             yyjson_mut_arr_insert(arr, bo, idx);
//             break;
//           }
//           default:
//             yyjson_mut_val* none = yyjson_mut_null(dom);
//             yyjson_mut_arr_insert(arr, none, idx);
//             break;
//         }
//       }
//     } else {
//       // 基本类型
//       switch (cur->values[0].type) {
//         case CONFIG_STRING: {
//           yyjson_mut_obj_add_str(dom, node, cur->key,
//           cur->values[0].value.str); break;
//         }
//         case CONFIG_U64: {
//           yyjson_mut_obj_add_uint(dom, node, cur->key,
//                                   cur->values[0].value.u64);
//           break;
//         }
//         case CONFIG_I64: {
//           yyjson_mut_obj_add_int(dom, node, cur->key,
//           cur->values[0].value.i64); break;
//         }
//         case CONFIG_F64: {
//           yyjson_mut_obj_add_real(dom, node, cur->key,
//                                   cur->values[0].value.f64);
//           break;
//         }
//         case CONFIG_BOOL: {
//           yyjson_mut_obj_add_bool(dom, node, cur->key,
//           cur->values[0].value.bo); break;
//         }
//         default:
//           yyjson_mut_obj_add_null(dom, node, cur->key);
//           break;
//       }
//     }
//   }
// }

void MonJson::FreeJsonNormal(ConfigItem_t* item) {
  // key需要这边释放 因为block并不会去递归到具体节点
  // 对于item进行判断 永远不可能出现既有object 又有value的数组
  if (item == nullptr) {
    return;
  }
  if (item->childCnt != 0 && item->valuesCnt != 0) {
    LOG_ERROR("get a error format");
    return;
  }

  if (item->key) {
    LOG_DEBUG("free object:{}", item->key);
    free(item->key);
    item->key = nullptr;
  }

  if (item->valuesCnt != 0) {
    // 说明是数组 如果是字符以及null类型 则需要进行内存释放 倒序遍历
    for (uint32_t idx = 0; idx < item->valuesCnt; idx++) {
      switch (item->values[idx].type) {
        case CONFIG_STRING:
          if (item->values[idx].value.str) {
            free(item->values[idx].value.str);
            item->values[idx].value.str = nullptr;
          }
          break;
        case CONFIG_NULL:
          if (item->values[idx].value.str) {
            free(item->values[idx].value.str);
            item->values[idx].value.str = nullptr;
          }
          break;
        default:
          break;
      }
    }
    if (item->values) {
      free(item->values);
      item->values = nullptr;
      item->valuesCnt = 0;
    }
  }

  if (item->childCnt != 0) {
    // 说明是数组 如果是字符以及null类型 则需要进行内存释放
    for (uint32_t idx = 0; idx < item->childCnt; idx++) {
      FreeJsonNormal(&item->children[idx]);
    }
  }
}

void MonJson::FreeJsonBlock(ConfigItem_t* item) {
  if (item == nullptr) {
    return;
  }

  if (item->childCnt != 0) {
    uint32_t idx = 0;
    for (; idx < item->childCnt; idx++) {
      FreeJsonBlock(&item->children[idx]);
    }

    if (item->children != nullptr && idx == item->childCnt) {
      LOG_DEBUG("free block");
      printf("%p\n", item->children);
      free(item->children);
      item->childCnt = 0;
    }
  }
}

void MonJson::FreeJson(ConfigItem_t* item) {
  FreeJsonNormal(item);
  FreeJsonBlock(item);
  free(item);
}

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

int32_t MonJson::ParseJsonFile(uint32_t flag, const char* buffer,
                               uint64_t size) {
  if (doc) {
    yyjson_doc_free(doc);
    doc = nullptr;
  }

  yyjson_read_err err;
  doc = yyjson_read(buffer, size, flag);
  if (doc == nullptr) {
    return ERR_CONFIG_READ;
  }
  return 0;
}

int32_t MonJson::TraverseJsonDom(ConfigItem_t* item, yyjson_val* cur) {
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
        temp.itemType = CONFIG_OBJECT;
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
  }
  if (!temps.empty()) {
    LOG_DEBUG("malloc size:{}", temps.size());
    ConfigItem_t* children =
        (ConfigItem_t*)malloc(sizeof(ConfigItem_t) * temps.size());
    memset(children, 0, sizeof(ConfigItem_t) * temps.size());
    // pod类型这样玩
    memcpy(children, temps.data(), sizeof(ConfigItem_t) * temps.size());
    item->children = children;
    item->childCnt = temps.size();
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
  // temp也要这样玩 防止各种类型
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
        // 这个是要支持的
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
    temp.itemType = CONFIG_OBJECT_ARRAY;
  } else {
    temp.itemType = CONFIG_ARRAY;
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
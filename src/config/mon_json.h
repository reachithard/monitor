#ifndef _MON_JSON_H_
#define _MON_JSON_H_

#include <string>
#include <vector>

#include "export/mon_config.h"
#include "yyjson.h"

namespace Monitor {
class MonJson {
 public:
  MonJson();
  ~MonJson();

  int32_t Decode(ConfigItem_t** item, uint32_t flag, const char* file);

  int32_t Decode(ConfigItem_t** item, const char* buffer, uint64_t size,
                 uint32_t flag);

  int32_t Encode(ConfigItem_t* item, const char* file);

  int32_t Encode(ConfigItem_t* item, const char* buffer, uint64_t size);

  const std::string& GetError() const { return errorInfo; }

  static void FreeJson(ConfigItem_t* item);

 protected:
  int32_t ParseJsonFile(uint32_t flag, const char* file);

  int32_t ParseJsonFile(uint32_t flag, const char* buffer, uint64_t size);

  int32_t ValidJsonFile();

  int32_t TraverseJsonDom(ConfigItem_t* item, yyjson_val* cur);

  int32_t TraverseJsonBool(yyjson_val* key, yyjson_val* value,
                           std::vector<ConfigItem_t>& temps);

  int32_t TraverseJsonNum(yyjson_val* key, yyjson_val* value,
                          std::vector<ConfigItem_t>& temps);

  int32_t TraverseJsonString(yyjson_val* key, yyjson_val* value,
                             std::vector<ConfigItem_t>& temps);

  int32_t TraverseJsonOther(yyjson_val* key, yyjson_val* value,
                            std::vector<ConfigItem_t>& temps);

  int32_t TraverseJsonArray(yyjson_val* key, yyjson_val* values,
                            std::vector<ConfigItem_t>& temps);

  static void FreeJsonNormal(ConfigItem_t* item);

  static void FreeJsonBlock(ConfigItem_t* item);

  int32_t WriteJsonDom(ConfigItem_t* cur, yyjson_mut_val* node);

 private:
  yyjson_doc* doc;
  yyjson_mut_doc* dom;
  std::string errorInfo;
};
}  // namespace Monitor

#endif  // _MON_JSON_H_
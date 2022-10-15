#ifndef _MON_JSON_H_
#define _MON_JSON_H_

#include <string>

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

  static int32_t FreeItem(ConfigItem_t* item);

 protected:
  int32_t Parse(ConfigItem_t** item, yyjson_val* obj, uint32_t& childCnt);

  static int32_t FreeItemImpl(ConfigItem_t* item);

 private:
  yyjson_doc* doc;
  std::string errorInfo;
};
}  // namespace Monitor

#endif  // _MON_JSON_H_
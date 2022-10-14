#ifndef _MON_JSON_H_
#define _MON_JSON_H_

#include <string>

#include "mon_config.h"
#include "yyjson.h"

namespace Monintor {
class MonJson {
 public:
  MonJson();
  ~MonJson();

  int32_t Decode(ConfigItem_t** item, uint32_t flag, const char* file);

  int32_t Decode(ConfigItem_t** item, const char* buffer, uint64_t size);

  int32_t Encode(ConfigItem_t* item, const char* file);

  int32_t Encode(ConfigItem_t** item, const char* buffer, uint64_t size);

  const std::string& GetError() const { return errorInfo; }

 protected:
  int32_t Parse(yyjson_val* obj);

 private:
  yyjson_doc* doc;
  std::string errorInfo;
};
}  // namespace Monintor

#endif  // _MON_JSON_H_
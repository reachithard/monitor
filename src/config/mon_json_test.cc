#include "mon_json.h"

#include <memory>

#include "export/mon_config.h"
#include "export/monitor_errors.h"
#include "gtest/gtest.h"
#include "test/test_runtime.h"

namespace Monitor {
TEST(MonJsonTests, ParseFileAbnormal_1) {
  using namespace Monitor;
  yyjson_read_flag flg =
      YYJSON_READ_ALLOW_COMMENTS | YYJSON_READ_ALLOW_TRAILING_COMMAS;
  std::unique_ptr<MonJson> ptr = std::make_unique<MonJson>();
  ASSERT_EQ(ptr->Decode(nullptr, flg,
                        TEST_DEFAULT_RUN_DIR
                        "/test/resources/mon_json_parsefile.json"),
            ERR_CONFIG_PARAM)
      << ptr->GetError();
}

TEST(MonJsonTests, ParseFileAbnormal_2) {
  using namespace Monitor;
  yyjson_read_flag flg =
      YYJSON_READ_ALLOW_COMMENTS | YYJSON_READ_ALLOW_TRAILING_COMMAS;
  std::unique_ptr<MonJson> ptr = std::make_unique<MonJson>();
  ConfigItem_t* item = nullptr;
  ASSERT_EQ(ptr->Decode(&item, flg, nullptr), ERR_CONFIG_PARAM)
      << ptr->GetError();
}

TEST(MonJsonTests, ParseFile) {
  using namespace Monitor;
  yyjson_read_flag flg =
      YYJSON_READ_ALLOW_COMMENTS | YYJSON_READ_ALLOW_TRAILING_COMMAS;
  std::unique_ptr<MonJson> ptr = std::make_unique<MonJson>();
  ConfigItem_t* item = nullptr;
  ASSERT_EQ(ptr->Decode(&item, flg,
                        TEST_DEFAULT_RUN_DIR
                        "/test/resources/mon_json_parsefile.json"),
            0)
      << ptr->GetError();
  // MonJson::FreeItem(item);
}
}  // namespace Monitor
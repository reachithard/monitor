#include "mon_json.h"

#include <memory>

#include "../test/test_runtime.h"
#include "gtest/gtest.h"
#include "mon_config.h"
namespace Monintor {
TEST(MonJsonTests, ParseFile) {
  using namespace Monintor;
  yyjson_read_flag flg =
      YYJSON_READ_ALLOW_COMMENTS | YYJSON_READ_ALLOW_TRAILING_COMMAS;
  std::unique_ptr<MonJson> ptr = std::make_unique<MonJson>();
  ASSERT_EQ(ptr->Decode(nullptr, flg,
                        TEST_DEFAULT_RUN_DIR
                        "/test/resources/mon_json_parsefile.json"),
            0)
      << ptr->GetError();
}
}  // namespace Monintor
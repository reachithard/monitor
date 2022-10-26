#include <memory>

#include "gtest/gtest.h"
#include "utils/logger.hpp"
#include "uuid/uuid.h"

namespace Monitor {
TEST(TestLibuuid, generate) {
  uuid_t uu;
  uuid_clear(uu);
  uuid_generate(uu);
  char uuid1_str[37] = {0};
  uuid_unparse(uu, uuid1_str);
  LOG_DEBUG("get uuid:{}", uuid1_str);
}
}  // namespace Monitor
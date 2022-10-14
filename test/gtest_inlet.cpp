#include <cstdio>

#include "../src/utils/logger.hpp"
#include "gtest/gtest.h"

#if GTEST_OS_ESP8266 || GTEST_OS_ESP32
#if GTEST_OS_ESP8266
extern "C" {
#endif
void setup() { testing::InitGoogleTest(); }

void loop() { RUN_ALL_TESTS(); }

#if GTEST_OS_ESP8266
}
#endif

#else

GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from %s\n", __FILE__);
  Logger::Singleton<Logger>::Get().Init("./logs/test.log");
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
#endif
#include "plugin.h"

void PluginRegister() {
  PLUGIN_DEBUG("hello world test plugin");
  ModuleRegister(nullptr);
}
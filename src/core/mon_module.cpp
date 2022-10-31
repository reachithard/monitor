#include "mon_module.h"

#include <dirent.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <unistd.h>

#include "export/mon_plugin.h"
#include "export/monitor_errors.h"
#include "utils/logger.hpp"

namespace Monitor {
int32_t MonModule::LoadPlugin(const std::string& dir, const std::string& module,
                              bool global) {
  int ret = 0;
  if (dir.empty() || module.empty()) {
    return ERR_MODULE_PARAM;
  }

  static const std::string suffix = ".so";
  constexpr uint32_t bufferSize = 512;
  std::string temp = module + suffix;
  if (temp.size() > bufferSize) {
    LOG_ERROR("file name too long");
    return ERR_MODULE_FILE_LONG;
  }

  // TODO 考虑到有符号链接情况 目前实现为这样
  DIR* dh = opendir(dir.c_str());

  if (dh == nullptr) {
    LOG_ERROR("open {} failed, please chech it", dir);
    return ERR_MODULE_FILE_OPEN;
  }

  struct dirent* de = nullptr;
  struct stat statbuf;
  while ((de = readdir(dh)) != nullptr) {
    /* code */
    if (strcasecmp(de->d_name, temp.c_str()) != 0) {
      continue;
    }

    temp = dir + "/" + module + suffix;
    if (temp.size() > bufferSize) {
      LOG_ERROR("file name too long");
      ret = ERR_MODULE_FILE_LONG;
      goto error;
    }

    if (lstat(temp.c_str(), &statbuf) == -1) {
      LOG_WARN("stat {} failed: {}", temp, errno);
      continue;
    } else if (!S_ISREG(statbuf.st_mode)) {
      LOG_WARN("{} is not a regular file", temp);
      continue;
    }

    ret = LoadFile(temp, global);
    if (ret == 0) {
      LOG_DEBUG("load success");
    } else {
      LOG_ERROR("load file:{} failed with:{}", temp, ret);
    }
  }

error:
  closedir(dh);
  return ret;
}

int32_t MonModule::DispatchConf(const std::string& plugin,
                                const ConfigItem_t* item) {
  int32_t ret = 0;
  std::unordered_map<std::string, std::unique_ptr<ConfCallback_t>>::iterator
      it = cfCallbacks.find(plugin);
  if (it == cfCallbacks.end()) {
    LOG_DEBUG("plugin:{} haven't conf", plugin);
  } else {
    ret = cfCallbacks[plugin]->callback(item);
  }
  return ret;
}

int32_t MonModule::LoadFile(const std::string& filename, bool global) {
  int32_t flag = RTLD_NOW;
  int32_t ret = 0;
  if (global) {
    flag |= RTLD_GLOBAL;
  }

  void* dl = dlopen(filename.c_str(), flag);
  if (dl == nullptr) {
    LOG_ERROR(
        "dlopen file:{} error with:{}, please use ldd to check the module "
        "library.",
        filename, dlerror());
    return ERR_MODULE_LOAD;
  }

  typedef void (*fptr)();
  fptr handle = (fptr)dlsym(dl, "PluginRegister");
  if (handle == nullptr) {
    LOG_ERROR("could't find symbol PluginRegister in {}", filename, dlerror());
    ret = ERR_MODULE_LOAD;
  } else {
    handle();
  }
  dlclose(dl);
  return ret;
}

int32_t MonModule::TriggerInit() {
  // TODO 加上锁
  int32_t ret = 0;
  for (std::unordered_map<std::string,
                          std::unique_ptr<InitCallback_t>>::iterator it =
           initCallbacks.begin();
       it != initCallbacks.end(); it++) {
    if (it->second->callback != nullptr) {
      int32_t temp = it->second->callback(nullptr);
      if (temp != 0) {
        ret = temp;
        LOG_ERROR("{}'s callback error:{}", it->first, temp);
      }
    } else {
      LOG_ERROR("can't find {}'s callback", it->first);
    }
  }
  return ret;
}

int32_t MonModule::RegisterModule(const std::string& name,
                                  const PluginCallbacks_t* callback) {
  int32_t ret = 0;
  // TODO 加锁
  if (callback->config != nullptr) {
    std::unique_ptr<ConfCallback_t> ptr = std::make_unique<ConfCallback_t>();
    ptr->callback = callback->config;
    cfCallbacks.insert(std::make_pair(name, std::move(ptr)));
  }

  if (callback->init != nullptr) {
    std::unique_ptr<InitCallback_t> ptr = std::make_unique<InitCallback_t>();
    ptr->callback = callback->init;
    initCallbacks.insert(std::make_pair(name, std::move(ptr)));
  }

  if (callback->init != nullptr) {
    std::unique_ptr<ReadCallback_t> ptr = std::make_unique<ReadCallback_t>();
    ptr->callback = callback->read;
    readCallbacks.insert(std::make_pair(name, std::move(ptr)));
  }

  if (callback->write != nullptr) {
    std::unique_ptr<WriteCallback_t> ptr = std::make_unique<WriteCallback_t>();
    ptr->callback = callback->write;
    writeCallbacks.insert(std::make_pair(name, std::move(ptr)));
  }

  if (callback->init != nullptr) {
    std::unique_ptr<ShutdownCallback_t> ptr =
        std::make_unique<ShutdownCallback_t>();
    ptr->callback = callback->shutdown;
    shutdownCallbacks.insert(std::make_pair(name, std::move(ptr)));
  }
  return ret;
}
}  // namespace Monitor
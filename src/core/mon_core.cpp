#include "mon_core.h"

#include "asio.hpp"
#include "asio/steady_timer.hpp"
#include "export/monitor_errors.h"

namespace Monitor {
MonCore::MonCore() {}

MonCore::~MonCore() {}

int32_t MonCore::Init(const std::string& file) {
  int32_t ret = 0;
  conf = std::make_unique<MonConfile>();
  if (conf == nullptr) {
    return ERR_CORE_CONF;
  }

  // 初始化完配置文件 则需要根据配置文件进行线程配置等之类的操作
  // 这里直接调用了插件的配置回调
  ret = conf->Init(file);
  if (ret != 0) {
    return ret;
  }

  // 进行module的init
  ret = Singleton<MonModule>::Get().TriggerInit();
  if (ret != 0) {
    return ret;
  }

  // 进行read和write read为excutor write为actor
  const MonGlobalConf_t& global = conf->GetGlobalConf();
  for (uint32_t idx = 0; idx < global.writeThreads; idx++) {
    std::unique_ptr<MonActor> actor = std::make_unique<MonActor>(this, idx);
    actor->Init();
    actors.push_back(std::move(actor));
  }
  return ret;
}

int32_t MonCore::Update() {
  for (uint32_t idx = 0; idx < actors.size(); idx++) {
    actors[idx]->Update();
  }

  int32_t ret = 0;
  asio::io_context ctx;
  asio::steady_timer timer(ctx);
  asio::error_code ignore;
  bool stop = false;

  while (true) {
    if (ret < 0) {
      break;
    }

    timer.expires_after(std::chrono::milliseconds(1));
    timer.wait(ignore);
  }
  return ret;
}

void MonCore::Dispatch(std::unique_ptr<MonMessage> msg) {}
}  // namespace Monitor
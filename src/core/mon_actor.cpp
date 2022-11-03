#include "mon_actor.h"

#include "mon_core.h"

namespace Monitor {
MonActor::MonActor(MonCore* icore, uint32_t actorId)
    : core(icore), id(actorId), work(std::make_unique<MonWork>()) {}

MonActor::~MonActor() {}

void MonActor::Send(std::unique_ptr<MonMessage> msg) {
  mq.enqueue(std::move(msg));
  asio::post(work->GetContext(), []() {

  });
}

void MonActor::Init() {}

void MonActor::Update() { work->Run(); }

void MonActor::Stop() { work->Stop(); }

void MonActor::ShutDown() {}
}  // namespace Monitor

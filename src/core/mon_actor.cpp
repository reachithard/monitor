#include "mon_actor.h"

#include "mon_core.h"

namespace Monitor {
MonActor::MonActor(MonCore* icore, uint32_t actorId)
    : core(icore), id(actorId), work(std::make_unique<MonWork>()) {}

MonActor::~MonActor() {}

void MonActor::Dispatch(std::unique_ptr<MonMessage> msg) {
  mq.enqueue(std::move(msg));
  asio::post(work->GetContext(), [this]() {
    // 这里从环形队列里面找一个 然后去触发回调
    std::unique_ptr<MonMessage> tmp;
    bool success = mq.try_dequeue(tmp);
    if (success) {
      // 出队列成功
      HandleMessage(this, std::move(tmp));
    }
  });
}

void MonActor::Init() {}

void MonActor::Update() { work->Run(); }

void MonActor::Stop() { work->Stop(); }

void MonActor::ShutDown() {}
}  // namespace Monitor

#ifndef _MON_ACTOR_H_
#define _MON_ACTOR_H_

#include <memory>

#include "mon_message.h"
#include "mon_work.h"
#include "moodycamel/concurrentqueue.h"
#include "utils/nocopy.h"

namespace Monitor {
class MonCore;

class MonActor : public Nocopy {
 public:
  MonActor(MonCore* icore, uint32_t actorId);
  ~MonActor();

  uint32_t GetId() const { return id; }

  void Dispatch(std::unique_ptr<MonMessage> msg);

  void Init();

  void Update();

  void Stop();

  void ShutDown();

 private:
  moodycamel::ConcurrentQueue<std::unique_ptr<MonMessage>> mq;
  std::unique_ptr<MonWork> work;
  MonCore* core;
  uint32_t id;

  // TODO 在这里面存模块信息 插件等
};

template <typename Actor, typename Message>
inline void HandleMessage(Actor&& s, std::unique_ptr<Message> m) {
  // 这里去调用插件的回调函数
}
}  // namespace Monitor

#endif  // _MON_ACTOR_H_
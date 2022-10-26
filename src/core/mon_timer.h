#ifndef _MON_TIMER_H_
#define _MON_TIMER_H_

#include <chrono>
#include <ctime>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "asio.hpp"
#include "asio/steady_timer.hpp"
#include "mon_work.h"
#include "utils/logger.hpp"
#include "utils/singleton.h"
#include "utils/uuid.h"

namespace Monitor {
class TimerMeta {
 public:
  TimerMeta(bool iloop, asio::io_context& ictx, uint64_t ims)
      : loop(iloop), timer(ictx), ms(ims) {}

  void Init() {
    timer.expires_after(std::chrono::milliseconds(ms));
    Uuid::Uuid_t uu;
    Uuid::UuidGenerate(uu);
    char uid[37] = {0};
    Uuid::UuidUnparse(uu, uid);
    uuid = std::string(uid);
  }

  std::function<void(const std::error_code&)> func;
  std::string uuid;
  bool loop;
  asio::steady_timer timer;
  uint64_t ms;
};

class MonTimer {
 public:
  MonTimer() {}
  ~MonTimer() { Stop(); }

  // TODO 注意生命周期之类
  const std::string& AddTimer(std::function<void(const std::error_code&)> func,
                              uint64_t ms, bool loop = false) {
    std::unique_ptr<TimerMeta> meta =
        std::make_unique<TimerMeta>(loop, worker.GetContext(), ms);
    meta->Init();
    meta->func = func;
    const std::string& uu = meta->uuid;
    meta->timer.expires_after(std::chrono::milliseconds(ms));
    meta->timer.async_wait(
        std::bind(&MonTimer::TimeOut, this, std::placeholders::_1, uu));
    std::lock_guard<std::mutex> lk(lock);
    timers.insert(std::make_pair(uu, std::move(meta)));
    return uu;
  }

  void Run() { worker.Run(); }

  void Stop() {
    std::lock_guard<std::mutex> lk(lock);
    timers.clear();
    worker.Stop();
  }

  void ClearTimer(const std::string& uu) {
    std::lock_guard<std::mutex> lk(lock);
    auto it = timers.find(uu);
    if (it == timers.end()) {
      LOG_ERROR("can't find key:{}", uu);
    } else {
      it->second->timer.cancel();
      timers.erase(it);
    }
  }

 protected:
  void TimeOut(const std::error_code& error, const std::string& uu) {
    std::lock_guard<std::mutex> lk(lock);
    auto it = timers.find(uu);
    if (it == timers.end()) {
      LOG_ERROR("can't find key:{}", uu);
    } else {
      it->second->func(error);
      if (it->second->loop) {
        it->second->timer.expires_after(
            std::chrono::milliseconds(it->second->ms));
        it->second->timer.async_wait(
            std::bind(&MonTimer::TimeOut, this, std::placeholders::_1, uu));
      } else {
        timers.erase(it);
      }
    }
  }

 private:
  std::mutex lock;
  std::unordered_map<std::string, std::unique_ptr<TimerMeta>> timers;
  MonWork worker;
};
}  // namespace Monitor
#endif  // _MON_TIMER_H_
#ifndef _MON_TIMER_H_
#define _MON_TIMER_H_

#include <chrono>
#include <ctime>
#include <functional>
#include <iostream>
#include <vector>

#include "asio.hpp"
#include "asio/steady_timer.hpp"
#include "utils/singleton.h"

namespace Monitor {
// struct TimeClock {
//   typedef std::chrono::steady_clock::duration duration;

//   typedef duration::rep rep;

//   typedef duration::period period;

//   typedef std::chrono::time_point<TimeClock> time_point;

//   static constexpr bool isSeady = false;

//   static time_point Now() noexcept {
//     return time_point() + std::chrono::seconds(std::time(0));
//   }
// };

// struct TimeWaitTraits {
//   static TimeClock::duration WaitDuration(const TimeClock::duration& d) {
//     if (d > std::chrono::seconds(1))
//       return d - std::chrono::seconds(1);
//     else if (d > std::chrono::seconds(0))
//       return std::chrono::milliseconds(10);
//     else
//       return std::chrono::seconds(0);
//   }

//   // Determine how long until the clock should be next polled to determine
//   // whether the absoluate time has been reached.
//   static TimeClock::duration WaitDuration(const TimeClock::time_point& t) {
//     return WaitDuration(t - TimeClock::Now());
//   }
// };

// typedef asio::basic_waitable_timer<TimeClock, TimeWaitTraits> timer;

struct TimerMeta {};

class MonTimer {
 public:
  MonTimer() : ctx(1), work(asio::make_work_guard(ctx)) {}
  ~MonTimer() {}
  void AddTimer(std::function<void(const std::error_code&)> func, uint64_t ms) {
    asio::steady_timer timer(ctx);
    timer.expires_after(std::chrono::milliseconds(ms));
    timer.async_wait(func);
    timers.push_back(std::move(timer));
  }

  void Run() { ctx.run(); }

 private:
  asio::io_context ctx;
  std::vector<asio::steady_timer> timers;
  asio::executor_work_guard<asio::io_context::executor_type> work;
};
}  // namespace Monitor
#endif  // _MON_TIMER_H_
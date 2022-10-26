#ifndef _MON_WORK_H_
#define _MON_WORK_H_

#include <thread>

#include "asio.hpp"
#include "utils/nocopy.h"

namespace Monitor {
class MonWork : public Nocopy {
 public:
  MonWork() : ctx(1), work(asio::make_work_guard(ctx)) {}

  ~MonWork() { Stop(); }
  asio::io_context& GetContext() { return ctx; }

  void Stop() {
    ctx.stop();
    if (th.joinable()) {
      th.join();
    }
  }

  void Run() {
    th = std::thread([this]() { ctx.run(); });
  }

 private:
  std::thread th;
  asio::io_context ctx;
  asio::executor_work_guard<asio::io_context::executor_type> work;
};
}  // namespace Monitor
#endif  // _MON_WORK_H_
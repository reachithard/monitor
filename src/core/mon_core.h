#ifndef _MON_CORE_H_
#define _MON_CORE_H_

#include <memory>
#include <vector>

#include "mon_actor.h"
#include "mon_confile.h"
#include "mon_message.h"
#include "mon_module.h"

namespace Monitor {
class MonCore {
 public:
  MonCore();

  ~MonCore();

  int32_t Init(const std::string& file);

  int32_t Update();

  void Dispatch(std::unique_ptr<MonMessage> msg);

  void NewActor();

  void RemoveActor();

 private:
  std::unique_ptr<MonConfile> conf;
  std::vector<std::unique_ptr<MonActor>> actors;
};
}  // namespace Monitor
#endif  // _MON_CORE_H_
#ifndef _MON_MESSAGE_H_
#define _MON_MESSAGE_H_

namespace Monitor {
class MonMessage final {
 public:
  MonMessage();
  ~MonMessage();

 private:
  uint8_t type = 0;
  uint32_t sender = 0;
  uint32_t receiver = 0;
  int32_t sessionId = 0;
};
}  // namespace Monitor
#endif  // _MON_MESSAGE_H_
#ifndef _MON_MESSAGE_H_
#define _MON_MESSAGE_H_

#include <utility>

#include "utils/nocopy.h"

namespace Monitor {
class MonMessage final : public Nocopy {
 public:
  MonMessage() {}

  ~MonMessage() {}

  MonMessage(const MonMessage&& other) noexcept
      : type(other.type),
        sender(other.sender),
        receiver(other.receiver),
        sessionId(other.sessionId),
        broadcast(other.broadcast) {}

  MonMessage& operator=(MonMessage&& other) noexcept {
    if (this != std::addressof(other)) {
      type = other.type;
      sender = other.sender;
      receiver = other.receiver;
      sessionId = other.sessionId;
      broadcast = other.broadcast;
    }
    return *this;
  }

  void SetType(uint8_t itype) { type = itype; }

  uint8_t GetType() { return type; }

  void SetSender(uint32_t isender) { sender = isender; }

  uint32_t GetSender() const { return sender; }

  void SetReceiver(uint32_t ireceiver) { receiver = ireceiver; }

  uint32_t GetReceiver() const { return receiver; }

  void SetSessionId(uint32_t isessionId) { sessionId = isessionId; }

  uint32_t GetSessionId() const { return sessionId; }

 private:
  bool broadcast = false;
  uint8_t type = 0;
  uint32_t sender = 0;
  uint32_t receiver = 0;
  int32_t sessionId = 0;
};
}  // namespace Monitor
#endif  // _MON_MESSAGE_H_
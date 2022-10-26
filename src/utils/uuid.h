#ifndef _UUID_H_
#define _UUID_H_

#include <string>

#include "uuid/uuid.h"

// uuid 是无状态的
namespace Uuid {
typedef uuid_t Uuid_t;
/* UUID Variant definitions */
#define M_UUID_VARIANT_NCS 0
#define M_UUID_VARIANT_DCE 1
#define M_UUID_VARIANT_MICROSOFT 2
#define M_UUID_VARIANT_OTHER 3

/* UUID Type definitions */
#define M_UUID_TYPE_DCE_TIME 1
#define M_UUID_TYPE_DCE_RANDOM 4

/* clear.c */
inline void UuidClear(Uuid_t uu) { uuid_clear(uu); }

/* compare.c */
inline int UuidCompare(const Uuid_t uu1, const Uuid_t uu2) {
  return uuid_compare(uu1, uu2);
}

/* copy.c */
inline void UuidCopy(Uuid_t dst, const Uuid_t src) { uuid_copy(dst, src); }

/* gen_uuid.c */
inline void UuidGenerate(Uuid_t out) {
  uuid_clear(out);
  uuid_generate(out);
}
inline void UuidGenerateRandom(Uuid_t out) {
  uuid_clear(out);
  uuid_generate_random(out);
}
inline void UuidGenerateTime(Uuid_t out) {
  uuid_clear(out);
  uuid_generate_time(out);
}
inline int UuidTenerateTimeSafe(Uuid_t out) {
  uuid_clear(out);
  return uuid_generate_time_safe(out);
}

/* isnull.c */
inline int UuidIsNull(const Uuid_t uu) { return uuid_is_null(uu); }

/* parse.c */
inline int UuidParse(const char* in, Uuid_t uu) { return uuid_parse(in, uu); }

/* unparse.c */
inline void UuidUnparse(const Uuid_t uu, char* out) {
  return uuid_unparse(uu, out);
}
inline void UuidUnparseLower(const Uuid_t uu, char* out) {
  return uuid_unparse_lower(uu, out);
}
inline void UuidUnparseUpper(const Uuid_t uu, char* out) {
  return uuid_unparse_upper(uu, out);
}

/* Uuidtime.c */
inline time_t UuidTime(const Uuid_t uu, struct timeval* ret_tv) {
  return uuid_time(uu, ret_tv);
}

inline int UuidType(const Uuid_t uu) { return uuid_type(uu); }

inline int UuidVariant(const Uuid_t uu) { return uuid_variant(uu); }

}  // namespace Uuid

#endif  // _UUID_H_
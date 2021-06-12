#pragma once

#include <cstdint>

#ifndef CONCATENATE
#define _CONCATENATE(x1, x2) x1##x2
#define CONCATENATE(x1, x2) _CONCATENATE(x1, x2)
#endif

#ifndef STRINGIFY
#define _STRINGIFY(x) #x
#define STRINGIFY(x) _STRINGIFY(x)
#endif

#ifndef PACKED
#define PACKED __attribute__((__packed__))
#endif

#define INSERT_PADDING_BYTES(num_bytes)                                        \
  uint8_t CONCATENATE(pad, __LINE__)[(num_bytes)];

#define STATIC_ASSERT_SIZE(type, size)                                         \
  static_assert(sizeof(type) == size,                                          \
                STRINGIFY(type) " should have size " STRINGIFY(size));

template<class T>
constexpr T *symbol(void *address) {
	return static_cast<T *>(address);
}
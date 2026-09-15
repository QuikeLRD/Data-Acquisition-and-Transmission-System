#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H

#include <stdint.h>

/* Result of any platform-layer operation. Shared by every platform module
   so callers handle success and failure the same way no matter which
   peripheral they are talking to. */
typedef enum {
    PLATFORM_OK = 0,
    PLATFORM_ERROR
} Platform_Status_t;

#endif /* PLATFORM_TYPES_H */

#ifndef RENDERER_MACROS_H
#define RENDERER_MACROS_H

#include <stdint.h>
#include <math.h>

#define RENDERER_MAKE_GROUP_COUNT(GLOBAL_SIZE, LOCAL_SIZE) ((int32_t)ceil((double)(GLOBAL_SIZE) / (LOCAL_SIZE)))

#endif // RENDERER_MACROS_H

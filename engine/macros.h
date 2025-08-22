#ifndef MACROS_H
#define MACROS_H

#include <stdio.h>
#include <stdint.h>
#include <intrin.h>
#include <math.h>

#ifdef BUILD_DEBUG
#  define VULKAN_CHECK(EXPRESSION)                          \
    {                                                       \
      VkResult result = (EXPRESSION);                       \
      if (result != VK_SUCCESS) {                           \
        printf("%s failed with %d\n", #EXPRESSION, result); \
        __debugbreak();                                     \
      }                                                     \
    }
#else
#  define VULKAN_CHECK(EXPRESSION, ...) (EXPRESSION)
#endif // BUILD_DEBUG

#endif // MACROS_H

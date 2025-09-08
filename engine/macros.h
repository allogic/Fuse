#ifndef MACROS_H
#define MACROS_H

#include <stdio.h>
#include <stdint.h>
#include <intrin.h>

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

#ifdef BUILD_DEBUG
#  define SPIRV_CHECK(EXPRESSION)                           \
    {                                                       \
      SpvReflectResult result = (EXPRESSION);               \
      if (result != SPV_REFLECT_RESULT_SUCCESS) {           \
        printf("%s failed with %d\n", #EXPRESSION, result); \
        __debugbreak();                                     \
      }                                                     \
    }
#else
#  define SPIRV_CHECK(EXPRESSION, ...) (EXPRESSION)
#endif // BUILD_DEBUG

#endif // MACROS_H

#ifndef EG_MACROS_H
#define EG_MACROS_H

#ifdef BUILD_DEBUG
#  define EG_VULKAN_CHECK(EXPRESSION)                       \
    {                                                       \
      VkResult result = (EXPRESSION);                       \
      if (result != VK_SUCCESS) {                           \
        printf("%s failed with %d\n", #EXPRESSION, result); \
        __debugbreak();                                     \
      }                                                     \
    }
#else
#  define EG_VULKAN_CHECK(EXPRESSION) (EXPRESSION)
#endif // BUILD_DEBUG

#endif // EG_MACROS_H

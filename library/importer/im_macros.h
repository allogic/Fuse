#ifndef IM_MACROS_H
#define IM_MACROS_H

#ifdef BUILD_DEBUG
#  define CGLTF_CHECK(EXPRESSION)                           \
    {                                                       \
      cgltf_result result = (EXPRESSION);                   \
      if (result != cgltf_result_success) {                 \
        printf("%s failed with %d\n", #EXPRESSION, result); \
        __debugbreak();                                     \
      }                                                     \
    }
#else
#  define CGLTF_CHECK(EXPRESSION) (EXPRESSION)
#endif // BUILD_DEBUG

#endif // IM_MACROS_H

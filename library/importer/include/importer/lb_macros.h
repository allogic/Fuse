#ifndef LB_IMPORTER_MACROS_H
#define LB_IMPORTER_MACROS_H

#ifdef BUILD_DEBUG
#  define LB_SPIRV_CHECK(EXPRESSION)                        \
    {                                                       \
      SpvReflectResult result = (EXPRESSION);               \
      if (result != SPV_REFLECT_RESULT_SUCCESS) {           \
        printf("%s failed with %d\n", #EXPRESSION, result); \
        __debugbreak();                                     \
      }                                                     \
    }
#else
#  define LB_SPIRV_CHECK(EXPRESSION) (EXPRESSION)
#endif // BUILD_DEBUG

#ifdef BUILD_DEBUG
#  define LB_GLTF_CHECK(EXPRESSION)                         \
    {                                                       \
      cgltf_result result = (EXPRESSION);                   \
      if (result != cgltf_result_success) {                 \
        printf("%s failed with %d\n", #EXPRESSION, result); \
        __debugbreak();                                     \
      }                                                     \
    }
#else
#  define LB_GLTF_CHECK(EXPRESSION) (EXPRESSION)
#endif // BUILD_DEBUG

#endif // LB_IMPORTER_MACROS_H

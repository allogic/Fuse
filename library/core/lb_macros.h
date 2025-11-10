#ifndef LB_CORE_MACROS_H
#define LB_CORE_MACROS_H

#define OFFSET_OF(TYPE, MEMBER) ((uint64_t)&(((TYPE *)0ULL)->MEMBER))

#define ARRAY_COUNT(ARRAY) ((uint64_t)(sizeof(ARRAY) / sizeof((ARRAY)[0ULL])))

#define ASSERT(EXPRESSION, MESSAGE) assert((EXPRESSION) && (MESSAGE))
#define STATIC_ASSERT(EXPRESSION) typedef uint8_t static_assert_of[(EXPRESSION) ? (1LL) : (-1LL)]

#define MIN(A, B) (((A) < (B)) ? (A) : (B))
#define MAX(A, B) (((A) > (B)) ? (A) : (B))

#define ALIGN_DOWN_BY(VALUE, ALIGNMENT) (((uint64_t)(VALUE)) & ~(((uint64_t)(ALIGNMENT)) - 1ULL))
#define ALIGN_UP_BY(VALUE, ALIGNMENT) ((((uint64_t)(VALUE)) + (((uint64_t)(ALIGNMENT)) - 1ULL)) & ~(((uint64_t)(ALIGNMENT)) - 1ULL))

#define IS_BIT_SET(VALUE, BIT) (((uint64_t)(VALUE)) & (BIT))
#define IS_BIT_NOT_SET(VALUE, BIT) (!(((uint64_t)(VALUE)) & (BIT)))

#define SET_BIT(VALUE, BIT) (((uint64_t)(VALUE)) | (1ULL << (BIT)))
#define CLEAR_BIT(VALUE, BIT) (((uint64_t)(VALUE)) & (~(1ULL << (BIT))))
#define TOGGLE_BIT(VALUE, BIT) (((uint64_t)(VALUE)) ^ (1ULL << (BIT)))

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
#  define VULKAN_CHECK(EXPRESSION) (EXPRESSION)
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
#  define SPIRV_CHECK(EXPRESSION) (EXPRESSION)
#endif // BUILD_DEBUG

#endif // LB_CORE_MACROS_H

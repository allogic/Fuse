#ifndef LB_CORE_MACROS_H
#define LB_CORE_MACROS_H

#define LB_OFFSET_OF(TYPE, MEMBER) ((uint64_t)&(((TYPE *)0ULL)->MEMBER))
#define LB_VIRTUAL_OF(PTR, TYPE, MEMBER) (((uint64_t)(PTR)) + ((uint64_t)&(((TYPE *)0ULL)->MEMBER)))

#define LB_ARRAY_COUNT(ARRAY) ((uint64_t)(sizeof(ARRAY) / sizeof((ARRAY)[0ULL])))

#define LB_ASSERT(EXPRESSION, MESSAGE) assert((EXPRESSION) && (MESSAGE))
#define LB_STATIC_ASSERT(EXPRESSION) typedef uint8_t static_assert_of[(EXPRESSION) ? (1LL) : (-1LL)]

#define LB_MIN(A, B) (((A) < (B)) ? (A) : (B))
#define LB_MAX(A, B) (((A) > (B)) ? (A) : (B))

#define LB_ALIGN_DOWN_BY(VALUE, ALIGNMENT) (((uint64_t)(VALUE)) & ~(((uint64_t)(ALIGNMENT)) - 1ULL))
#define LB_ALIGN_UP_BY(VALUE, ALIGNMENT) ((((uint64_t)(VALUE)) + (((uint64_t)(ALIGNMENT)) - 1ULL)) & ~(((uint64_t)(ALIGNMENT)) - 1ULL))

#define LB_IS_BIT_SET(VALUE, BIT) (((uint64_t)(VALUE)) & (BIT))
#define LB_IS_BIT_NOT_SET(VALUE, BIT) (!(((uint64_t)(VALUE)) & (BIT)))

#define LB_SET_BIT(VALUE, BIT) (((uint64_t)(VALUE)) | (1ULL << (BIT)))
#define LB_CLEAR_BIT(VALUE, BIT) (((uint64_t)(VALUE)) & (~(1ULL << (BIT))))
#define LB_TOGGLE_BIT(VALUE, BIT) (((uint64_t)(VALUE)) ^ (1ULL << (BIT)))

#ifdef BUILD_DEBUG
#  define LB_VULKAN_CHECK(EXPRESSION)                       \
    {                                                       \
      VkResult result = (EXPRESSION);                       \
      if (result != VK_SUCCESS) {                           \
        printf("%s failed with %d\n", #EXPRESSION, result); \
        __debugbreak();                                     \
      }                                                     \
    }
#else
#  define LB_VULKAN_CHECK(EXPRESSION) (EXPRESSION)
#endif // BUILD_DEBUG

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

#endif // LB_CORE_MACROS_H

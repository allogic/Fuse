#ifndef EG_MACROS_H
#define EG_MACROS_H

#define EG_OFFSET_OF(TYPE, MEMBER) ((uint64_t)&(((TYPE *)0ULL)->MEMBER))
#define EG_VIRTUAL_OF(PTR, TYPE, MEMBER) (((uint64_t)(PTR)) + ((uint64_t)&(((TYPE *)0ULL)->MEMBER)))

#define EG_ARRAY_COUNT(ARRAY) ((uint64_t)(sizeof(ARRAY) / sizeof((ARRAY)[0ULL])))

#define EG_ASSERT(EXPRESSION, MESSAGE) assert((EXPRESSION) && (MESSAGE))
#define EG_STATIC_ASSERT(EXPRESSION) typedef uint8_t static_assert_of[(EXPRESSION) ? (1LL) : (-1LL)]

#define EG_MIN(A, B) (((A) < (B)) ? (A) : (B))
#define EG_MAX(A, B) (((A) > (B)) ? (A) : (B))

#define EG_ALIGN_DOWN_BY(VALUE, ALIGNMENT) (((uint64_t)(VALUE)) & ~(((uint64_t)(ALIGNMENT)) - 1ULL))
#define EG_ALIGN_UP_BY(VALUE, ALIGNMENT) ((((uint64_t)(VALUE)) + (((uint64_t)(ALIGNMENT)) - 1ULL)) & ~(((uint64_t)(ALIGNMENT)) - 1ULL))

#define EG_IS_BIT_SET(VALUE, BIT) (((uint64_t)(VALUE)) & (BIT))
#define EG_IS_BIT_NOT_SET(VALUE, BIT) (!(((uint64_t)(VALUE)) & (BIT)))

#define EG_SET_BIT(VALUE, BIT) (((uint64_t)(VALUE)) | (1ULL << (BIT)))
#define EG_CLEAR_BIT(VALUE, BIT) (((uint64_t)(VALUE)) & (~(1ULL << (BIT))))
#define EG_TOGGLE_BIT(VALUE, BIT) (((uint64_t)(VALUE)) ^ (1ULL << (BIT)))

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

#ifdef BUILD_DEBUG
#  define EG_SPIRV_CHECK(EXPRESSION)                        \
    {                                                       \
      SpvReflectResult result = (EXPRESSION);               \
      if (result != SPV_REFLECT_RESULT_SUCCESS) {           \
        printf("%s failed with %d\n", #EXPRESSION, result); \
        __debugbreak();                                     \
      }                                                     \
    }
#else
#  define EG_SPIRV_CHECK(EXPRESSION) (EXPRESSION)
#endif // BUILD_DEBUG

#ifdef BUILD_DEBUG
#  define EG_GLTF_CHECK(EXPRESSION)                         \
    {                                                       \
      cgltf_result result = (EXPRESSION);                   \
      if (result != cgltf_result_success) {                 \
        printf("%s failed with %d\n", #EXPRESSION, result); \
        __debugbreak();                                     \
      }                                                     \
    }
#else
#  define EG_GLTF_CHECK(EXPRESSION) (EXPRESSION)
#endif // BUILD_DEBUG

#ifdef BUILD_DEBUG
#  define EG_SQL_CHECK(EXPRESSION)                                                     \
    {                                                                                  \
      uint32_t result = (EXPRESSION);                                                  \
      if (result != SQLITE_OK) {                                                       \
        printf("%s failed with %s\n", #EXPRESSION, sqlite3_errmsg(s_database_handle)); \
        __debugbreak();                                                                \
      }                                                                                \
    }
#else
#  define EG_SQL_CHECK(EXPRESSION) (EXPRESSION)
#endif // BUILD_DEBUG

#ifdef BUILD_DEBUG
#  define EG_SQL_CHECK_STATUS(EXPRESSION, STATUS)                                      \
    {                                                                                  \
      uint32_t result = (EXPRESSION);                                                  \
      if (result != (STATUS)) {                                                        \
        printf("%s failed with %s\n", #EXPRESSION, sqlite3_errmsg(s_database_handle)); \
        __debugbreak();                                                                \
      }                                                                                \
    }
#else
#  define EG_SQL_CHECK_STATUS(EXPRESSION, STATUS) (EXPRESSION)
#endif // BUILD_DEBUG

#endif // EG_MACROS_H

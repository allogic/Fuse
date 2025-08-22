#ifndef CORE_MACROS_H
#define CORE_MACROS_H

#include <stdint.h>

#define CORE_OFFSET_OF(TYPE, MEMBER) ((uint64_t)&(((TYPE *)0ULL)->MEMBER))

#define CORE_ARRAY_COUNT(ARRAY) ((uint64_t)(sizeof(ARRAY) / sizeof((ARRAY)[0ULL])))

#define CORE_STATIC_ASSERT(EXPRESSION) typedef uint8_t static_assert_of[(EXPRESSION) ? (1LL) : (-1LL)]

#define CORE_MIN(A, B) (((A) < (B)) ? (A) : (B))
#define CORE_MAX(A, B) (((A) > (B)) ? (A) : (B))

#define CORE_ALIGN_DOWN_BY(VALUE, ALIGNMENT) (((uint64_t)(VALUE)) & ~(((uint64_t)(ALIGNMENT)) - 1ULL))
#define CORE_ALIGN_UP_BY(VALUE, ALIGNMENT) ((((uint64_t)(VALUE)) + (((uint64_t)(ALIGNMENT)) - 1ULL)) & ~(((uint64_t)(ALIGNMENT)) - 1ULL))

#define CORE_IS_BIT_SET(VALUE, BIT) (((uint64_t)(VALUE)) & (1ULL << (BIT)))
#define CORE_IS_BIT_NOT_SET(VALUE, BIT) (!(((uint64_t)(VALUE)) & (1ULL << (BIT))))

#define CORE_SET_BIT(VALUE, BIT) (((uint64_t)(VALUE)) | (1ULL << (BIT)))
#define CORE_CLEAR_BIT(VALUE, BIT) (((uint64_t)(VALUE)) & (~(1ULL << (BIT))))
#define CORE_TOGGLE_BIT(VALUE, BIT) (((uint64_t)(VALUE)) ^ (1ULL << (BIT)))

#endif // CORE_MACROS_H

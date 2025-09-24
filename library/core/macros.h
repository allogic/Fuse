#ifndef CORE_MACROS_H
#define CORE_MACROS_H

#include <stdint.h>

#define OFFSET_OF(TYPE, MEMBER) ((uint64_t)&(((TYPE *)0ULL)->MEMBER))

#define ARRAY_COUNT(ARRAY) ((uint64_t)(sizeof(ARRAY) / sizeof((ARRAY)[0ULL])))

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

#endif // CORE_MACROS_H

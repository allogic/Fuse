#ifndef EG_MATH_VECTOR2_H
#define EG_MATH_VECTOR2_H

#include <engine/eg_fwd.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline eg_vector2_t eg_vector2_zero(void);
__forceinline eg_vector2_t eg_vector2_one(void);
__forceinline eg_vector2_t eg_vector2_right(void);
__forceinline eg_vector2_t eg_vector2_up(void);
__forceinline eg_vector2_t eg_vector2_left(void);
__forceinline eg_vector2_t eg_vector2_down(void);
__forceinline eg_vector2_t eg_vector2_xy(float x, float y);
__forceinline eg_vector2_t eg_vector2_negate(eg_vector2_t a);
__forceinline eg_vector2_t eg_vector2_add(eg_vector2_t a, eg_vector2_t b);
__forceinline eg_vector2_t eg_vector2_sub(eg_vector2_t a, eg_vector2_t b);
__forceinline eg_vector2_t eg_vector2_mul(eg_vector2_t a, eg_vector2_t b);
__forceinline eg_vector2_t eg_vector2_div(eg_vector2_t a, eg_vector2_t b);
__forceinline eg_vector2_t eg_vector2_adds(eg_vector2_t a, float b);
__forceinline eg_vector2_t eg_vector2_subs(eg_vector2_t a, float b);
__forceinline eg_vector2_t eg_vector2_muls(eg_vector2_t a, float b);
__forceinline eg_vector2_t eg_vector2_divs(eg_vector2_t a, float b);
__forceinline eg_vector2_t eg_vector2_norm(eg_vector2_t a);
__forceinline float eg_vector2_dot(eg_vector2_t a, eg_vector2_t b);
__forceinline float eg_vector2_length(eg_vector2_t a);
__forceinline float eg_vector2_length2(eg_vector2_t a);
__forceinline void eg_vector2_print(eg_vector2_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <engine/math/eg_vector2.inl>

#endif // EG_MATH_VECTOR2_H

#ifndef EG_MATH_IVECTOR3_H
#define EG_MATH_IVECTOR3_H

#include <engine/eg_fwd.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline eg_ivector3_t eg_ivector3_zero(void);
__forceinline eg_ivector3_t eg_ivector3_one(void);
__forceinline eg_ivector3_t eg_ivector3_right(void);
__forceinline eg_ivector3_t eg_ivector3_up(void);
__forceinline eg_ivector3_t eg_ivector3_front(void);
__forceinline eg_ivector3_t eg_ivector3_left(void);
__forceinline eg_ivector3_t eg_ivector3_down(void);
__forceinline eg_ivector3_t eg_ivector3_back(void);
__forceinline eg_ivector3_t eg_ivector3_xyz(int32_t x, int32_t y, int32_t z);
__forceinline eg_ivector3_t eg_ivector3_negate(eg_ivector3_t a);
__forceinline eg_ivector3_t eg_ivector3_add(eg_ivector3_t a, eg_ivector3_t b);
__forceinline eg_ivector3_t eg_ivector3_sub(eg_ivector3_t a, eg_ivector3_t b);
__forceinline eg_ivector3_t eg_ivector3_mul(eg_ivector3_t a, eg_ivector3_t b);
__forceinline eg_ivector3_t eg_ivector3_div(eg_ivector3_t a, eg_ivector3_t b);
__forceinline eg_ivector3_t eg_ivector3_adds(eg_ivector3_t a, int32_t b);
__forceinline eg_ivector3_t eg_ivector3_subs(eg_ivector3_t a, int32_t b);
__forceinline eg_ivector3_t eg_ivector3_muls(eg_ivector3_t a, int32_t b);
__forceinline eg_ivector3_t eg_ivector3_divs(eg_ivector3_t a, int32_t b);
__forceinline float eg_ivector3_dot(eg_ivector3_t a, eg_ivector3_t b);
__forceinline float eg_ivector3_length(eg_ivector3_t a);
__forceinline float eg_ivector3_length2(eg_ivector3_t a);
__forceinline void eg_ivector3_print(eg_ivector3_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <engine/math/eg_ivector3.inl>

#endif // EG_MATH_IVECTOR3_H

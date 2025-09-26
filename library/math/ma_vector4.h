#ifndef MA_VECTOR4_H
#define MA_VECTOR4_H

#include <math.h>
#include <stdio.h>

#include <library/math/ma_constants.h>
#include <library/math/ma_forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline vector4_t vector4_zero(void);
__forceinline vector4_t vector4_one(void);
__forceinline vector4_t vector4_xyzw(float x, float y, float z, float w);
__forceinline vector4_t vector4_negate(vector4_t a);
__forceinline vector4_t vector4_add(vector4_t a, vector4_t b);
__forceinline vector4_t vector4_sub(vector4_t a, vector4_t b);
__forceinline vector4_t vector4_mul(vector4_t a, vector4_t b);
__forceinline vector4_t vector4_div(vector4_t a, vector4_t b);
__forceinline vector4_t vector4_adds(vector4_t a, float b);
__forceinline vector4_t vector4_subs(vector4_t a, float b);
__forceinline vector4_t vector4_muls(vector4_t a, float b);
__forceinline vector4_t vector4_divs(vector4_t a, float b);
__forceinline vector4_t vector4_norm(vector4_t a);
__forceinline float vector4_dot(vector4_t a, vector4_t b);
__forceinline float vector4_length(vector4_t a);
__forceinline float vector4_length2(vector4_t a);
__forceinline void vector4_print(vector4_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <library/math/ma_vector4.inl>

#endif // MA_VECTOR4_H

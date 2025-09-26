#ifndef MA_VECTOR3_H
#define MA_VECTOR3_H

#include <math.h>
#include <stdio.h>

#include <library/math/ma_constants.h>
#include <library/math/ma_forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline vector3_t vector3_zero(void);
__forceinline vector3_t vector3_one(void);
__forceinline vector3_t vector3_right(void);
__forceinline vector3_t vector3_up(void);
__forceinline vector3_t vector3_front(void);
__forceinline vector3_t vector3_left(void);
__forceinline vector3_t vector3_down(void);
__forceinline vector3_t vector3_back(void);
__forceinline vector3_t vector3_xyz(float x, float y, float z);
__forceinline vector3_t vector3_negate(vector3_t a);
__forceinline vector3_t vector3_add(vector3_t a, vector3_t b);
__forceinline vector3_t vector3_sub(vector3_t a, vector3_t b);
__forceinline vector3_t vector3_mul(vector3_t a, vector3_t b);
__forceinline vector3_t vector3_div(vector3_t a, vector3_t b);
__forceinline vector3_t vector3_adds(vector3_t a, float b);
__forceinline vector3_t vector3_subs(vector3_t a, float b);
__forceinline vector3_t vector3_muls(vector3_t a, float b);
__forceinline vector3_t vector3_divs(vector3_t a, float b);
__forceinline vector3_t vector3_norm(vector3_t a);
__forceinline vector3_t vector3_cross(vector3_t a, vector3_t b);
__forceinline vector3_t vector3_rotate(vector3_t a, quaternion_t b);
__forceinline float vector3_dot(vector3_t a, vector3_t b);
__forceinline float vector3_length(vector3_t a);
__forceinline float vector3_length2(vector3_t a);
__forceinline void vector3_print(vector3_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <library/math/ma_vector3.inl>

#endif // MA_VECTOR3_H

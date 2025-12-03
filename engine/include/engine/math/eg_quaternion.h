#ifndef EG_MATH_QUATERNION_H
#define EG_MATH_QUATERNION_H

#include <engine/eg_fwd.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline eg_quaternion_t eg_quaternion_zero(void);
__forceinline eg_quaternion_t eg_quaternion_identity(void);
__forceinline eg_quaternion_t eg_quaternion_xyzw(float x, float y, float z, float w);
__forceinline eg_quaternion_t eg_quaternion_add(eg_quaternion_t a, eg_quaternion_t b);
__forceinline eg_quaternion_t eg_quaternion_adds(eg_quaternion_t a, float b);
__forceinline eg_quaternion_t eg_quaternion_mul(eg_quaternion_t a, eg_quaternion_t b);
__forceinline eg_quaternion_t eg_quaternion_muls(eg_quaternion_t a, float b);
__forceinline eg_quaternion_t eg_quaternion_conjugate(eg_quaternion_t a);
__forceinline eg_vector3_t eg_quaternion_right(eg_quaternion_t a);
__forceinline eg_vector3_t eg_quaternion_up(eg_quaternion_t a);
__forceinline eg_vector3_t eg_quaternion_front(eg_quaternion_t a);
__forceinline eg_vector3_t eg_quaternion_left(eg_quaternion_t a);
__forceinline eg_vector3_t eg_quaternion_down(eg_quaternion_t a);
__forceinline eg_vector3_t eg_quaternion_back(eg_quaternion_t a);
__forceinline eg_vector3_t eg_eg_quaternion_to_euler_angles(eg_quaternion_t a);
__forceinline eg_vector3_t eg_eg_quaternion_to_euler_angles_xyzw(float x, float y, float z, float w);
__forceinline eg_quaternion_t eg_quaternion_from_euler_angles(eg_vector3_t a);
__forceinline eg_quaternion_t eg_quaternion_from_euler_angles_pyr(float p, float y, float r);
__forceinline eg_quaternion_t eg_quaternion_angle_axis(float a, eg_vector3_t b);
__forceinline eg_quaternion_t eg_quaternion_norm(eg_quaternion_t a);
__forceinline float eg_quaternion_dot(eg_quaternion_t a, eg_quaternion_t b);
__forceinline float eg_quaternion_length(eg_quaternion_t a);
__forceinline float eg_quaternion_length2(eg_quaternion_t a);
__forceinline void eg_quaternion_print(eg_quaternion_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <engine/math/eg_quaternion.inl>

#endif // EG_MATH_QUATERNION_H

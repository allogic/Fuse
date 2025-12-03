#ifndef EG_MATH_MATRIX4_H
#define EG_MATH_MATRIX4_H

#include <engine/eg_fwd.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

__forceinline eg_matrix4_t eg_matrix4_zero(void);
__forceinline eg_matrix4_t eg_matrix4_identity(void);
__forceinline eg_matrix4_t eg_matrix4_inverse(eg_matrix4_t a);
__forceinline eg_vector3_t eg_matrix4_position(eg_matrix4_t a);
__forceinline eg_quaternion_t eg_matrix4_rotation(eg_matrix4_t a);
__forceinline eg_vector3_t eg_matrix4_euler_angles(eg_matrix4_t a);
__forceinline eg_vector3_t eg_matrix4_scale(eg_matrix4_t a);
__forceinline eg_matrix4_t eg_matrix4_mul(eg_matrix4_t a, eg_matrix4_t b);
__forceinline eg_vector4_t eg_matrix4_mul_vector(eg_matrix4_t a, eg_vector4_t b);
__forceinline eg_matrix4_t eg_matrix4_ortho(float left, float right, float bottom, float top, float near_z, float far_z);
__forceinline eg_matrix4_t eg_matrix4_persp(float fov, float aspect_ratio, float near_z, float far_z);
__forceinline eg_matrix4_t eg_matrix4_look_at(eg_vector3_t eye, eg_vector3_t center, eg_vector3_t up);
__forceinline void eg_matrix4_decompose(eg_matrix4_t a, eg_vector3_t *p, eg_quaternion_t *r, eg_vector3_t *s);
__forceinline void eg_matrix4_print(eg_matrix4_t a);

#ifdef __cplusplus
}
#endif // __cplusplus

#include <engine/math/eg_matrix4.inl>

#endif // EG_MATH_MATRIX4_H

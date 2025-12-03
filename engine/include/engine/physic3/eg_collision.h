#ifndef EG_PHYSIC3_COLLISION_H
#define EG_PHYSIC3_COLLISION_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

eg_hit3_t eg_sphere_triangle_collision(eg_vector3_t p, float r, eg_vector3_t a, eg_vector3_t b, eg_vector3_t c);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_PHYSIC3_COLLISION_H

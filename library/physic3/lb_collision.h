#ifndef LB_PHYSIC3_COLLISION_H
#define LB_PHYSIC3_COLLISION_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

hit3_t sphere_triangle_collision(vector3_t p, float r, vector3_t a, vector3_t b, vector3_t c);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LB_PHYSIC3_COLLISION_H

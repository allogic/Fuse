#ifndef LB_PHYSIC3_COLLISION_H
#define LB_PHYSIC3_COLLISION_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

lb_hit3_t lb_sphere_triangle_collision(lb_vector3_t p, float r, lb_vector3_t a, lb_vector3_t b, lb_vector3_t c);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LB_PHYSIC3_COLLISION_H

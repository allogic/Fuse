#ifndef EG_PHYSICS_COLLISION_H
#define EG_PHYSICS_COLLISION_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct collision_t {
  uint8_t hit;
  vector3_t point;
  vector3_t normal;
  float penetration;
} collision_t;

vector3_t closest_point_on_triangle(vector3_t p, vector3_t a, vector3_t b, vector3_t c);

collision_t sphere_triangle_collision(vector3_t p, float r, vector3_t a, vector3_t b, vector3_t c);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_PHYSICS_COLLISION_H

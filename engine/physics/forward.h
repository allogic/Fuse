#ifndef PHYSICS_FORWARD_H
#define PHYSICS_FORWARD_H

#include <stdint.h>

#include <library/math/api.h>

typedef struct physics_ray_t {
  math_vector3_t origin;
  math_vector3_t direction;
} physics_ray_t;

typedef struct physics_aabb_t {
  math_vector3_t min;
  math_vector3_t max;
} physics_aabb_t;

#endif // PHYSICS_FORWARD_H

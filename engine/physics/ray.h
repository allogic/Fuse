#ifndef PHYSICS_RAY_H
#define PHYSICS_RAY_H

#include <stdio.h>

#include <engine/physics/constants.h>
#include <engine/physics/forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

physics_ray_t physics_ray_from(vector3_t origin, vector3_t direction);
void physics_ray_cast(physics_ray_t ray, int32_t max_steps);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PHYSICS_RAY_H

#ifndef CAMERA_H
#define CAMERA_H

#include <stdint.h>

#include <engine/math/api.h>

typedef struct camera_t {
  float fov;
  float near_z;
  float far_z;
  int32_t ray_steps;
} camera_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

camera_t *camera_create(void);
void camera_destroy(camera_t *camera);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CAMERA_H

#ifndef EG_CAMERA_COMPONENTS_H
#define EG_CAMERA_COMPONENTS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct camera_t {
  float fov;
  float near_z;
  float far_z;
} camera_t;

ECS_COMPONENT_DECLARE(camera_t);

void camera_init(camera_t *camera);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_CAMERA_COMPONENTS_H

#ifndef EG_CAMERA_COMPONENT_H
#define EG_CAMERA_COMPONENT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct eg_camera_t {
  float fov;
  float near_z;
  float far_z;
} eg_camera_t;

ECS_COMPONENT_DECLARE(eg_camera_t);

void eg_camera_init(eg_camera_t *camera);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_CAMERA_COMPONENT_H

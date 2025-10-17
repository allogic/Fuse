#ifndef EG_CAMERA_H
#define EG_CAMERA_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct camera_t {
  float fov;
  float near_z;
  float far_z;
} camera_t;

ECS_COMPONENT_DECLARE(camera_t);

camera_t camera_create(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_CAMERA_H

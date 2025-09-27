#ifndef EG_CAMERA_H
#define EG_CAMERA_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

camera_t *camera_create(void);
void camera_destroy(camera_t *camera);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_CAMERA_H

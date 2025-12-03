#include <engine/eg_pch.h>

#include <engine/component/eg_camera.h>

void eg_camera_create(eg_camera_t *camera) {
  camera->fov = 45.0F;
  camera->near_z = 1.0F;
  camera->far_z = 1.0E+6F;
  camera->width = 512;
  camera->height = 512;
  camera->viewport = eg_viewport_create(camera->width, camera->height);
}
void eg_camera_destroy(eg_camera_t *camera) {
  eg_viewport_destroy(camera->viewport);
}

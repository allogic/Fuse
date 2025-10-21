#include <engine/eg_pch.h>

#include <engine/component/eg_camera.h>

void camera_init(camera_t *camera) {
  camera->fov = 45.0F;
  camera->near_z = 1.0F;
  camera->far_z = 1.0E+6F;
}

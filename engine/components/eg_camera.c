#include <engine/eg_pch.h>

#include <engine/components/eg_camera.h>

camera_t camera_create(void) {
  camera_t camera = {0};

  camera.fov = 45.0F;
  camera.near_z = 1.0F;
  camera.far_z = 1.0E+6F;

  return camera;
}

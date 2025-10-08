#include <engine/eg_pch.h>
#include <engine/eg_camera.h>

camera_t *camera_create(void) {
  camera_t *camera = (camera_t *)heap_alloc(sizeof(camera_t), 0, 0);

  memset(camera, 0, sizeof(camera_t));

  camera->fov = 45.0F;
  camera->near_z = 1.0F;
  camera->far_z = 1.0E+6F;

  return camera;
}
void camera_destroy(camera_t *camera) {
  heap_free(camera);
}

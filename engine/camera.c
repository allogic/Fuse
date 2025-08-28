#include <stdlib.h>

#include <engine/camera.h>

#include <library/core/api.h>

#include <library/math/api.h>

camera_t *camera_create(void) {
  camera_t *camera = (camera_t *)heap_alloc(sizeof(camera_t));
  camera->fov = 45.0F;
  camera->near_z = 1.0F;
  camera->far_z = 1.0E+6F;
  camera->ray_steps = 200;

  return camera;
}
void camera_destroy(camera_t *camera) {
  heap_free(camera);
}

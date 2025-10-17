#include <engine/eg_pch.h>
#include <engine/eg_scene_manager.h>

static scene_t *s_scene_manager_active_scene = 0;

void scene_manager_create(void) {
}
scene_t *scene_manager_active_scene(void) {
  return s_scene_manager_active_scene;
}
void scene_manager_set_active_scene(scene_t *scene) {
  s_scene_manager_active_scene = scene;
}
void scene_manager_destroy(void) {
}

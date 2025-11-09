#include <editor/ed_pch.h>
#include <editor/ed_ecs_compat.h>

camera_t const *ecs_camera(ecs_world_t const *world, ecs_entity_t entity) {
  return ecs_get(world, entity, camera_t);
}
camera_t *ecs_camera_mut(ecs_world_t const *world, ecs_entity_t entity) {
  return ecs_get_mut(world, entity, camera_t);
}

editor_controller_t const *ecs_editor_controller(ecs_world_t const *world, ecs_entity_t entity) {
  return ecs_get(world, entity, editor_controller_t);
}
editor_controller_t *ecs_editor_controller_mut(ecs_world_t const *world, ecs_entity_t entity) {
  return ecs_get_mut(world, entity, editor_controller_t);
}

rigidbody_t const *ecs_rigidbody(ecs_world_t const *world, ecs_entity_t entity) {
  return ecs_get(world, entity, rigidbody_t);
}
rigidbody_t *ecs_rigidbody_mut(ecs_world_t const *world, ecs_entity_t entity) {
  return ecs_get_mut(world, entity, rigidbody_t);
}

transform_t const *ecs_transform(ecs_world_t const *world, ecs_entity_t entity) {
  return ecs_get(world, entity, transform_t);
}
transform_t *ecs_transform_mut(ecs_world_t const *world, ecs_entity_t entity) {
  return ecs_get_mut(world, entity, transform_t);
}

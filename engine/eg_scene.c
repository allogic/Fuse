#include <engine/eg_pch.h>
#include <engine/eg_scene.h>

#include <engine/system/eg_controller.h>
#include <engine/system/eg_rigidbody.h>

static void eg_scene_create_root(eg_scene_t *scene);
static void eg_scene_create_player(eg_scene_t *scene);

eg_scene_t *eg_scene_create(eg_context_t *context) {
  eg_scene_t *scene = (eg_scene_t *)heap_alloc(sizeof(eg_scene_t), 1, 0);

  scene->world = ecs_init_w_args(0, 0);

  ECS_COMPONENT_DEFINE(scene->world, eg_transform_t);
  ECS_COMPONENT_DEFINE(scene->world, eg_rigidbody_t);
  ECS_COMPONENT_DEFINE(scene->world, eg_camera_t);
  ECS_COMPONENT_DEFINE(scene->world, eg_editor_controller_t);

  scene->controller_system = ecs_system(scene->world, {.ctx = context,
                                                       .entity = ecs_entity(scene->world, {.name = "controller system"}),
                                                       .query.terms = {
                                                         {.id = ecs_id(eg_transform_t)},
                                                         {.id = ecs_id(eg_rigidbody_t)},
                                                         {.id = ecs_id(eg_editor_controller_t)}},
                                                       .run = eg_editor_controller_update});

  scene->rigidbody_system = ecs_system(scene->world, {.ctx = context,
                                                      .entity = ecs_entity(scene->world, {.name = "rigidbody system"}),
                                                      .query.terms = {
                                                        {.id = ecs_id(eg_transform_t)},
                                                        {.id = ecs_id(eg_rigidbody_t)}},
                                                      .run = eg_rigidbody_update});

  eg_scene_create_root(scene);
  eg_scene_create_player(scene);

  return scene;
}
void eg_scene_update(eg_scene_t *scene) {
  EG_PROFILER_SCOPE_BEGIN(EG_PROFILER_SAMPLE_LANE_SCENE);

  ecs_run(scene->world, scene->controller_system, 0.0F, 0);
  ecs_run(scene->world, scene->rigidbody_system, 0.0F, 0);

  EG_PROFILER_SCOPE_END(EG_PROFILER_SAMPLE_LANE_SCENE);
}
void eg_scene_destroy(eg_scene_t *scene) {
  ecs_fini(scene->world);

  heap_free(scene);
}

eg_camera_t const *eg_scene_camera(eg_scene_t *scene, ecs_entity_t entity) {
  return ecs_get(scene->world, entity, eg_camera_t);
}
eg_camera_t *eg_scene_camera_mut(eg_scene_t *scene, ecs_entity_t entity) {
  return ecs_get_mut(scene->world, entity, eg_camera_t);
}

eg_editor_controller_t const *eg_scene_editor_controller(eg_scene_t *scene, ecs_entity_t entity) {
  return ecs_get(scene->world, entity, eg_editor_controller_t);
}
eg_editor_controller_t *eg_scene_editor_controller_mut(eg_scene_t *scene, ecs_entity_t entity) {
  return ecs_get_mut(scene->world, entity, eg_editor_controller_t);
}

eg_rigidbody_t const *eg_scene_rigidbody(eg_scene_t *scene, ecs_entity_t entity) {
  return ecs_get(scene->world, entity, eg_rigidbody_t);
}
eg_rigidbody_t *eg_scene_rigidbody_mut(eg_scene_t *scene, ecs_entity_t entity) {
  return ecs_get_mut(scene->world, entity, eg_rigidbody_t);
}

eg_transform_t const *eg_scene_transform(eg_scene_t *scene, ecs_entity_t entity) {
  return ecs_get(scene->world, entity, eg_transform_t);
}
eg_transform_t *eg_scene_transform_mut(eg_scene_t *scene, ecs_entity_t entity) {
  return ecs_get_mut(scene->world, entity, eg_transform_t);
}

static void eg_scene_create_root(eg_scene_t *scene) {
  scene->root = ecs_entity(scene->world, {.name = "root"});
}
static void eg_scene_create_player(eg_scene_t *scene) {
  scene->player = ecs_entity(scene->world, {.parent = scene->root, .name = "player"});

  ecs_add(scene->world, scene->player, eg_transform_t);
  ecs_add(scene->world, scene->player, eg_rigidbody_t);
  ecs_add(scene->world, scene->player, eg_camera_t);
  ecs_add(scene->world, scene->player, eg_editor_controller_t);

  eg_transform_t *transform = ecs_get_mut(scene->world, scene->player, eg_transform_t);
  eg_rigidbody_t *rigidbody = ecs_get_mut(scene->world, scene->player, eg_rigidbody_t);
  eg_camera_t *camera = ecs_get_mut(scene->world, scene->player, eg_camera_t);
  eg_editor_controller_t *editor_controller = ecs_get_mut(scene->world, scene->player, eg_editor_controller_t);

  eg_transform_init(transform);
  eg_transform_set_position_xyz(transform, 0.0F, 0.0F, -10.0F);

  eg_rigidbody_init(rigidbody);

  eg_camera_init(camera);

  eg_editor_controller_init(editor_controller);
}

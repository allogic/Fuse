#include <engine/eg_pch.h>
#include <engine/eg_scene.h>

#include <engine/system/eg_controller.h>
#include <engine/system/eg_rigidbody.h>

static void scene_create_player(scene_t *scene);

scene_t *scene_create(context_t *context) {
  scene_t *scene = (scene_t *)heap_alloc(sizeof(scene_t), 1, 0);

  scene->world = ecs_init_w_args(0, 0);

  ECS_COMPONENT_DEFINE(scene->world, transform_t);
  ECS_COMPONENT_DEFINE(scene->world, rigidbody_t);
  ECS_COMPONENT_DEFINE(scene->world, camera_t);
  ECS_COMPONENT_DEFINE(scene->world, editor_controller_t);

  scene->controller_system = ecs_system(scene->world, {.ctx = context,
                                                       .entity = ecs_entity(scene->world, {.name = "controller system"}),
                                                       .query.terms = {
                                                         {.id = ecs_id(transform_t)},
                                                         {.id = ecs_id(rigidbody_t)},
                                                         {.id = ecs_id(editor_controller_t)}},
                                                       .run = editor_controller_update});

  scene->rigidbody_system = ecs_system(scene->world, {.ctx = context,
                                                      .entity = ecs_entity(scene->world, {.name = "rigidbody system"}),
                                                      .query.terms = {
                                                        {.id = ecs_id(transform_t)},
                                                        {.id = ecs_id(rigidbody_t)}},
                                                      .run = rigidbody_update});

  scene_create_player(scene);

  return scene;
}
void scene_update(scene_t *scene) {
  ecs_run(scene->world, scene->controller_system, 0.0F, 0);
  ecs_run(scene->world, scene->rigidbody_system, 0.0F, 0);
}
void scene_destroy(scene_t *scene) {
  ecs_fini(scene->world);

  heap_free(scene);
}

static void scene_create_player(scene_t *scene) {
  scene->player = ecs_entity(scene->world, {.name = "player"});

  ecs_add(scene->world, scene->player, transform_t);
  ecs_add(scene->world, scene->player, rigidbody_t);
  ecs_add(scene->world, scene->player, camera_t);
  ecs_add(scene->world, scene->player, editor_controller_t);

  transform_t *transform = ecs_get_mut(scene->world, scene->player, transform_t);
  rigidbody_t *rigidbody = ecs_get_mut(scene->world, scene->player, rigidbody_t);
  camera_t *camera = ecs_get_mut(scene->world, scene->player, camera_t);
  editor_controller_t *editor_controller = ecs_get_mut(scene->world, scene->player, editor_controller_t);

  transform_init(transform);
  transform_set_position_xyz(transform, 0.0F, 0.0F, -10.0F);

  rigidbody_init(rigidbody);

  camera_init(camera);

  editor_controller_init(editor_controller);
}

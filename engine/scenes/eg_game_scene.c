#include <engine/eg_pch.h>

#include <engine/scenes/eg_game_scene.h>

static void game_scene_create_default_player(scene_t *scene);
static void game_scene_create_default_queries(scene_t *scene);

scene_t *game_scene_create(void) {
  scene_t *scene = (scene_t *)heap_alloc(sizeof(scene_t), 1, 0);

  scene->ecs = ecs_init_w_args(0, 0);

  ECS_COMPONENT_DEFINE(scene->ecs, transform_t);
  ECS_COMPONENT_DEFINE(scene->ecs, camera_t);
  ECS_COMPONENT_DEFINE(scene->ecs, editor_controller_t);

  game_scene_create_default_player(scene);
  game_scene_create_default_queries(scene);

  return scene;
}
void game_scene_update(scene_t *scene) {
  // TODO: use pipelines with multithreading?
  // #define FLECS_PIPELINE
  // ecs_progress(scene->ecs, 0);

  // ecs_iter_t iter = ecs_query_iter(scene->ecs, scene->query);
  //
  // while (ecs_query_next(&iter)) {
  //
  //   transform_t *transform = ecs_field(&iter, transform_t, 0);
  //
  //   for (int i = 0; i < iter.count; i++) {
  //     p[i].x += v[i].x;
  //     p[i].y += v[i].y;
  //
  //     printf("%s: {%f, %f}\n", ecs_get_name(scene->ecs, iter.entities[i]), p[i].x, p[i].y);
  //   }
  // }
}
void game_scene_destroy(scene_t *scene) {
  // ecs_query_fini(scene->query);

  ecs_fini(scene->ecs);

  heap_free(scene);
}

static void game_scene_create_default_player(scene_t *scene) {
  scene->player = ecs_entity(scene->ecs, {.name = "player"});

  transform_t transform = transform_create();
  camera_t camera = camera_create();
  editor_controller_t editor_controller = editor_controller_create();

  ecs_set(scene->ecs, scene->player, transform_t, transform);
  ecs_set(scene->ecs, scene->player, camera_t, camera);
  ecs_set(scene->ecs, scene->player, editor_controller_t, editor_controller);
}
static void game_scene_create_default_queries(scene_t *scene) {
  // scene->qeury = ecs_query(ecs, {.terms = {
  //                                  {.id = ecs_id(transform_t)},
  //                                  // {.id = ecs_id(Velocity), .inout = EcsIn}},
  //                                .cache_kind = EcsQueryCacheAuto});
}

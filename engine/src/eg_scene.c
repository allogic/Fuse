#include <engine/eg_pch.h>

struct eg_scene_t {
  uint64_t index;
  eg_scene_asset_t asset;
  ecs_world_t *world;
  ecs_entity_t root;
  ecs_entity_t player;
  ecs_entity_t controller_system;
  ecs_entity_t rigidbody_system;
};

static void eg_scene_create_root(eg_scene_t *scene);
static void eg_scene_create_player(eg_scene_t *scene);

static void eg_scene_cleanup(eg_scene_t *scene, ecs_entity_t entity);

eg_scene_t *eg_scene_create(eg_scene_asset_id_t scene_asset_id) {
  eg_scene_t *scene = (eg_scene_t *)eg_heap_alloc(sizeof(eg_scene_t), 1, 0);

  eg_context_set_scene(scene, &scene->index);

  scene->asset = eg_database_load_scene_asset_by_id(scene_asset_id);
  scene->world = ecs_init_w_args(0, 0);

  ECS_COMPONENT_DEFINE(scene->world, eg_camera_t);
  ECS_COMPONENT_DEFINE(scene->world, eg_editor_controller_t);
  ECS_COMPONENT_DEFINE(scene->world, eg_rigidbody_t);
  ECS_COMPONENT_DEFINE(scene->world, eg_script_t);
  ECS_COMPONENT_DEFINE(scene->world, eg_transform_t);

  scene->controller_system = ecs_system(scene->world, {.ctx = 0,
                                                       .entity = ecs_entity(scene->world, {.name = "controller_system"}),
                                                       .query.terms = {
                                                         {.id = ecs_id(eg_transform_t)},
                                                         {.id = ecs_id(eg_rigidbody_t)},
                                                         {.id = ecs_id(eg_editor_controller_t)}},
                                                       .run = eg_editor_controller_update});
  scene->rigidbody_system = ecs_system(scene->world, {.ctx = 0,
                                                      .entity = ecs_entity(scene->world, {.name = "rigidbody_system"}),
                                                      .query.terms = {
                                                        {.id = ecs_id(eg_transform_t)},
                                                        {.id = ecs_id(eg_rigidbody_t)}},
                                                      .run = eg_rigidbody_update});

  eg_scene_create_root(scene);
  eg_scene_create_player(scene);

  return scene;
}
void eg_scene_update(eg_scene_t *scene) {
  ecs_run(scene->world, scene->controller_system, 0.0F, 0);
  ecs_run(scene->world, scene->rigidbody_system, 0.0F, 0);
}
void eg_scene_destroy(eg_scene_t *scene) {
  eg_scene_t **scenes = eg_context_scenes();

  scenes[scene->index] = 0;

  eg_scene_cleanup(scene, scene->root);

  ecs_fini(scene->world);

  eg_database_destroy_scene_asset(&scene->asset);

  eg_heap_free(scene);
}

ecs_world_t *eg_scene_world(eg_scene_t *scene) {
  return scene->world;
}
ecs_entity_t eg_scene_root(eg_scene_t *scene) {
  return scene->root;
}
ecs_entity_t eg_scene_player(eg_scene_t *scene) {
  return scene->player;
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

eg_script_t const *eg_scene_script(eg_scene_t *scene, ecs_entity_t entity) {
  return ecs_get(scene->world, entity, eg_script_t);
}
eg_script_t *eg_scene_script_mut(eg_scene_t *scene, ecs_entity_t entity) {
  return ecs_get_mut(scene->world, entity, eg_script_t);
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

  eg_transform_create(transform);
  eg_transform_set_position_xyz(transform, 0.0F, 0.0F, -10.0F);

  eg_rigidbody_create(rigidbody);

  eg_camera_create(camera);

  eg_editor_controller_create(editor_controller);
}

static void eg_scene_cleanup(eg_scene_t *scene, ecs_entity_t entity) {
  eg_camera_t *camera = ecs_get_mut(scene->world, entity, eg_camera_t);
  eg_editor_controller_t *editor_controller = ecs_get_mut(scene->world, entity, eg_editor_controller_t);
  eg_rigidbody_t *rigidbody = ecs_get_mut(scene->world, entity, eg_rigidbody_t);
  eg_script_t *script = ecs_get_mut(scene->world, entity, eg_script_t);
  eg_transform_t *transform = ecs_get_mut(scene->world, entity, eg_transform_t);

  if (camera) {
    eg_camera_destroy(camera);
  }

  if (editor_controller) {
    eg_editor_controller_destroy(editor_controller);
  }

  if (rigidbody) {
    eg_rigidbody_destroy(rigidbody);
  }

  if (script) {
    eg_script_destroy(script);
  }

  if (transform) {
    eg_transform_destroy(transform);
  }

  ecs_iter_t child_iter = ecs_children(scene->world, entity);

  ecs_children_next(&child_iter);

  do {

    uint32_t child_index = 0;
    uint32_t child_count = child_iter.count;

    while (child_index < child_count) {

      eg_scene_cleanup(scene, child_iter.entities[child_index]);

      child_index++;
    }

  } while (ecs_children_next(&child_iter));
}

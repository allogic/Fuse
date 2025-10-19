#ifndef LB_FWD_H
#define LB_FWD_H

typedef struct scene_t {
  ecs_world_t *world;
  ecs_entity_t player;
  ecs_entity_t controller_system;
  ecs_entity_t rigidbody_system;
} scene_t;

#endif // LB_FWD_H

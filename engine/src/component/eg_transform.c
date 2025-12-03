#include <engine/eg_pch.h>

#include <engine/component/eg_transform.h>

void eg_transform_create(eg_transform_t *transform) {
  transform->local_position = eg_vector3_zero();
  transform->local_rotation = eg_quaternion_identity();
  transform->local_scale = eg_vector3_one();
  transform->world_position = eg_vector3_zero();
  transform->world_rotation = eg_quaternion_identity();
  transform->world_scale = eg_vector3_one();
}
void eg_transform_destroy(eg_transform_t *transform) {
}

eg_matrix4_t eg_transform_matrix(eg_transform_t *transform) {
  float xx = transform->world_rotation.x * transform->world_rotation.x;
  float yy = transform->world_rotation.y * transform->world_rotation.y;
  float zz = transform->world_rotation.z * transform->world_rotation.z;

  float xy = transform->world_rotation.x * transform->world_rotation.y;
  float xz = transform->world_rotation.x * transform->world_rotation.z;
  float yz = transform->world_rotation.y * transform->world_rotation.z;

  float wx = transform->world_rotation.w * transform->world_rotation.x;
  float wy = transform->world_rotation.w * transform->world_rotation.y;
  float wz = transform->world_rotation.w * transform->world_rotation.z;

  eg_matrix4_t r = {
    (1.0F - 2.0F * (yy + zz)) * transform->world_scale.x,
    2.0F * (xy - wz) * transform->world_scale.y,
    2.0F * (xz + wy) * transform->world_scale.z,
    0.0F,
    2.0F * (xy + wz) * transform->world_scale.x,
    (1.0F - 2.0F * (xx + zz)) * transform->world_scale.y,
    2.0F * (yz - wx) * transform->world_scale.z,
    0.0F,
    2.0F * (xz - wy) * transform->world_scale.x,
    2.0F * (yz + wx) * transform->world_scale.y,
    (1.0F - 2.0F * (xx + yy)) * transform->world_scale.z,
    0.0F,
    transform->world_position.x,
    transform->world_position.y,
    transform->world_position.z,
    1.0F,
  };

  return r;
}

void eg_transform_compute_world_position(ecs_world_t *world, ecs_entity_t entity) {
  ecs_entity_t parent = ecs_get_parent(world, entity);

  eg_transform_t *transform = ecs_get_mut(world, entity, eg_transform_t);
  eg_transform_t *parent_transform = ecs_get_mut(world, parent, eg_transform_t);

  if (parent_transform) {
    transform->world_position = eg_vector3_add(parent_transform->world_position, eg_vector3_rotate(transform->local_position, parent_transform->world_rotation));
  } else {
    transform->world_position = transform->local_position;
  }

  ecs_iter_t iter = ecs_children(world, entity);

  while (ecs_children_next(&iter)) {

    uint64_t child_index = 0;
    uint64_t child_count = iter.count;

    while (child_index < child_count) {

      ecs_entity_t child = iter.entities[child_index];

      eg_transform_compute_world_position(world, child);

      child_index++;
    }
  }
}
void eg_transform_compute_world_rotation(ecs_world_t *world, ecs_entity_t entity) {
  ecs_entity_t parent = ecs_get_parent(world, entity);

  eg_transform_t *transform = ecs_get_mut(world, entity, eg_transform_t);
  eg_transform_t *parent_transform = ecs_get_mut(world, parent, eg_transform_t);

  if (parent_transform) {
    transform->world_rotation = eg_quaternion_mul(transform->local_rotation, parent_transform->world_rotation);
  } else {
    transform->world_rotation = transform->local_rotation;
  }

  ecs_iter_t iter = ecs_children(world, entity);

  while (ecs_children_next(&iter)) {

    uint64_t child_index = 0;
    uint64_t child_count = iter.count;

    while (child_index < child_count) {

      ecs_entity_t child = iter.entities[child_index];

      eg_transform_compute_world_rotation(world, child);

      child_index++;
    }
  }
}
void eg_transform_compute_world_scale(ecs_world_t *world, ecs_entity_t entity) {
  ecs_entity_t parent = ecs_get_parent(world, entity);

  eg_transform_t *transform = ecs_get_mut(world, entity, eg_transform_t);
  eg_transform_t *parent_transform = ecs_get_mut(world, parent, eg_transform_t);

  if (parent_transform) {
    transform->world_scale = eg_vector3_mul(transform->local_scale, parent_transform->world_scale);
  } else {
    transform->world_scale = transform->local_scale;
  }

  ecs_iter_t iter = ecs_children(world, entity);

  while (ecs_children_next(&iter)) {

    uint64_t child_index = 0;
    uint64_t child_count = iter.count;

    while (child_index < child_count) {

      ecs_entity_t child = iter.entities[child_index];

      eg_transform_compute_world_scale(world, child);

      child_index++;
    }
  }
}

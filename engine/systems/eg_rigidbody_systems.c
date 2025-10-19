#include <engine/eg_pch.h>

#include <engine/systems/eg_rigidbody_systems.h>

static void rigidbody_system_handle_linear_velocity(transform_t *transform, rigidbody_t *rigidbody);
static void rigidbody_system_handle_angular_velocity(transform_t *transform, rigidbody_t *rigidbody);

void rigidbody_system_update(ecs_iter_t *iter) {
  while (ecs_query_next(iter)) {

    transform_t *transforms = ecs_field(iter, transform_t, 0);
    rigidbody_t *rigidbodies = ecs_field(iter, rigidbody_t, 1);

    uint64_t entity_index = 0;
    uint64_t entity_count = iter->count;

    while (entity_index < entity_count) {

      rigidbody_system_handle_linear_velocity(&transforms[entity_index], &rigidbodies[entity_index]);
      rigidbody_system_handle_angular_velocity(&transforms[entity_index], &rigidbodies[entity_index]);

      entity_index++;
    }
  }
}

static void rigidbody_system_handle_linear_velocity(transform_t *transform, rigidbody_t *rigidbody) {
  vector3_t velocity = rigidbody->linear_velocity;
  vector3_t velocity_step = vector3_muls(vector3_muls(rigidbody->linear_velocity, rigidbody->linear_drag), (float)g_globals.context_delta_time);

  transform->local_position = vector3_add(transform->local_position, velocity);
  rigidbody->linear_velocity = vector3_sub(rigidbody->linear_velocity, velocity_step);
}
static void rigidbody_system_handle_angular_velocity(transform_t *transform, rigidbody_t *rigidbody) {
  vector3_t velocity = rigidbody->angular_velocity;
  vector3_t velocity_step = vector3_muls(vector3_muls(rigidbody->angular_velocity, rigidbody->angular_drag), (float)g_globals.context_delta_time);

  quaternion_t qp = quaternion_angle_axis(velocity.x, transform_local_right(transform));
  quaternion_t qy = quaternion_angle_axis(velocity.y, vector3_up());

  quaternion_t qd = quaternion_mul(qy, qp);

  transform->local_rotation = quaternion_norm(quaternion_mul(qd, transform->local_rotation));
  rigidbody->angular_velocity = vector3_sub(rigidbody->angular_velocity, velocity_step);
}

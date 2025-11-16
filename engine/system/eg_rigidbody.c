#include <engine/eg_pch.h>

#include <engine/system/eg_rigidbody.h>

static void eg_rigidbody_handle_linear_velocity(eg_context_t *context, eg_transform_t *transform, eg_rigidbody_t *rigidbody);
static void eg_rigidbody_handle_angular_velocity(eg_context_t *context, eg_transform_t *transform, eg_rigidbody_t *rigidbody);

void eg_rigidbody_update(ecs_iter_t *iter) {
  while (ecs_query_next(iter)) {

    eg_transform_t *transforms = ecs_field(iter, eg_transform_t, 0);
    eg_rigidbody_t *rigidbodies = ecs_field(iter, eg_rigidbody_t, 1);

    uint64_t entity_index = 0;
    uint64_t entity_count = iter->count;

    while (entity_index < entity_count) {

      eg_rigidbody_handle_linear_velocity(iter->ctx, &transforms[entity_index], &rigidbodies[entity_index]);
      eg_rigidbody_handle_angular_velocity(iter->ctx, &transforms[entity_index], &rigidbodies[entity_index]);

      entity_index++;
    }
  }
}

static void eg_rigidbody_handle_linear_velocity(eg_context_t *context, eg_transform_t *transform, eg_rigidbody_t *rigidbody) {
  vector3_t velocity = rigidbody->linear_velocity;
  vector3_t velocity_step = vector3_muls(vector3_muls(rigidbody->linear_velocity, rigidbody->linear_drag), (float)context->delta_time);

  transform->local_position = vector3_add(transform->local_position, velocity);
  rigidbody->linear_velocity = vector3_sub(rigidbody->linear_velocity, velocity_step);
}
static void eg_rigidbody_handle_angular_velocity(eg_context_t *context, eg_transform_t *transform, eg_rigidbody_t *rigidbody) {
  vector3_t velocity = rigidbody->angular_velocity;
  vector3_t velocity_step = vector3_muls(vector3_muls(rigidbody->angular_velocity, rigidbody->angular_drag), (float)context->delta_time);

  quaternion_t qp = quaternion_angle_axis(velocity.x, eg_transform_local_right(transform));
  quaternion_t qy = quaternion_angle_axis(velocity.y, vector3_up());

  quaternion_t qd = quaternion_mul(qy, qp);

  transform->local_rotation = quaternion_norm(quaternion_mul(qd, transform->local_rotation));
  rigidbody->angular_velocity = vector3_sub(rigidbody->angular_velocity, velocity_step);
}

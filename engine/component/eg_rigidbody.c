#include <engine/eg_pch.h>

#include <engine/component/eg_rigidbody.h>

void rigidbody_init(rigidbody_t *rigidbody) {
  rigidbody->linear_velocity = vector3_zero();
  rigidbody->angular_velocity = vector3_zero();
  rigidbody->linear_drag = 20.0F;
  rigidbody->angular_drag = 20.0F;
}

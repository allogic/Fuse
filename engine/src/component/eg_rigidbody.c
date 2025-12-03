#include <engine/eg_pch.h>

#include <engine/component/eg_rigidbody.h>

void eg_rigidbody_create(eg_rigidbody_t *rigidbody) {
  rigidbody->linear_velocity = eg_vector3_zero();
  rigidbody->angular_velocity = eg_vector3_zero();
  rigidbody->linear_drag = 20.0F;
  rigidbody->angular_drag = 20.0F;
}
void eg_rigidbody_destroy(eg_rigidbody_t *rigidbody) {
}

#ifndef EG_RIGIDBODY_COMPONENT_H
#define EG_RIGIDBODY_COMPONENT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct eg_rigidbody_t {
  eg_vector3_t linear_velocity;
  eg_vector3_t angular_velocity;
  float linear_drag;
  float angular_drag;
} eg_rigidbody_t;

ECS_COMPONENT_DECLARE(eg_rigidbody_t);

void eg_rigidbody_create(eg_rigidbody_t *rigidbody);
void eg_rigidbody_destroy(eg_rigidbody_t *rigidbody);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_RIGIDBODY_COMPONENT_H

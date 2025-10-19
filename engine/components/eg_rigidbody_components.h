#ifndef EG_RIGIDBODY_COMPONENTS_H
#define EG_RIGIDBODY_COMPONENTS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct rigidbody_t {
  vector3_t linear_velocity;
  vector3_t angular_velocity;
  float linear_drag;
  float angular_drag;
} rigidbody_t;

ECS_COMPONENT_DECLARE(rigidbody_t);

void rigidbody_init(rigidbody_t *rigidbody);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_RIGIDBODY_COMPONENTS_H

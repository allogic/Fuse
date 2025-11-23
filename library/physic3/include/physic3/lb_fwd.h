#ifndef LB_PHYSIC3_FWD
#define LB_PHYSIC3_FWD

typedef struct lb_hit3_t {
  uint8_t collision;
  lb_vector3_t point;
  lb_vector3_t normal;
  float penetration;
} lb_hit3_t;

#endif // LB_PHYSIC3_FWD

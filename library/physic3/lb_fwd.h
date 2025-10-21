#ifndef LB_PHYSIC3_FWD
#define LB_PHYSIC3_FWD

typedef struct hit3_t {
  uint8_t collision;
  vector3_t point;
  vector3_t normal;
  float penetration;
} hit3_t;

#endif // LB_PHYSIC3_FWD

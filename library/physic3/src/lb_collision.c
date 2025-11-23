#include <physic3/lb_pch.h>
#include <physic3/lb_collision.h>
#include <physic3/lb_geometry.h>

lb_hit3_t lb_sphere_triangle_collision(lb_vector3_t p, float r, lb_vector3_t a, lb_vector3_t b, lb_vector3_t c) {
  lb_hit3_t hit = {0};

  lb_vector3_t closest = lb_closest_point_on_triangle(p, a, b, c);
  lb_vector3_t diff = lb_vector3_sub(closest, p);

  float dist2 = lb_vector3_length2(diff);

  if (dist2 <= (r * r)) {

    float dist = sqrtf(dist2);

    hit.collision = 1;
    hit.point = closest;
    hit.normal = (dist > LB_EPSILON_4) ? lb_vector3_muls(diff, 1.0F / dist) : lb_vector3_up();
    hit.penetration = r - dist;
  }

  return hit;
}

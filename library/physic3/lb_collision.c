#include <library/lb_pch.h>

#include <library/physic3/lb_collision.h>
#include <library/physic3/lb_geometry.h>

hit3_t sphere_triangle_collision(vector3_t p, float r, vector3_t a, vector3_t b, vector3_t c) {
  hit3_t hit = {0};

  vector3_t closest = closest_point_on_triangle(p, a, b, c);
  vector3_t diff = vector3_sub(closest, p);

  float dist2 = vector3_length2(diff);

  if (dist2 <= (r * r)) {

    float dist = sqrtf(dist2);

    hit.collision = 1;
    hit.point = closest;
    hit.normal = (dist > EPSILON_4) ? vector3_muls(diff, 1.0F / dist) : vector3_up();
    hit.penetration = r - dist;
  }

  return hit;
}

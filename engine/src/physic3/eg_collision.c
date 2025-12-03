#include <engine/eg_pch.h>

#include <engine/physic3/eg_collision.h>
#include <engine/physic3/eg_geometry.h>

eg_hit3_t eg_sphere_triangle_collision(eg_vector3_t p, float r, eg_vector3_t a, eg_vector3_t b, eg_vector3_t c) {
  eg_hit3_t hit = {0};

  eg_vector3_t closest = eg_closest_point_on_triangle(p, a, b, c);
  eg_vector3_t diff = eg_vector3_sub(closest, p);

  float dist2 = eg_vector3_length2(diff);

  if (dist2 <= (r * r)) {

    float dist = sqrtf(dist2);

    hit.collision = 1;
    hit.point = closest;
    hit.normal = (dist > EG_EPSILON_4) ? eg_vector3_muls(diff, 1.0F / dist) : eg_vector3_up();
    hit.penetration = r - dist;
  }

  return hit;
}

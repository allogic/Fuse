#include <engine/eg_pch.h>

#include <engine/physics/eg_collision.h>

vector3_t closest_point_on_triangle(vector3_t p, vector3_t a, vector3_t b, vector3_t c) {
  vector3_t ab = vector3_sub(b, a);
  vector3_t ac = vector3_sub(c, a);
  vector3_t ap = vector3_sub(p, a);

  float d1 = vector3_dot(ab, ap);
  float d2 = vector3_dot(ac, ap);

  if ((d1 <= 0.0F) && (d2 <= 0.0F)) {
    return a;
  }

  vector3_t bp = vector3_sub(p, b);
  float d3 = vector3_dot(ab, bp);
  float d4 = vector3_dot(ac, bp);

  if ((d3 >= 0.0F) && (d4 <= d3)) {
    return b;
  }

  float vc = d1 * d4 - d3 * d2;
  if ((vc <= 0.0F) && (d1 >= 0.0F) && (d3 <= 0.0F)) {
    float v = d1 / (d1 - d3);
    return vector3_add(a, vector3_muls(ab, v));
  }

  vector3_t cp = vector3_sub(p, c);
  float d5 = vector3_dot(ab, cp);
  float d6 = vector3_dot(ac, cp);
  if ((d6 >= 0.0F) && (d5 <= d6)) {
    return c;
  }

  float vb = d5 * d2 - d1 * d6;
  if ((vb <= 0.0F) && (d2 >= 0.0F) && (d6 <= 0.0F)) {
    float w = d2 / (d2 - d6);
    return vector3_add(a, vector3_muls(ac, w));
  }

  float va = d3 * d6 - d5 * d4;
  if ((va <= 0.0F) && ((d4 - d3) >= 0.0F) && ((d5 - d6) >= 0.0F)) {
    float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
    return vector3_add(b, vector3_muls(vector3_sub(c, b), w));
  }

  float denom = 1.0F / (va + vb + vc);
  float v = vb * denom;
  float w = vc * denom;
  return vector3_add(a, vector3_add(vector3_muls(ab, v), vector3_muls(ac, w)));
}

collision_t sphere_triangle_collision(vector3_t p, float r, vector3_t a, vector3_t b, vector3_t c) {
  collision_t collision = {0};

  vector3_t closest = closest_point_on_triangle(p, a, b, c);
  vector3_t diff = vector3_sub(closest, p);

  float dist2 = vector3_length2(diff);

  if (dist2 <= (r * r)) {

    float dist = sqrtf(dist2);

    collision.hit = 1;
    collision.point = closest;
    collision.normal = (dist > 0.0001F) ? vector3_muls(diff, 1.0F / dist) : vector3_up();
    collision.penetration = r - dist;
  }

  return collision;
}

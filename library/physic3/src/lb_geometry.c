#include <physic3/lb_pch.h>
#include <physic3/lb_geometry.h>

lb_vector3_t lb_closest_point_on_triangle(lb_vector3_t p, lb_vector3_t a, lb_vector3_t b, lb_vector3_t c) {
  lb_vector3_t ab = lb_vector3_sub(b, a);
  lb_vector3_t ac = lb_vector3_sub(c, a);
  lb_vector3_t ap = lb_vector3_sub(p, a);

  float d1 = lb_vector3_dot(ab, ap);
  float d2 = lb_vector3_dot(ac, ap);

  if ((d1 <= 0.0F) && (d2 <= 0.0F)) {
    return a;
  }

  lb_vector3_t bp = lb_vector3_sub(p, b);
  float d3 = lb_vector3_dot(ab, bp);
  float d4 = lb_vector3_dot(ac, bp);

  if ((d3 >= 0.0F) && (d4 <= d3)) {
    return b;
  }

  float vc = d1 * d4 - d3 * d2;
  if ((vc <= 0.0F) && (d1 >= 0.0F) && (d3 <= 0.0F)) {
    float v = d1 / (d1 - d3);
    return lb_vector3_add(a, lb_vector3_muls(ab, v));
  }

  lb_vector3_t cp = lb_vector3_sub(p, c);
  float d5 = lb_vector3_dot(ab, cp);
  float d6 = lb_vector3_dot(ac, cp);
  if ((d6 >= 0.0F) && (d5 <= d6)) {
    return c;
  }

  float vb = d5 * d2 - d1 * d6;
  if ((vb <= 0.0F) && (d2 >= 0.0F) && (d6 <= 0.0F)) {
    float w = d2 / (d2 - d6);
    return lb_vector3_add(a, lb_vector3_muls(ac, w));
  }

  float va = d3 * d6 - d5 * d4;
  if ((va <= 0.0F) && ((d4 - d3) >= 0.0F) && ((d5 - d6) >= 0.0F)) {
    float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
    return lb_vector3_add(b, lb_vector3_muls(lb_vector3_sub(c, b), w));
  }

  float denom = 1.0F / (va + vb + vc);
  float v = vb * denom;
  float w = vc * denom;

  return lb_vector3_add(a, lb_vector3_add(lb_vector3_muls(ab, v), lb_vector3_muls(ac, w)));
}

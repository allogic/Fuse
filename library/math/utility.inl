__forceinline float math_utility_deg_to_rad(float a) {
  return a * MATH_DEG_TO_RAD;
}
__forceinline float math_utility_rad_to_deg(float a) {
  return a * MATH_RAD_TO_DEG;
}
__forceinline float math_utility_clamp(float a, float min, float max) {
  return fmaxf(min, fminf(max, a));
}

__forceinline float math_deg_to_rad(float a) {
  return a * DEG_TO_RAD;
}
__forceinline float math_rad_to_deg(float a) {
  return a * RAD_TO_DEG;
}
__forceinline float math_clamp(float a, float min, float max) {
  return fmaxf(min, fminf(max, a));
}

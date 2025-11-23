__forceinline float lb_deg_to_rad(float a) {
  return a * LB_DEG_TO_RAD;
}
__forceinline float lb_rad_to_deg(float a) {
  return a * LB_RAD_TO_DEG;
}
__forceinline float lb_clamp(float a, float min, float max) {
  return fmaxf(min, fminf(max, a));
}

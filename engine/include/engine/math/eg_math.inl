__forceinline float eg_deg_to_rad(float a) {
  return a * EG_DEG_TO_RAD;
}
__forceinline float eg_rad_to_deg(float a) {
  return a * EG_RAD_TO_DEG;
}
__forceinline float eg_clamp(float a, float min, float max) {
  return fmaxf(min, fminf(max, a));
}

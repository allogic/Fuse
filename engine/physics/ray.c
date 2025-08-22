#include <engine/physics/ray.h>

physics_ray_t physics_ray_from(math_vector3_t origin, math_vector3_t direction) {
  physics_ray_t r = {
    origin,
    direction,
  };

  return r;
}
void physics_ray_cast(physics_ray_t ray, int32_t max_steps) {
  /*
  float result = -1.0F;

  int32_t step_index = 0;

  math_vector3_t ray_direction_inv = math_vector3_div_scalar_inv(1.0F, ray_direction);
  math_vector3_t ray_direction_sign = math_vector3_sign(ray_direction);

  math_vector3_t mask = { 0.0F, 0.0F, 0.0F };

  math_vector3_t world_position = ray_origin;
  math_vector3_t sample_position = math_vector3_floor(world_position);
  math_vector3_t side_distance = (sample_position - world_position + 0.5F + ray_direction_sign * 0.5F) * ray_direction_inv;

  while (step_index < max_steps)
  {
          uint32_t voxel = get_voxel(ivec3(sample_position), lod);

          bool is_solid = VOXEL_IS_SOLID(voxel);

          if (is_solid)
          {
                  result = 1.0F;

                  break;
          }

          mask = math_vector3_mul(math_vector3_step(side_distance.xyz, side_distance.yzx), math_vector3_step(side_distance.xyz, side_distance.zxy));

          sample_position = math_vector3_add(sample_position, math_vector3_mul(mask, ray_direction_sign));
          side_distance = math_vector3_add(side_distance, math_vector3_mul(mask, math_vector3_mul(ray_direction_sign, ray_direction_inv)));

          step_index++;
  }

  math_vector3_t mini = (sample_position - ray_origin + 0.5F - 0.5F * ray_direction_sign) * ray_direction_inv;

  float t = max(mini.x, max(mini.y, mini.z));

  return hit_t(t * result, lod, sample_position, normalize(mask));
  */
}

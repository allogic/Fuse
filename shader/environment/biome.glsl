#ifndef TERRAIN_BIOME_H
#define TERRAIN_BIOME_H

#include "../math/utility.glsl"

#include "../noise/curl.glsl"
#include "../noise/fbm.glsl"
#include "../noise/gradient.glsl"
#include "../noise/perlin.glsl"
#include "../noise/simplex.glsl"
#include "../noise/voronoi.glsl"

#define DENSITY_THRESHOLD (0.5)

float biome_default(in vec3 position, in float ground_level, in float cluster_height)
{
	float density = 0.0;

	//density += noise_curl(position * 0.05);
	//density += noise_fbm(position * 0.05, 1.0, 1.0, 4);
	//density += noise_perlin(position * 0.1) * 2.0;
	//density += noise_voronoi(position * 0.1, 0.0, 0.0);

	density += noise_gradient(position * 0.01);
	density *= noise_simplex(position * 0.01) * 2.0;
	density += noise_fbm(position * 0.02, 0.7, 1.0, 4);
	density += noise_fbm((position + vec3(44324.0223, 213.055, -9895.432)) * 0.04, 0.7, 1.0, 4) * 2.0;

	density = soft_terrace(density, 2, 1.8);

	float height_threshold = ground_level / cluster_height;
	float blend_factor = smoothstep(height_threshold + 0.2, height_threshold - 0.2, position.y / cluster_height);

	density += blend_factor;
	density *= blend_factor;

	return density;
}

#endif // TERRAIN_BIOME_H

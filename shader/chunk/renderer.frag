#version 460 core

#extension GL_ARB_shading_language_include : require
#extension GL_EXT_nonuniform_qualifier : require

#define VOXEL_EMPTY (0U)

#define VOXEL_IS_SOLID(VOXEL) ((VOXEL & 0x80000000U) == 0x80000000U)

#define VOXEL_SET_SOLID(VOXEL) (VOXEL | 0x80000000U)

struct hit_t
{
	float dist;
	int lod;
	vec3 position;
	vec3 direction;
};

layout (location = 0) out vec4 output_color;

layout (binding = 0) uniform time_info_t
{
	float time;
	float delta_time;
} time_info;

layout (binding = 1) uniform screen_info_t
{
	vec2 resolution;
} screen_info;

layout (binding = 2) uniform camera_info_t
{
	vec3 world_position;
	int reserved;
	mat4 view;
	mat4 projection;
	mat4 view_projection;
	mat4 view_projection_inv;
	int max_ray_steps;
} camera_info;

layout (binding = 3) uniform cluster_info_t
{
	ivec3 chunk_count;
	int chunk_size;
	int max_lod_levels;
} cluster_info;

layout (binding = 4) uniform usampler3D chunks[];

uint get_voxel(in ivec3 world_position, in int lod)
{
	int chunk_size = cluster_info.chunk_size / (1 << lod);

	if (all(greaterThanEqual(world_position, ivec3(0))))
	{
		if (all(lessThan(world_position, cluster_info.chunk_count * chunk_size)))
		{
			ivec3 chunk_position = world_position / chunk_size;
			ivec3 voxel_position = world_position % chunk_size;

			int chunk_index = int(
				chunk_position.x +
				chunk_position.y * cluster_info.chunk_count.x +
				chunk_position.z * cluster_info.chunk_count.x * cluster_info.chunk_count.y
			);

			chunk_index = lod + (chunk_index * cluster_info.max_lod_levels);

			if (chunk_index < (cluster_info.chunk_count.x * cluster_info.chunk_count.y * cluster_info.chunk_count.z * cluster_info.max_lod_levels))
			{
				return uint(texelFetch(chunks[chunk_index], voxel_position, 0).r);
			}
		}
	}

	return VOXEL_EMPTY;
}

// TODO: https://github.com/theolundqvist/parallax-voxel-ray-marcher/blob/main/shaders/EDAN35/voxel.frag
hit_t raycast(in vec3 ray_origin, in vec3 ray_direction)
{
	float result = -1.0;

	int lod = 0;
	int step_index = 0;

	vec3 ray_direction_inv = 1.0 / ray_direction;
	vec3 ray_direction_sign = sign(ray_direction);
	
	vec3 mask = vec3(0.0);

	int chunk_size = 1 << lod;
	float chunk_size_inv = 1.0 / float(chunk_size);

	vec3 world_position = ray_origin; // * chunk_size_inv;
	vec3 sample_position = floor(world_position);
	vec3 side_distance = (sample_position - world_position + 0.5 + ray_direction_sign * 0.5) * ray_direction_inv;

	//float lod_switch_distance = 50.0;

	while (step_index < camera_info.max_ray_steps)
	{
		/*
		float travel_distance = length(sample_position * chunk_size - ray_origin);

		if ((travel_distance > lod_switch_distance) && (lod < 1))
		{
			lod++;

			chunk_size = 1 << lod;
			chunk_size_inv = 1.0 / float(chunk_size);

			//world_position = ray_origin * chunk_size_inv;
			//sample_position = floor(world_position);
			//side_distance = (sample_position - world_position + 0.5 + ray_direction_sign * 0.5) * ray_direction_inv;

			continue;
		}
		*/

		uint voxel = get_voxel(ivec3(sample_position), lod);

		bool is_solid = VOXEL_IS_SOLID(voxel);

		if (is_solid)
		{
			result = 1.0;

			break;

			/*
			if (lod == 0)
			{
				result = 1.0;

				break;
			}

			lod--;

			chunk_size = 1 << lod;
			chunk_size_inv = 1.0 / float(chunk_size);

			world_position = ray_origin * chunk_size_inv;
			sample_position = floor(world_position);
			side_distance = (sample_position - world_position + 0.5 + ray_direction_sign * 0.5) * ray_direction_inv;

			continue;
			*/
		}

		mask = step(side_distance.xyz, side_distance.yzx) * step(side_distance.xyz, side_distance.zxy);

		sample_position += mask * ray_direction_sign;
		side_distance += mask * ray_direction_sign * ray_direction_inv;

		step_index++;
	}
	
	vec3 mini = (sample_position - ray_origin + 0.5 - 0.5 * ray_direction_sign) * ray_direction_inv;

	float t = max(mini.x, max(mini.y, mini.z));

	return hit_t(t * result, lod, sample_position, normalize(mask));
}

void main()
{
	vec2 ndc = (2.0 * gl_FragCoord.xy / screen_info.resolution) - vec2(1.0);

	vec3 ray_origin = camera_info.world_position;
	vec3 ray_direction = normalize(vec3(camera_info.view_projection_inv * vec4(ndc, 1.0, 1.0)));

	hit_t hit = raycast(ray_origin, ray_direction);

	ray_origin *= 1.0 / float(1 << hit.lod); // TODO

	if (hit.dist > 0.0)
	{
		vec3 position = ray_origin + ray_direction * hit.dist;
		vec3 normal = -hit.direction * sign(ray_direction);
		vec3 uvw = position - hit.position;
		vec2 uv = vec2(dot(hit.direction.yzx, uvw), dot(hit.direction.zxy, uvw));

		vec3 p0 = hit.position + normal + hit.direction.yzx;
		vec3 p1 = hit.position + normal - hit.direction.yzx;
		vec3 p2 = hit.position + normal + hit.direction.zxy;
		vec3 p3 = hit.position + normal - hit.direction.zxy;
		vec3 p4 = hit.position + normal + hit.direction.yzx + hit.direction.zxy;
		vec3 p5 = hit.position + normal - hit.direction.yzx + hit.direction.zxy;
		vec3 p6 = hit.position + normal - hit.direction.yzx - hit.direction.zxy;
		vec3 p7 = hit.position + normal + hit.direction.yzx - hit.direction.zxy;

		uint v0 = get_voxel(ivec3(p0), hit.lod);
		uint v1 = get_voxel(ivec3(p1), hit.lod);
		uint v2 = get_voxel(ivec3(p2), hit.lod);
		uint v3 = get_voxel(ivec3(p3), hit.lod);
		uint v4 = get_voxel(ivec3(p4), hit.lod);
		uint v5 = get_voxel(ivec3(p5), hit.lod);
		uint v6 = get_voxel(ivec3(p6), hit.lod);
		uint v7 = get_voxel(ivec3(p7), hit.lod);

		bool s0 = VOXEL_IS_SOLID(v0);
		bool s1 = VOXEL_IS_SOLID(v1);
		bool s2 = VOXEL_IS_SOLID(v2);
		bool s3 = VOXEL_IS_SOLID(v3);
		bool s4 = VOXEL_IS_SOLID(v4);
		bool s5 = VOXEL_IS_SOLID(v5);
		bool s6 = VOXEL_IS_SOLID(v6);
		bool s7 = VOXEL_IS_SOLID(v7);

		vec4 vc = vec4(s0, s1, s2, s3);
		vec4 vd = vec4(s4, s5, s6, s7);
	
		vec2 st = 1.0 - uv;
		vec4 wa = vec4(uv.x, st.x, uv.y, st.y) * vc;
		vec4 wb = vec4(uv.x * uv.y, st.x * uv.y, st.x * st.y, uv.x * st.y) * vd * (1.0 - vc.xzyw) * (1.0 - vc.zywx);

		float occ = 1.0;

		occ = wa.x + wa.y + wa.z + wa.w + wb.x + wb.y + wb.z + wb.w;
		occ = 1.0 - occ / 8.0;
		occ = occ * occ;

		//vec3 color = vec3(1.0) * (0.5 + 0.5 * normal.y) * occ;
		vec3 color = vec3(uv, 0.0);

		output_color = vec4(color, 1.0);
	}
	else
	{
		discard;
	}
}

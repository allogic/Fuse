#version 460 core

#extension GL_ARB_shading_language_include : require
#extension GL_EXT_nonuniform_qualifier : require

// TODO: make input/output structs..

layout (location = 0) in ivec3 vertex_position;

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

void main()
{

}
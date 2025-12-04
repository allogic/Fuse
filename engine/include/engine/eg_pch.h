#ifndef EG_PCH_H
#define EG_PCH_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include <spirv_reflect/spirv_reflect.h>

#include <sqlite3/sqlite3.h>

#define FLECS_PIPELINE
#include <flecs/flecs.h>

#include <engine/eg_macros.h>
#include <engine/eg_fwd.h>

#include <engine/math/eg_math.h>
#include <engine/math/eg_vector2.h>
#include <engine/math/eg_vector3.h>
#include <engine/math/eg_vector4.h>
#include <engine/math/eg_ivector2.h>
#include <engine/math/eg_ivector3.h>
#include <engine/math/eg_ivector4.h>
#include <engine/math/eg_quaternion.h>
#include <engine/math/eg_matrix4.h>

#include <engine/container/eg_map.h>
#include <engine/container/eg_set.h>
#include <engine/container/eg_string.h>
#include <engine/container/eg_vector.h>

#include <engine/component/eg_camera.h>
#include <engine/component/eg_controller.h>
#include <engine/component/eg_rigidbody.h>
#include <engine/component/eg_script.h>
#include <engine/component/eg_transform.h>

#include <engine/system/eg_controller.h>
#include <engine/system/eg_rigidbody.h>

#include <engine/eg_buffer.h>
#include <engine/eg_clang.h>
#include <engine/eg_context.h>
#include <engine/eg_database.h>
#include <engine/eg_filesys.h>
#include <engine/eg_heap.h>
#include <engine/eg_image.h>
#include <engine/eg_importer.h>
#include <engine/eg_model.h>
#include <engine/eg_glslang.h>
#include <engine/eg_pipeline.h>
#include <engine/eg_profiler.h>
#include <engine/eg_random.h>
#include <engine/eg_scene.h>
#include <engine/eg_swapchain.h>
#include <engine/eg_terrain.h>
#include <engine/eg_viewport.h>

#include <engine/renderer3/eg_renderer.h>
#include <engine/renderer3/eg_renderpass.h>

#include <engine/physic3/eg_collision.h>
#include <engine/physic3/eg_geometry.h>

#endif // EG_PCH_H

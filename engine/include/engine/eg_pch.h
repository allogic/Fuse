#ifndef EG_PCH_H
#define EG_PCH_H

#define VK_USE_PLATFORM_WIN32_KHR

#include <core/lb_api.h>
#include <container/lb_api.h>
#include <database/lb_api.h>
#include <importer/lb_api.h>
#include <math/lb_api.h>
#include <physic3/lb_api.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define FLECS_PIPELINE
#include <flecs/flecs.h>

#include <engine/eg_macros.h>
#include <engine/eg_fwd.h>

#include <engine/component/eg_camera.h>
#include <engine/component/eg_controller.h>
#include <engine/component/eg_rigidbody.h>
#include <engine/component/eg_script.h>
#include <engine/component/eg_transform.h>

#include <engine/eg_context.h>
#include <engine/eg_swapchain.h>
#include <engine/eg_renderer.h>
#include <engine/eg_scene.h>
#include <engine/eg_buffer.h>
#include <engine/eg_image.h>
#include <engine/eg_viewport.h>
#include <engine/eg_profiler.h>

#endif // EG_PCH_H

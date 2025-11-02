#ifndef LB_PCH_H
#define LB_PCH_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <intrin.h>
#include <time.h>
#include <math.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#define FLECS_PIPELINE
#include <flecs/flecs.h>

#include <stb_json/stb_json.h>

#include <library/lb_fwd.h>

#include <library/container/lb_api.h>
#include <library/core/lb_api.h>
#include <library/database/lb_api.h>
#include <library/importer/lb_api.h>
#include <library/math/lb_api.h>
#include <library/physic3/lb_api.h>

#endif // LB_PCH_H

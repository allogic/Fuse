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

#include <library/core/co_macros.h>
#include <library/database/db_macros.h>
#include <library/globals/go_macros.h>
#include <library/math/ma_macros.h>

#include <library/core/co_config.h>
#include <library/database/db_config.h>
#include <library/globals/go_config.h>
#include <library/math/ma_config.h>

#include <library/core/co_forward.h>
#include <library/database/db_forward.h>
#include <library/globals/go_forward.h>
#include <library/math/ma_forward.h>

#include <library/core/co_constants.h>
#include <library/database/db_constants.h>
#include <library/globals/go_constants.h>
#include <library/math/ma_constants.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include <spirv_reflect/spirv_reflect.h>

#include <sqlite3/sqlite3.h>

#endif // LB_PCH_H

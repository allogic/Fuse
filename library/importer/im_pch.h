#ifndef IM_PCH_H
#define IM_PCH_H

#include <spirv_reflect/spirv_reflect.h>

#define CGLTF_IMPLEMENTATION
#include <cgltf/cgltf.h>

#define STB_JSON_IMPLEMENTATION
#include <stb_json/stb_json.h>

#include <library/database/db_api.h>

#include <library/importer/im_macros.h>
#include <library/importer/im_config.h>
#include <library/importer/im_fwd.h>
#include <library/importer/im_constants.h>

#endif // IM_PCH_H

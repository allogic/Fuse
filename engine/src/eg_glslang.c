#include <engine/eg_pch.h>

#include <glslang/Include/glslang_c_interface.h>
#include <glslang/Include/glslang_c_shader_types.h>
#include <glslang/Public/resource_limits_c.h>

static glslang_shader_t *eg_glslang_compile_shader(char const *source_code, int32_t version, glslang_stage_t stage);
static glslang_program_t *eg_glslang_link_shader(glslang_shader_t *shader, glslang_stage_t stage);

void eg_glslang_create(void) {
  glslang_initialize_process();
}
eg_spirv_bin_t eg_glslang_compile_graphic(eg_string_t *source_code, int32_t version) {
  eg_spirv_bin_t binary = {0};

  int64_t pragma_vertex_position = eg_string_find(source_code, "#pragma vertex");
  int64_t pragma_fragment_position = eg_string_find(source_code, "#pragma fragment");

  uint64_t source_code_size = eg_string_size(source_code);

  if (pragma_vertex_position >= 0 && pragma_fragment_position >= 0) {

    uint8_t order_valid =
      pragma_vertex_position <
      pragma_fragment_position <
      source_code_size;

    if (order_valid) {

      eg_string_t *vertex_shader_source_code = eg_string_substr(source_code, pragma_vertex_position, pragma_fragment_position);
      eg_string_t *fragment_shader_source_code = eg_string_substr(source_code, pragma_fragment_position, source_code_size);

      glslang_shader_t *vertex_shader = eg_glslang_compile_shader(eg_string_buffer(vertex_shader_source_code), version, GLSLANG_STAGE_VERTEX);
      glslang_shader_t *fragment_shader = eg_glslang_compile_shader(eg_string_buffer(fragment_shader_source_code), version, GLSLANG_STAGE_FRAGMENT);

      if (vertex_shader) {

        glslang_program_t *vertex_program = eg_glslang_link_shader(vertex_shader, GLSLANG_STAGE_VERTEX);

        if (vertex_program) {

          uint64_t spirv_size = glslang_program_SPIRV_get_size(vertex_program);
          uint32_t *spirv = eg_heap_alloc(spirv_size * sizeof(uint32_t), 1, 0);

          glslang_program_SPIRV_get(vertex_program, spirv);

          printf("Vertex SPIR-V generated\n");

          binary.vertex_size = spirv_size;
          binary.vertex_words = spirv;

          glslang_program_delete(vertex_program);
        }

        glslang_shader_delete(vertex_shader);
      }

      if (fragment_shader) {

        glslang_program_t *fragment_program = eg_glslang_link_shader(fragment_shader, GLSLANG_STAGE_FRAGMENT);

        if (fragment_program) {

          uint64_t spirv_size = glslang_program_SPIRV_get_size(fragment_program);
          uint32_t *spirv = eg_heap_alloc(spirv_size * sizeof(uint32_t), 1, 0);

          glslang_program_SPIRV_get(fragment_program, spirv);

          printf("Fragment SPIR-V generated\n");

          binary.fragment_size = spirv_size;
          binary.fragment_words = spirv;

          glslang_program_delete(fragment_program);
        }

        glslang_shader_delete(fragment_shader);
      }
    }
  }

  return binary;
}
void eg_glslang_destroy(void) {
  glslang_finalize_process();
}

static glslang_shader_t *eg_glslang_compile_shader(char const *source_code, int32_t version, glslang_stage_t stage) {
  glslang_input_t input = {
    .language = GLSLANG_SOURCE_GLSL,
    .stage = stage,
    .client = GLSLANG_CLIENT_VULKAN,
    .client_version = GLSLANG_TARGET_VULKAN_1_1,
    .target_language = GLSLANG_TARGET_SPV,
    .target_language_version = GLSLANG_TARGET_SPV_1_6,
    .code = source_code,
    .default_version = version,
    .default_profile = GLSLANG_NO_PROFILE,
    .force_default_version_and_profile = 0,
    .forward_compatible = 0,
    .messages = GLSLANG_MSG_DEFAULT_BIT,
    .resource = glslang_default_resource(),
  };

  glslang_shader_t *shader = glslang_shader_create(&input);

  if (glslang_shader_preprocess(shader, &input) == 0) {

    printf("GLSL preprocessing failed\n");
    printf("\t%s\n", glslang_shader_get_info_log(shader));
    printf("\t%s\n", glslang_shader_get_info_debug_log(shader));

    glslang_shader_delete(shader);

    return 0;
  }

  if (glslang_shader_parse(shader, &input) == 0) {

    printf("GLSL parsing failed\n");
    printf("\t%s\n", glslang_shader_get_info_log(shader));
    printf("\t%s\n", glslang_shader_get_info_debug_log(shader));

    glslang_shader_delete(shader);

    return 0;
  }

  return shader;
}
static glslang_program_t *eg_glslang_link_shader(glslang_shader_t *shader, glslang_stage_t stage) {
  glslang_program_t *program = glslang_program_create();

  glslang_program_add_shader(program, shader);

  if (glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT) == 0) {

    printf("GLSL linking failed\n");
    printf("\t%s\n", glslang_program_get_info_log(program));
    printf("\t%s\n", glslang_program_get_info_debug_log(program));

  } else {

    glslang_program_SPIRV_generate(program, stage);
  }

  return program;
}

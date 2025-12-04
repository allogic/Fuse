#ifndef EG_GLSLANG_H
#define EG_GLSLANG_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void eg_glslang_create(void);
eg_spirv_bin_t eg_glslang_compile(eg_string_t *source_code, int32_t version);
void eg_glslang_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_GLSLANG_H

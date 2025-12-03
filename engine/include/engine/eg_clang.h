#ifndef EG_CLANG_H
#define EG_CLANG_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void eg_clang_create(void);
void eg_clang_compile(char const *source_code);
void eg_clang_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_CLANG_H

#ifndef LB_COMPILER_H
#define LB_COMPILER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void lb_compiler_create(void);
void lb_compiler_destroy(void);

void lb_compiler_compile(char const *source_code);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LB_COMPILER_H

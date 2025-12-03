#ifndef EG_SCRIPT_COMPONENT_H
#define EG_SCRIPT_COMPONENT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct eg_script_t {
  eg_script_create_proc_t create_proc;
  eg_script_update_proc_t update_proc;
  eg_script_fixed_update_proc_t fixed_update_proc;
  eg_script_destroy_proc_t destroy_proc;
} eg_script_t;

ECS_COMPONENT_DECLARE(eg_script_t);

void eg_script_create(eg_script_t *script);
void eg_script_destroy(eg_script_t *script);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_SCRIPT_COMPONENT_H

#ifndef EG_CONTEXT_H
#define EG_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern eg_editor_create_proc_t g_context_editor_create_proc;
extern eg_editor_refresh_proc_t g_context_editor_refresh_proc;
extern eg_editor_draw_proc_t g_context_editor_draw_proc;
extern eg_editor_destroy_proc_t g_context_editor_destroy_proc;
extern eg_editor_message_proc_t g_context_editor_message_proc;

eg_context_t *eg_context_create(int32_t width, int32_t height, uint8_t is_editor_mode);
void eg_context_run(eg_context_t *context);
void eg_context_destroy(eg_context_t *context);

uint8_t eg_context_is_keyboard_key_pressed(eg_context_t *context, eg_keyboard_key_t key);
uint8_t eg_context_is_keyboard_key_held(eg_context_t *context, eg_keyboard_key_t key);
uint8_t eg_context_is_keyboard_key_released(eg_context_t *context, eg_keyboard_key_t key);

uint8_t eg_context_is_mouse_key_pressed(eg_context_t *context, eg_mouse_key_t key);
uint8_t eg_context_is_mouse_key_held(eg_context_t *context, eg_mouse_key_t key);
uint8_t eg_context_is_mouse_key_released(eg_context_t *context, eg_mouse_key_t key);

int32_t eg_context_find_memory_type(eg_context_t *context, uint32_t type_filter, VkMemoryPropertyFlags memory_property_flags);

VkCommandBuffer eg_context_begin_command_buffer(eg_context_t *context);
void eg_context_end_command_buffer(eg_context_t *context, VkCommandBuffer command_buffer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_CONTEXT_H

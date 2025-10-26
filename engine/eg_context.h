#ifndef EG_CONTEXT_H
#define EG_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

context_t *context_create(int32_t width, int32_t height, uint8_t standalone);
uint8_t context_is_running(context_t *context);
void context_begin_frame(context_t *context);
void context_end_frame(context_t *context);
void context_destroy(context_t *context);

uint8_t context_is_keyboard_key_pressed(context_t *context, keyboard_key_t key);
uint8_t context_is_keyboard_key_held(context_t *context, keyboard_key_t key);
uint8_t context_is_keyboard_key_released(context_t *context, keyboard_key_t key);

uint8_t context_is_mouse_key_pressed(context_t *context, mouse_key_t key);
uint8_t context_is_mouse_key_held(context_t *context, mouse_key_t key);
uint8_t context_is_mouse_key_released(context_t *context, mouse_key_t key);

int32_t context_find_memory_type(context_t *context, uint32_t type_filter, VkMemoryPropertyFlags memory_property_flags);

VkCommandBuffer context_begin_command_buffer(context_t *context);
void context_end_command_buffer(context_t *context, VkCommandBuffer command_buffer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_CONTEXT_H

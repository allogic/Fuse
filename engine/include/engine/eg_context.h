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

uint8_t eg_context_is_editor_mode(eg_context_t *context);
uint32_t eg_context_mouse_position_x(eg_context_t *context);
uint32_t eg_context_mouse_position_y(eg_context_t *context);
float eg_context_time(eg_context_t *context);
float eg_context_delta_time(eg_context_t *context);
VkDevice eg_context_device(eg_context_t *context);
int32_t eg_context_primary_queue_index(eg_context_t *context);
int32_t eg_context_present_queue_index(eg_context_t *context);
VkQueue eg_context_primary_queue(eg_context_t *context);
VkQueue eg_context_present_queue(eg_context_t *context);
VkSurfaceKHR eg_context_surface(eg_context_t *context);
uint32_t eg_context_surface_min_image_count(eg_context_t *context);
VkSurfaceTransformFlagBitsKHR eg_context_surface_transform(eg_context_t *context);
VkPresentModeKHR eg_context_surface_present_mode(eg_context_t *context);
VkFormat eg_context_surface_image_color_format(eg_context_t *context);
VkFormat eg_context_surface_image_depth_format(eg_context_t *context);
VkColorSpaceKHR eg_context_surface_image_color_space(eg_context_t *context);
uint32_t eg_context_window_width(eg_context_t *context);
uint32_t eg_context_window_height(eg_context_t *context);
eg_renderer_t *eg_context_renderer(eg_context_t *context);
float eg_context_max_anisotropy(eg_context_t *context);
eg_swapchain_t *eg_context_swapchain(eg_context_t *context);
eg_renderer_t *eg_context_renderer(eg_context_t *context);

void eg_context_set_swapchain(eg_context_t *context, eg_swapchain_t *swapchain);
void eg_context_set_renderer(eg_context_t *context, eg_renderer_t *renderer);

// TODO: move this stuff away..

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

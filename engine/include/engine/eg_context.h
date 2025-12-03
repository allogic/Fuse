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

eg_context_t *eg_context_current(void);

void eg_context_create(int32_t width, int32_t height, uint8_t is_editor_mode);
void eg_context_run(void);
void eg_context_destroy(void);

HWND eg_context_window_handle(void);
uint8_t eg_context_is_editor_mode(void);
uint32_t eg_context_mouse_position_x(void);
uint32_t eg_context_mouse_position_y(void);
float eg_context_time(void);
float eg_context_delta_time(void);
VkInstance eg_context_instance(void);
VkPhysicalDevice eg_context_physical_device(void);
VkDevice eg_context_device(void);
VkCommandPool eg_context_command_pool(void);
int32_t eg_context_primary_queue_index(void);
int32_t eg_context_present_queue_index(void);
VkQueue eg_context_primary_queue(void);
VkQueue eg_context_present_queue(void);
VkSurfaceKHR eg_context_surface(void);
uint32_t eg_context_surface_min_image_count(void);
uint32_t eg_context_surface_max_image_count(void);
VkSurfaceTransformFlagBitsKHR eg_context_surface_transform(void);
VkPresentModeKHR eg_context_surface_present_mode(void);
VkFormat eg_context_surface_image_color_format(void);
VkFormat eg_context_surface_image_depth_format(void);
VkColorSpaceKHR eg_context_surface_image_color_space(void);
uint32_t eg_context_window_width(void);
uint32_t eg_context_window_height(void);
uint32_t eg_context_titlebar_height(void);
uint32_t eg_context_statusbar_height(void);
float eg_context_max_anisotropy(void);
eg_viewport_t **eg_context_viewports(void);
eg_scene_t **eg_context_scenes(void);
eg_viewport_t *eg_context_main_viewport(void);
eg_scene_t *eg_context_main_scene(void);

void eg_context_set_running(uint8_t is_running);
void eg_context_set_viewport(eg_viewport_t *viewport, uint64_t *index);
void eg_context_set_scene(eg_scene_t *scene, uint64_t *index);

// TODO: move this stuff away..

uint8_t eg_context_is_keyboard_key_pressed(eg_keyboard_key_t key);
uint8_t eg_context_is_keyboard_key_held(eg_keyboard_key_t key);
uint8_t eg_context_is_keyboard_key_released(eg_keyboard_key_t key);

uint8_t eg_context_is_mouse_key_pressed(eg_mouse_key_t key);
uint8_t eg_context_is_mouse_key_held(eg_mouse_key_t key);
uint8_t eg_context_is_mouse_key_released(eg_mouse_key_t key);

int32_t eg_context_find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags memory_property_flags);

VkCommandBuffer eg_context_begin_command_buffer();
void eg_context_end_command_buffer(VkCommandBuffer command_buffer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_CONTEXT_H

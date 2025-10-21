#ifndef LB_GLOBAL_GLOBAL_H
#define LB_GLOBAL_GLOBAL_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern HMODULE g_context_module_handle;
extern HWND g_context_window_handle;
extern double g_context_time;
extern double g_context_delta_time;
extern int32_t g_context_mouse_position_x;
extern int32_t g_context_mouse_position_y;
extern int32_t g_context_mouse_wheel_delta;
extern int32_t g_context_surface_width;
extern int32_t g_context_surface_height;
extern int32_t g_context_graphic_queue_index;
extern int32_t g_context_present_queue_index;
extern uint8_t g_context_window_should_close;
extern VkInstance g_context_instance;
extern VkSurfaceKHR g_context_surface;
extern VkSurfaceCapabilitiesKHR g_context_surface_capabilities;
extern VkSurfaceFormatKHR g_context_prefered_surface_format;
extern VkPresentModeKHR g_context_prefered_present_mode;
extern VkPhysicalDevice g_context_physical_device;
extern VkPhysicalDeviceProperties g_context_physical_device_properties;
extern VkPhysicalDeviceFeatures g_context_physical_device_features;
extern VkPhysicalDeviceMemoryProperties g_context_physical_device_memory_properties;
extern VkDevice g_context_device;
extern VkQueue g_context_graphic_queue;
extern VkQueue g_context_present_queue;
extern VkCommandPool g_context_command_pool;

extern uint8_t g_swapchain_is_dirty;
extern uint32_t g_swapchain_min_image_count;
extern uint32_t g_swapchain_image_count;
extern VkFormat g_swapchain_depth_format;
extern VkSwapchainKHR g_swapchain;
extern VkRenderPass g_swapchain_render_pass;
extern VkFramebuffer *g_swapchain_frame_buffers;

extern uint8_t g_renderer_is_dirty;
extern uint8_t g_renderer_enable_debug;
extern uint32_t g_renderer_frames_in_flight;
extern uint32_t g_renderer_frame_index;
extern uint32_t g_renderer_image_index;
extern VkCommandBuffer *g_renderer_graphic_command_buffers;

extern scene_t *g_scene_curr_active;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LB_GLOBAL_GLOBAL_H

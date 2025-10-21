#include <library/lb_pch.h>

#include <library/global/lb_global.h>

HMODULE g_context_module_handle = 0;
HWND g_context_window_handle = 0;
double g_context_time = 0.0;
double g_context_delta_time = 0.0;
int32_t g_context_mouse_position_x = 0;
int32_t g_context_mouse_position_y = 0;
int32_t g_context_mouse_wheel_delta = 0;
int32_t g_context_surface_width = 0;
int32_t g_context_surface_height = 0;
int32_t g_context_graphic_queue_index = 0;
int32_t g_context_present_queue_index = 0;
uint8_t g_context_window_should_close = 0;
VkInstance g_context_instance = 0;
VkSurfaceKHR g_context_surface = 0;
VkSurfaceCapabilitiesKHR g_context_surface_capabilities = {0};
VkSurfaceFormatKHR g_context_prefered_surface_format = {0};
VkPresentModeKHR g_context_prefered_present_mode = 0;
VkPhysicalDevice g_context_physical_device = 0;
VkPhysicalDeviceProperties g_context_physical_device_properties = {0};
VkPhysicalDeviceFeatures g_context_physical_device_features = {0};
VkPhysicalDeviceMemoryProperties g_context_physical_device_memory_properties = {0};
VkDevice g_context_device = 0;
VkQueue g_context_graphic_queue = 0;
VkQueue g_context_present_queue = 0;
VkCommandPool g_context_command_pool = 0;

uint8_t g_swapchain_is_dirty = 0;
uint32_t g_swapchain_min_image_count = 0;
uint32_t g_swapchain_image_count = 0;
VkFormat g_swapchain_depth_format = 0;
VkSwapchainKHR g_swapchain = 0;
VkRenderPass g_swapchain_render_pass = 0;
VkFramebuffer *g_swapchain_frame_buffers = 0;

uint8_t g_renderer_is_dirty = 0;
uint8_t g_renderer_enable_debug = 0;
uint32_t g_renderer_frames_in_flight = 0;
uint32_t g_renderer_frame_index = 0;
uint32_t g_renderer_image_index = 0;
VkCommandBuffer *g_renderer_graphic_command_buffers = 0;

scene_t *g_scene_curr_active = 0;

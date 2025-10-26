#include <library/lb_pch.h>

#include <library/global/lb_global.h>

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

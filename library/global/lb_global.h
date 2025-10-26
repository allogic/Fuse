#ifndef LB_GLOBAL_GLOBAL_H
#define LB_GLOBAL_GLOBAL_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

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

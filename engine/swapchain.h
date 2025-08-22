#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <stdint.h>

#include <vulkan/vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern uint8_t g_swapchain_is_dirty;

extern VkSwapchainKHR g_swapchain;

extern int32_t g_swapchain_image_count;

extern VkRenderPass g_swapchain_render_pass;

extern VkFramebuffer *g_swapchain_frame_buffers;

void swapchain_create(void);
void swapchain_destroy(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SWAPCHAIN_H

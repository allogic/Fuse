#ifndef EG_SWAPCHAIN_H
#define EG_SWAPCHAIN_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

eg_swapchain_t *eg_swapchain_current(void);

void eg_swapchain_create(eg_swapchain_asset_id_t swapchain_asset_id);
void eg_swapchain_destroy(void);

uint8_t eg_swapchain_is_dirty(void);
uint32_t eg_swapchain_image_count(void);
VkSwapchainKHR eg_swapchain_handle(void);
VkFramebuffer eg_swapchain_frame_buffer(uint32_t index);

void eg_swapchain_set_dirty(uint8_t is_dirty);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_SWAPCHAIN_H

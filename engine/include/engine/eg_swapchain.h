#ifndef EG_SWAPCHAIN_H
#define EG_SWAPCHAIN_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void eg_swapchain_create(eg_context_t *context, lb_swapchain_asset_id_t swapchain_asset_id);
void eg_swapchain_destroy(eg_swapchain_t *swapchain);

uint8_t eg_swapchain_is_dirty(eg_swapchain_t *swapchain);
uint32_t eg_swapchain_min_image_count(eg_swapchain_t *swapchain);
uint32_t eg_swapchain_image_count(eg_swapchain_t *swapchain);
VkSwapchainKHR eg_swapchain_handle(eg_swapchain_t *swapchain);

void eg_swapchain_set_dirty(eg_swapchain_t *swapchain, uint8_t is_dirty);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_SWAPCHAIN_H

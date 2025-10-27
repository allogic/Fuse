#ifndef EG_SWAPCHAIN_H
#define EG_SWAPCHAIN_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

swapchain_t *swapchain_create(context_t *context);
void swapchain_destroy(swapchain_t *swapchain);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_SWAPCHAIN_H

#ifndef EG_VIEWPORT_H
#define EG_VIEWPORT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

eg_viewport_t *eg_viewport_create(eg_context_t *context, uint32_t width, uint32_t height);
void eg_viewport_resize(eg_viewport_t *viewport, uint32_t width, uint32_t height);
void eg_viewport_destroy(eg_viewport_t *viewport);

uint32_t eg_viewport_width(eg_viewport_t *viewport);
uint32_t eg_viewport_height(eg_viewport_t *viewport);
VkImage *eg_viewport_color_images(eg_viewport_t *viewport);
VkImage *eg_viewport_depth_images(eg_viewport_t *viewport);
VkFramebuffer *eg_viewport_frame_buffers(eg_viewport_t *viewport);
VkSampler eg_viewport_color_sampler(eg_viewport_t *viewport, uint32_t index);
VkSampler eg_viewport_depth_sampler(eg_viewport_t *viewport, uint32_t index);
VkImageView eg_viewport_color_image_view(eg_viewport_t *viewport, uint32_t index);
VkImageView eg_viewport_depth_image_view(eg_viewport_t *viewport, uint32_t index);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_VIEWPORT_H

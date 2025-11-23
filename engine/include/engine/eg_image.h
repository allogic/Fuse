#ifndef EG_IMAGE_H
#define EG_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

eg_image_t *eg_image_create(eg_context_t *context, uint32_t width, uint32_t height, uint32_t depth, uint32_t channels, VkImageType type, VkImageViewType view_type, VkImageUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkImageAspectFlags aspect_flags, VkFormat format, VkImageTiling tiling, VkFilter filter);
void eg_image_map(eg_image_t *image);
void eg_image_unmap(eg_image_t *image);
void eg_image_copy_to_image(eg_image_t *image, eg_image_t *target, VkCommandBuffer command_buffer);
void eg_image_copy_to_buffer(eg_image_t *image, eg_buffer_t *target, VkCommandBuffer command_buffer);
void eg_image_destroy(eg_image_t *image);

VkImageView eg_image_create_view(eg_context_t *context, VkImage image, VkImageViewType view_type, VkImageAspectFlags aspect_flags, VkFormat format);
VkSampler eg_image_create_sampler(eg_context_t *context, VkImage image, VkFilter filter);
void eg_image_layout_transition(eg_context_t *context, VkImage image, VkCommandBuffer command_buffer, VkImageLayout old_layout, VkImageLayout new_layout, VkImageAspectFlags aspect_mask);

eg_image_t *eg_image_create_2d(eg_context_t *context, void *buffer, uint32_t width, uint32_t height, uint32_t channels, VkFormat format, VkImageTiling tiling, VkFilter filter);
eg_image_t *eg_image_create_2d_depth_stencil(eg_context_t *context, uint32_t width, uint32_t height, uint32_t channels, VkFormat format, VkImageTiling tiling, VkFilter filter);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_IMAGE_H

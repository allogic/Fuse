#ifndef EG_IMAGE_H
#define EG_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

image_t *image_create(context_t *context, uint32_t width, uint32_t height, uint32_t depth, uint32_t channels, VkImageType type, VkImageViewType view_type, VkImageUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkImageAspectFlags aspect_flags, VkFormat format, VkImageTiling tiling, VkFilter filter);
void image_map(image_t *image);
void image_unmap(image_t *image);
void image_copy_to_image(image_t *image, image_t *target, VkCommandBuffer command_buffer);
void image_copy_to_buffer(image_t *image, buffer_t *target, VkCommandBuffer command_buffer);
void image_destroy(image_t *image);

VkImageView image_create_view(context_t *context, VkImage image, VkImageViewType view_type, VkImageAspectFlags aspect_flags, VkFormat format);
VkSampler image_create_sampler(context_t *context, VkImage image, VkFilter filter);
void image_layout_transition(context_t *context, VkImage image, VkCommandBuffer command_buffer, VkImageLayout old_layout, VkImageLayout new_layout, VkImageAspectFlags aspect_mask);

image_t *image_create_2d(context_t *context, void *buffer, uint32_t width, uint32_t height, uint32_t channels, VkFormat format, VkImageTiling tiling, VkFilter filter);
image_t *image_create_2d_depth_stencil(context_t *context, uint32_t width, uint32_t height, uint32_t channels, VkFormat format, VkImageTiling tiling, VkFilter filter);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_IMAGE_H

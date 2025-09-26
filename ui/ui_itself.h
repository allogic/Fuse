#ifndef UI_ITSELF_H
#define UI_ITSELF_H

#include <stdint.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <vulkan/vulkan.h>

#ifdef BUILD_DEBUG
#  define UI_CREATE(...) ui_create(__VA_ARGS__)
#  define UI_DRAW(...) ui_draw(__VA_ARGS__)
#  define UI_DESTROY(...) ui_destroy(__VA_ARGS__)

#  define UI_FORWARD_MESSAGE(...) ui_forward_message(__VA_ARGS__)
#else
#  define UI_CREATE(...)
#  define UI_DRAW(...)
#  define UI_DESTROY(...)

#  define UI_FORWARD_MESSAGE(...)
#endif // BUILD_DEBUG

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void ui_create(HWND window_handle, VkInstance instance, VkPhysicalDevice physical_device, VkDevice device, uint32_t queue_index, VkQueue queue, VkRenderPass render_pass, uint32_t min_image_count);
void ui_draw(VkCommandBuffer command_buffer);
void ui_destroy(VkDevice device);

void ui_forward_message(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // UI_ITSELF_H

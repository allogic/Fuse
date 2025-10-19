#ifndef GO_ITSELF_H
#define GO_ITSELF_H

typedef struct globals_t {
  HMODULE context_module_handle;
  HWND context_window_handle;
  double context_time;
  double context_delta_time;
  int32_t context_mouse_position_x;
  int32_t context_mouse_position_y;
  int32_t context_mouse_wheel_delta;
  int32_t context_surface_width;
  int32_t context_surface_height;
  int32_t context_graphic_queue_index;
  int32_t context_present_queue_index;
  uint8_t context_window_should_close;
  VkInstance context_instance;
  VkSurfaceKHR context_surface;
  VkSurfaceCapabilitiesKHR context_surface_capabilities;
  VkSurfaceFormatKHR context_prefered_surface_format;
  VkPresentModeKHR context_prefered_present_mode;
  VkPhysicalDevice context_physical_device;
  VkPhysicalDeviceProperties context_physical_device_properties;
  VkPhysicalDeviceFeatures context_physical_device_features;
  VkPhysicalDeviceMemoryProperties context_physical_device_memory_properties;
  VkDevice context_device;
  VkQueue context_graphic_queue;
  VkQueue context_present_queue;
  VkCommandPool context_command_pool;

  uint8_t swapchain_is_dirty;
  uint32_t swapchain_min_image_count;
  uint32_t swapchain_image_count;
  VkFormat swapchain_depth_format;
  VkSwapchainKHR swapchain;
  VkRenderPass swapchain_render_pass;
  VkFramebuffer *swapchain_frame_buffers;

  uint8_t renderer_is_dirty;
  uint8_t renderer_enable_debug;
  uint32_t renderer_frames_in_flight;
  uint32_t renderer_frame_index;
  uint32_t renderer_image_index;
  VkCommandBuffer *renderer_graphic_command_buffers;

  scene_t *scene_curr_active;
} globals_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern globals_t g_globals;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // GO_ITSELF_H

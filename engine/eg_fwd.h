#ifndef EG_FWD_H
#define EG_FWD_H

typedef enum eg_key_state_t {
  EG_KEY_STATE_UP,
  EG_KEY_STATE_PRESSED,
  EG_KEY_STATE_DOWN,
  EG_KEY_STATE_RELEASED,
} eg_key_state_t;

typedef enum eg_keyboard_key_t {
  EG_KEYBOARD_KEY_BACK = 0x08,
  EG_KEYBOARD_KEY_TAB = 0x09,
  EG_KEYBOARD_KEY_RETURN = 0x0D,
  EG_KEYBOARD_KEY_SHIFT = 0x10,
  EG_KEYBOARD_KEY_CONTROL = 0x11,
  EG_KEYBOARD_KEY_MENU = 0x12,
  EG_KEYBOARD_KEY_PAUSE = 0x13,
  EG_KEYBOARD_KEY_CAPITAL = 0x14,
  EG_KEYBOARD_KEY_ESCAPE = 0x1B,
  EG_KEYBOARD_KEY_SPACE = 0x20,
  EG_KEYBOARD_KEY_PRIOR = 0x21,
  EG_KEYBOARD_KEY_NEXT = 0x22,
  EG_KEYBOARD_KEY_END = 0x23,
  EG_KEYBOARD_KEY_HOME = 0x24,
  EG_KEYBOARD_KEY_LEFT = 0x25,
  EG_KEYBOARD_KEY_UP = 0x26,
  EG_KEYBOARD_KEY_RIGHT = 0x27,
  EG_KEYBOARD_KEY_DOWN = 0x28,
  EG_KEYBOARD_KEY_SELECT = 0x29,
  EG_KEYBOARD_KEY_PRINT = 0x2A,
  EG_KEYBOARD_KEY_EXE = 0x2B,
  EG_KEYBOARD_KEY_SNAPSHOT = 0x2C,
  EG_KEYBOARD_KEY_INSERT = 0x2D,
  EG_KEYBOARD_KEY_DELETE = 0x2E,
  EG_KEYBOARD_KEY_HELP = 0x2F,
  EG_KEYBOARD_KEY_0 = '0',
  EG_KEYBOARD_KEY_1 = '1',
  EG_KEYBOARD_KEY_2 = '2',
  EG_KEYBOARD_KEY_3 = '3',
  EG_KEYBOARD_KEY_4 = '4',
  EG_KEYBOARD_KEY_5 = '5',
  EG_KEYBOARD_KEY_6 = '6',
  EG_KEYBOARD_KEY_7 = '7',
  EG_KEYBOARD_KEY_8 = '8',
  EG_KEYBOARD_KEY_9 = '9',
  EG_KEYBOARD_KEY_A = 'A',
  EG_KEYBOARD_KEY_B = 'B',
  EG_KEYBOARD_KEY_C = 'C',
  EG_KEYBOARD_KEY_D = 'D',
  EG_KEYBOARD_KEY_E = 'E',
  EG_KEYBOARD_KEY_F = 'F',
  EG_KEYBOARD_KEY_G = 'G',
  EG_KEYBOARD_KEY_H = 'H',
  EG_KEYBOARD_KEY_I = 'I',
  EG_KEYBOARD_KEY_J = 'J',
  EG_KEYBOARD_KEY_K = 'K',
  EG_KEYBOARD_KEY_L = 'L',
  EG_KEYBOARD_KEY_M = 'M',
  EG_KEYBOARD_KEY_N = 'N',
  EG_KEYBOARD_KEY_O = 'O',
  EG_KEYBOARD_KEY_P = 'P',
  EG_KEYBOARD_KEY_Q = 'Q',
  EG_KEYBOARD_KEY_R = 'R',
  EG_KEYBOARD_KEY_S = 'S',
  EG_KEYBOARD_KEY_T = 'T',
  EG_KEYBOARD_KEY_U = 'U',
  EG_KEYBOARD_KEY_V = 'V',
  EG_KEYBOARD_KEY_W = 'W',
  EG_KEYBOARD_KEY_X = 'X',
  EG_KEYBOARD_KEY_Y = 'Y',
  EG_KEYBOARD_KEY_Z = 'Z',
  EG_KEYBOARD_KEY_NUMPAD0 = 0x60,
  EG_KEYBOARD_KEY_NUMPAD1 = 0x61,
  EG_KEYBOARD_KEY_NUMPAD2 = 0x62,
  EG_KEYBOARD_KEY_NUMPAD3 = 0x63,
  EG_KEYBOARD_KEY_NUMPAD4 = 0x64,
  EG_KEYBOARD_KEY_NUMPAD5 = 0x65,
  EG_KEYBOARD_KEY_NUMPAD6 = 0x66,
  EG_KEYBOARD_KEY_NUMPAD7 = 0x67,
  EG_KEYBOARD_KEY_NUMPAD8 = 0x68,
  EG_KEYBOARD_KEY_NUMPAD9 = 0x69,
  EG_KEYBOARD_KEY_MULTIPLY = 0x6A,
  EG_KEYBOARD_KEY_ADD = 0x6B,
  EG_KEYBOARD_KEY_SEPARATOR = 0x6C,
  EG_KEYBOARD_KEY_SUBTRACT = 0x6D,
  EG_KEYBOARD_KEY_DECIMAL = 0x6E,
  EG_KEYBOARD_KEY_DIVIDE = 0x6F,
  EG_KEYBOARD_KEY_F1 = 0x70,
  EG_KEYBOARD_KEY_F2 = 0x71,
  EG_KEYBOARD_KEY_F3 = 0x72,
  EG_KEYBOARD_KEY_F4 = 0x73,
  EG_KEYBOARD_KEY_F5 = 0x74,
  EG_KEYBOARD_KEY_F6 = 0x75,
  EG_KEYBOARD_KEY_F7 = 0x76,
  EG_KEYBOARD_KEY_F8 = 0x77,
  EG_KEYBOARD_KEY_F9 = 0x78,
  EG_KEYBOARD_KEY_F10 = 0x79,
  EG_KEYBOARD_KEY_F11 = 0x7A,
  EG_KEYBOARD_KEY_F12 = 0x7B,
  EG_KEYBOARD_KEY_F13 = 0x7C,
  EG_KEYBOARD_KEY_F14 = 0x7D,
  EG_KEYBOARD_KEY_F15 = 0x7E,
  EG_KEYBOARD_KEY_F16 = 0x7F,
  EG_KEYBOARD_KEY_F17 = 0x80,
  EG_KEYBOARD_KEY_F18 = 0x81,
  EG_KEYBOARD_KEY_F19 = 0x82,
  EG_KEYBOARD_KEY_F20 = 0x83,
  EG_KEYBOARD_KEY_F21 = 0x84,
  EG_KEYBOARD_KEY_F22 = 0x85,
  EG_KEYBOARD_KEY_F23 = 0x86,
  EG_KEYBOARD_KEY_F24 = 0x87,
  EG_KEYBOARD_KEY_LEFT_SHIFT = 0xA0,
  EG_KEYBOARD_KEY_RIGHT_SHIFT = 0xA1,
  EG_KEYBOARD_KEY_LEFT_CONTROL = 0xA2,
  EG_KEYBOARD_KEY_RIGHT_CONTROL = 0xA3,
  EG_KEYBOARD_KEY_LEFT_MENU = 0xA4,
  EG_KEYBOARD_KEY_RIGHT_MENU = 0xA5,
} eg_keyboard_key_t;

typedef enum eg_mouse_key_t {
  EG_MOUSE_KEY_LEFT,
  EG_MOUSE_KEY_MIDDLE,
  EG_MOUSE_KEY_RIGHT,
} eg_mouse_key_t;

typedef void (*eg_editor_create_proc_t)(struct eg_context_t *context);
typedef void (*eg_editor_refresh_proc_t)(struct eg_context_t *context);
typedef void (*eg_editor_draw_proc_t)(struct eg_context_t *context);
typedef void (*eg_editor_destroy_proc_t)(struct eg_context_t *context);
typedef LRESULT (*eg_editor_message_proc_t)(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

typedef enum eg_profiler_sample_lane_t {
  EG_PROFILER_SAMPLE_LANE_CONTEXT,
  EG_PROFILER_SAMPLE_LANE_SCENE,
  EG_PROFILER_SAMPLE_LANE_RENDERER,
  EG_PROFILER_SAMPLE_LANE_COUNT,
} eg_profiler_sample_lane_t;

typedef struct eg_cpu_frame_sample_t {
  float time;
  float delta;
} eg_cpu_frame_sample_t;

typedef struct eg_world_settings_t {
  scene_asset_id_t default_scene_id;
  swapchain_asset_id_t default_swapchain_id;
  renderer_asset_id_t default_renderer_id;
} eg_world_settings_t;

typedef struct eg_context_t {
  HMODULE module_handle;
  HWND window_handle;
  MSG window_message;
  LARGE_INTEGER time_freq;
  LARGE_INTEGER time_start;
  LARGE_INTEGER time_end;
  double time;
  double delta_time;
  double elapsed_time_since_fps_count_update;
  uint64_t frame_index;
  uint32_t fps_counter;
  uint32_t mouse_position_x;
  uint32_t mouse_position_y;
  uint32_t mouse_wheel_delta;
  uint32_t window_width;
  uint32_t window_height;
  uint32_t window_titlebar_height;
  uint32_t window_statusbar_height;
  int32_t window_border_width;
  int32_t graphic_queue_index;
  int32_t present_queue_index;
  uint8_t is_editor_mode;
  uint8_t is_window_running;
  VkInstance instance;
  VkSurfaceKHR surface;
  VkSurfaceCapabilitiesKHR surface_capabilities;
  VkSurfaceFormatKHR prefered_surface_format;
  VkPresentModeKHR prefered_present_mode;
  VkPhysicalDevice physical_device;
  VkPhysicalDeviceProperties physical_device_properties;
  VkPhysicalDeviceFeatures physical_device_features;
  VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
  VkDevice device;
  VkQueue graphic_queue;
  VkQueue present_queue;
  VkCommandPool command_pool;
  eg_key_state_t event_keyboard_key_states[0xFF];
  eg_key_state_t event_mouse_key_states[0x3];
#ifdef BUILD_DEBUG
  PFN_vkCreateDebugUtilsMessengerEXT create_debug_utils_messenger_ext;
  PFN_vkDestroyDebugUtilsMessengerEXT destroy_debug_utils_messenger_ext;
  VkDebugUtilsMessengerEXT debug_messenger;
#endif // BUILD_DEBUG
  eg_world_settings_t world_settings;
  struct eg_viewport_t *viewport[0xFF];
  struct eg_swapchain_t *swapchain;
  struct eg_renderer_t *renderer;
  struct eg_scene_t *scene;
} eg_context_t;

typedef struct eg_swapchain_t {
  eg_context_t *context;
  uint8_t is_dirty;
  uint64_t min_image_count;
  uint64_t image_count;
  VkFormat depth_format;
  VkSwapchainKHR handle;
  VkRenderPass main_render_pass;
  VkImage *color_image;
  VkImageView *color_image_view;
  VkImage *depth_image;
  VkDeviceMemory *depth_device_memory;
  VkImageView *depth_image_view;
  VkFramebuffer *frame_buffer;
} eg_swapchain_t;

typedef struct eg_viewport_t {
  eg_context_t *context;
  uint64_t link_index;
  uint32_t width;
  uint32_t height;
  VkImage *color_image;
  VkDeviceMemory *color_device_memory;
  VkImageView *color_image_view;
  VkSampler *color_sampler;
  VkImage *depth_image;
  VkDeviceMemory *depth_device_memory;
  VkImageView *depth_image_view;
  VkSampler *depth_sampler;
  VkFramebuffer *frame_buffer;
} eg_viewport_t;

typedef struct eg_buffer_t {
  eg_context_t *context;
  uint64_t size;
  VkBuffer handle;
  VkDeviceMemory device_memory;
  void *mapped_memory;
} eg_buffer_t;

typedef struct eg_image_t {
  eg_context_t *context;
  uint64_t size;
  uint32_t width;
  uint32_t height;
  uint32_t depth;
  uint32_t channels;
  VkImageUsageFlags usage;
  VkImageAspectFlags aspect_flags;
  VkImageType type;
  VkImageViewType view_type;
  VkFormat format;
  VkImageTiling tiling;
  VkFilter filter;
  VkImage handle;
  VkDeviceMemory device_memory;
  void *mapped_memory;
} eg_image_t;

typedef struct eg_time_info_t {
  float time;
  float delta_time;
} eg_time_info_t;
typedef struct eg_screen_info_t {
  vector2_t resolution;
} eg_screen_info_t;
typedef struct eg_camera_info_t {
  vector3_t position;
  int32_t reserved;
  matrix4_t view;
  matrix4_t projection;
  matrix4_t view_projection;
  matrix4_t view_projection_inv;
} eg_camera_info_t;

typedef struct eg_debug_vertex_t {
  vector3_t position;
  vector4_t color;
} eg_debug_vertex_t;
typedef struct eg_terrain_vertex_t {
  vector3_t position;
  vector3_t normal;
  vector4_t color;
} eg_terrain_vertex_t;

typedef struct eg_scene_t {
  eg_context_t *context;
  ecs_world_t *world;
  ecs_entity_t root;
  ecs_entity_t player;
  ecs_entity_t controller_system;
  ecs_entity_t rigidbody_system;
} eg_scene_t;

typedef enum eg_renderer_pipeline_link_type_t {
  EG_RENDERER_PIPELINE_LINK_TYPE_DEBUG,
  EG_RENDERER_PIPELINE_LINK_TYPE_PBR,
} eg_renderer_pipeline_link_type_t;

typedef struct eg_renderer_t {
  eg_context_t *context;
  uint8_t is_dirty;
  uint8_t is_debug_enabled;
  uint64_t frames_in_flight;
  uint32_t frame_index;
  uint32_t image_index;
  VkCommandBuffer *command_buffer;
  uint32_t pipeline_type[0xFF];
  void *pipeline_link[0xFF];
  VkSemaphore *render_finished_semaphore;
  VkSemaphore *image_available_semaphore;
  VkFence *frame_fence;
  eg_time_info_t **time_infos;
  eg_screen_info_t **screen_infos;
  eg_camera_info_t **camera_infos;
  char time_info_descriptor_binding_name[0xFF];
  char screen_info_descriptor_binding_name[0xFF];
  char camera_info_descriptor_binding_name[0xFF];
  map_t *descriptor_binding_buffers_per_frame;
  eg_debug_vertex_t **debug_line_vertices;
  uint32_t **debug_line_indices;
  eg_buffer_t **debug_line_vertex_buffers;
  eg_buffer_t **debug_line_index_buffers;
  uint32_t *debug_line_vertex_offset;
  uint32_t *debug_line_index_offset;
  VkRenderPass gbuffer_render_pass;
} eg_renderer_t;

typedef struct eg_graphic_pipeline_t {
  eg_context_t *context;
  eg_renderer_t *renderer;
  pipeline_resource_t resource;
  uint64_t vertex_input_binding_count;
  uint64_t descriptor_binding_count;
  uint64_t descriptor_pool_size_count;
  uint64_t descriptor_set_count;
  uint8_t interleaved_vertex_input_buffer;
  VkBuffer **vertex_input_binding_buffers_per_frame;
  uint64_t **vertex_input_binding_offsets_per_frame;
  VkBuffer **descriptor_binding_buffers_per_frame;
  VkBuffer *index_buffer_per_frame;
  map_t *auto_link_descriptor_binding_buffers_per_frame;
  VkVertexInputBindingDescription *vertex_input_binding_descriptions;
  VkVertexInputAttributeDescription *vertex_input_attribute_descriptions;
  VkDescriptorPoolSize *descriptor_pool_sizes;
  VkDescriptorSetLayoutBinding *descriptor_set_layout_bindings;
  vector_t descriptor_set_layouts;
  vector_t descriptor_sets;
  vector_t write_descriptor_sets;
  vector_t vertex_input_bindings;
  vector_t descriptor_bindings;
  VkDescriptorPool descriptor_pool;
  VkDescriptorSetLayout descriptor_set_layout;
  VkPipelineLayout pipeline_layout;
  VkPipeline handle;
} eg_graphic_pipeline_t;
typedef struct eg_compute_pipeline_t {
  eg_context_t *context;
  eg_renderer_t *renderer;
  pipeline_resource_t resource;
  uint64_t descriptor_binding_count;
  uint64_t descriptor_pool_size_count;
  VkBuffer **descriptor_binding_buffers_per_frame;
  map_t *auto_link_descriptor_binding_buffers_per_frame;
  VkDescriptorPoolSize *descriptor_pool_sizes;
  VkDescriptorSetLayoutBinding *descriptor_set_layout_bindings;
  vector_t descriptor_set_layouts;
  vector_t descriptor_sets;
  vector_t write_descriptor_sets;
  vector_t descriptor_bindings;
  VkDescriptorPool descriptor_pool;
  VkDescriptorSetLayout descriptor_set_layout;
  VkPipelineLayout pipeline_layout;
  VkPipeline handle;
} eg_compute_pipeline_t;

typedef struct eg_terrain_t {
  eg_context_t *context;
} eg_terrain_t;

#endif // EG_FWD_H

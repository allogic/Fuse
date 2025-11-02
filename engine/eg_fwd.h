#ifndef EG_FWD_H
#define EG_FWD_H

typedef enum key_state_t {
  KEY_STATE_UP,
  KEY_STATE_PRESSED,
  KEY_STATE_DOWN,
  KEY_STATE_RELEASED,
} key_state_t;

typedef enum keyboard_key_t {
  KEYBOARD_KEY_BACK = 0x08,
  KEYBOARD_KEY_TAB = 0x09,
  KEYBOARD_KEY_RETURN = 0x0D,
  KEYBOARD_KEY_SHIFT = 0x10,
  KEYBOARD_KEY_CONTROL = 0x11,
  KEYBOARD_KEY_MENU = 0x12,
  KEYBOARD_KEY_PAUSE = 0x13,
  KEYBOARD_KEY_CAPITAL = 0x14,
  KEYBOARD_KEY_ESCAPE = 0x1B,
  KEYBOARD_KEY_SPACE = 0x20,
  KEYBOARD_KEY_PRIOR = 0x21,
  KEYBOARD_KEY_NEXT = 0x22,
  KEYBOARD_KEY_END = 0x23,
  KEYBOARD_KEY_HOME = 0x24,
  KEYBOARD_KEY_LEFT = 0x25,
  KEYBOARD_KEY_UP = 0x26,
  KEYBOARD_KEY_RIGHT = 0x27,
  KEYBOARD_KEY_DOWN = 0x28,
  KEYBOARD_KEY_SELECT = 0x29,
  KEYBOARD_KEY_PRINT = 0x2A,
  KEYBOARD_KEY_EXE = 0x2B,
  KEYBOARD_KEY_SNAPSHOT = 0x2C,
  KEYBOARD_KEY_INSERT = 0x2D,
  KEYBOARD_KEY_DELETE = 0x2E,
  KEYBOARD_KEY_HELP = 0x2F,
  KEYBOARD_KEY_0 = '0',
  KEYBOARD_KEY_1 = '1',
  KEYBOARD_KEY_2 = '2',
  KEYBOARD_KEY_3 = '3',
  KEYBOARD_KEY_4 = '4',
  KEYBOARD_KEY_5 = '5',
  KEYBOARD_KEY_6 = '6',
  KEYBOARD_KEY_7 = '7',
  KEYBOARD_KEY_8 = '8',
  KEYBOARD_KEY_9 = '9',
  KEYBOARD_KEY_A = 'A',
  KEYBOARD_KEY_B = 'B',
  KEYBOARD_KEY_C = 'C',
  KEYBOARD_KEY_D = 'D',
  KEYBOARD_KEY_E = 'E',
  KEYBOARD_KEY_F = 'F',
  KEYBOARD_KEY_G = 'G',
  KEYBOARD_KEY_H = 'H',
  KEYBOARD_KEY_I = 'I',
  KEYBOARD_KEY_J = 'J',
  KEYBOARD_KEY_K = 'K',
  KEYBOARD_KEY_L = 'L',
  KEYBOARD_KEY_M = 'M',
  KEYBOARD_KEY_N = 'N',
  KEYBOARD_KEY_O = 'O',
  KEYBOARD_KEY_P = 'P',
  KEYBOARD_KEY_Q = 'Q',
  KEYBOARD_KEY_R = 'R',
  KEYBOARD_KEY_S = 'S',
  KEYBOARD_KEY_T = 'T',
  KEYBOARD_KEY_U = 'U',
  KEYBOARD_KEY_V = 'V',
  KEYBOARD_KEY_W = 'W',
  KEYBOARD_KEY_X = 'X',
  KEYBOARD_KEY_Y = 'Y',
  KEYBOARD_KEY_Z = 'Z',
  KEYBOARD_KEY_NUMPAD0 = 0x60,
  KEYBOARD_KEY_NUMPAD1 = 0x61,
  KEYBOARD_KEY_NUMPAD2 = 0x62,
  KEYBOARD_KEY_NUMPAD3 = 0x63,
  KEYBOARD_KEY_NUMPAD4 = 0x64,
  KEYBOARD_KEY_NUMPAD5 = 0x65,
  KEYBOARD_KEY_NUMPAD6 = 0x66,
  KEYBOARD_KEY_NUMPAD7 = 0x67,
  KEYBOARD_KEY_NUMPAD8 = 0x68,
  KEYBOARD_KEY_NUMPAD9 = 0x69,
  KEYBOARD_KEY_MULTIPLY = 0x6A,
  KEYBOARD_KEY_ADD = 0x6B,
  KEYBOARD_KEY_SEPARATOR = 0x6C,
  KEYBOARD_KEY_SUBTRACT = 0x6D,
  KEYBOARD_KEY_DECIMAL = 0x6E,
  KEYBOARD_KEY_DIVIDE = 0x6F,
  KEYBOARD_KEY_F1 = 0x70,
  KEYBOARD_KEY_F2 = 0x71,
  KEYBOARD_KEY_F3 = 0x72,
  KEYBOARD_KEY_F4 = 0x73,
  KEYBOARD_KEY_F5 = 0x74,
  KEYBOARD_KEY_F6 = 0x75,
  KEYBOARD_KEY_F7 = 0x76,
  KEYBOARD_KEY_F8 = 0x77,
  KEYBOARD_KEY_F9 = 0x78,
  KEYBOARD_KEY_F10 = 0x79,
  KEYBOARD_KEY_F11 = 0x7A,
  KEYBOARD_KEY_F12 = 0x7B,
  KEYBOARD_KEY_F13 = 0x7C,
  KEYBOARD_KEY_F14 = 0x7D,
  KEYBOARD_KEY_F15 = 0x7E,
  KEYBOARD_KEY_F16 = 0x7F,
  KEYBOARD_KEY_F17 = 0x80,
  KEYBOARD_KEY_F18 = 0x81,
  KEYBOARD_KEY_F19 = 0x82,
  KEYBOARD_KEY_F20 = 0x83,
  KEYBOARD_KEY_F21 = 0x84,
  KEYBOARD_KEY_F22 = 0x85,
  KEYBOARD_KEY_F23 = 0x86,
  KEYBOARD_KEY_F24 = 0x87,
  KEYBOARD_KEY_LEFT_SHIFT = 0xA0,
  KEYBOARD_KEY_RIGHT_SHIFT = 0xA1,
  KEYBOARD_KEY_LEFT_CONTROL = 0xA2,
  KEYBOARD_KEY_RIGHT_CONTROL = 0xA3,
  KEYBOARD_KEY_LEFT_MENU = 0xA4,
  KEYBOARD_KEY_RIGHT_MENU = 0xA5,
} keyboard_key_t;

typedef enum mouse_key_t {
  MOUSE_KEY_LEFT,
  MOUSE_KEY_MIDDLE,
  MOUSE_KEY_RIGHT,
} mouse_key_t;

typedef void (*imgui_create_proc_t)(struct context_t *context);
typedef void (*imgui_draw_proc_t)(struct context_t *context);
typedef void (*imgui_destroy_proc_t)(struct context_t *context);
typedef LRESULT (*imgui_message_proc_t)(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

typedef struct context_t {
  HMODULE module_handle;
  HWND window_handle;
  MSG window_message;
  double time;
  double delta_time;
  int32_t mouse_position_x;
  int32_t mouse_position_y;
  int32_t mouse_wheel_delta;
  int32_t window_width;
  int32_t window_height;
  int32_t window_titlebar_height;
  int32_t window_statusbar_height;
  int32_t window_border_width;
  int32_t graphic_queue_index;
  int32_t present_queue_index;
  uint8_t window_should_close;
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
  key_state_t event_keyboard_key_states[0xFF];
  key_state_t event_mouse_key_states[0x3];
  LARGE_INTEGER timer_freq;
  LARGE_INTEGER timer_begin;
  LARGE_INTEGER timer_end;
#ifdef BUILD_DEBUG
  PFN_vkCreateDebugUtilsMessengerEXT create_debug_utils_messenger_ext;
  PFN_vkDestroyDebugUtilsMessengerEXT destroy_debug_utils_messenger_ext;
  VkDebugUtilsMessengerEXT debug_messenger;
#endif // BUILD_DEBUG
  struct swapchain_t *swapchain;
  struct renderer_t *renderer;
  struct scene_t *scene;
} context_t;

typedef struct swapchain_t {
  context_t *context;
  uint8_t is_dirty;
  uint64_t min_image_count;
  uint64_t image_count;
  VkFormat depth_format;
  VkSwapchainKHR handle;
  VkRenderPass render_pass;
  VkFramebuffer *frame_buffers;
  VkImage *color_image;
  VkImage *depth_image;
  VkDeviceMemory *depth_image_device_memory;
  VkImageView *color_image_view;
  VkImageView *depth_image_view;
} swapchain_t;

typedef struct buffer_t {
  context_t *context;
  uint64_t size;
  VkBuffer handle;
  VkDeviceMemory device_memory;
  void *mapped_memory;
} buffer_t;

typedef struct time_info_t {
  float time;
  float delta_time;
} time_info_t;
typedef struct screen_info_t {
  vector2_t resolution;
} screen_info_t;
typedef struct camera_info_t {
  vector3_t position;
  int32_t reserved;
  matrix4_t view;
  matrix4_t projection;
  matrix4_t view_projection;
  matrix4_t view_projection_inv;
} camera_info_t;

typedef struct debug_vertex_t {
  vector3_t position;
  vector4_t color;
} debug_vertex_t;
typedef struct terrain_vertex_t {
  vector3_t position;
  vector3_t normal;
  vector4_t color;
} terrain_vertex_t;

typedef struct scene_t {
  context_t *context;
  ecs_world_t *world;
  ecs_entity_t root;
  ecs_entity_t player;
  ecs_entity_t controller_system;
  ecs_entity_t rigidbody_system;
} scene_t;

typedef struct renderer_t {
  context_t *context;
  uint8_t is_dirty;
  uint8_t enable_debug;
  uint64_t frames_in_flight;
  uint32_t frame_index;
  uint32_t image_index;
  VkCommandBuffer *graphic_command_buffers;
  uint32_t pipeline_types[0xFF];
  void *pipeline_links[0xFF];
  VkSemaphore *render_finished_semaphores;
  VkSemaphore *image_available_semaphores;
  VkFence *frame_fences;
  time_info_t **time_infos;
  screen_info_t **screen_infos;
  camera_info_t **camera_infos;
  char time_info_descriptor_binding_name[0xFF];
  char screen_info_descriptor_binding_name[0xFF];
  char camera_info_descriptor_binding_name[0xFF];
  map_t *descriptor_binding_buffers_per_frame;
  debug_vertex_t **debug_line_vertices;
  uint32_t **debug_line_indices;
  buffer_t **debug_line_vertex_buffers;
  buffer_t **debug_line_index_buffers;
  uint32_t *debug_line_vertex_offsets;
  uint32_t *debug_line_index_offsets;
} renderer_t;

typedef struct graphic_pipeline_t {
  context_t *context;
  renderer_t *renderer;
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
} graphic_pipeline_t;
typedef struct compute_pipeline_t {
  context_t *context;
  renderer_t *renderer;
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
} compute_pipeline_t;

typedef struct terrain_t {
  void *dummy; // TODO
} terrain_t;

#endif // EG_FWD_H

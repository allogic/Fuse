#include <engine/eg_pch.h>

// #define EG_DISABLE_SWAPCHAIN
// #define EG_DISABLE_RENDERER
// #define EG_DISABLE_SCENE

struct eg_context_t {
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
  int32_t primary_queue_index;
  int32_t present_queue_index;
  uint8_t is_editor_mode;
  uint8_t is_window_running;
  VkInstance instance;
  VkSurfaceKHR surface;
  VkSurfaceCapabilitiesKHR surface_capabilities;
  VkSurfaceFormatKHR prefered_surface_format;
  VkFormat prefered_image_depth_format;
  VkPresentModeKHR prefered_present_mode;
  VkPhysicalDevice physical_device;
  VkPhysicalDeviceProperties physical_device_properties;
  VkPhysicalDeviceFeatures physical_device_features;
  VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
  VkDevice device;
  VkQueue primary_queue;
  VkQueue present_queue;
  VkCommandPool command_pool;
  eg_key_state_t event_keyboard_key_states[0xFF];
  eg_key_state_t event_mouse_key_states[0x3];
#ifdef BUILD_DEBUG
  PFN_vkCreateDebugUtilsMessengerEXT create_debug_utils_messenger_proc;
  PFN_vkDestroyDebugUtilsMessengerEXT destroy_debug_utils_messenger_proc;
  VkDebugUtilsMessengerEXT debug_messenger;
#endif // BUILD_DEBUG
  eg_world_settings_t world_settings;
  eg_viewport_t *viewport[0xFF];
  eg_scene_t *scene[0xFF];
  eg_viewport_t *main_viewport;
  eg_scene_t *main_scene;
};

static LRESULT eg_context_window_message_proc(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

#ifdef BUILD_DEBUG
static VkBool32 eg_context_vulkan_message_proc(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, VkDebugUtilsMessengerCallbackDataEXT const *callback_data, void *user_data);
#endif // BUILD_DEBUG

static void eg_context_create_window(void);
static void eg_context_create_instance(void);
static void eg_context_create_surface(void);
static void eg_context_create_device(void);
static void eg_context_create_command_pool(void);

static void eg_context_destroy_window(void);
static void eg_context_destroy_instance(void);
static void eg_context_destroy_surface(void);
static void eg_context_destroy_device(void);
static void eg_context_destroy_command_pool(void);

static void eg_context_check_surface_capabilities(void);
static void eg_context_check_physical_device_extensions(void);

static void eg_context_resize_surface(void);

static void eg_context_find_physical_device(void);
static void eg_context_find_physical_device_queue_families(void);

static eg_context_t *s_context_current = 0;

static char const *s_context_window_class = "FUSE_CLASS";
static char const *s_context_window_name = "FUSE";

#ifdef BUILD_DEBUG
static char const *s_context_validation_layers[] = {
  "VK_LAYER_KHRONOS_validation",
};
#endif // BUILD_DEBUG

static char const *s_context_layer_extensions[] = {
  "VK_KHR_surface",
  "VK_KHR_win32_surface",
#ifdef BUILD_DEBUG
  "VK_EXT_debug_utils",
#endif // BUILD_DEBUG
};

static char const *s_context_device_extensions[] = {
  "VK_KHR_swapchain",
  "VK_EXT_descriptor_indexing",
};

eg_editor_create_proc_t g_context_editor_create_proc = 0;
eg_editor_refresh_proc_t g_context_editor_refresh_proc = 0;
eg_editor_draw_proc_t g_context_editor_draw_proc = 0;
eg_editor_destroy_proc_t g_context_editor_destroy_proc = 0;
eg_editor_message_proc_t g_context_editor_message_proc = 0;

eg_context_t *eg_context_current(void) {
  return s_context_current;
}

void eg_context_create(int32_t width, int32_t height, uint8_t is_editor_mode) {
  eg_database_create();

  eg_world_settings_t world_settings = {0};

  world_settings.default_scene_asset_id = eg_database_load_default_scene_asset_id();
  world_settings.default_swapchain_asset_id = eg_database_load_default_swapchain_asset_id();
  world_settings.default_renderer_asset_id = eg_database_load_default_renderer_asset_id();

  s_context_current = (eg_context_t *)eg_heap_alloc(sizeof(eg_context_t), 1, 0);

  s_context_current->is_editor_mode = is_editor_mode;
  s_context_current->is_window_running = 1;
  s_context_current->window_titlebar_height = 65;
  s_context_current->window_statusbar_height = 30;
  s_context_current->window_border_width = 1;
  s_context_current->window_width = width;
  s_context_current->window_height = height;
  s_context_current->primary_queue_index = -1;
  s_context_current->present_queue_index = -1;
  s_context_current->prefered_image_depth_format = VK_FORMAT_D32_SFLOAT; // TODO
  s_context_current->world_settings = world_settings;

  eg_context_create_window();
  eg_context_create_instance();
  eg_context_create_surface();

  eg_context_find_physical_device();
  eg_context_find_physical_device_queue_families();

  eg_context_check_physical_device_extensions();

  eg_context_create_device();

  eg_context_check_surface_capabilities();

  eg_context_resize_surface();

  eg_context_create_command_pool();

  eg_renderpass_create_main();
  eg_renderpass_create_gbuffer();

#ifndef EG_DISABLE_SWAPCHAIN
  eg_swapchain_create(world_settings.default_swapchain_asset_id);
#endif // EG_DISABLE_SWAPCHAIN

#ifndef EG_DISABLE_RENDERER
  eg_renderer_create(world_settings.default_renderer_asset_id);
#endif // EG_DISABLE_RENDERER

#ifndef EG_DISABLE_SCENE
  s_context_current->main_scene = eg_scene_create(world_settings.default_scene_asset_id);
#endif // EG_DISABLE_SCENE

  // eg_importer_import_default_assets(); // TODO: remove this..
}
void eg_context_run(void) {
  QueryPerformanceFrequency(&s_context_current->time_freq);

  while (s_context_current->is_window_running) {

    s_context_current->mouse_wheel_delta = 0;

    uint8_t keyboard_key_index = 0;
    uint8_t keyboard_key_count = 0xFF;

    while (keyboard_key_index < keyboard_key_count) {

      if (s_context_current->event_keyboard_key_states[keyboard_key_index] == EG_KEY_STATE_PRESSED) {
        s_context_current->event_keyboard_key_states[keyboard_key_index] = EG_KEY_STATE_DOWN;
      } else if (s_context_current->event_keyboard_key_states[keyboard_key_index] == EG_KEY_STATE_RELEASED) {
        s_context_current->event_keyboard_key_states[keyboard_key_index] = EG_KEY_STATE_UP;
      }

      keyboard_key_index++;
    }

    uint8_t mouse_key_index = 0;
    uint8_t mouse_key_count = 0x3;

    while (mouse_key_index < mouse_key_count) {

      if (s_context_current->event_mouse_key_states[mouse_key_index] == EG_KEY_STATE_PRESSED) {
        s_context_current->event_mouse_key_states[mouse_key_index] = EG_KEY_STATE_DOWN;
      } else if (s_context_current->event_mouse_key_states[mouse_key_index] == EG_KEY_STATE_RELEASED) {
        s_context_current->event_mouse_key_states[mouse_key_index] = EG_KEY_STATE_UP;
      }

      mouse_key_index++;
    }

#ifndef EG_DISABLE_SWAPCHAIN
    if (eg_swapchain_is_dirty()) {

      eg_swapchain_set_dirty(0);

      EG_VULKAN_CHECK(vkQueueWaitIdle(eg_context_primary_queue()));
      EG_VULKAN_CHECK(vkQueueWaitIdle(eg_context_present_queue()));

#  ifndef EG_DISABLE_RENDERER
      eg_renderer_destroy();
#  endif // EG_DISABLE_RENDERER

      eg_swapchain_destroy();

      eg_context_resize_surface();

      eg_swapchain_create(s_context_current->world_settings.default_swapchain_asset_id);

#  ifndef EG_DISABLE_RENDERER
      eg_renderer_create(s_context_current->world_settings.default_renderer_asset_id);
#  endif // EG_DISABLE_RENDERER
    }
#endif // EG_DISABLE_SWAPCHAIN

#ifndef EG_DISABLE_RENDERER
    if (eg_renderer_is_dirty()) {

      eg_renderer_set_dirty(0);

      EG_VULKAN_CHECK(vkQueueWaitIdle(eg_context_primary_queue()));
      EG_VULKAN_CHECK(vkQueueWaitIdle(eg_context_present_queue()));

      eg_renderer_destroy();

      eg_renderer_create(s_context_current->world_settings.default_renderer_asset_id);
    }
#endif // EG_DISABLE_RENDERER

    while (PeekMessageA(&s_context_current->window_message, 0, 0, 0, PM_REMOVE)) {

      TranslateMessage(&s_context_current->window_message);
      DispatchMessageA(&s_context_current->window_message);
    }

    QueryPerformanceCounter(&s_context_current->time_start); // TODO: measure whole loop..?

#ifndef EG_DISABLE_RENDERER
    eg_renderer_update();
    eg_renderer_draw();
#endif // EG_DISABLE_RENDERER

    QueryPerformanceCounter(&s_context_current->time_end); // TODO: measure whole loop..?

    s_context_current->delta_time = ((double)(s_context_current->time_end.QuadPart - s_context_current->time_start.QuadPart)) / ((double)s_context_current->time_freq.QuadPart);
    s_context_current->time += s_context_current->delta_time;

    s_context_current->elapsed_time_since_fps_count_update += s_context_current->delta_time;
    s_context_current->fps_counter++;
    s_context_current->frame_index++;

    if (s_context_current->elapsed_time_since_fps_count_update > 1.0) {

      s_context_current->elapsed_time_since_fps_count_update = 0.0;
      s_context_current->fps_counter = 0;
    }
  }
}
void eg_context_destroy(void) {
  EG_VULKAN_CHECK(vkQueueWaitIdle(eg_context_primary_queue()));
  EG_VULKAN_CHECK(vkQueueWaitIdle(eg_context_present_queue()));

#ifndef EG_DISABLE_SCENE
  eg_scene_destroy(s_context_current->main_scene);
#endif // EG_DISABLE_SCENE

#ifndef EG_DISABLE_RENDERER
  eg_renderer_destroy();
#endif // EG_DISABLE_RENDERER

#ifndef EG_DISABLE_SWAPCHAIN
  eg_swapchain_destroy();
#endif // EG_DISABLE_SWAPCHAIN

  eg_renderpass_destroy_gbuffer();
  eg_renderpass_destroy_main();

  eg_context_destroy_command_pool();
  eg_context_destroy_device();
  eg_context_destroy_surface();
  eg_context_destroy_instance();
  eg_context_destroy_window();

  eg_heap_free(s_context_current);

  eg_database_destroy();
}

HWND eg_context_window_handle(void) {
  return s_context_current->window_handle;
}
uint8_t eg_context_is_editor_mode(void) {
  return s_context_current->is_editor_mode;
}
uint32_t eg_context_mouse_position_x(void) {
  return s_context_current->mouse_position_x;
}
uint32_t eg_context_mouse_position_y(void) {
  return s_context_current->mouse_position_y;
}
float eg_context_time(void) {
  return (float)s_context_current->time;
}
float eg_context_delta_time(void) {
  return (float)s_context_current->delta_time;
}
VkInstance eg_context_instance(void) {
  return s_context_current->instance;
}
VkPhysicalDevice eg_context_physical_device(void) {
  return s_context_current->physical_device;
}
VkDevice eg_context_device(void) {
  return s_context_current->device;
}
VkCommandPool eg_context_command_pool(void) {
  return s_context_current->command_pool;
}
int32_t eg_context_primary_queue_index(void) {
  return s_context_current->primary_queue_index;
}
int32_t eg_context_present_queue_index(void) {
  return s_context_current->present_queue_index;
}
VkQueue eg_context_primary_queue(void) {
  return s_context_current->primary_queue;
}
VkQueue eg_context_present_queue(void) {
  return s_context_current->present_queue;
}
VkSurfaceKHR eg_context_surface(void) {
  return s_context_current->surface;
}
uint32_t eg_context_surface_min_image_count(void) {
  return s_context_current->surface_capabilities.minImageCount;
}
uint32_t eg_context_surface_max_image_count(void) {
  return s_context_current->surface_capabilities.maxImageCount;
}
VkSurfaceTransformFlagBitsKHR eg_context_surface_transform(void) {
  return s_context_current->surface_capabilities.currentTransform;
}
VkPresentModeKHR eg_context_surface_present_mode(void) {
  return s_context_current->prefered_present_mode;
}
VkFormat eg_context_surface_image_color_format(void) {
  return s_context_current->prefered_surface_format.format;
}
VkFormat eg_context_surface_image_depth_format(void) {
  return s_context_current->prefered_image_depth_format;
}
VkColorSpaceKHR eg_context_surface_image_color_space(void) {
  return s_context_current->prefered_surface_format.colorSpace;
}
uint32_t eg_context_window_width(void) {
  return s_context_current->window_width;
}
uint32_t eg_context_window_height(void) {
  return s_context_current->window_height;
}
uint32_t eg_context_titlebar_height(void) {
  return s_context_current->window_titlebar_height;
}
uint32_t eg_context_statusbar_height(void) {
  return s_context_current->window_statusbar_height;
}
float eg_context_max_anisotropy(void) {
  return s_context_current->physical_device_properties.limits.maxSamplerAnisotropy;
}
eg_viewport_t **eg_context_viewports(void) {
  return s_context_current->viewport;
}
eg_scene_t **eg_context_scenes(void) {
  return s_context_current->scene;
}
eg_viewport_t *eg_context_main_viewport(void) {
  return s_context_current->main_viewport;
}
eg_scene_t *eg_context_main_scene(void) {
  return s_context_current->main_scene;
}

void eg_context_set_running(uint8_t is_running) {
  s_context_current->is_window_running = is_running;
}
void eg_context_set_viewport(eg_viewport_t *viewport, uint64_t *index) {
  while (s_context_current->viewport[*index]) {
    (*index)++;
  }

  EG_ASSERT((*index) < 0xFF, "Invalid viewport index");

  s_context_current->viewport[*index] = viewport;
}
void eg_context_set_scene(eg_scene_t *scene, uint64_t *index) {
  while (s_context_current->scene[*index]) {
    (*index)++;
  }

  EG_ASSERT((*index) < 0xFF, "Invalid scene index");

  s_context_current->scene[*index] = scene;
}

uint8_t eg_context_is_keyboard_key_pressed(eg_keyboard_key_t key) {
  return s_context_current->event_keyboard_key_states[key] == EG_KEY_STATE_PRESSED;
}
uint8_t eg_context_is_keyboard_key_held(eg_keyboard_key_t key) {
  return (s_context_current->event_keyboard_key_states[key] == EG_KEY_STATE_DOWN) || (s_context_current->event_keyboard_key_states[key] == EG_KEY_STATE_PRESSED);
}
uint8_t eg_context_is_keyboard_key_released(eg_keyboard_key_t key) {
  return s_context_current->event_keyboard_key_states[key] == EG_KEY_STATE_RELEASED;
}

uint8_t eg_context_is_mouse_key_pressed(eg_mouse_key_t key) {
  return s_context_current->event_mouse_key_states[key] == EG_KEY_STATE_PRESSED;
}
uint8_t eg_context_is_mouse_key_held(eg_mouse_key_t key) {
  return (s_context_current->event_mouse_key_states[key] == EG_KEY_STATE_DOWN) || (s_context_current->event_mouse_key_states[key] == EG_KEY_STATE_PRESSED);
}
uint8_t eg_context_is_mouse_key_released(eg_mouse_key_t key) {
  return s_context_current->event_mouse_key_states[key] == EG_KEY_STATE_RELEASED;
}

int32_t eg_context_find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags memory_property_flags) {
  int32_t memory_type = -1;

  uint32_t memory_type_index = 0;

  while (memory_type_index < s_context_current->physical_device_memory_properties.memoryTypeCount) {

    if ((type_filter & (1 << memory_type_index)) && ((s_context_current->physical_device_memory_properties.memoryTypes[memory_type_index].propertyFlags & memory_property_flags) == memory_property_flags)) {
      memory_type = (int32_t)memory_type_index;

      break;
    }

    memory_type_index++;
  }

  return memory_type;
}

VkCommandBuffer eg_context_begin_command_buffer(void) {
  VkCommandBuffer command_buffer = 0;

  VkCommandBufferAllocateInfo command_buffer_allocate_info = {0};
  command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_allocate_info.commandPool = s_context_current->command_pool;
  command_buffer_allocate_info.commandBufferCount = 1;

  EG_VULKAN_CHECK(vkAllocateCommandBuffers(s_context_current->device, &command_buffer_allocate_info, &command_buffer));

  VkCommandBufferBeginInfo command_buffer_begin_info = {0};
  command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  EG_VULKAN_CHECK(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));

  return command_buffer;
}
void eg_context_end_command_buffer(VkCommandBuffer command_buffer) {
  EG_VULKAN_CHECK(vkEndCommandBuffer(command_buffer));

  VkSubmitInfo submit_info = {0};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffer;

  EG_VULKAN_CHECK(vkQueueSubmit(s_context_current->primary_queue, 1, &submit_info, 0));
  EG_VULKAN_CHECK(vkQueueWaitIdle(s_context_current->primary_queue));

  vkFreeCommandBuffers(s_context_current->device, s_context_current->command_pool, 1, &command_buffer);
}

static LRESULT eg_context_window_message_proc(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param) {
  eg_context_t *context = (eg_context_t *)GetWindowLongPtr(window_handle, GWLP_USERDATA);

  if (context) {
    if (context->is_editor_mode) {
      g_context_editor_message_proc(window_handle, message, w_param, l_param);
    }
  }

  switch (message) {
    case WM_CREATE: {

      ShowWindow(window_handle, SW_SHOW);

      break;
    }
    case WM_CLOSE: {

      context->is_window_running = 0;

      break;
    }
    case WM_NCCREATE: {

      SetWindowLongPtr(window_handle, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT *)l_param)->lpCreateParams);

      return TRUE;
    }
    case WM_NCDESTROY: {

      SetWindowLongPtr(window_handle, GWLP_USERDATA, 0);

      break;
    }
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN: {

      UINT scan_code = MapVirtualKeyA((UINT)w_param, MAPVK_VK_TO_VSC);
      UINT virtual_key = MapVirtualKeyExA(scan_code, MAPVK_VSC_TO_VK_EX, GetKeyboardLayout(0));

      switch (virtual_key) {
        case EG_KEYBOARD_KEY_LEFT_SHIFT:
          context->event_keyboard_key_states[EG_KEYBOARD_KEY_LEFT_SHIFT] = ((context->event_keyboard_key_states[EG_KEYBOARD_KEY_LEFT_SHIFT] == EG_KEY_STATE_UP) || (context->event_keyboard_key_states[EG_KEYBOARD_KEY_LEFT_SHIFT] == EG_KEY_STATE_RELEASED)) ? EG_KEY_STATE_PRESSED : EG_KEY_STATE_DOWN;
          break;
        case EG_KEYBOARD_KEY_RIGHT_SHIFT:
          context->event_keyboard_key_states[EG_KEYBOARD_KEY_RIGHT_SHIFT] = ((context->event_keyboard_key_states[EG_KEYBOARD_KEY_RIGHT_SHIFT] == EG_KEY_STATE_UP) || (context->event_keyboard_key_states[EG_KEYBOARD_KEY_RIGHT_SHIFT] == EG_KEY_STATE_RELEASED)) ? EG_KEY_STATE_PRESSED : EG_KEY_STATE_DOWN;
          break;
        case EG_KEYBOARD_KEY_LEFT_CONTROL:
          context->event_keyboard_key_states[EG_KEYBOARD_KEY_LEFT_CONTROL] = ((context->event_keyboard_key_states[EG_KEYBOARD_KEY_LEFT_CONTROL] == EG_KEY_STATE_UP) || (context->event_keyboard_key_states[EG_KEYBOARD_KEY_LEFT_CONTROL] == EG_KEY_STATE_RELEASED)) ? EG_KEY_STATE_PRESSED : EG_KEY_STATE_DOWN;
          break;
        case EG_KEYBOARD_KEY_RIGHT_CONTROL:
          context->event_keyboard_key_states[EG_KEYBOARD_KEY_RIGHT_CONTROL] = ((context->event_keyboard_key_states[EG_KEYBOARD_KEY_RIGHT_CONTROL] == EG_KEY_STATE_UP) || (context->event_keyboard_key_states[EG_KEYBOARD_KEY_RIGHT_CONTROL] == EG_KEY_STATE_RELEASED)) ? EG_KEY_STATE_PRESSED : EG_KEY_STATE_DOWN;
          break;
        case EG_KEYBOARD_KEY_LEFT_MENU:
          context->event_keyboard_key_states[EG_KEYBOARD_KEY_LEFT_MENU] = ((context->event_keyboard_key_states[EG_KEYBOARD_KEY_LEFT_MENU] == EG_KEY_STATE_UP) || (context->event_keyboard_key_states[EG_KEYBOARD_KEY_LEFT_MENU] == EG_KEY_STATE_RELEASED)) ? EG_KEY_STATE_PRESSED : EG_KEY_STATE_DOWN;
          break;
        case EG_KEYBOARD_KEY_RIGHT_MENU:
          context->event_keyboard_key_states[EG_KEYBOARD_KEY_RIGHT_MENU] = ((context->event_keyboard_key_states[EG_KEYBOARD_KEY_RIGHT_MENU] == EG_KEY_STATE_UP) || (context->event_keyboard_key_states[EG_KEYBOARD_KEY_RIGHT_MENU] == EG_KEY_STATE_RELEASED)) ? EG_KEY_STATE_PRESSED : EG_KEY_STATE_DOWN;
          break;
        default:
          context->event_keyboard_key_states[virtual_key] = ((context->event_keyboard_key_states[virtual_key] == EG_KEY_STATE_UP) || (context->event_keyboard_key_states[virtual_key] == EG_KEY_STATE_RELEASED)) ? EG_KEY_STATE_PRESSED : EG_KEY_STATE_DOWN;
          break;
      }

      break;
    }
    case WM_KEYUP:
    case WM_SYSKEYUP: {

      UINT scan_code = MapVirtualKeyA((UINT)w_param, MAPVK_VK_TO_VSC);
      UINT virtual_key = MapVirtualKeyExA(scan_code, MAPVK_VSC_TO_VK_EX, GetKeyboardLayout(0));

      switch (virtual_key) {
        case EG_KEYBOARD_KEY_LEFT_SHIFT:
          context->event_keyboard_key_states[EG_KEYBOARD_KEY_LEFT_SHIFT] = ((context->event_keyboard_key_states[EG_KEYBOARD_KEY_LEFT_SHIFT] == EG_KEY_STATE_DOWN) || (context->event_keyboard_key_states[EG_KEYBOARD_KEY_LEFT_SHIFT] == EG_KEY_STATE_PRESSED)) ? EG_KEY_STATE_RELEASED : EG_KEY_STATE_UP;
          break;
        case EG_KEYBOARD_KEY_RIGHT_SHIFT:
          context->event_keyboard_key_states[EG_KEYBOARD_KEY_RIGHT_SHIFT] = ((context->event_keyboard_key_states[EG_KEYBOARD_KEY_RIGHT_SHIFT] == EG_KEY_STATE_DOWN) || (context->event_keyboard_key_states[EG_KEYBOARD_KEY_RIGHT_SHIFT] == EG_KEY_STATE_PRESSED)) ? EG_KEY_STATE_RELEASED : EG_KEY_STATE_UP;
          break;
        case EG_KEYBOARD_KEY_LEFT_CONTROL:
          context->event_keyboard_key_states[EG_KEYBOARD_KEY_LEFT_CONTROL] = ((context->event_keyboard_key_states[EG_KEYBOARD_KEY_LEFT_CONTROL] == EG_KEY_STATE_DOWN) || (context->event_keyboard_key_states[EG_KEYBOARD_KEY_LEFT_CONTROL] == EG_KEY_STATE_PRESSED)) ? EG_KEY_STATE_RELEASED : EG_KEY_STATE_UP;
          break;
        case EG_KEYBOARD_KEY_RIGHT_CONTROL:
          context->event_keyboard_key_states[EG_KEYBOARD_KEY_RIGHT_CONTROL] = ((context->event_keyboard_key_states[EG_KEYBOARD_KEY_RIGHT_CONTROL] == EG_KEY_STATE_DOWN) || (context->event_keyboard_key_states[EG_KEYBOARD_KEY_RIGHT_CONTROL] == EG_KEY_STATE_PRESSED)) ? EG_KEY_STATE_RELEASED : EG_KEY_STATE_UP;
          break;
        case EG_KEYBOARD_KEY_LEFT_MENU:
          context->event_keyboard_key_states[EG_KEYBOARD_KEY_LEFT_MENU] = ((context->event_keyboard_key_states[EG_KEYBOARD_KEY_LEFT_MENU] == EG_KEY_STATE_DOWN) || (context->event_keyboard_key_states[EG_KEYBOARD_KEY_LEFT_MENU] == EG_KEY_STATE_PRESSED)) ? EG_KEY_STATE_RELEASED : EG_KEY_STATE_UP;
          break;
        case EG_KEYBOARD_KEY_RIGHT_MENU:
          context->event_keyboard_key_states[EG_KEYBOARD_KEY_RIGHT_MENU] = ((context->event_keyboard_key_states[EG_KEYBOARD_KEY_RIGHT_MENU] == EG_KEY_STATE_DOWN) || (context->event_keyboard_key_states[EG_KEYBOARD_KEY_RIGHT_MENU] == EG_KEY_STATE_PRESSED)) ? EG_KEY_STATE_RELEASED : EG_KEY_STATE_UP;
          break;
        default:
          context->event_keyboard_key_states[virtual_key] = ((context->event_keyboard_key_states[virtual_key] == EG_KEY_STATE_DOWN) || (context->event_keyboard_key_states[virtual_key] == EG_KEY_STATE_PRESSED)) ? EG_KEY_STATE_RELEASED : EG_KEY_STATE_UP;
          break;
      }

      break;
    }
    case WM_LBUTTONDOWN: {

      context->event_mouse_key_states[EG_MOUSE_KEY_LEFT] = ((context->event_mouse_key_states[EG_MOUSE_KEY_LEFT] == EG_KEY_STATE_UP) || (context->event_mouse_key_states[EG_MOUSE_KEY_LEFT] == EG_KEY_STATE_RELEASED)) ? EG_KEY_STATE_PRESSED : EG_KEY_STATE_DOWN;

      break;
    }
    case WM_LBUTTONUP: {

      context->event_mouse_key_states[EG_MOUSE_KEY_LEFT] = ((context->event_mouse_key_states[EG_MOUSE_KEY_LEFT] == EG_KEY_STATE_DOWN) || (context->event_mouse_key_states[EG_MOUSE_KEY_LEFT] == EG_KEY_STATE_PRESSED)) ? EG_KEY_STATE_RELEASED : EG_KEY_STATE_UP;

      break;
    }
    case WM_MBUTTONDOWN: {

      context->event_mouse_key_states[EG_MOUSE_KEY_MIDDLE] = ((context->event_mouse_key_states[EG_MOUSE_KEY_MIDDLE] == EG_KEY_STATE_UP) || (context->event_mouse_key_states[EG_MOUSE_KEY_MIDDLE] == EG_KEY_STATE_RELEASED)) ? EG_KEY_STATE_PRESSED : EG_KEY_STATE_DOWN;

      break;
    }
    case WM_MBUTTONUP: {

      context->event_mouse_key_states[EG_MOUSE_KEY_MIDDLE] = ((context->event_mouse_key_states[EG_MOUSE_KEY_MIDDLE] == EG_KEY_STATE_DOWN) || (context->event_mouse_key_states[EG_MOUSE_KEY_MIDDLE] == EG_KEY_STATE_PRESSED)) ? EG_KEY_STATE_RELEASED : EG_KEY_STATE_UP;

      break;
    }
    case WM_RBUTTONDOWN: {

      context->event_mouse_key_states[EG_MOUSE_KEY_RIGHT] = ((context->event_mouse_key_states[EG_MOUSE_KEY_RIGHT] == EG_KEY_STATE_UP) || (context->event_mouse_key_states[EG_MOUSE_KEY_RIGHT] == EG_KEY_STATE_RELEASED)) ? EG_KEY_STATE_PRESSED : EG_KEY_STATE_DOWN;

      break;
    }
    case WM_RBUTTONUP: {

      context->event_mouse_key_states[EG_MOUSE_KEY_RIGHT] = ((context->event_mouse_key_states[EG_MOUSE_KEY_RIGHT] == EG_KEY_STATE_DOWN) || (context->event_mouse_key_states[EG_MOUSE_KEY_RIGHT] == EG_KEY_STATE_PRESSED)) ? EG_KEY_STATE_RELEASED : EG_KEY_STATE_UP;

      break;
    }
    case WM_LBUTTONDBLCLK: {

      break;
    }
    case WM_MBUTTONDBLCLK: {

      break;
    }
    case WM_RBUTTONDBLCLK: {

      break;
    }
    case WM_MOUSEMOVE: {

      INT mouse_x = LOWORD(l_param);
      INT mouse_y = HIWORD(l_param);

      context->mouse_position_x = mouse_x;
      context->mouse_position_y = mouse_y;

      break;
    }
    case WM_MOUSEWHEEL: {

      context->mouse_wheel_delta = GET_WHEEL_DELTA_WPARAM(w_param);

      break;
    }
    case WM_NCCALCSIZE: {

      INT border_size_x = GetSystemMetrics(SM_CXFRAME);
      INT border_size_y = GetSystemMetrics(SM_CYFRAME);

      NCCALCSIZE_PARAMS *calc_size_params = (NCCALCSIZE_PARAMS *)l_param;
      RECT *requested_client_rect = calc_size_params->rgrc;

      requested_client_rect->right -= border_size_x;
      requested_client_rect->left += border_size_x;
      requested_client_rect->bottom -= border_size_y;
      requested_client_rect->top += 0;

      return WVR_ALIGNLEFT | WVR_ALIGNTOP;
    }
    case WM_NCHITTEST: {

      INT mouse_x = LOWORD(l_param);
      INT mouse_y = HIWORD(l_param);
      INT border_size_y = GetSystemMetrics(SM_CYFRAME);

      POINT pt = {mouse_x, mouse_y};
      ScreenToClient(window_handle, &pt);

      RECT rect = {0};
      GetClientRect(window_handle, &rect);

      BOOL left = pt.x <= context->window_border_width;
      BOOL right = pt.x >= (rect.right - context->window_border_width);
      BOOL top = (pt.y <= context->window_border_width) || (pt.y < border_size_y);
      BOOL bottom = pt.y >= (rect.bottom - context->window_border_width);

      if (top && left) {
        return HTTOPLEFT;
      }

      if (top && right) {
        return HTTOPRIGHT;
      }

      if (bottom && left) {
        return HTBOTTOMLEFT;
      }

      if (bottom && right) {
        return HTBOTTOMRIGHT;
      }

      if (left) {
        return HTLEFT;
      }

      if (right) {
        return HTRIGHT;
      }

      if (top) {
        return HTTOP;
      }

      if (bottom) {
        return HTBOTTOM;
      }

      return HTCLIENT;
    }
    default: {

      return DefWindowProcA(window_handle, message, w_param, l_param);
    }
  }

  return 1;
}

#ifdef BUILD_DEBUG
static VkBool32 eg_context_vulkan_message_proc(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, VkDebugUtilsMessengerCallbackDataEXT const *callback_data, void *user_data) {
  printf("%s\n", callback_data->pMessage);

  return 0;
}
#endif // BUILD_DEBUG

static void eg_context_create_window(void) {
  s_context_current->module_handle = GetModuleHandleA(0);

  WNDCLASSEX window_class_ex = {0};
  window_class_ex.cbSize = sizeof(WNDCLASSEX);
  window_class_ex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  window_class_ex.lpfnWndProc = eg_context_window_message_proc;
  window_class_ex.cbClsExtra = 0;
  window_class_ex.cbWndExtra = 0;
  window_class_ex.hInstance = s_context_current->module_handle;
  window_class_ex.hIcon = LoadIconA(0, IDI_APPLICATION);
  window_class_ex.hCursor = LoadCursorA(0, IDC_ARROW);
  window_class_ex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  window_class_ex.lpszMenuName = 0;
  window_class_ex.lpszClassName = s_context_window_class;
  window_class_ex.hIconSm = LoadIconA(0, IDI_APPLICATION);

  RegisterClassExA(&window_class_ex);

  INT screen_width = GetSystemMetrics(SM_CXSCREEN);
  INT screen_height = GetSystemMetrics(SM_CYSCREEN);
  INT window_position_x = (screen_width - s_context_current->window_width) / 2;
  INT window_position_y = (screen_height - s_context_current->window_height) / 2;

  DWORD style = 0;

  style |= WS_POPUP;
  style |= WS_THICKFRAME;

  s_context_current->window_handle = CreateWindowExA(0, s_context_window_class, s_context_window_name, style, window_position_x, window_position_y, s_context_current->window_width, s_context_current->window_height, 0, 0, s_context_current->module_handle, s_context_current);
}
static void eg_context_create_instance(void) {
  VkApplicationInfo app_info = {0};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "";
  app_info.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
  app_info.pEngineName = "";
  app_info.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_1;

  VkInstanceCreateInfo instance_create_info = {0};
  instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_create_info.pApplicationInfo = &app_info;
  instance_create_info.enabledExtensionCount = EG_ARRAY_COUNT(s_context_layer_extensions);
  instance_create_info.ppEnabledExtensionNames = s_context_layer_extensions;

#ifdef BUILD_DEBUG
  VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {0};
  debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  debug_create_info.pfnUserCallback = eg_context_vulkan_message_proc;

  instance_create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debug_create_info;
  instance_create_info.enabledLayerCount = EG_ARRAY_COUNT(s_context_validation_layers);
  instance_create_info.ppEnabledLayerNames = s_context_validation_layers;
#endif // BUILD_DEBUG

  EG_VULKAN_CHECK(vkCreateInstance(&instance_create_info, 0, &s_context_current->instance));

#ifdef BUILD_DEBUG
  s_context_current->create_debug_utils_messenger_proc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(s_context_current->instance, "vkCreateDebugUtilsMessengerEXT");
  s_context_current->destroy_debug_utils_messenger_proc = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(s_context_current->instance, "vkDestroyDebugUtilsMessengerEXT");

  EG_VULKAN_CHECK(s_context_current->create_debug_utils_messenger_proc(s_context_current->instance, &debug_create_info, 0, &s_context_current->debug_messenger));
#endif // BUILD_DEBUG
}
static void eg_context_create_surface(void) {
  VkWin32SurfaceCreateInfoKHR surface_create_info = {0};
  surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  surface_create_info.hwnd = s_context_current->window_handle;
  surface_create_info.hinstance = s_context_current->module_handle;

  EG_VULKAN_CHECK(vkCreateWin32SurfaceKHR(s_context_current->instance, &surface_create_info, 0, &s_context_current->surface));
}
static void eg_context_create_device(void) {
  VkDeviceQueueCreateInfo device_queue_create_infos[2] = {0};

  float queue_priority = 1.0F;

  device_queue_create_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  device_queue_create_infos[0].queueFamilyIndex = s_context_current->primary_queue_index;
  device_queue_create_infos[0].queueCount = 1;
  device_queue_create_infos[0].pQueuePriorities = &queue_priority;

  device_queue_create_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  device_queue_create_infos[1].queueFamilyIndex = s_context_current->present_queue_index;
  device_queue_create_infos[1].queueCount = 1;
  device_queue_create_infos[1].pQueuePriorities = &queue_priority;

  VkPhysicalDeviceDescriptorIndexingFeatures physical_device_descriptor_indexing_features = {0};
  physical_device_descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
  physical_device_descriptor_indexing_features.pNext = 0;

  VkPhysicalDeviceFeatures2 physical_device_features_2 = {0};
  physical_device_features_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
  physical_device_features_2.pNext = &physical_device_descriptor_indexing_features;

  vkGetPhysicalDeviceFeatures2(s_context_current->physical_device, &physical_device_features_2);

  if (!physical_device_descriptor_indexing_features.runtimeDescriptorArray) {
    // TODO: properly handle the lack of unsupported features..
  }

  // TODO
  // physical_device_features_2.features.samplerAnisotropy = 1;
  // physical_device_features_2.features.shaderFloat64 = 1;

  VkDeviceCreateInfo device_create_info = {0};
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.pQueueCreateInfos = device_queue_create_infos;
  device_create_info.queueCreateInfoCount = EG_ARRAY_COUNT(device_queue_create_infos);
  device_create_info.pEnabledFeatures = 0;
  device_create_info.ppEnabledExtensionNames = s_context_device_extensions;
  device_create_info.enabledExtensionCount = EG_ARRAY_COUNT(s_context_device_extensions);
  device_create_info.pNext = &physical_device_features_2;

#ifdef BUILD_DEBUG
  device_create_info.ppEnabledLayerNames = s_context_validation_layers;
  device_create_info.enabledLayerCount = EG_ARRAY_COUNT(s_context_validation_layers);
#endif // BUILD_DEBUG

  EG_VULKAN_CHECK(vkCreateDevice(s_context_current->physical_device, &device_create_info, 0, &s_context_current->device));

  vkGetDeviceQueue(s_context_current->device, s_context_current->primary_queue_index, 0, &s_context_current->primary_queue);
  vkGetDeviceQueue(s_context_current->device, s_context_current->present_queue_index, 0, &s_context_current->present_queue);
}
static void eg_context_create_command_pool(void) {
  VkCommandPoolCreateInfo command_pool_create_info = {0};
  command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  command_pool_create_info.queueFamilyIndex = s_context_current->primary_queue_index;

  EG_VULKAN_CHECK(vkCreateCommandPool(s_context_current->device, &command_pool_create_info, 0, &s_context_current->command_pool));
}

static void eg_context_destroy_window(void) {
  DestroyWindow(s_context_current->window_handle);

  UnregisterClassA(s_context_window_class, s_context_current->module_handle);
}
static void eg_context_destroy_instance(void) {
#ifdef BUILD_DEBUG
  s_context_current->destroy_debug_utils_messenger_proc(s_context_current->instance, s_context_current->debug_messenger, 0);
#endif // BUILD_DEBUG

  vkDestroyInstance(s_context_current->instance, 0);
}
static void eg_context_destroy_surface(void) {
  vkDestroySurfaceKHR(s_context_current->instance, s_context_current->surface, 0);
}
static void eg_context_destroy_device(void) {
  vkDestroyDevice(s_context_current->device, 0);
}
static void eg_context_destroy_command_pool(void) {
  vkDestroyCommandPool(s_context_current->device, s_context_current->command_pool, 0);
}

static void eg_context_check_surface_capabilities(void) {
  int32_t surface_format_count = 0;
  EG_VULKAN_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(s_context_current->physical_device, s_context_current->surface, &surface_format_count, 0));

  VkSurfaceFormatKHR *surface_formats = (VkSurfaceFormatKHR *)eg_heap_alloc(sizeof(VkSurfaceFormatKHR) * surface_format_count, 0, 0);
  EG_VULKAN_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(s_context_current->physical_device, s_context_current->surface, &surface_format_count, surface_formats));

  int32_t present_mode_count = 0;
  EG_VULKAN_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(s_context_current->physical_device, s_context_current->surface, &present_mode_count, 0));

  VkPresentModeKHR *present_modes = (VkPresentModeKHR *)eg_heap_alloc(sizeof(VkPresentModeKHR) * present_mode_count, 0, 0);
  EG_VULKAN_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(s_context_current->physical_device, s_context_current->surface, &present_mode_count, present_modes));

  uint64_t surface_format_index = 0;

  while (surface_format_index < surface_format_count) {

    VkSurfaceFormatKHR surface_format = surface_formats[surface_format_index];

    if ((surface_format.format == VK_FORMAT_B8G8R8A8_UNORM) && (surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)) {

      s_context_current->prefered_surface_format = surface_format;

      break;
    }

    surface_format_index++;
  }

  uint64_t present_mode_index = 0;

  while (present_mode_index < present_mode_count) {

    VkPresentModeKHR present_mode = present_modes[present_mode_index];

    if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {

      s_context_current->prefered_present_mode = present_mode;

      break;
    }

    present_mode_index++;
  }

  eg_heap_free(surface_formats);
  eg_heap_free(present_modes);
}
static void eg_context_check_physical_device_extensions(void) {
  int32_t available_device_extension_count = 0;
  EG_VULKAN_CHECK(vkEnumerateDeviceExtensionProperties(s_context_current->physical_device, 0, &available_device_extension_count, 0));

  VkExtensionProperties *available_device_extensions = (VkExtensionProperties *)eg_heap_alloc(sizeof(VkExtensionProperties) * available_device_extension_count, 0, 0);
  EG_VULKAN_CHECK(vkEnumerateDeviceExtensionProperties(s_context_current->physical_device, 0, &available_device_extension_count, available_device_extensions));

  printf("Device Extensions\n");

  uint64_t device_extension_index = 0;
  uint64_t device_extension_count = EG_ARRAY_COUNT(s_context_device_extensions);

  while (device_extension_index < device_extension_count) {

    uint8_t device_extensions_available = 0;

    uint64_t available_device_extension_index = 0;

    while (available_device_extension_index < available_device_extension_count) {

      VkExtensionProperties properties = available_device_extensions[available_device_extension_index];

      if (strcmp(s_context_device_extensions[device_extension_index], properties.extensionName) == 0) {

        printf("  Found %s\n", s_context_device_extensions[device_extension_index]);

        device_extensions_available = 1;

        break;
      }

      available_device_extension_index++;
    }

    if (device_extensions_available == 0) {

      printf("  Missing %s\n", s_context_device_extensions[device_extension_index]);

      break;
    }

    device_extension_index++;
  }

  printf("\n");

  eg_heap_free(available_device_extensions);
}

static void eg_context_resize_surface(void) {
  EG_VULKAN_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(s_context_current->physical_device, s_context_current->surface, &s_context_current->surface_capabilities));

  s_context_current->window_width = s_context_current->surface_capabilities.currentExtent.width;
  s_context_current->window_height = s_context_current->surface_capabilities.currentExtent.height;
}

static void eg_context_find_physical_device(void) {
  int32_t physical_device_count = 0;
  EG_VULKAN_CHECK(vkEnumeratePhysicalDevices(s_context_current->instance, &physical_device_count, 0));

  VkPhysicalDevice *physical_devices = (VkPhysicalDevice *)eg_heap_alloc(sizeof(VkPhysicalDevice) * physical_device_count, 0, 0);
  EG_VULKAN_CHECK(vkEnumeratePhysicalDevices(s_context_current->instance, &physical_device_count, physical_devices));

  uint64_t physical_device_index = 0;

  while (physical_device_index < physical_device_count) {

    VkPhysicalDevice physical_device = physical_devices[physical_device_index];

    vkGetPhysicalDeviceProperties(physical_device, &s_context_current->physical_device_properties);
    vkGetPhysicalDeviceFeatures(physical_device, &s_context_current->physical_device_features);
    vkGetPhysicalDeviceMemoryProperties(physical_device, &s_context_current->physical_device_memory_properties);

    if (s_context_current->physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {

      if (s_context_current->physical_device_features.geometryShader &&
          s_context_current->physical_device_features.samplerAnisotropy &&
          s_context_current->physical_device_features.shaderFloat64) {

        s_context_current->physical_device = physical_device;

        break;
      }
    }

    physical_device_index++;
  }

  eg_heap_free(physical_devices);
}
static void eg_context_find_physical_device_queue_families(void) {
  int32_t queue_family_property_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(s_context_current->physical_device, &queue_family_property_count, 0);

  VkQueueFamilyProperties *queue_family_properties = (VkQueueFamilyProperties *)eg_heap_alloc(sizeof(VkQueueFamilyProperties) * queue_family_property_count, 0, 0);
  vkGetPhysicalDeviceQueueFamilyProperties(s_context_current->physical_device, &queue_family_property_count, queue_family_properties);

  uint64_t physical_device_queue_family_property_index = 0;

  while (physical_device_queue_family_property_index < queue_family_property_count) {

    VkQueueFamilyProperties properties = queue_family_properties[physical_device_queue_family_property_index];

    uint32_t graphic_support = 0;
    uint32_t compute_support = 0;
    uint32_t present_support = 0;

    graphic_support = properties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
    compute_support = properties.queueFlags & VK_QUEUE_COMPUTE_BIT;

    EG_VULKAN_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(s_context_current->physical_device, (uint32_t)physical_device_queue_family_property_index, s_context_current->surface, &present_support));

    if (graphic_support && compute_support && (s_context_current->primary_queue_index == -1)) {

      s_context_current->primary_queue_index = (uint32_t)physical_device_queue_family_property_index;

    } else if (present_support && (s_context_current->present_queue_index == -1)) {

      s_context_current->present_queue_index = (uint32_t)physical_device_queue_family_property_index;
    }

    if ((s_context_current->primary_queue_index != -1) && (s_context_current->present_queue_index != -1)) {
      break;
    }

    physical_device_queue_family_property_index++;
  }

  eg_heap_free(queue_family_properties);

  printf("Queue Indices\n");
  printf("  Primary Queue Index %d\n", s_context_current->primary_queue_index);
  printf("  Present Queue Index %d\n", s_context_current->present_queue_index);
  printf("\n");
}

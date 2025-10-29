#include <engine/eg_pch.h>
#include <engine/eg_context.h>
#include <engine/eg_swapchain.h>
#include <engine/eg_renderer.h>

static LRESULT context_window_message_proc(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

#ifdef BUILD_DEBUG
static VkBool32 context_vulkan_message_proc(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, VkDebugUtilsMessengerCallbackDataEXT const *callback_data, void *user_data);
#endif // BUILD_DEBUG

static void context_create_window(context_t *context);
static void context_create_instance(context_t *context);
static void context_create_surface(context_t *context);
static void context_create_device(context_t *context);
static void context_create_command_pool(context_t *context);

static void context_destroy_window(context_t *context);
static void context_destroy_instance(context_t *context);
static void context_destroy_surface(context_t *context);
static void context_destroy_device(context_t *context);
static void context_destroy_command_pool(context_t *context);

static void context_check_surface_capabilities(context_t *context);
static void context_check_physical_device_extensions(context_t *context);

static void context_resize_surface(context_t *context);

static void context_find_physical_device(context_t *context);
static void context_find_physical_device_queue_families(context_t *context);

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

imgui_create_proc_t g_context_imgui_create_proc = 0;
imgui_draw_proc_t g_context_imgui_draw_proc = 0;
imgui_destroy_proc_t g_context_imgui_destroy_proc = 0;
imgui_message_proc_t g_context_imgui_message_proc = 0;

context_t *context_create(int32_t width, int32_t height) {
  context_t *context = (context_t *)heap_alloc(sizeof(context_t), 1, 0);

  context->surface_width = width;
  context->surface_height = height;
  context->graphic_queue_index = -1;
  context->present_queue_index = -1;

  context_create_window(context);
  context_create_instance(context);
  context_create_surface(context);

  context_find_physical_device(context);
  context_find_physical_device_queue_families(context);

  context_check_physical_device_extensions(context);

  context_create_device(context);

  context_check_surface_capabilities(context);

  context_resize_surface(context);

  context_create_command_pool(context);

  database_create();

  swapchain_create(context);
  renderer_create(context);

  QueryPerformanceFrequency(&context->timer_freq);

  return context;
}
uint8_t context_is_running(context_t *context) {
  return context->window_should_close == 0;
}
void context_begin_frame(context_t *context) {
  context->mouse_wheel_delta = 0;

  uint8_t keyboard_key_index = 0;
  uint8_t keyboard_key_count = 0xFF;

  while (keyboard_key_index < keyboard_key_count) {

    if (context->event_keyboard_key_states[keyboard_key_index] == KEY_STATE_PRESSED) {
      context->event_keyboard_key_states[keyboard_key_index] = KEY_STATE_DOWN;
    } else if (context->event_keyboard_key_states[keyboard_key_index] == KEY_STATE_RELEASED) {
      context->event_keyboard_key_states[keyboard_key_index] = KEY_STATE_UP;
    }

    keyboard_key_index++;
  }

  uint8_t mouse_key_index = 0;
  uint8_t mouse_key_count = 0x3;

  while (mouse_key_index < mouse_key_count) {

    if (context->event_mouse_key_states[mouse_key_index] == KEY_STATE_PRESSED) {
      context->event_mouse_key_states[mouse_key_index] = KEY_STATE_DOWN;
    } else if (context->event_mouse_key_states[mouse_key_index] == KEY_STATE_RELEASED) {
      context->event_mouse_key_states[mouse_key_index] = KEY_STATE_UP;
    }

    mouse_key_index++;
  }

  if (context->swapchain->is_dirty) {
    context->swapchain->is_dirty = 0;

    renderer_destroy(context->renderer);
    swapchain_destroy(context->swapchain);

    context_resize_surface(context);

    swapchain_create(context);
    renderer_create(context);
  }

  if (context->renderer->is_dirty) {
    context->renderer->is_dirty = 0;

    renderer_destroy(context->renderer);

    renderer_create(context);
  }

  while (PeekMessageA(&context->window_message, 0, 0, 0, PM_REMOVE)) {
    TranslateMessage(&context->window_message);
    DispatchMessageA(&context->window_message);
  }

  QueryPerformanceCounter(&context->timer_begin);
}
void context_end_frame(context_t *context) {
  QueryPerformanceCounter(&context->timer_end);

  context->delta_time = (((double)context->timer_end.QuadPart) - ((double)context->timer_begin.QuadPart)) / ((double)context->timer_freq.QuadPart);
  context->time += context->delta_time;
}
void context_destroy(context_t *context) {
  renderer_destroy(context->renderer);
  swapchain_destroy(context->swapchain);

  database_destroy();

  context_destroy_command_pool(context);
  context_destroy_device(context);
  context_destroy_surface(context);
  context_destroy_instance(context);
  context_destroy_window(context);

  heap_free(context);
}

uint8_t context_is_keyboard_key_pressed(context_t *context, keyboard_key_t key) {
  return context->event_keyboard_key_states[key] == KEY_STATE_PRESSED;
}
uint8_t context_is_keyboard_key_held(context_t *context, keyboard_key_t key) {
  return (context->event_keyboard_key_states[key] == KEY_STATE_DOWN) || (context->event_keyboard_key_states[key] == KEY_STATE_PRESSED);
}
uint8_t context_is_keyboard_key_released(context_t *context, keyboard_key_t key) {
  return context->event_keyboard_key_states[key] == KEY_STATE_RELEASED;
}

uint8_t context_is_mouse_key_pressed(context_t *context, mouse_key_t key) {
  return context->event_mouse_key_states[key] == KEY_STATE_PRESSED;
}
uint8_t context_is_mouse_key_held(context_t *context, mouse_key_t key) {
  return (context->event_mouse_key_states[key] == KEY_STATE_DOWN) || (context->event_mouse_key_states[key] == KEY_STATE_PRESSED);
}
uint8_t context_is_mouse_key_released(context_t *context, mouse_key_t key) {
  return context->event_mouse_key_states[key] == KEY_STATE_RELEASED;
}

int32_t context_find_memory_type(context_t *context, uint32_t type_filter, VkMemoryPropertyFlags memory_property_flags) {
  int32_t memory_type = -1;

  uint32_t memory_type_index = 0;

  while (memory_type_index < context->physical_device_memory_properties.memoryTypeCount) {

    if ((type_filter & (1 << memory_type_index)) && ((context->physical_device_memory_properties.memoryTypes[memory_type_index].propertyFlags & memory_property_flags) == memory_property_flags)) {
      memory_type = (int32_t)memory_type_index;

      break;
    }

    memory_type_index++;
  }

  return memory_type;
}

VkCommandBuffer context_begin_command_buffer(context_t *context) {
  VkCommandBuffer command_buffer = 0;

  VkCommandBufferAllocateInfo command_buffer_allocate_info = {0};
  command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_allocate_info.commandPool = context->command_pool;
  command_buffer_allocate_info.commandBufferCount = 1;

  VULKAN_CHECK(vkAllocateCommandBuffers(context->device, &command_buffer_allocate_info, &command_buffer));

  VkCommandBufferBeginInfo command_buffer_begin_info = {0};
  command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  VULKAN_CHECK(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));

  return command_buffer;
}
void context_end_command_buffer(context_t *context, VkCommandBuffer command_buffer) {
  VULKAN_CHECK(vkEndCommandBuffer(command_buffer));

  VkSubmitInfo submit_info = {0};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffer;

  VULKAN_CHECK(vkQueueSubmit(context->graphic_queue, 1, &submit_info, 0));
  VULKAN_CHECK(vkQueueWaitIdle(context->graphic_queue));

  vkFreeCommandBuffers(context->device, context->command_pool, 1, &command_buffer);
}

static LRESULT context_window_message_proc(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param) {
  if (g_context_imgui_message_proc) {
    g_context_imgui_message_proc(window_handle, message, w_param, l_param);
  }

  context_t *context = (context_t *)GetWindowLongPtr(window_handle, GWLP_USERDATA);

  switch (message) {
    case WM_CREATE: {
      ShowWindow(window_handle, SW_SHOW);

      break;
    }
    case WM_CLOSE: {
      context->window_should_close = 1;

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
        case KEYBOARD_KEY_LEFT_SHIFT:
          context->event_keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] = ((context->event_keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] == KEY_STATE_UP) || (context->event_keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        case KEYBOARD_KEY_RIGHT_SHIFT:
          context->event_keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] = ((context->event_keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] == KEY_STATE_UP) || (context->event_keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        case KEYBOARD_KEY_LEFT_CONTROL:
          context->event_keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] = ((context->event_keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] == KEY_STATE_UP) || (context->event_keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        case KEYBOARD_KEY_RIGHT_CONTROL:
          context->event_keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] = ((context->event_keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] == KEY_STATE_UP) || (context->event_keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        case KEYBOARD_KEY_LEFT_MENU:
          context->event_keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] = ((context->event_keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] == KEY_STATE_UP) || (context->event_keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        case KEYBOARD_KEY_RIGHT_MENU:
          context->event_keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] = ((context->event_keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] == KEY_STATE_UP) || (context->event_keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
        default:
          context->event_keyboard_key_states[virtual_key] = ((context->event_keyboard_key_states[virtual_key] == KEY_STATE_UP) || (context->event_keyboard_key_states[virtual_key] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;
          break;
      }

      break;
    }
    case WM_KEYUP:
    case WM_SYSKEYUP: {
      UINT scan_code = MapVirtualKeyA((UINT)w_param, MAPVK_VK_TO_VSC);
      UINT virtual_key = MapVirtualKeyExA(scan_code, MAPVK_VSC_TO_VK_EX, GetKeyboardLayout(0));

      switch (virtual_key) {
        case KEYBOARD_KEY_LEFT_SHIFT:
          context->event_keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] = ((context->event_keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] == KEY_STATE_DOWN) || (context->event_keyboard_key_states[KEYBOARD_KEY_LEFT_SHIFT] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        case KEYBOARD_KEY_RIGHT_SHIFT:
          context->event_keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] = ((context->event_keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] == KEY_STATE_DOWN) || (context->event_keyboard_key_states[KEYBOARD_KEY_RIGHT_SHIFT] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        case KEYBOARD_KEY_LEFT_CONTROL:
          context->event_keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] = ((context->event_keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] == KEY_STATE_DOWN) || (context->event_keyboard_key_states[KEYBOARD_KEY_LEFT_CONTROL] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        case KEYBOARD_KEY_RIGHT_CONTROL:
          context->event_keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] = ((context->event_keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] == KEY_STATE_DOWN) || (context->event_keyboard_key_states[KEYBOARD_KEY_RIGHT_CONTROL] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        case KEYBOARD_KEY_LEFT_MENU:
          context->event_keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] = ((context->event_keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] == KEY_STATE_DOWN) || (context->event_keyboard_key_states[KEYBOARD_KEY_LEFT_MENU] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        case KEYBOARD_KEY_RIGHT_MENU:
          context->event_keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] = ((context->event_keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] == KEY_STATE_DOWN) || (context->event_keyboard_key_states[KEYBOARD_KEY_RIGHT_MENU] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
        default:
          context->event_keyboard_key_states[virtual_key] = ((context->event_keyboard_key_states[virtual_key] == KEY_STATE_DOWN) || (context->event_keyboard_key_states[virtual_key] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;
          break;
      }

      break;
    }
    case WM_LBUTTONDOWN: {
      context->event_mouse_key_states[MOUSE_KEY_LEFT] = ((context->event_mouse_key_states[MOUSE_KEY_LEFT] == KEY_STATE_UP) || (context->event_mouse_key_states[MOUSE_KEY_LEFT] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;

      break;
    }
    case WM_LBUTTONUP: {
      context->event_mouse_key_states[MOUSE_KEY_LEFT] = ((context->event_mouse_key_states[MOUSE_KEY_LEFT] == KEY_STATE_DOWN) || (context->event_mouse_key_states[MOUSE_KEY_LEFT] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;

      break;
    }
    case WM_MBUTTONDOWN: {
      context->event_mouse_key_states[MOUSE_KEY_MIDDLE] = ((context->event_mouse_key_states[MOUSE_KEY_MIDDLE] == KEY_STATE_UP) || (context->event_mouse_key_states[MOUSE_KEY_MIDDLE] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;

      break;
    }
    case WM_MBUTTONUP: {
      context->event_mouse_key_states[MOUSE_KEY_MIDDLE] = ((context->event_mouse_key_states[MOUSE_KEY_MIDDLE] == KEY_STATE_DOWN) || (context->event_mouse_key_states[MOUSE_KEY_MIDDLE] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;

      break;
    }
    case WM_RBUTTONDOWN: {
      context->event_mouse_key_states[MOUSE_KEY_RIGHT] = ((context->event_mouse_key_states[MOUSE_KEY_RIGHT] == KEY_STATE_UP) || (context->event_mouse_key_states[MOUSE_KEY_RIGHT] == KEY_STATE_RELEASED)) ? KEY_STATE_PRESSED : KEY_STATE_DOWN;

      break;
    }
    case WM_RBUTTONUP: {
      context->event_mouse_key_states[MOUSE_KEY_RIGHT] = ((context->event_mouse_key_states[MOUSE_KEY_RIGHT] == KEY_STATE_DOWN) || (context->event_mouse_key_states[MOUSE_KEY_RIGHT] == KEY_STATE_PRESSED)) ? KEY_STATE_RELEASED : KEY_STATE_UP;

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
    default: {
      return DefWindowProcA(window_handle, message, w_param, l_param);
    }
  }

  return 1;
}

#ifdef BUILD_DEBUG
static VkBool32 context_vulkan_message_proc(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, VkDebugUtilsMessengerCallbackDataEXT const *callback_data, void *user_data) {
  printf("%s\n", callback_data->pMessage);

  return 0;
}
#endif // BUILD_DEBUG

static void context_create_window(context_t *context) {
  context->module_handle = GetModuleHandleA(0);

  WNDCLASSEX window_class_ex = {0};
  window_class_ex.cbSize = sizeof(WNDCLASSEX);
  window_class_ex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  window_class_ex.lpfnWndProc = context_window_message_proc;
  window_class_ex.cbClsExtra = 0;
  window_class_ex.cbWndExtra = 0;
  window_class_ex.hInstance = context->module_handle;
  window_class_ex.hIcon = LoadIconA(0, IDI_APPLICATION);
  window_class_ex.hCursor = LoadCursorA(0, IDC_ARROW);
  window_class_ex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  window_class_ex.lpszMenuName = 0;
  window_class_ex.lpszClassName = s_context_window_class;
  window_class_ex.hIconSm = LoadIconA(0, IDI_APPLICATION);

  RegisterClassExA(&window_class_ex);

  INT screen_width = GetSystemMetrics(SM_CXSCREEN);
  INT screen_height = GetSystemMetrics(SM_CYSCREEN);
  INT window_position_x = (screen_width - context->surface_width) / 2;
  INT window_position_y = (screen_height - context->surface_height) / 2;

  context->window_handle = CreateWindowExA(0, s_context_window_class, s_context_window_name, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, window_position_x, window_position_y, context->surface_width, context->surface_height, 0, 0, context->module_handle, context);
}
static void context_create_instance(context_t *context) {
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
  instance_create_info.enabledExtensionCount = ARRAY_COUNT(s_context_layer_extensions);
  instance_create_info.ppEnabledExtensionNames = s_context_layer_extensions;

#ifdef BUILD_DEBUG
  VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {0};
  debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  debug_create_info.pfnUserCallback = context_vulkan_message_proc;

  instance_create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debug_create_info;
  instance_create_info.enabledLayerCount = ARRAY_COUNT(s_context_validation_layers);
  instance_create_info.ppEnabledLayerNames = s_context_validation_layers;
#endif // BUILD_DEBUG

  VULKAN_CHECK(vkCreateInstance(&instance_create_info, 0, &context->instance));

#ifdef BUILD_DEBUG
  context->create_debug_utils_messenger_ext = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context->instance, "vkCreateDebugUtilsMessengerEXT");
  context->destroy_debug_utils_messenger_ext = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context->instance, "vkDestroyDebugUtilsMessengerEXT");

  VULKAN_CHECK(context->create_debug_utils_messenger_ext(context->instance, &debug_create_info, 0, &context->debug_messenger));
#endif // BUILD_DEBUG
}
static void context_create_surface(context_t *context) {
  VkWin32SurfaceCreateInfoKHR surface_create_info = {0};
  surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  surface_create_info.hwnd = context->window_handle;
  surface_create_info.hinstance = context->module_handle;

  VULKAN_CHECK(vkCreateWin32SurfaceKHR(context->instance, &surface_create_info, 0, &context->surface));
}
static void context_create_device(context_t *context) {
  VkDeviceQueueCreateInfo device_queue_create_infos[2] = {0};

  float queue_priority = 1.0F;

  device_queue_create_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  device_queue_create_infos[0].queueFamilyIndex = context->graphic_queue_index;
  device_queue_create_infos[0].queueCount = 1;
  device_queue_create_infos[0].pQueuePriorities = &queue_priority;

  device_queue_create_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  device_queue_create_infos[1].queueFamilyIndex = context->present_queue_index;
  device_queue_create_infos[1].queueCount = 1;
  device_queue_create_infos[1].pQueuePriorities = &queue_priority;

  VkPhysicalDeviceDescriptorIndexingFeatures physical_device_descriptor_indexing_features = {0};
  physical_device_descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
  physical_device_descriptor_indexing_features.pNext = 0;

  VkPhysicalDeviceFeatures2 physical_device_features_2 = {0};
  physical_device_features_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
  physical_device_features_2.pNext = &physical_device_descriptor_indexing_features;

  vkGetPhysicalDeviceFeatures2(context->physical_device, &physical_device_features_2);

  if (!physical_device_descriptor_indexing_features.runtimeDescriptorArray) {
    // TODO: properly handle the lack of unsupported features..
  }

  // TODO
  // physical_device_features_2.features.samplerAnisotropy = 1;
  // physical_device_features_2.features.shaderFloat64 = 1;

  VkDeviceCreateInfo device_create_info = {0};
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.pQueueCreateInfos = device_queue_create_infos;
  device_create_info.queueCreateInfoCount = ARRAY_COUNT(device_queue_create_infos);
  device_create_info.pEnabledFeatures = 0;
  device_create_info.ppEnabledExtensionNames = s_context_device_extensions;
  device_create_info.enabledExtensionCount = ARRAY_COUNT(s_context_device_extensions);
  device_create_info.pNext = &physical_device_features_2;

#ifdef BUILD_DEBUG
  device_create_info.ppEnabledLayerNames = s_context_validation_layers;
  device_create_info.enabledLayerCount = ARRAY_COUNT(s_context_validation_layers);
#endif // BUILD_DEBUG

  VULKAN_CHECK(vkCreateDevice(context->physical_device, &device_create_info, 0, &context->device));

  vkGetDeviceQueue(context->device, context->graphic_queue_index, 0, &context->graphic_queue);
  vkGetDeviceQueue(context->device, context->present_queue_index, 0, &context->present_queue);
}
static void context_create_command_pool(context_t *context) {
  VkCommandPoolCreateInfo command_pool_create_info = {0};
  command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  command_pool_create_info.queueFamilyIndex = context->graphic_queue_index;

  VULKAN_CHECK(vkCreateCommandPool(context->device, &command_pool_create_info, 0, &context->command_pool));
}

static void context_destroy_window(context_t *context) {
  DestroyWindow(context->window_handle);

  UnregisterClassA(s_context_window_class, context->module_handle);
}
static void context_destroy_instance(context_t *context) {
#ifdef BUILD_DEBUG
  context->destroy_debug_utils_messenger_ext(context->instance, context->debug_messenger, 0);
#endif // BUILD_DEBUG

  vkDestroyInstance(context->instance, 0);
}
static void context_destroy_surface(context_t *context) {
  vkDestroySurfaceKHR(context->instance, context->surface, 0);
}
static void context_destroy_device(context_t *context) {
  vkDestroyDevice(context->device, 0);
}
static void context_destroy_command_pool(context_t *context) {
  vkDestroyCommandPool(context->device, context->command_pool, 0);
}

static void context_check_surface_capabilities(context_t *context) {
  int32_t surface_format_count = 0;
  VULKAN_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(context->physical_device, context->surface, &surface_format_count, 0));

  VkSurfaceFormatKHR *surface_formats = (VkSurfaceFormatKHR *)heap_alloc(sizeof(VkSurfaceFormatKHR) * surface_format_count, 0, 0);
  VULKAN_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(context->physical_device, context->surface, &surface_format_count, surface_formats));

  int32_t present_mode_count = 0;
  VULKAN_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(context->physical_device, context->surface, &present_mode_count, 0));

  VkPresentModeKHR *present_modes = (VkPresentModeKHR *)heap_alloc(sizeof(VkPresentModeKHR) * present_mode_count, 0, 0);
  VULKAN_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(context->physical_device, context->surface, &present_mode_count, present_modes));

  uint64_t surface_format_index = 0;
  while (surface_format_index < surface_format_count) {
    VkSurfaceFormatKHR surface_format = surface_formats[surface_format_index];

    if ((surface_format.format == VK_FORMAT_B8G8R8A8_UNORM) && (surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)) {
      context->prefered_surface_format = surface_format;

      break;
    }

    surface_format_index++;
  }

  uint64_t present_mode_index = 0;
  while (present_mode_index < present_mode_count) {
    VkPresentModeKHR present_mode = present_modes[present_mode_index];

    if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      context->prefered_present_mode = present_mode;

      break;
    }

    present_mode_index++;
  }

  heap_free(surface_formats);
  heap_free(present_modes);
}
static void context_check_physical_device_extensions(context_t *context) {
  int32_t available_device_extension_count = 0;
  VULKAN_CHECK(vkEnumerateDeviceExtensionProperties(context->physical_device, 0, &available_device_extension_count, 0));

  VkExtensionProperties *available_device_extensions = (VkExtensionProperties *)heap_alloc(sizeof(VkExtensionProperties) * available_device_extension_count, 0, 0);
  VULKAN_CHECK(vkEnumerateDeviceExtensionProperties(context->physical_device, 0, &available_device_extension_count, available_device_extensions));

  printf("Device Extensions\n");

  uint64_t device_extension_index = 0;
  uint64_t device_extension_count = ARRAY_COUNT(s_context_device_extensions);
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

  heap_free(available_device_extensions);
}

static void context_resize_surface(context_t *context) {
  VULKAN_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->physical_device, context->surface, &context->surface_capabilities));

  context->surface_width = context->surface_capabilities.currentExtent.width;
  context->surface_height = context->surface_capabilities.currentExtent.height;
}

static void context_find_physical_device(context_t *context) {
  int32_t physical_device_count = 0;
  VULKAN_CHECK(vkEnumeratePhysicalDevices(context->instance, &physical_device_count, 0));

  VkPhysicalDevice *physical_devices = (VkPhysicalDevice *)heap_alloc(sizeof(VkPhysicalDevice) * physical_device_count, 0, 0);
  VULKAN_CHECK(vkEnumeratePhysicalDevices(context->instance, &physical_device_count, physical_devices));

  uint64_t physical_device_index = 0;
  while (physical_device_index < physical_device_count) {
    VkPhysicalDevice physical_device = physical_devices[physical_device_index];

    vkGetPhysicalDeviceProperties(physical_device, &context->physical_device_properties);
    vkGetPhysicalDeviceFeatures(physical_device, &context->physical_device_features);
    vkGetPhysicalDeviceMemoryProperties(physical_device, &context->physical_device_memory_properties);

    if (context->physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      if (context->physical_device_features.geometryShader &&
          context->physical_device_features.samplerAnisotropy &&
          context->physical_device_features.shaderFloat64) {
        context->physical_device = physical_device;

        break;
      }
    }

    physical_device_index++;
  }

  heap_free(physical_devices);
}
static void context_find_physical_device_queue_families(context_t *context) {
  int32_t queue_family_property_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(context->physical_device, &queue_family_property_count, 0);

  VkQueueFamilyProperties *queue_family_properties = (VkQueueFamilyProperties *)heap_alloc(sizeof(VkQueueFamilyProperties) * queue_family_property_count, 0, 0);
  vkGetPhysicalDeviceQueueFamilyProperties(context->physical_device, &queue_family_property_count, queue_family_properties);

  uint64_t physical_device_queue_family_property_index = 0;
  while (physical_device_queue_family_property_index < queue_family_property_count) {
    VkQueueFamilyProperties properties = queue_family_properties[physical_device_queue_family_property_index];

    uint32_t graphic_support = 0;
    uint32_t compute_support = 0;
    uint32_t present_support = 0;

    graphic_support = properties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
    compute_support = properties.queueFlags & VK_QUEUE_COMPUTE_BIT;

    VULKAN_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(context->physical_device, (uint32_t)physical_device_queue_family_property_index, context->surface, &present_support));

    if (graphic_support && compute_support && (context->graphic_queue_index == -1)) {
      context->graphic_queue_index = (uint32_t)physical_device_queue_family_property_index;
    } else if (present_support && (context->present_queue_index == -1)) {
      context->present_queue_index = (uint32_t)physical_device_queue_family_property_index;
    }

    if ((context->graphic_queue_index != -1) && (context->present_queue_index != -1)) {
      break;
    }

    physical_device_queue_family_property_index++;
  }

  heap_free(queue_family_properties);

  printf("Queue Indices\n");
  printf("  Graphic Queue Index %d\n", context->graphic_queue_index);
  printf("  Present Queue Index %d\n", context->present_queue_index);
  printf("\n");
}

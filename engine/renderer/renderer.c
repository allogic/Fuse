#include <stdlib.h>

#include <library/core/api.h>

#include <engine/context.h>
#include <engine/macros.h>
#include <engine/swapchain.h>

#include <engine/renderer/config.h>
#include <engine/renderer/forward.h>
#include <engine/renderer/macros.h>
#include <engine/renderer/renderer.h>
#include <engine/renderer/pipeline.h>

// TODO: implement sparse textures..

static void renderer_compute_local_variables(void);

static void renderer_build_pipelines(void);

static void renderer_create_command_buffer(void);
static void renderer_create_sync_objects(void);

static void renderer_create_time_buffer(void);
static void renderer_create_screen_buffer(void);
static void renderer_create_camera_buffer(void);
static void renderer_create_debug_line_vertex_buffer(void);
static void renderer_create_debug_line_index_buffer(void);

static void renderer_update_uniform_buffers(transform_t *transform, camera_t *camera);

static void renderer_record_compute_commands(void);
static void renderer_record_graphics_commands(void);

static void renderer_destroy_command_buffer(void);
static void renderer_destroy_sync_objects(void);

static void renderer_destroy_time_buffer(void);
static void renderer_destroy_screen_buffer(void);
static void renderer_destroy_camera_buffer(void);
static void renderer_destroy_debug_line_vertex_buffer(void);
static void renderer_destroy_debug_line_index_buffer(void);

static void renderer_destroy_pipelines(void);

uint8_t g_renderer_is_dirty = 0;

uint8_t g_renderer_enable_debug = 0;

int32_t g_renderer_frames_in_flight = 0;

static VkCommandBuffer *s_renderer_graphic_command_buffers = 0;

static VkSemaphore *s_renderer_graphic_complete_semaphores = 0;
static VkSemaphore *s_renderer_present_complete_semaphores = 0;

static VkFence *s_renderer_frame_fences = 0;

static int32_t s_renderer_frames_in_flight = 0;

static int32_t s_renderer_frame_index = 0;
static int32_t s_renderer_image_index = 0;

static vector_t s_renderer_pipelines = {0};

static VkBuffer *s_renderer_time_buffer = 0;
static VkBuffer *s_renderer_screen_buffer = 0;
static VkBuffer *s_renderer_camera_buffer = 0;
static VkBuffer *s_renderer_debug_line_vertex_buffer = 0;
static VkBuffer *s_renderer_debug_line_index_buffer = 0;

static VkDeviceMemory *s_renderer_time_buffer_device_memory = 0;
static VkDeviceMemory *s_renderer_screen_buffer_device_memory = 0;
static VkDeviceMemory *s_renderer_camera_buffer_device_memory = 0;
static VkDeviceMemory *s_renderer_debug_line_vertex_buffer_device_memory = 0;
static VkDeviceMemory *s_renderer_debug_line_index_buffer_device_memory = 0;

static renderer_time_t **s_renderer_time = 0;
static renderer_screen_t **s_renderer_screen = 0;
static renderer_camera_t **s_renderer_camera = 0;
static renderer_debug_line_vertex_t **s_renderer_debug_line_vertex = 0;
static renderer_debug_line_index_t **s_renderer_debug_line_index = 0;

static int32_t *s_renderer_debug_line_vertex_offset = 0;
static int32_t *s_renderer_debug_line_index_offset = 0;

void renderer_create(void) {
  renderer_compute_local_variables();

  renderer_create_command_buffer();
  renderer_create_sync_objects();

  renderer_build_pipelines();

  renderer_create_time_buffer();
  renderer_create_screen_buffer();
  renderer_create_camera_buffer();
  renderer_create_debug_line_vertex_buffer();
  renderer_create_debug_line_index_buffer();

  s_renderer_debug_line_vertex_offset = (int32_t *)heap_alloc(sizeof(int32_t) * s_renderer_frames_in_flight);
  s_renderer_debug_line_index_offset = (int32_t *)heap_alloc(sizeof(int32_t) * s_renderer_frames_in_flight);
}
void renderer_update(void) {
  if (g_renderer_enable_debug) {
    vector3_t right_position = {0.0F, 0.0F, 0.0F};
    vector3_t right_direction = {1.0F, 0.0F, 0.0F};
    vector4_t right_color = {1.0F, 0.0F, 0.0F, 1.0F};

    renderer_draw_debug_line(right_position, right_direction, right_color);

    vector3_t up_position = {0.0F, 0.0F, 0.0F};
    vector3_t up_direction = {0.0F, 1.0F, 0.0F};
    vector4_t up_color = {0.0F, 1.0F, 0.0F, 1.0F};

    renderer_draw_debug_line(up_position, up_direction, up_color);

    vector3_t front_position = {0.0F, 0.0F, 0.0F};
    vector3_t front_direction = {0.0F, 0.0F, 1.0F};
    vector4_t front_color = {0.0F, 0.0F, 1.0F, 1.0F};

    renderer_draw_debug_line(front_position, front_direction, front_color);
  }
}
void renderer_draw(transform_t *transform, camera_t *camera) {
  VkResult wait_for_frame_fence_result = vkWaitForFences(g_context_device, 1, &s_renderer_frame_fences[s_renderer_frame_index], 1, UINT64_MAX);

  switch (wait_for_frame_fence_result) {
    case VK_SUCCESS: {
      break;
    }
    case VK_ERROR_DEVICE_LOST: {
      break;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  VkResult reset_frame_fence_result = vkResetFences(g_context_device, 1, &s_renderer_frame_fences[s_renderer_frame_index]);

  switch (reset_frame_fence_result) {
    case VK_SUCCESS: {
      break;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  VkResult reset_graphics_command_buffer_result = vkResetCommandBuffer(s_renderer_graphic_command_buffers[s_renderer_frame_index], 0);

  switch (reset_graphics_command_buffer_result) {
    case VK_SUCCESS: {
      break;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  VkResult acquire_next_image_result = vkAcquireNextImageKHR(
    g_context_device,
    g_swapchain,
    UINT64_MAX,
    s_renderer_present_complete_semaphores[s_renderer_frame_index],
    0,
    &s_renderer_image_index);

  switch (acquire_next_image_result) {
    case VK_SUCCESS: {
      break;
    }
    case VK_NOT_READY: {
      return;
    }
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR: {
      g_swapchain_is_dirty = 1;

      return;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  renderer_update_uniform_buffers(transform, camera);

  VkCommandBufferBeginInfo command_buffer_begin_info = {0};
  command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  command_buffer_begin_info.pInheritanceInfo = 0;

  VkResult begin_graphics_command_buffer_result = vkBeginCommandBuffer(s_renderer_graphic_command_buffers[s_renderer_frame_index], &command_buffer_begin_info);

  switch (begin_graphics_command_buffer_result) {
    case VK_SUCCESS: {
      break;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  renderer_record_compute_commands();
  renderer_record_graphics_commands();

  VkResult end_graphics_command_buffer_result = vkEndCommandBuffer(s_renderer_graphic_command_buffers[s_renderer_frame_index]);

  switch (end_graphics_command_buffer_result) {
    case VK_SUCCESS: {
      break;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  VkPipelineStageFlags graphics_wait_stages[] = {VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  VkSubmitInfo graphics_submit_info = {0};
  graphics_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  graphics_submit_info.pWaitSemaphores = &s_renderer_present_complete_semaphores[s_renderer_frame_index];
  graphics_submit_info.waitSemaphoreCount = 1;
  graphics_submit_info.pSignalSemaphores = &s_renderer_graphic_complete_semaphores[s_renderer_frame_index];
  graphics_submit_info.signalSemaphoreCount = 1;
  graphics_submit_info.pCommandBuffers = &s_renderer_graphic_command_buffers[s_renderer_frame_index];
  graphics_submit_info.commandBufferCount = 1;
  graphics_submit_info.pWaitDstStageMask = graphics_wait_stages;

  VkResult graphics_queue_submit_result = vkQueueSubmit(g_context_graphics_queue, 1, &graphics_submit_info, s_renderer_frame_fences[s_renderer_frame_index]);

  switch (graphics_queue_submit_result) {
    case VK_SUCCESS: {
      break;
    }
    case VK_ERROR_DEVICE_LOST: {
      return;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  VkPresentInfoKHR present_info = {0};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.pWaitSemaphores = &s_renderer_graphic_complete_semaphores[s_renderer_frame_index];
  present_info.waitSemaphoreCount = 1;
  present_info.pSwapchains = &g_swapchain;
  present_info.swapchainCount = 1;
  present_info.pImageIndices = &s_renderer_image_index;

  VkResult present_queue_result = vkQueuePresentKHR(g_context_present_queue, &present_info);

  switch (present_queue_result) {
    case VK_SUCCESS: {
      break;
    }
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR: {
      g_swapchain_is_dirty = 1;

      return;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  s_renderer_frame_index = (s_renderer_frame_index + 1) % s_renderer_frames_in_flight;
}
void renderer_destroy(void) {
  VULKAN_CHECK(vkQueueWaitIdle(g_context_graphics_queue));
  VULKAN_CHECK(vkQueueWaitIdle(g_context_present_queue));

  heap_free(s_renderer_debug_line_vertex_offset);
  heap_free(s_renderer_debug_line_index_offset);

  renderer_destroy_time_buffer();
  renderer_destroy_screen_buffer();
  renderer_destroy_camera_buffer();
  renderer_destroy_debug_line_vertex_buffer();
  renderer_destroy_debug_line_index_buffer();

  renderer_destroy_pipelines();

  renderer_destroy_sync_objects();
  renderer_destroy_command_buffer();
}

void renderer_draw_debug_line(vector3_t from, vector3_t to, vector4_t color) {
  if (g_renderer_enable_debug) {
    s_renderer_debug_line_vertex[s_renderer_frame_index][s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 0].position = (vector3_t){from.x, from.y, from.z};
    s_renderer_debug_line_vertex[s_renderer_frame_index][s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 1].position = (vector3_t){to.x, to.y, to.z};

    s_renderer_debug_line_vertex[s_renderer_frame_index][s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 0].color = (vector4_t){color.x, color.y, color.z, color.w};
    s_renderer_debug_line_vertex[s_renderer_frame_index][s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 1].color = (vector4_t){color.x, color.y, color.z, color.w};

    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 0] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 0;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 1] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 1;

    s_renderer_debug_line_vertex_offset[s_renderer_frame_index] += 2;
    s_renderer_debug_line_index_offset[s_renderer_frame_index] += 2;
  }
}
void renderer_draw_debug_box(vector3_t position, vector3_t size, vector4_t color) {
  if (g_renderer_enable_debug) {
    s_renderer_debug_line_vertex[s_renderer_frame_index][s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 0].position = (vector3_t){position.x, position.y, position.z};
    s_renderer_debug_line_vertex[s_renderer_frame_index][s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 1].position = (vector3_t){position.x, position.y + size.y, position.z};
    s_renderer_debug_line_vertex[s_renderer_frame_index][s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 2].position = (vector3_t){position.x + size.x, position.y, position.z};
    s_renderer_debug_line_vertex[s_renderer_frame_index][s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 3].position = (vector3_t){position.x + size.x, position.y + size.y, position.z};
    s_renderer_debug_line_vertex[s_renderer_frame_index][s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 4].position = (vector3_t){position.x, position.y, position.z + size.z};
    s_renderer_debug_line_vertex[s_renderer_frame_index][s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 5].position = (vector3_t){position.x, position.y + size.y, position.z + size.z};
    s_renderer_debug_line_vertex[s_renderer_frame_index][s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 6].position = (vector3_t){position.x + size.x, position.y, position.z + size.z};
    s_renderer_debug_line_vertex[s_renderer_frame_index][s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 7].position = (vector3_t){position.x + size.x, position.y + size.y, position.z + size.z};

    s_renderer_debug_line_vertex[s_renderer_frame_index][s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 0].color = (vector4_t){color.x, color.y, color.z, color.w};
    s_renderer_debug_line_vertex[s_renderer_frame_index][s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 1].color = (vector4_t){color.x, color.y, color.z, color.w};
    s_renderer_debug_line_vertex[s_renderer_frame_index][s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 2].color = (vector4_t){color.x, color.y, color.z, color.w};
    s_renderer_debug_line_vertex[s_renderer_frame_index][s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 3].color = (vector4_t){color.x, color.y, color.z, color.w};
    s_renderer_debug_line_vertex[s_renderer_frame_index][s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 4].color = (vector4_t){color.x, color.y, color.z, color.w};
    s_renderer_debug_line_vertex[s_renderer_frame_index][s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 5].color = (vector4_t){color.x, color.y, color.z, color.w};
    s_renderer_debug_line_vertex[s_renderer_frame_index][s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 6].color = (vector4_t){color.x, color.y, color.z, color.w};
    s_renderer_debug_line_vertex[s_renderer_frame_index][s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 7].color = (vector4_t){color.x, color.y, color.z, color.w};

    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 0] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 0;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 1] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 1;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 2] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 1;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 3] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 3;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 4] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 3;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 5] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 2;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 6] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 2;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 7] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 0;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 8] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 4;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 9] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 5;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 10] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 5;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 11] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 7;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 12] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 7;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 13] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 6;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 14] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 6;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 15] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 4;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 16] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 0;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 17] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 4;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 18] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 1;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 19] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 5;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 20] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 2;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 21] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 6;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 22] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 3;
    s_renderer_debug_line_index[s_renderer_frame_index][s_renderer_debug_line_index_offset[s_renderer_frame_index] + 23] = s_renderer_debug_line_vertex_offset[s_renderer_frame_index] + 7;

    s_renderer_debug_line_vertex_offset[s_renderer_frame_index] += 8;
    s_renderer_debug_line_index_offset[s_renderer_frame_index] += 24;
  }
}

static void renderer_compute_local_variables(void) {
  s_renderer_frames_in_flight = g_renderer_frames_in_flight;
  s_renderer_frames_in_flight = MAX(s_renderer_frames_in_flight, 1);
  s_renderer_frames_in_flight = MIN(s_renderer_frames_in_flight, g_swapchain_image_count);
  g_renderer_frames_in_flight = s_renderer_frames_in_flight;

  s_renderer_frame_index = 0;
  s_renderer_image_index = 0;
}

static void renderer_build_pipelines(void) {
  s_renderer_pipelines = vector_create(sizeof(pipeline_t));

  pipeline_t chunk_renderer_pipeline = pipeline_create(PIPELINE_TYPE_GRAPHIC, s_renderer_frames_in_flight, "chunk_renderer", ROOT_DIR "/shader/chunk/renderer.vert.spv", ROOT_DIR "/shader/chunk/renderer.frag.spv");
  pipeline_t debug_line_pipeline = pipeline_create(PIPELINE_TYPE_GRAPHIC, s_renderer_frames_in_flight, "debug_line", ROOT_DIR "/shader/debug/line.vert.spv", ROOT_DIR "/shader/debug//line.frag.spv");

  vector_push(&s_renderer_pipelines, &chunk_renderer_pipeline);
  vector_push(&s_renderer_pipelines, &debug_line_pipeline);
}

static void renderer_create_command_buffer(void) {
  s_renderer_graphic_command_buffers = (VkCommandBuffer *)heap_alloc(sizeof(VkCommandBuffer) * s_renderer_frames_in_flight);

  VkCommandBufferAllocateInfo command_buffer_alloc_create_info = {0};
  command_buffer_alloc_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_alloc_create_info.commandPool = g_context_command_pool;
  command_buffer_alloc_create_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_alloc_create_info.commandBufferCount = s_renderer_frames_in_flight;

  VULKAN_CHECK(vkAllocateCommandBuffers(g_context_device, &command_buffer_alloc_create_info, s_renderer_graphic_command_buffers));
}
static void renderer_create_sync_objects(void) {
  s_renderer_graphic_complete_semaphores = (VkSemaphore *)heap_alloc(sizeof(VkSemaphore) * s_renderer_frames_in_flight);
  s_renderer_present_complete_semaphores = (VkSemaphore *)heap_alloc(sizeof(VkSemaphore) * s_renderer_frames_in_flight);
  s_renderer_frame_fences = (VkFence *)heap_alloc(sizeof(VkFence) * s_renderer_frames_in_flight);

  VkSemaphoreCreateInfo semaphore_create_info = {0};
  semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphore_create_info.flags = 0;

  VkFenceCreateInfo fence_create_info = {0};
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    VULKAN_CHECK(vkCreateSemaphore(g_context_device, &semaphore_create_info, 0, &s_renderer_graphic_complete_semaphores[frame_index]));
    VULKAN_CHECK(vkCreateSemaphore(g_context_device, &semaphore_create_info, 0, &s_renderer_present_complete_semaphores[frame_index]));

    VULKAN_CHECK(vkCreateFence(g_context_device, &fence_create_info, 0, &s_renderer_frame_fences[frame_index]));

    frame_index++;
  }
}

static void renderer_create_time_buffer(void) {
  s_renderer_time_buffer = (VkBuffer *)heap_alloc(sizeof(VkBuffer) * s_renderer_frames_in_flight);
  s_renderer_time_buffer_device_memory = (VkDeviceMemory *)heap_alloc(sizeof(VkDeviceMemory) * s_renderer_frames_in_flight);
  s_renderer_time = (renderer_time_t **)heap_alloc(sizeof(renderer_time_t *) * s_renderer_frames_in_flight);

  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    VkDeviceSize size = sizeof(renderer_time_t);

    VkBufferCreateInfo buffer_create_info = {0};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = size;
    buffer_create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VULKAN_CHECK(vkCreateBuffer(g_context_device, &buffer_create_info, 0, &s_renderer_time_buffer[frame_index]));

    VkMemoryRequirements memory_requirements = {0};

    vkGetBufferMemoryRequirements(g_context_device, s_renderer_time_buffer[frame_index], &memory_requirements);

    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = context_find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);

    VULKAN_CHECK(vkAllocateMemory(g_context_device, &memory_allocate_info, 0, &s_renderer_time_buffer_device_memory[frame_index]));
    VULKAN_CHECK(vkBindBufferMemory(g_context_device, s_renderer_time_buffer[frame_index], s_renderer_time_buffer_device_memory[frame_index], 0));
    VULKAN_CHECK(vkMapMemory(g_context_device, s_renderer_time_buffer_device_memory[frame_index], 0, size, 0, &s_renderer_time[frame_index]));

    frame_index++;
  }
}
static void renderer_create_screen_buffer(void) {
  s_renderer_screen_buffer = (VkBuffer *)heap_alloc(sizeof(VkBuffer) * s_renderer_frames_in_flight);
  s_renderer_screen_buffer_device_memory = (VkDeviceMemory *)heap_alloc(sizeof(VkDeviceMemory) * s_renderer_frames_in_flight);
  s_renderer_screen = (renderer_screen_t **)heap_alloc(sizeof(renderer_screen_t *) * s_renderer_frames_in_flight);

  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    VkDeviceSize size = sizeof(renderer_screen_t);

    VkBufferCreateInfo buffer_create_info = {0};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = size;
    buffer_create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VULKAN_CHECK(vkCreateBuffer(g_context_device, &buffer_create_info, 0, &s_renderer_screen_buffer[frame_index]));

    VkMemoryRequirements memory_requirements = {0};

    vkGetBufferMemoryRequirements(g_context_device, s_renderer_screen_buffer[frame_index], &memory_requirements);

    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = context_find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);

    VULKAN_CHECK(vkAllocateMemory(g_context_device, &memory_allocate_info, 0, &s_renderer_screen_buffer_device_memory[frame_index]));
    VULKAN_CHECK(vkBindBufferMemory(g_context_device, s_renderer_screen_buffer[frame_index], s_renderer_screen_buffer_device_memory[frame_index], 0));
    VULKAN_CHECK(vkMapMemory(g_context_device, s_renderer_screen_buffer_device_memory[frame_index], 0, size, 0, &s_renderer_screen[frame_index]));

    frame_index++;
  }
}
static void renderer_create_camera_buffer(void) {
  s_renderer_camera_buffer = (VkBuffer *)heap_alloc(sizeof(VkBuffer) * s_renderer_frames_in_flight);
  s_renderer_camera_buffer_device_memory = (VkDeviceMemory *)heap_alloc(sizeof(VkDeviceMemory) * s_renderer_frames_in_flight);
  s_renderer_camera = (renderer_camera_t **)heap_alloc(sizeof(renderer_camera_t *) * s_renderer_frames_in_flight);

  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    VkDeviceSize size = sizeof(renderer_camera_t);

    VkBufferCreateInfo buffer_create_info = {0};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = size;
    buffer_create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VULKAN_CHECK(vkCreateBuffer(g_context_device, &buffer_create_info, 0, &s_renderer_camera_buffer[frame_index]));

    VkMemoryRequirements memory_requirements = {0};

    vkGetBufferMemoryRequirements(g_context_device, s_renderer_camera_buffer[frame_index], &memory_requirements);

    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = context_find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);

    VULKAN_CHECK(vkAllocateMemory(g_context_device, &memory_allocate_info, 0, &s_renderer_camera_buffer_device_memory[frame_index]));
    VULKAN_CHECK(vkBindBufferMemory(g_context_device, s_renderer_camera_buffer[frame_index], s_renderer_camera_buffer_device_memory[frame_index], 0));
    VULKAN_CHECK(vkMapMemory(g_context_device, s_renderer_camera_buffer_device_memory[frame_index], 0, size, 0, &s_renderer_camera[frame_index]));

    frame_index++;
  }
}
static void renderer_create_debug_line_vertex_buffer(void) {
  s_renderer_debug_line_vertex_buffer = (VkBuffer *)heap_alloc(sizeof(VkBuffer) * s_renderer_frames_in_flight);
  s_renderer_debug_line_vertex_buffer_device_memory = (VkDeviceMemory *)heap_alloc(sizeof(VkDeviceMemory) * s_renderer_frames_in_flight);
  s_renderer_debug_line_vertex = (renderer_debug_line_vertex_t **)heap_alloc(sizeof(renderer_debug_line_vertex_t *) * s_renderer_frames_in_flight);

  uint64_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    VkDeviceSize size = sizeof(renderer_debug_line_vertex_t) * RENDERER_DEBUG_LINE_VERTEX_COUNT;

    VkBufferCreateInfo buffer_create_info = {0};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = size;
    buffer_create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VULKAN_CHECK(vkCreateBuffer(g_context_device, &buffer_create_info, 0, &s_renderer_debug_line_vertex_buffer[frame_index]));

    VkMemoryRequirements memory_requirements = {0};

    vkGetBufferMemoryRequirements(g_context_device, s_renderer_debug_line_vertex_buffer[frame_index], &memory_requirements);

    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = context_find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);

    VULKAN_CHECK(vkAllocateMemory(g_context_device, &memory_allocate_info, 0, &s_renderer_debug_line_vertex_buffer_device_memory[frame_index]));
    VULKAN_CHECK(vkBindBufferMemory(g_context_device, s_renderer_debug_line_vertex_buffer[frame_index], s_renderer_debug_line_vertex_buffer_device_memory[frame_index], 0));
    VULKAN_CHECK(vkMapMemory(g_context_device, s_renderer_debug_line_vertex_buffer_device_memory[frame_index], 0, size, 0, &s_renderer_debug_line_vertex[frame_index]));

    frame_index++;
  }
}
static void renderer_create_debug_line_index_buffer(void) {
  s_renderer_debug_line_index_buffer = (VkBuffer *)heap_alloc(sizeof(VkBuffer) * s_renderer_frames_in_flight);
  s_renderer_debug_line_index_buffer_device_memory = (VkDeviceMemory *)heap_alloc(sizeof(VkDeviceMemory) * s_renderer_frames_in_flight);
  s_renderer_debug_line_index = (renderer_debug_line_index_t **)heap_alloc(sizeof(renderer_debug_line_index_t *) * s_renderer_frames_in_flight);

  uint64_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    VkDeviceSize size = sizeof(renderer_debug_line_index_t) * RENDERER_DEBUG_LINE_INDEX_COUNT;

    VkBufferCreateInfo buffer_create_info = {0};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = size;
    buffer_create_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VULKAN_CHECK(vkCreateBuffer(g_context_device, &buffer_create_info, 0, &s_renderer_debug_line_index_buffer[frame_index]));

    VkMemoryRequirements memory_requirements = {0};

    vkGetBufferMemoryRequirements(g_context_device, s_renderer_debug_line_index_buffer[frame_index], &memory_requirements);

    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = context_find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);

    VULKAN_CHECK(vkAllocateMemory(g_context_device, &memory_allocate_info, 0, &s_renderer_debug_line_index_buffer_device_memory[frame_index]));
    VULKAN_CHECK(vkBindBufferMemory(g_context_device, s_renderer_debug_line_index_buffer[frame_index], s_renderer_debug_line_index_buffer_device_memory[frame_index], 0));
    VULKAN_CHECK(vkMapMemory(g_context_device, s_renderer_debug_line_index_buffer_device_memory[frame_index], 0, size, 0, &s_renderer_debug_line_index[frame_index]));

    frame_index++;
  }
}

static void renderer_update_uniform_buffers(transform_t *transform, camera_t *camera) {
  s_renderer_time[s_renderer_frame_index]->time = (float)g_context_time;
  s_renderer_time[s_renderer_frame_index]->delta_time = (float)g_context_delta_time;

  s_renderer_screen[s_renderer_frame_index]->resolution = vector2_xy((float)g_context_surface_width, (float)g_context_surface_height);

  vector3_t eye = transform->world_position;
  vector3_t center = vector3_add(transform->world_position, transform->local_front);
  vector3_t up = vector3_down();

  float fov = math_deg_to_rad(camera->fov);
  float aspect_ratio = (float)g_context_surface_width / (float)g_context_surface_height;
  float near_z = camera->near_z;
  float far_z = camera->far_z;

  matrix4_t view = matrix4_look_at(eye, center, up);
  matrix4_t projection = matrix4_persp(fov, aspect_ratio, near_z, far_z);
  matrix4_t view_projection = matrix4_mul(view, projection);
  matrix4_t view_projection_inv = matrix4_inverse(view_projection);

  s_renderer_camera[s_renderer_frame_index]->world_position = transform->world_position;
  s_renderer_camera[s_renderer_frame_index]->view = view;
  s_renderer_camera[s_renderer_frame_index]->projection = projection;
  s_renderer_camera[s_renderer_frame_index]->view_projection = view_projection;
  s_renderer_camera[s_renderer_frame_index]->view_projection_inv = view_projection_inv;
  s_renderer_camera[s_renderer_frame_index]->max_ray_steps = camera->ray_steps;
}

static void renderer_record_compute_commands(void) {
  // vkCmdBindPipeline(s_renderer_graphics_command_buffers[s_renderer_frame_index], VK_PIPELINE_BIND_POINT_COMPUTE, s_renderer_chunk_editor_pipeline);
  // vkCmdBindDescriptorSets(s_renderer_graphics_command_buffers[s_renderer_frame_index], VK_PIPELINE_BIND_POINT_COMPUTE, s_renderer_chunk_editor_pipeline_layout, 0, 1, &s_renderer_chunk_editor_descriptor_sets[s_renderer_frame_index], 0, 0);
  // vkCmdDispatch(s_renderer_graphics_command_buffers[s_renderer_frame_index], group_count_x, group_count_y, group_count_z);
}
static void renderer_record_graphics_commands(void) {
  VkClearValue color_clear_value = {0};
  color_clear_value.color.float32[0] = 0.0F;
  color_clear_value.color.float32[1] = 0.0F;
  color_clear_value.color.float32[2] = 0.0F;
  color_clear_value.color.float32[3] = 1.0F;

  VkClearValue depth_clear_value = {0};
  depth_clear_value.depthStencil.depth = 1.0F;
  depth_clear_value.depthStencil.stencil = 0;

  VkClearValue clear_values[] = {color_clear_value, depth_clear_value};

  VkRenderPassBeginInfo render_pass_create_info = {0};
  render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_create_info.renderPass = g_swapchain_render_pass;
  render_pass_create_info.framebuffer = g_swapchain_frame_buffers[s_renderer_image_index];
  render_pass_create_info.renderArea.offset.x = 0;
  render_pass_create_info.renderArea.offset.y = 0;
  render_pass_create_info.renderArea.extent.width = g_context_surface_width;
  render_pass_create_info.renderArea.extent.height = g_context_surface_height;
  render_pass_create_info.pClearValues = clear_values;
  render_pass_create_info.clearValueCount = ARRAY_COUNT(clear_values);

  vkCmdBeginRenderPass(s_renderer_graphic_command_buffers[s_renderer_frame_index], &render_pass_create_info, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport = {0};
  viewport.x = 0.0F;
  viewport.y = 0.0F;
  viewport.width = (float)g_context_surface_width;
  viewport.height = (float)g_context_surface_height;
  viewport.minDepth = 0.0F;
  viewport.maxDepth = 1.0F;

  vkCmdSetViewport(s_renderer_graphic_command_buffers[s_renderer_frame_index], 0, 1, &viewport);

  VkRect2D scissor = {0};
  scissor.offset.x = 0;
  scissor.offset.y = 0;
  scissor.extent.width = g_context_surface_width;
  scissor.extent.height = g_context_surface_height;

  vkCmdSetScissor(s_renderer_graphic_command_buffers[s_renderer_frame_index], 0, 1, &scissor);

  {
    // VkBuffer vertex_buffers[] = {s_renderer_cluster_vertex_buffer[s_renderer_frame_index]};
    // uint64_t vertex_offsets[] = {0, 0};
    //
    // vkCmdBindPipeline(s_renderer_graphics_command_buffers[s_renderer_frame_index], VK_PIPELINE_BIND_POINT_GRAPHICS, s_renderer_chunk_renderer_pipeline);
    // vkCmdBindVertexBuffers(s_renderer_graphics_command_buffers[s_renderer_frame_index], 0, ARRAY_COUNT(vertex_buffers), vertex_buffers, vertex_offsets);
    // vkCmdBindIndexBuffer(s_renderer_graphics_command_buffers[s_renderer_frame_index], s_renderer_cluster_index_buffer[s_renderer_frame_index], 0, VK_INDEX_TYPE_UINT32);
    // vkCmdBindDescriptorSets(s_renderer_graphics_command_buffers[s_renderer_frame_index], VK_PIPELINE_BIND_POINT_GRAPHICS, s_renderer_chunk_renderer_pipeline_layout, 0, 1, &s_renderer_chunk_renderer_descriptor_sets[s_renderer_frame_index], 0, 0);
    // vkCmdDrawIndexed(s_renderer_graphics_command_buffers[s_renderer_frame_index], RENDERER_CLUSTER_INDEX_COUNT, 1, 0, 0, 0);
  }

  {
    if (g_renderer_enable_debug) {
      VkBuffer vertex_buffers[] = {s_renderer_debug_line_vertex_buffer[s_renderer_frame_index]};
      uint64_t vertex_offsets[] = {0};

      // vkCmdBindPipeline(s_renderer_graphic_command_buffers[s_renderer_frame_index], VK_PIPELINE_BIND_POINT_GRAPHICS, s_renderer_debug_line_pipeline);
      // vkCmdBindVertexBuffers(s_renderer_graphic_command_buffers[s_renderer_frame_index], 0, ARRAY_COUNT(vertex_buffers), vertex_buffers, vertex_offsets);
      // vkCmdBindIndexBuffer(s_renderer_graphic_command_buffers[s_renderer_frame_index], s_renderer_debug_line_index_buffer[s_renderer_frame_index], 0, VK_INDEX_TYPE_UINT32);
      // vkCmdBindDescriptorSets(s_renderer_graphic_command_buffers[s_renderer_frame_index], VK_PIPELINE_BIND_POINT_GRAPHICS, s_renderer_debug_line_pipeline_layout, 0, 1, &s_renderer_debug_line_descriptor_sets[s_renderer_frame_index], 0, 0);
      // vkCmdDrawIndexed(s_renderer_graphic_command_buffers[s_renderer_frame_index], s_renderer_debug_line_index_offset[s_renderer_frame_index], 1, 0, 0, 0);

      s_renderer_debug_line_vertex_offset[s_renderer_frame_index] = 0;
      s_renderer_debug_line_index_offset[s_renderer_frame_index] = 0;
    }
  }

  vkCmdEndRenderPass(s_renderer_graphic_command_buffers[s_renderer_frame_index]);
}

static void renderer_destroy_command_buffer(void) {
  vkFreeCommandBuffers(g_context_device, g_context_command_pool, s_renderer_frames_in_flight, s_renderer_graphic_command_buffers);

  heap_free(s_renderer_graphic_command_buffers);
}
static void renderer_destroy_sync_objects(void) {
  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    vkDestroySemaphore(g_context_device, s_renderer_graphic_complete_semaphores[frame_index], 0);
    vkDestroySemaphore(g_context_device, s_renderer_present_complete_semaphores[frame_index], 0);

    vkDestroyFence(g_context_device, s_renderer_frame_fences[frame_index], 0);

    frame_index++;
  }

  heap_free(s_renderer_graphic_complete_semaphores);
  heap_free(s_renderer_present_complete_semaphores);
  heap_free(s_renderer_frame_fences);
}

static void renderer_destroy_time_buffer(void) {
  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    vkUnmapMemory(g_context_device, s_renderer_time_buffer_device_memory[frame_index]);
    vkFreeMemory(g_context_device, s_renderer_time_buffer_device_memory[frame_index], 0);
    vkDestroyBuffer(g_context_device, s_renderer_time_buffer[frame_index], 0);

    frame_index++;
  }

  heap_free(s_renderer_time_buffer);
  heap_free(s_renderer_time_buffer_device_memory);
  heap_free(s_renderer_time);
}
static void renderer_destroy_screen_buffer(void) {
  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    vkUnmapMemory(g_context_device, s_renderer_screen_buffer_device_memory[frame_index]);
    vkFreeMemory(g_context_device, s_renderer_screen_buffer_device_memory[frame_index], 0);
    vkDestroyBuffer(g_context_device, s_renderer_screen_buffer[frame_index], 0);

    frame_index++;
  }

  heap_free(s_renderer_screen_buffer);
  heap_free(s_renderer_screen_buffer_device_memory);
  heap_free(s_renderer_screen);
}
static void renderer_destroy_camera_buffer(void) {
  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    vkUnmapMemory(g_context_device, s_renderer_camera_buffer_device_memory[frame_index]);
    vkFreeMemory(g_context_device, s_renderer_camera_buffer_device_memory[frame_index], 0);
    vkDestroyBuffer(g_context_device, s_renderer_camera_buffer[frame_index], 0);

    frame_index++;
  }

  heap_free(s_renderer_camera_buffer);
  heap_free(s_renderer_camera_buffer_device_memory);
  heap_free(s_renderer_camera);
}
static void renderer_destroy_debug_line_vertex_buffer(void) {
  uint64_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    vkUnmapMemory(g_context_device, s_renderer_debug_line_vertex_buffer_device_memory[frame_index]);
    vkFreeMemory(g_context_device, s_renderer_debug_line_vertex_buffer_device_memory[frame_index], 0);
    vkDestroyBuffer(g_context_device, s_renderer_debug_line_vertex_buffer[frame_index], 0);

    frame_index++;
  }

  heap_free(s_renderer_debug_line_vertex_buffer);
  heap_free(s_renderer_debug_line_vertex_buffer_device_memory);
  heap_free(s_renderer_debug_line_vertex);
}
static void renderer_destroy_debug_line_index_buffer(void) {
  uint64_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    vkUnmapMemory(g_context_device, s_renderer_debug_line_index_buffer_device_memory[frame_index]);
    vkFreeMemory(g_context_device, s_renderer_debug_line_index_buffer_device_memory[frame_index], 0);
    vkDestroyBuffer(g_context_device, s_renderer_debug_line_index_buffer[frame_index], 0);

    frame_index++;
  }

  heap_free(s_renderer_debug_line_index_buffer);
  heap_free(s_renderer_debug_line_index_buffer_device_memory);
  heap_free(s_renderer_debug_line_index);
}

static void renderer_destroy_pipelines(void) {
  uint64_t pipeline_index = 0;
  uint64_t pipeline_count = vector_count(&s_renderer_pipelines);

  while (pipeline_index < pipeline_count) {
    pipeline_t *pipeline = (pipeline_t *)vector_at(&s_renderer_pipelines, pipeline_index);

    pipeline_destroy(pipeline);

    pipeline_index++;
  }

  vector_destroy(&s_renderer_pipelines);
}

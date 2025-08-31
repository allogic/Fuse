#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <library/core/api.h>

#include <engine/context.h>
#include <engine/macros.h>
#include <engine/swapchain.h>

#include <engine/renderer/config.h>
#include <engine/renderer/forward.h>
#include <engine/renderer/macros.h>
#include <engine/renderer/renderer.h>

// TODO: implement sparse textures..

static void renderer_compute_local_variables(void);

static void renderer_create_command_buffer(void);
static void renderer_create_sync_objects(void);
static void renderer_create_descriptor_pools(void);
static void renderer_create_descriptor_set_layouts(void);
static void renderer_create_descriptor_sets(void);
static void renderer_create_pipeline_layouts(void);

static void renderer_create_chunk_editor_pipeline(void);
static void renderer_create_chunk_generator_pipeline(void);
static void renderer_create_chunk_mipmap_pipeline(void);
static void renderer_create_chunk_renderer_pipeline(void);
static void renderer_create_debug_line_pipeline(void);

static void renderer_create_time_buffer(void);
static void renderer_create_screen_buffer(void);
static void renderer_create_camera_buffer(void);
static void renderer_create_cluster_buffer(void);
static void renderer_create_cluster_vertex_buffer(void);
static void renderer_create_cluster_index_buffer(void);
static void renderer_create_debug_line_vertex_buffer(void);
static void renderer_create_debug_line_index_buffer(void);

static void renderer_create_chunk_images(void);

static void renderer_create_cluster_vertices(void);
static void renderer_create_cluster_indices(void);

static void renderer_update_chunk_editor_descriptor_sets(void);
static void renderer_update_chunk_generator_descriptor_sets(void);
static void renderer_update_chunk_mipmap_descriptor_sets(void);
static void renderer_update_chunk_renderer_descriptor_sets(void);
static void renderer_update_debug_line_descriptor_sets(void);

static void renderer_update_uniform_buffers(transform_t *transform, camera_t *camera);

static void renderer_record_compute_commands(void);
static void renderer_record_graphics_commands(void);

static void renderer_destroy_command_buffer(void);
static void renderer_destroy_sync_objects(void);
static void renderer_destroy_descriptor_pools(void);
static void renderer_destroy_descriptor_set_layouts(void);
static void renderer_destroy_descriptor_sets(void);
static void renderer_destroy_pipeline_layouts(void);
static void renderer_destroy_pipelines(void);

static void renderer_destroy_time_buffer(void);
static void renderer_destroy_screen_buffer(void);
static void renderer_destroy_camera_buffer(void);
static void renderer_destroy_cluster_buffer(void);
static void renderer_destroy_cluster_vertex_buffer(void);
static void renderer_destroy_cluster_index_buffer(void);
static void renderer_destroy_debug_line_vertex_buffer(void);
static void renderer_destroy_debug_line_index_buffer(void);

static void renderer_destroy_chunk_images(void);

static LRESULT WINAPI renderer_cluster_worker_thread(PVOID user_param);

uint8_t g_renderer_is_dirty = 0;

uint8_t g_renderer_enable_debug = 0;

int32_t g_renderer_frames_in_flight = 0;

int32_t g_renderer_chunk_count_x = RENDERER_CHUNK_COUNT_X;
int32_t g_renderer_chunk_count_y = RENDERER_CHUNK_COUNT_Y;
int32_t g_renderer_chunk_count_z = RENDERER_CHUNK_COUNT_Z;

int32_t g_renderer_chunk_size = RENDERER_CHUNK_SIZE;

static VkVertexInputBindingDescription const s_renderer_chunk_renderer_vertex_input_binding_descriptions[] = {
  {0, sizeof(renderer_cluster_vertex_t), VK_VERTEX_INPUT_RATE_VERTEX},
};
static VkVertexInputBindingDescription const s_renderer_debug_line_vertex_input_binding_descriptions[] = {
  {0, sizeof(renderer_debug_line_vertex_t), VK_VERTEX_INPUT_RATE_VERTEX},
};

static VkVertexInputAttributeDescription const s_renderer_chunk_renderer_vertex_input_attribute_descriptions[] = {
  {0, 0, VK_FORMAT_R32G32B32_SINT, 0},
};
static VkVertexInputAttributeDescription const s_renderer_debug_line_vertex_input_attribute_descriptions[] = {
  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0},
  {1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, OFFSET_OF(renderer_debug_line_vertex_t, color)},
};

static VkPushConstantRange const s_renderer_chunk_editor_push_constant_ranges[] = {
  {VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(renderer_chunk_editor_push_constant_t)},
};
static VkPushConstantRange const s_renderer_chunk_generator_push_constant_ranges[] = {
  {VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(renderer_chunk_generator_push_constant_t)},
};
static VkPushConstantRange const s_renderer_chunk_mipmap_push_constant_ranges[] = {
  {VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(renderer_chunk_mipmap_push_constant_t)},
};

static char const *s_renderer_chunk_editor_shader_file_path = ROOT_DIR "/shader/chunk/editor.comp.spv";
static char const *s_renderer_chunk_generator_shader_file_path = ROOT_DIR "/shader/chunk/generator.comp.spv";
static char const *s_renderer_chunk_mipmap_shader_file_path = ROOT_DIR "/shader/chunk/mipmap.comp.spv";
static char const *s_renderer_chunk_renderer_vertex_shader_file_path = ROOT_DIR "/shader/chunk/renderer.vert.spv";
static char const *s_renderer_chunk_renderer_fragment_shader_file_path = ROOT_DIR "/shader/chunk/renderer.frag.spv";
static char const *s_renderer_debug_line_vertex_shader_file_path = ROOT_DIR "/shader/debug/line.vert.spv";
static char const *s_renderer_debug_line_fragment_shader_file_path = ROOT_DIR "/shader/debug/line.frag.spv";

static VkCommandBuffer *s_renderer_graphics_command_buffers = 0;

static VkSemaphore *s_renderer_graphics_complete_semaphores = 0;
static VkSemaphore *s_renderer_present_complete_semaphores = 0;

static VkFence *s_renderer_frame_fences = 0;

static int32_t s_renderer_frames_in_flight = 0;

static int32_t *s_renderer_cluster_is_dirty = 0;

static int32_t s_renderer_chunk_count_x = 0;
static int32_t s_renderer_chunk_count_y = 0;
static int32_t s_renderer_chunk_count_z = 0;
static int32_t s_renderer_chunk_count = 0;

static int32_t s_renderer_frame_index = 0;
static int32_t s_renderer_image_index = 0;

static int32_t s_renderer_chunk_lod_levels = 0;

static VkDescriptorPool s_renderer_chunk_editor_descriptor_pool = 0;
static VkDescriptorPool s_renderer_chunk_generator_descriptor_pool = 0;
static VkDescriptorPool s_renderer_chunk_mipmap_descriptor_pool = 0;
static VkDescriptorPool s_renderer_chunk_renderer_descriptor_pool = 0;
static VkDescriptorPool s_renderer_debug_line_descriptor_pool = 0;

static VkDescriptorSetLayout s_renderer_chunk_editor_descriptor_set_layout = 0;
static VkDescriptorSetLayout s_renderer_chunk_generator_descriptor_set_layout = 0;
static VkDescriptorSetLayout s_renderer_chunk_mipmap_descriptor_set_layout = 0;
static VkDescriptorSetLayout s_renderer_chunk_renderer_descriptor_set_layout = 0;
static VkDescriptorSetLayout s_renderer_debug_line_descriptor_set_layout = 0;

static VkPipelineLayout s_renderer_chunk_editor_pipeline_layout = 0;
static VkPipelineLayout s_renderer_chunk_generator_pipeline_layout = 0;
static VkPipelineLayout s_renderer_chunk_mipmap_pipeline_layout = 0;
static VkPipelineLayout s_renderer_chunk_renderer_pipeline_layout = 0;
static VkPipelineLayout s_renderer_debug_line_pipeline_layout = 0;

static VkPipeline s_renderer_chunk_editor_pipeline = 0;
static VkPipeline s_renderer_chunk_generator_pipeline = 0;
static VkPipeline s_renderer_chunk_mipmap_pipeline = 0;
static VkPipeline s_renderer_chunk_renderer_pipeline = 0;
static VkPipeline s_renderer_debug_line_pipeline = 0;

static VkDescriptorSet *s_renderer_chunk_editor_descriptor_sets = 0;
static VkDescriptorSet *s_renderer_chunk_generator_descriptor_sets = 0;
static VkDescriptorSet *s_renderer_chunk_mipmap_descriptor_sets = 0;
static VkDescriptorSet *s_renderer_chunk_renderer_descriptor_sets = 0;
static VkDescriptorSet *s_renderer_debug_line_descriptor_sets = 0;

static VkBuffer *s_renderer_time_buffer = 0;
static VkBuffer *s_renderer_screen_buffer = 0;
static VkBuffer *s_renderer_camera_buffer = 0;
static VkBuffer *s_renderer_cluster_buffer = 0;
static VkBuffer *s_renderer_cluster_vertex_buffer = 0;
static VkBuffer *s_renderer_cluster_index_buffer = 0;
static VkBuffer *s_renderer_debug_line_vertex_buffer = 0;
static VkBuffer *s_renderer_debug_line_index_buffer = 0;

static VkDeviceMemory *s_renderer_time_buffer_device_memory = 0;
static VkDeviceMemory *s_renderer_screen_buffer_device_memory = 0;
static VkDeviceMemory *s_renderer_camera_buffer_device_memory = 0;
static VkDeviceMemory *s_renderer_cluster_buffer_device_memory = 0;
static VkDeviceMemory *s_renderer_cluster_vertex_buffer_device_memory = 0;
static VkDeviceMemory *s_renderer_cluster_index_buffer_device_memory = 0;
static VkDeviceMemory *s_renderer_debug_line_vertex_buffer_device_memory = 0;
static VkDeviceMemory *s_renderer_debug_line_index_buffer_device_memory = 0;
static VkDeviceMemory **s_renderer_chunk_image_device_memory = 0;

static renderer_time_t **s_renderer_time = 0;
static renderer_screen_t **s_renderer_screen = 0;
static renderer_camera_t **s_renderer_camera = 0;
static renderer_cluster_t **s_renderer_cluster = 0;
static renderer_cluster_vertex_t **s_renderer_cluster_vertex = 0;
static renderer_cluster_index_t **s_renderer_cluster_index = 0;
static renderer_debug_line_vertex_t **s_renderer_debug_line_vertex = 0;
static renderer_debug_line_index_t **s_renderer_debug_line_index = 0;

static VkImage **s_renderer_chunk_image = 0;

static VkImageView ***s_renderer_chunk_image_view = 0;

static VkSampler ***s_renderer_chunk_sampler = 0;

static int32_t *s_renderer_debug_line_vertex_offset = 0;
static int32_t *s_renderer_debug_line_index_offset = 0;

static HANDLE s_renderer_cluster_worker_mutex = 0;
static HANDLE s_renderer_cluster_worker_thread = 0;

static uint8_t s_renderer_cluster_worker_should_stop = 0;

void renderer_create(void) {
  renderer_compute_local_variables();

  s_renderer_cluster_is_dirty = (int32_t *)heap_alloc(sizeof(int32_t) * g_renderer_frames_in_flight);

  memset(s_renderer_cluster_is_dirty, 1, sizeof(int32_t) * g_renderer_frames_in_flight);

  renderer_create_command_buffer();
  renderer_create_sync_objects();

  renderer_create_descriptor_pools();
  renderer_create_descriptor_set_layouts();
  renderer_create_descriptor_sets();

  // renderer_create_pipeline_layouts();

  // renderer_create_chunk_editor_pipeline();
  // renderer_create_chunk_generator_pipeline();
  // renderer_create_chunk_mipmap_pipeline();
  // renderer_create_chunk_renderer_pipeline();
  // renderer_create_debug_line_pipeline();

#include "engine/renderer/pipelines_layouts.h"
#include "engine/renderer/pipelines.h"

  renderer_create_time_buffer();
  renderer_create_screen_buffer();
  renderer_create_camera_buffer();
  renderer_create_cluster_buffer();
  renderer_create_cluster_vertex_buffer();
  renderer_create_cluster_index_buffer();
  renderer_create_debug_line_vertex_buffer();
  renderer_create_debug_line_index_buffer();

  renderer_create_chunk_images();

  renderer_create_cluster_vertices();
  renderer_create_cluster_indices();

  renderer_update_chunk_editor_descriptor_sets();
  renderer_update_chunk_generator_descriptor_sets();
  renderer_update_chunk_mipmap_descriptor_sets();
  renderer_update_chunk_renderer_descriptor_sets();
  renderer_update_debug_line_descriptor_sets();

  s_renderer_debug_line_vertex_offset = (int32_t *)heap_alloc(sizeof(int32_t) * s_renderer_frames_in_flight);
  s_renderer_debug_line_index_offset = (int32_t *)heap_alloc(sizeof(int32_t) * s_renderer_frames_in_flight);

  s_renderer_cluster_worker_should_stop = 0;

  // s_renderer_cluster_worker_mutex = CreateMutexA(0, 0, 0);
  // s_renderer_cluster_worker_thread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)renderer_cluster_chunk_worker_thread, 0, 0, NULL); // TODO
}
void renderer_update(void) {
  if (g_renderer_enable_debug) {
    vector3_t cluster_position = {0.0F, 0.0F, 0.0F};
    vector3_t cluster_size = {(float)s_renderer_chunk_count_x * (float)g_renderer_chunk_size, (float)s_renderer_chunk_count_y * (float)g_renderer_chunk_size, (float)s_renderer_chunk_count_z * (float)g_renderer_chunk_size};
    vector4_t cluster_color = {0.2F, 0.5F, 0.8F, 1.0F};

    renderer_draw_debug_box(cluster_position, cluster_size, cluster_color);

    vector3_t chunk_position = {0.0F, 0.0F, 0.0F};
    vector3_t chunk_size = {(float)g_renderer_chunk_size, (float)g_renderer_chunk_size, (float)g_renderer_chunk_size};
    vector4_t chunk_color = {0.2F, 0.5F, 0.8F, 1.0F};

    renderer_draw_debug_box(chunk_position, chunk_size, chunk_color);

    vector3_t voxel_position = {0.0F, 0.0F, 0.0F};
    vector3_t voxel_size = {1.0F, 1.0F, 1.0F};
    vector4_t voxel_color = {1.0F, 0.0F, 0.0F, 1.0F};

    renderer_draw_debug_box(voxel_position, voxel_size, voxel_color);
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

  VkResult reset_graphics_command_buffer_result = vkResetCommandBuffer(s_renderer_graphics_command_buffers[s_renderer_frame_index], 0);

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

  VkResult begin_graphics_command_buffer_result = vkBeginCommandBuffer(s_renderer_graphics_command_buffers[s_renderer_frame_index], &command_buffer_begin_info);

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

  VkResult end_graphics_command_buffer_result = vkEndCommandBuffer(s_renderer_graphics_command_buffers[s_renderer_frame_index]);

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
  graphics_submit_info.pSignalSemaphores = &s_renderer_graphics_complete_semaphores[s_renderer_frame_index];
  graphics_submit_info.signalSemaphoreCount = 1;
  graphics_submit_info.pCommandBuffers = &s_renderer_graphics_command_buffers[s_renderer_frame_index];
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
  present_info.pWaitSemaphores = &s_renderer_graphics_complete_semaphores[s_renderer_frame_index];
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
  s_renderer_cluster_worker_should_stop = 1;

  // WaitForSingleObject(s_renderer_cluster_worker_thread, INFINITE);

  // CloseHandle(s_renderer_cluster_worker_thread);
  // CloseHandle(s_renderer_cluster_worker_mutex);

  VULKAN_CHECK(vkQueueWaitIdle(g_context_graphics_queue));
  VULKAN_CHECK(vkQueueWaitIdle(g_context_present_queue));

  heap_free(s_renderer_debug_line_vertex_offset);
  heap_free(s_renderer_debug_line_index_offset);

  renderer_destroy_chunk_images();

  renderer_destroy_debug_line_index_buffer();
  renderer_destroy_debug_line_vertex_buffer();
  renderer_destroy_cluster_index_buffer();
  renderer_destroy_cluster_vertex_buffer();
  renderer_destroy_cluster_buffer();
  renderer_destroy_camera_buffer();
  renderer_destroy_screen_buffer();
  renderer_destroy_time_buffer();

  renderer_destroy_pipelines();
  renderer_destroy_pipeline_layouts();
  renderer_destroy_descriptor_sets();
  renderer_destroy_descriptor_set_layouts();
  renderer_destroy_descriptor_pools();
  renderer_destroy_sync_objects();
  renderer_destroy_command_buffer();

  heap_free(s_renderer_cluster_is_dirty);
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

  s_renderer_chunk_count_x = g_renderer_chunk_count_x;
  s_renderer_chunk_count_y = g_renderer_chunk_count_y;
  s_renderer_chunk_count_z = g_renderer_chunk_count_z;
  s_renderer_chunk_count = g_renderer_chunk_count_x * g_renderer_chunk_count_y * g_renderer_chunk_count_z;

  s_renderer_frame_index = 0;
  s_renderer_image_index = 0;

  s_renderer_chunk_lod_levels = 6;
}

static void renderer_create_command_buffer(void) {
  s_renderer_graphics_command_buffers = (VkCommandBuffer *)heap_alloc(sizeof(VkCommandBuffer) * s_renderer_frames_in_flight);

  VkCommandBufferAllocateInfo command_buffer_alloc_create_info = {0};
  command_buffer_alloc_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_alloc_create_info.commandPool = g_context_command_pool;
  command_buffer_alloc_create_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_alloc_create_info.commandBufferCount = s_renderer_frames_in_flight;

  VULKAN_CHECK(vkAllocateCommandBuffers(g_context_device, &command_buffer_alloc_create_info, s_renderer_graphics_command_buffers));
}
static void renderer_create_sync_objects(void) {
  s_renderer_graphics_complete_semaphores = (VkSemaphore *)heap_alloc(sizeof(VkSemaphore) * s_renderer_frames_in_flight);
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
    VULKAN_CHECK(vkCreateSemaphore(g_context_device, &semaphore_create_info, 0, &s_renderer_graphics_complete_semaphores[frame_index]));
    VULKAN_CHECK(vkCreateSemaphore(g_context_device, &semaphore_create_info, 0, &s_renderer_present_complete_semaphores[frame_index]));

    VULKAN_CHECK(vkCreateFence(g_context_device, &fence_create_info, 0, &s_renderer_frame_fences[frame_index]));

    frame_index++;
  }
}
static void renderer_create_descriptor_pools(void) {
  VkDescriptorPoolSize chunk_editor_descriptor_pool_sizes[] =
    {
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1},
      {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, s_renderer_chunk_count},
    };

  VkDescriptorPoolCreateInfo chunk_editor_descriptor_pool_create_info = {0};
  chunk_editor_descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  chunk_editor_descriptor_pool_create_info.pPoolSizes = chunk_editor_descriptor_pool_sizes;
  chunk_editor_descriptor_pool_create_info.poolSizeCount = ARRAY_COUNT(chunk_editor_descriptor_pool_sizes);
  chunk_editor_descriptor_pool_create_info.maxSets = s_renderer_frames_in_flight;

  VULKAN_CHECK(vkCreateDescriptorPool(g_context_device, &chunk_editor_descriptor_pool_create_info, 0, &s_renderer_chunk_editor_descriptor_pool));

  VkDescriptorPoolSize chunk_generator_descriptor_pool_sizes[] =
    {
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1},
      {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, s_renderer_chunk_count},
    };

  VkDescriptorPoolCreateInfo chunk_generator_descriptor_pool_create_info = {0};
  chunk_generator_descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  chunk_generator_descriptor_pool_create_info.pPoolSizes = chunk_generator_descriptor_pool_sizes;
  chunk_generator_descriptor_pool_create_info.poolSizeCount = ARRAY_COUNT(chunk_generator_descriptor_pool_sizes);
  chunk_generator_descriptor_pool_create_info.maxSets = s_renderer_frames_in_flight;

  VULKAN_CHECK(vkCreateDescriptorPool(g_context_device, &chunk_generator_descriptor_pool_create_info, 0, &s_renderer_chunk_generator_descriptor_pool));

  VkDescriptorPoolSize chunk_mipmap_descriptor_pool_sizes[] =
    {
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1},
      {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, s_renderer_chunk_count * s_renderer_chunk_lod_levels},
    };

  VkDescriptorPoolCreateInfo chunk_mipmap_descriptor_pool_create_info = {0};
  chunk_mipmap_descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  chunk_mipmap_descriptor_pool_create_info.pPoolSizes = chunk_mipmap_descriptor_pool_sizes;
  chunk_mipmap_descriptor_pool_create_info.poolSizeCount = ARRAY_COUNT(chunk_mipmap_descriptor_pool_sizes);
  chunk_mipmap_descriptor_pool_create_info.maxSets = s_renderer_frames_in_flight;

  VULKAN_CHECK(vkCreateDescriptorPool(g_context_device, &chunk_mipmap_descriptor_pool_create_info, 0, &s_renderer_chunk_mipmap_descriptor_pool));

  VkDescriptorPoolSize chunk_renderer_descriptor_pool_sizes[] =
    {
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4},
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, s_renderer_chunk_count * s_renderer_chunk_lod_levels},
    };

  VkDescriptorPoolCreateInfo chunk_renderer_descriptor_pool_create_info = {0};
  chunk_renderer_descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  chunk_renderer_descriptor_pool_create_info.pPoolSizes = chunk_renderer_descriptor_pool_sizes;
  chunk_renderer_descriptor_pool_create_info.poolSizeCount = ARRAY_COUNT(chunk_renderer_descriptor_pool_sizes);
  chunk_renderer_descriptor_pool_create_info.maxSets = s_renderer_frames_in_flight;

  VULKAN_CHECK(vkCreateDescriptorPool(g_context_device, &chunk_renderer_descriptor_pool_create_info, 0, &s_renderer_chunk_renderer_descriptor_pool));

  VkDescriptorPoolSize debug_line_descriptor_pool_sizes[] =
    {
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3},
    };

  VkDescriptorPoolCreateInfo debug_line_descriptor_pool_create_info = {0};
  debug_line_descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  debug_line_descriptor_pool_create_info.pPoolSizes = debug_line_descriptor_pool_sizes;
  debug_line_descriptor_pool_create_info.poolSizeCount = ARRAY_COUNT(debug_line_descriptor_pool_sizes);
  debug_line_descriptor_pool_create_info.maxSets = s_renderer_frames_in_flight;

  VULKAN_CHECK(vkCreateDescriptorPool(g_context_device, &debug_line_descriptor_pool_create_info, 0, &s_renderer_debug_line_descriptor_pool));
}
static void renderer_create_descriptor_set_layouts(void) {
  VkDescriptorSetLayoutBinding chunk_editor_descriptor_set_layout_bindings[] =
    {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0},
      {1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, s_renderer_chunk_count * s_renderer_chunk_lod_levels, VK_SHADER_STAGE_COMPUTE_BIT, 0},
    };

  VkDescriptorSetLayoutCreateInfo chunk_editor_descriptor_set_layout_create_info = {0};
  chunk_editor_descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  chunk_editor_descriptor_set_layout_create_info.pBindings = chunk_editor_descriptor_set_layout_bindings;
  chunk_editor_descriptor_set_layout_create_info.bindingCount = ARRAY_COUNT(chunk_editor_descriptor_set_layout_bindings);
  chunk_editor_descriptor_set_layout_create_info.pNext = 0;

  VULKAN_CHECK(vkCreateDescriptorSetLayout(g_context_device, &chunk_editor_descriptor_set_layout_create_info, 0, &s_renderer_chunk_editor_descriptor_set_layout));

  VkDescriptorSetLayoutBinding chunk_generator_descriptor_set_layout_bindings[] =
    {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0},
      {1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, s_renderer_chunk_count * s_renderer_chunk_lod_levels, VK_SHADER_STAGE_COMPUTE_BIT, 0},
    };

  VkDescriptorSetLayoutCreateInfo chunk_generator_descriptor_set_layout_create_info = {0};
  chunk_generator_descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  chunk_generator_descriptor_set_layout_create_info.pBindings = chunk_generator_descriptor_set_layout_bindings;
  chunk_generator_descriptor_set_layout_create_info.bindingCount = ARRAY_COUNT(chunk_generator_descriptor_set_layout_bindings);
  chunk_generator_descriptor_set_layout_create_info.pNext = 0;

  VULKAN_CHECK(vkCreateDescriptorSetLayout(g_context_device, &chunk_generator_descriptor_set_layout_create_info, 0, &s_renderer_chunk_generator_descriptor_set_layout));

  VkDescriptorSetLayoutBinding chunk_mipmap_descriptor_set_layout_bindings[] =
    {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0},
      {1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, s_renderer_chunk_count * s_renderer_chunk_lod_levels, VK_SHADER_STAGE_COMPUTE_BIT, 0},
    };

  VkDescriptorSetLayoutCreateInfo chunk_mipmap_descriptor_set_layout_create_info = {0};
  chunk_mipmap_descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  chunk_mipmap_descriptor_set_layout_create_info.pBindings = chunk_mipmap_descriptor_set_layout_bindings;
  chunk_mipmap_descriptor_set_layout_create_info.bindingCount = ARRAY_COUNT(chunk_mipmap_descriptor_set_layout_bindings);
  chunk_mipmap_descriptor_set_layout_create_info.pNext = 0;

  VULKAN_CHECK(vkCreateDescriptorSetLayout(g_context_device, &chunk_mipmap_descriptor_set_layout_create_info, 0, &s_renderer_chunk_mipmap_descriptor_set_layout));

  VkDescriptorSetLayoutBinding chunk_renderer_descriptor_set_layout_bindings[] =
    {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, 0},
      {1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, 0},
      {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0},
      {3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, 0},
      {4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, s_renderer_chunk_count * s_renderer_chunk_lod_levels, VK_SHADER_STAGE_FRAGMENT_BIT, 0},
    };

  VkDescriptorSetLayoutCreateInfo chunk_renderer_descriptor_set_layout_create_info = {0};
  chunk_renderer_descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  chunk_renderer_descriptor_set_layout_create_info.pBindings = chunk_renderer_descriptor_set_layout_bindings;
  chunk_renderer_descriptor_set_layout_create_info.bindingCount = ARRAY_COUNT(chunk_renderer_descriptor_set_layout_bindings);
  chunk_renderer_descriptor_set_layout_create_info.pNext = 0;

  VULKAN_CHECK(vkCreateDescriptorSetLayout(g_context_device, &chunk_renderer_descriptor_set_layout_create_info, 0, &s_renderer_chunk_renderer_descriptor_set_layout));

  VkDescriptorSetLayoutBinding debug_line_descriptor_set_layout_bindings[] =
    {
      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0},
      {1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0},
      {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0},
    };

  VkDescriptorSetLayoutCreateInfo debug_line_descriptor_set_layout_create_info = {0};
  debug_line_descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  debug_line_descriptor_set_layout_create_info.pBindings = debug_line_descriptor_set_layout_bindings;
  debug_line_descriptor_set_layout_create_info.bindingCount = ARRAY_COUNT(debug_line_descriptor_set_layout_bindings);
  debug_line_descriptor_set_layout_create_info.pNext = 0;

  VULKAN_CHECK(vkCreateDescriptorSetLayout(g_context_device, &debug_line_descriptor_set_layout_create_info, 0, &s_renderer_debug_line_descriptor_set_layout));
}
static void renderer_create_descriptor_sets(void) {
  int32_t chunk_editor_descriptor_set_count = s_renderer_frames_in_flight;
  int32_t chunk_generator_descriptor_set_count = s_renderer_frames_in_flight;
  int32_t chunk_mipmap_descriptor_set_count = s_renderer_frames_in_flight;
  int32_t chunk_renderer_descriptor_set_count = s_renderer_frames_in_flight;
  int32_t debug_line_descriptor_set_count = s_renderer_frames_in_flight;

  s_renderer_chunk_editor_descriptor_sets = (VkDescriptorSet *)heap_alloc(sizeof(VkDescriptorSet) * chunk_editor_descriptor_set_count);
  s_renderer_chunk_generator_descriptor_sets = (VkDescriptorSet *)heap_alloc(sizeof(VkDescriptorSet) * chunk_generator_descriptor_set_count);
  s_renderer_chunk_mipmap_descriptor_sets = (VkDescriptorSet *)heap_alloc(sizeof(VkDescriptorSet) * chunk_mipmap_descriptor_set_count);
  s_renderer_chunk_renderer_descriptor_sets = (VkDescriptorSet *)heap_alloc(sizeof(VkDescriptorSet) * chunk_renderer_descriptor_set_count);
  s_renderer_debug_line_descriptor_sets = (VkDescriptorSet *)heap_alloc(sizeof(VkDescriptorSet) * debug_line_descriptor_set_count);

  VkDescriptorSetLayout *chunk_editor_descriptor_set_layouts = (VkDescriptorSetLayout *)heap_alloc(sizeof(VkDescriptorSetLayout) * chunk_editor_descriptor_set_count);
  VkDescriptorSetLayout *chunk_generator_descriptor_set_layouts = (VkDescriptorSetLayout *)heap_alloc(sizeof(VkDescriptorSetLayout) * chunk_generator_descriptor_set_count);
  VkDescriptorSetLayout *chunk_mipmap_descriptor_set_layouts = (VkDescriptorSetLayout *)heap_alloc(sizeof(VkDescriptorSetLayout) * chunk_mipmap_descriptor_set_count);
  VkDescriptorSetLayout *chunk_renderer_descriptor_set_layouts = (VkDescriptorSetLayout *)heap_alloc(sizeof(VkDescriptorSetLayout) * chunk_renderer_descriptor_set_count);
  VkDescriptorSetLayout *debug_line_descriptor_set_layouts = (VkDescriptorSetLayout *)heap_alloc(sizeof(VkDescriptorSetLayout) * debug_line_descriptor_set_count);

  int32_t chunk_editor_descriptor_set_index = 0;
  while (chunk_editor_descriptor_set_index < chunk_editor_descriptor_set_count) {
    chunk_editor_descriptor_set_layouts[chunk_editor_descriptor_set_index] = s_renderer_chunk_editor_descriptor_set_layout;

    chunk_editor_descriptor_set_index++;
  }

  int32_t chunk_generator_descriptor_set_index = 0;
  while (chunk_generator_descriptor_set_index < chunk_generator_descriptor_set_count) {
    chunk_generator_descriptor_set_layouts[chunk_generator_descriptor_set_index] = s_renderer_chunk_generator_descriptor_set_layout;

    chunk_generator_descriptor_set_index++;
  }

  int32_t chunk_mipmap_descriptor_set_index = 0;
  while (chunk_mipmap_descriptor_set_index < chunk_mipmap_descriptor_set_count) {
    chunk_mipmap_descriptor_set_layouts[chunk_mipmap_descriptor_set_index] = s_renderer_chunk_mipmap_descriptor_set_layout;

    chunk_mipmap_descriptor_set_index++;
  }

  int32_t chunk_renderer_descriptor_set_index = 0;
  while (chunk_renderer_descriptor_set_index < chunk_renderer_descriptor_set_count) {
    chunk_renderer_descriptor_set_layouts[chunk_renderer_descriptor_set_index] = s_renderer_chunk_renderer_descriptor_set_layout;

    chunk_renderer_descriptor_set_index++;
  }

  int32_t debug_line_descriptor_set_index = 0;
  while (debug_line_descriptor_set_index < debug_line_descriptor_set_count) {
    debug_line_descriptor_set_layouts[debug_line_descriptor_set_index] = s_renderer_debug_line_descriptor_set_layout;

    debug_line_descriptor_set_index++;
  }

  VkDescriptorSetAllocateInfo chunk_editor_descriptor_set_allocate_info = {0};
  chunk_editor_descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  chunk_editor_descriptor_set_allocate_info.descriptorSetCount = s_renderer_frames_in_flight;
  chunk_editor_descriptor_set_allocate_info.descriptorPool = s_renderer_chunk_editor_descriptor_pool;
  chunk_editor_descriptor_set_allocate_info.pSetLayouts = chunk_editor_descriptor_set_layouts;

  VULKAN_CHECK(vkAllocateDescriptorSets(g_context_device, &chunk_editor_descriptor_set_allocate_info, s_renderer_chunk_editor_descriptor_sets));

  VkDescriptorSetAllocateInfo chunk_generator_descriptor_set_allocate_info = {0};
  chunk_generator_descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  chunk_generator_descriptor_set_allocate_info.descriptorSetCount = s_renderer_frames_in_flight;
  chunk_generator_descriptor_set_allocate_info.descriptorPool = s_renderer_chunk_generator_descriptor_pool;
  chunk_generator_descriptor_set_allocate_info.pSetLayouts = chunk_generator_descriptor_set_layouts;

  VULKAN_CHECK(vkAllocateDescriptorSets(g_context_device, &chunk_generator_descriptor_set_allocate_info, s_renderer_chunk_generator_descriptor_sets));

  VkDescriptorSetAllocateInfo chunk_mipmap_descriptor_set_allocate_info = {0};
  chunk_mipmap_descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  chunk_mipmap_descriptor_set_allocate_info.descriptorSetCount = s_renderer_frames_in_flight;
  chunk_mipmap_descriptor_set_allocate_info.descriptorPool = s_renderer_chunk_mipmap_descriptor_pool;
  chunk_mipmap_descriptor_set_allocate_info.pSetLayouts = chunk_mipmap_descriptor_set_layouts;

  VULKAN_CHECK(vkAllocateDescriptorSets(g_context_device, &chunk_mipmap_descriptor_set_allocate_info, s_renderer_chunk_mipmap_descriptor_sets));

  VkDescriptorSetAllocateInfo chunk_renderer_descriptor_set_allocate_info = {0};
  chunk_renderer_descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  chunk_renderer_descriptor_set_allocate_info.descriptorSetCount = s_renderer_frames_in_flight;
  chunk_renderer_descriptor_set_allocate_info.descriptorPool = s_renderer_chunk_renderer_descriptor_pool;
  chunk_renderer_descriptor_set_allocate_info.pSetLayouts = chunk_renderer_descriptor_set_layouts;

  VULKAN_CHECK(vkAllocateDescriptorSets(g_context_device, &chunk_renderer_descriptor_set_allocate_info, s_renderer_chunk_renderer_descriptor_sets));

  VkDescriptorSetAllocateInfo debug_line_descriptor_set_allocate_info = {0};
  debug_line_descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  debug_line_descriptor_set_allocate_info.descriptorSetCount = s_renderer_frames_in_flight;
  debug_line_descriptor_set_allocate_info.descriptorPool = s_renderer_debug_line_descriptor_pool;
  debug_line_descriptor_set_allocate_info.pSetLayouts = debug_line_descriptor_set_layouts;

  VULKAN_CHECK(vkAllocateDescriptorSets(g_context_device, &debug_line_descriptor_set_allocate_info, s_renderer_debug_line_descriptor_sets));

  heap_free(chunk_editor_descriptor_set_layouts);
  heap_free(chunk_generator_descriptor_set_layouts);
  heap_free(chunk_mipmap_descriptor_set_layouts);
  heap_free(chunk_renderer_descriptor_set_layouts);
  heap_free(debug_line_descriptor_set_layouts);
}
static void renderer_create_pipeline_layouts(void) {
  VkPipelineLayoutCreateInfo chunk_editor_pipeline_layout_create_info = {0};
  chunk_editor_pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  chunk_editor_pipeline_layout_create_info.setLayoutCount = 1;
  chunk_editor_pipeline_layout_create_info.pSetLayouts = &s_renderer_chunk_editor_descriptor_set_layout;
  chunk_editor_pipeline_layout_create_info.pPushConstantRanges = s_renderer_chunk_editor_push_constant_ranges;
  chunk_editor_pipeline_layout_create_info.pushConstantRangeCount = ARRAY_COUNT(s_renderer_chunk_editor_push_constant_ranges);

  VULKAN_CHECK(vkCreatePipelineLayout(g_context_device, &chunk_editor_pipeline_layout_create_info, 0, &s_renderer_chunk_editor_pipeline_layout));

  VkPipelineLayoutCreateInfo chunk_generator_pipeline_layout_create_info = {0};
  chunk_generator_pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  chunk_generator_pipeline_layout_create_info.setLayoutCount = 1;
  chunk_generator_pipeline_layout_create_info.pSetLayouts = &s_renderer_chunk_generator_descriptor_set_layout;
  chunk_generator_pipeline_layout_create_info.pPushConstantRanges = s_renderer_chunk_generator_push_constant_ranges;
  chunk_generator_pipeline_layout_create_info.pushConstantRangeCount = ARRAY_COUNT(s_renderer_chunk_generator_push_constant_ranges);

  VULKAN_CHECK(vkCreatePipelineLayout(g_context_device, &chunk_generator_pipeline_layout_create_info, 0, &s_renderer_chunk_generator_pipeline_layout));

  VkPipelineLayoutCreateInfo chunk_mipmap_pipeline_layout_create_info = {0};
  chunk_mipmap_pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  chunk_mipmap_pipeline_layout_create_info.setLayoutCount = 1;
  chunk_mipmap_pipeline_layout_create_info.pSetLayouts = &s_renderer_chunk_mipmap_descriptor_set_layout;
  chunk_mipmap_pipeline_layout_create_info.pPushConstantRanges = s_renderer_chunk_mipmap_push_constant_ranges;
  chunk_mipmap_pipeline_layout_create_info.pushConstantRangeCount = ARRAY_COUNT(s_renderer_chunk_mipmap_push_constant_ranges);

  VULKAN_CHECK(vkCreatePipelineLayout(g_context_device, &chunk_mipmap_pipeline_layout_create_info, 0, &s_renderer_chunk_mipmap_pipeline_layout));

  VkPipelineLayoutCreateInfo chunk_renderer_pipeline_layout_create_info = {0};
  chunk_renderer_pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  chunk_renderer_pipeline_layout_create_info.setLayoutCount = 1;
  chunk_renderer_pipeline_layout_create_info.pSetLayouts = &s_renderer_chunk_renderer_descriptor_set_layout;
  chunk_renderer_pipeline_layout_create_info.pPushConstantRanges = 0;
  chunk_renderer_pipeline_layout_create_info.pushConstantRangeCount = 0;

  VULKAN_CHECK(vkCreatePipelineLayout(g_context_device, &chunk_renderer_pipeline_layout_create_info, 0, &s_renderer_chunk_renderer_pipeline_layout));

  VkPipelineLayoutCreateInfo debug_line_pipeline_layout_create_info = {0};
  debug_line_pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  debug_line_pipeline_layout_create_info.setLayoutCount = 1;
  debug_line_pipeline_layout_create_info.pSetLayouts = &s_renderer_debug_line_descriptor_set_layout;
  debug_line_pipeline_layout_create_info.pPushConstantRanges = 0;
  debug_line_pipeline_layout_create_info.pushConstantRangeCount = 0;

  VULKAN_CHECK(vkCreatePipelineLayout(g_context_device, &debug_line_pipeline_layout_create_info, 0, &s_renderer_debug_line_pipeline_layout));
}

static void renderer_create_chunk_editor_pipeline(void) {
  uint8_t *compute_shader_bytes = 0;

  uint64_t compute_shader_size = 0;

  filesystem_read_binary(&compute_shader_bytes, &compute_shader_size, s_renderer_chunk_editor_shader_file_path);

  VkShaderModule compute_module = 0;

  {
    VkShaderModuleCreateInfo shader_module_create_info = {0};
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.codeSize = compute_shader_size;
    shader_module_create_info.pCode = (uint32_t const *)compute_shader_bytes;

    VULKAN_CHECK(vkCreateShaderModule(g_context_device, &shader_module_create_info, 0, &compute_module));
  }

  VkPipelineShaderStageCreateInfo compute_shader_stage_create_info = {0};
  compute_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  compute_shader_stage_create_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
  compute_shader_stage_create_info.module = compute_module;
  compute_shader_stage_create_info.pName = "main";

  VkComputePipelineCreateInfo compute_pipeline_create_info = {0};
  compute_pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  compute_pipeline_create_info.layout = s_renderer_chunk_editor_pipeline_layout;
  compute_pipeline_create_info.stage = compute_shader_stage_create_info;

  VULKAN_CHECK(vkCreateComputePipelines(g_context_device, 0, 1, &compute_pipeline_create_info, 0, &s_renderer_chunk_editor_pipeline));

  vkDestroyShaderModule(g_context_device, compute_module, 0);

  heap_free(compute_shader_bytes);
}
static void renderer_create_chunk_generator_pipeline(void) {
  uint8_t *compute_shader_bytes = 0;

  uint64_t compute_shader_size = 0;

  filesystem_read_binary(&compute_shader_bytes, &compute_shader_size, s_renderer_chunk_generator_shader_file_path);

  VkShaderModule compute_module = 0;

  {
    VkShaderModuleCreateInfo shader_module_create_info = {0};
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.codeSize = compute_shader_size;
    shader_module_create_info.pCode = (uint32_t const *)compute_shader_bytes;

    VULKAN_CHECK(vkCreateShaderModule(g_context_device, &shader_module_create_info, 0, &compute_module));
  }

  VkPipelineShaderStageCreateInfo compute_shader_stage_create_info = {0};
  compute_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  compute_shader_stage_create_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
  compute_shader_stage_create_info.module = compute_module;
  compute_shader_stage_create_info.pName = "main";

  VkComputePipelineCreateInfo compute_pipeline_create_info = {0};
  compute_pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  compute_pipeline_create_info.layout = s_renderer_chunk_generator_pipeline_layout;
  compute_pipeline_create_info.stage = compute_shader_stage_create_info;

  VULKAN_CHECK(vkCreateComputePipelines(g_context_device, 0, 1, &compute_pipeline_create_info, 0, &s_renderer_chunk_generator_pipeline));

  vkDestroyShaderModule(g_context_device, compute_module, 0);

  heap_free(compute_shader_bytes);
}
static void renderer_create_chunk_mipmap_pipeline(void) {
  uint8_t *compute_shader_bytes = 0;

  uint64_t compute_shader_size = 0;

  filesystem_read_binary(&compute_shader_bytes, &compute_shader_size, s_renderer_chunk_mipmap_shader_file_path);

  VkShaderModule compute_module = 0;

  {
    VkShaderModuleCreateInfo shader_module_create_info = {0};
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.codeSize = compute_shader_size;
    shader_module_create_info.pCode = (uint32_t const *)compute_shader_bytes;

    VULKAN_CHECK(vkCreateShaderModule(g_context_device, &shader_module_create_info, 0, &compute_module));
  }

  VkPipelineShaderStageCreateInfo compute_shader_stage_create_info = {0};
  compute_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  compute_shader_stage_create_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
  compute_shader_stage_create_info.module = compute_module;
  compute_shader_stage_create_info.pName = "main";

  VkComputePipelineCreateInfo compute_pipeline_create_info = {0};
  compute_pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  compute_pipeline_create_info.layout = s_renderer_chunk_mipmap_pipeline_layout;
  compute_pipeline_create_info.stage = compute_shader_stage_create_info;

  VULKAN_CHECK(vkCreateComputePipelines(g_context_device, 0, 1, &compute_pipeline_create_info, 0, &s_renderer_chunk_mipmap_pipeline));

  vkDestroyShaderModule(g_context_device, compute_module, 0);

  heap_free(compute_shader_bytes);
}
static void renderer_create_chunk_renderer_pipeline(void) {
  uint8_t *vertex_shader_bytes = 0;
  uint8_t *fragment_shader_bytes = 0;

  uint64_t vertex_shader_size = 0;
  uint64_t fragment_shader_size = 0;

  filesystem_read_binary(&vertex_shader_bytes, &vertex_shader_size, s_renderer_chunk_renderer_vertex_shader_file_path);
  filesystem_read_binary(&fragment_shader_bytes, &fragment_shader_size, s_renderer_chunk_renderer_fragment_shader_file_path);

  VkShaderModule vertex_module = 0;
  VkShaderModule fragment_module = 0;

  {
    VkShaderModuleCreateInfo shader_module_create_info = {0};
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.codeSize = vertex_shader_size;
    shader_module_create_info.pCode = (uint32_t const *)vertex_shader_bytes;

    VULKAN_CHECK(vkCreateShaderModule(g_context_device, &shader_module_create_info, 0, &vertex_module));
  }

  {
    VkShaderModuleCreateInfo shader_module_create_info = {0};
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.codeSize = fragment_shader_size;
    shader_module_create_info.pCode = (uint32_t const *)fragment_shader_bytes;

    VULKAN_CHECK(vkCreateShaderModule(g_context_device, &shader_module_create_info, 0, &fragment_module));
  }

  VkPipelineShaderStageCreateInfo vertex_shader_stage_create_info = {0};
  vertex_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertex_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertex_shader_stage_create_info.module = vertex_module;
  vertex_shader_stage_create_info.pName = "main";

  VkPipelineShaderStageCreateInfo fragment_shader_stage_create_info = {0};
  fragment_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragment_shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragment_shader_stage_create_info.module = fragment_module;
  fragment_shader_stage_create_info.pName = "main";

  VkPipelineShaderStageCreateInfo shader_stages[] = {vertex_shader_stage_create_info, fragment_shader_stage_create_info};

  VkPipelineVertexInputStateCreateInfo vertex_input_create_info = {0};
  vertex_input_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_create_info.pVertexBindingDescriptions = s_renderer_chunk_renderer_vertex_input_binding_descriptions;
  vertex_input_create_info.vertexBindingDescriptionCount = ARRAY_COUNT(s_renderer_chunk_renderer_vertex_input_binding_descriptions);
  vertex_input_create_info.pVertexAttributeDescriptions = s_renderer_chunk_renderer_vertex_input_attribute_descriptions;
  vertex_input_create_info.vertexAttributeDescriptionCount = ARRAY_COUNT(s_renderer_chunk_renderer_vertex_input_attribute_descriptions);

  VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info = {0};
  input_assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly_create_info.primitiveRestartEnable = 0;

  VkViewport viewport = {0};
  viewport.x = 0.0F;
  viewport.y = 0.0F;
  viewport.width = (float)g_context_surface_width;
  viewport.height = (float)g_context_surface_height;
  viewport.minDepth = 0.0F;
  viewport.maxDepth = 1.0F;

  VkRect2D scissor = {0};
  scissor.offset.x = 0;
  scissor.offset.y = 0;
  scissor.extent.width = g_context_surface_width;
  scissor.extent.height = g_context_surface_height;

  VkPipelineViewportStateCreateInfo viewport_state_create_info = {0};
  viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state_create_info.viewportCount = 1;
  viewport_state_create_info.pViewports = &viewport;
  viewport_state_create_info.scissorCount = 1;
  viewport_state_create_info.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer_create_info = {0};
  rasterizer_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer_create_info.depthClampEnable = 0;
  rasterizer_create_info.rasterizerDiscardEnable = 0;
  rasterizer_create_info.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer_create_info.lineWidth = 1.0F;
  rasterizer_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer_create_info.depthBiasEnable = 0;
  rasterizer_create_info.depthBiasConstantFactor = 0.0F;
  rasterizer_create_info.depthBiasClamp = 0.0F;
  rasterizer_create_info.depthBiasSlopeFactor = 0.0F;

  VkPipelineMultisampleStateCreateInfo multisampling_create_info = {0};
  multisampling_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling_create_info.sampleShadingEnable = 0;
  multisampling_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling_create_info.minSampleShading = 1.0F;
  multisampling_create_info.pSampleMask = 0;
  multisampling_create_info.alphaToCoverageEnable = 0;
  multisampling_create_info.alphaToOneEnable = 0;

  VkPipelineColorBlendAttachmentState color_blend_attachment = {0};
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = 1;
  color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {0};
  depth_stencil_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil_state_create_info.depthTestEnable = 1;
  depth_stencil_state_create_info.depthWriteEnable = 1;
  depth_stencil_state_create_info.depthCompareOp = VK_COMPARE_OP_LESS;
  depth_stencil_state_create_info.depthBoundsTestEnable = 0;
  depth_stencil_state_create_info.stencilTestEnable = 0;

  VkPipelineColorBlendStateCreateInfo color_blend_create_info = {0};
  color_blend_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend_create_info.logicOpEnable = 0;
  color_blend_create_info.logicOp = VK_LOGIC_OP_COPY;
  color_blend_create_info.attachmentCount = 1;
  color_blend_create_info.pAttachments = &color_blend_attachment;
  color_blend_create_info.blendConstants[0] = 0.0F;
  color_blend_create_info.blendConstants[1] = 0.0F;
  color_blend_create_info.blendConstants[2] = 0.0F;
  color_blend_create_info.blendConstants[3] = 0.0F;

  VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {0};
  dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state_create_info.pDynamicStates = dynamic_states;
  dynamic_state_create_info.dynamicStateCount = ARRAY_COUNT(dynamic_states);

  VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {0};
  graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  graphics_pipeline_create_info.pStages = shader_stages;
  graphics_pipeline_create_info.stageCount = ARRAY_COUNT(shader_stages);
  graphics_pipeline_create_info.pVertexInputState = &vertex_input_create_info;
  graphics_pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;
  graphics_pipeline_create_info.pViewportState = &viewport_state_create_info;
  graphics_pipeline_create_info.pRasterizationState = &rasterizer_create_info;
  graphics_pipeline_create_info.pMultisampleState = &multisampling_create_info;
  graphics_pipeline_create_info.pDepthStencilState = &depth_stencil_state_create_info;
  graphics_pipeline_create_info.pColorBlendState = &color_blend_create_info;
  graphics_pipeline_create_info.pDynamicState = &dynamic_state_create_info;
  graphics_pipeline_create_info.layout = s_renderer_chunk_renderer_pipeline_layout;
  graphics_pipeline_create_info.renderPass = g_swapchain_render_pass;
  graphics_pipeline_create_info.subpass = 0;
  graphics_pipeline_create_info.basePipelineHandle = 0;

  VULKAN_CHECK(vkCreateGraphicsPipelines(g_context_device, 0, 1, &graphics_pipeline_create_info, 0, &s_renderer_chunk_renderer_pipeline));

  vkDestroyShaderModule(g_context_device, vertex_module, 0);
  vkDestroyShaderModule(g_context_device, fragment_module, 0);

  heap_free(vertex_shader_bytes);
  heap_free(fragment_shader_bytes);
}
static void renderer_create_debug_line_pipeline(void) {
  uint8_t *vertex_shader_bytes = 0;
  uint8_t *fragment_shader_bytes = 0;

  uint64_t vertex_shader_size = 0;
  uint64_t fragment_shader_size = 0;

  filesystem_read_binary(&vertex_shader_bytes, &vertex_shader_size, s_renderer_debug_line_vertex_shader_file_path);
  filesystem_read_binary(&fragment_shader_bytes, &fragment_shader_size, s_renderer_debug_line_fragment_shader_file_path);

  VkShaderModule vertex_module = 0;
  VkShaderModule fragment_module = 0;

  {
    VkShaderModuleCreateInfo shader_module_create_info = {0};
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.codeSize = vertex_shader_size;
    shader_module_create_info.pCode = (uint32_t const *)vertex_shader_bytes;

    VULKAN_CHECK(vkCreateShaderModule(g_context_device, &shader_module_create_info, 0, &vertex_module));
  }

  {
    VkShaderModuleCreateInfo shader_module_create_info = {0};
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.codeSize = fragment_shader_size;
    shader_module_create_info.pCode = (uint32_t const *)fragment_shader_bytes;

    VULKAN_CHECK(vkCreateShaderModule(g_context_device, &shader_module_create_info, 0, &fragment_module));
  }

  VkPipelineShaderStageCreateInfo vertex_shader_stage_create_info = {0};
  vertex_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertex_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertex_shader_stage_create_info.module = vertex_module;
  vertex_shader_stage_create_info.pName = "main";

  VkPipelineShaderStageCreateInfo fragment_shader_stage_create_info = {0};
  fragment_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragment_shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragment_shader_stage_create_info.module = fragment_module;
  fragment_shader_stage_create_info.pName = "main";

  VkPipelineShaderStageCreateInfo shader_stages[] = {vertex_shader_stage_create_info, fragment_shader_stage_create_info};

  VkPipelineVertexInputStateCreateInfo vertex_input_create_info = {0};
  vertex_input_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_create_info.pVertexBindingDescriptions = s_renderer_debug_line_vertex_input_binding_descriptions;
  vertex_input_create_info.vertexBindingDescriptionCount = ARRAY_COUNT(s_renderer_debug_line_vertex_input_binding_descriptions);
  vertex_input_create_info.pVertexAttributeDescriptions = s_renderer_debug_line_vertex_input_attribute_descriptions;
  vertex_input_create_info.vertexAttributeDescriptionCount = ARRAY_COUNT(s_renderer_debug_line_vertex_input_attribute_descriptions);

  VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info = {0};
  input_assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
  input_assembly_create_info.primitiveRestartEnable = 0;

  VkViewport viewport = {0};
  viewport.x = 0.0F;
  viewport.y = 0.0F;
  viewport.width = (float)g_context_surface_width;
  viewport.height = (float)g_context_surface_height;
  viewport.minDepth = 0.0F;
  viewport.maxDepth = 1.0F;

  VkRect2D scissor = {0};
  scissor.offset.x = 0;
  scissor.offset.y = 0;
  scissor.extent.width = g_context_surface_width;
  scissor.extent.height = g_context_surface_height;

  VkPipelineViewportStateCreateInfo viewport_state_create_info = {0};
  viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state_create_info.viewportCount = 1;
  viewport_state_create_info.pViewports = &viewport;
  viewport_state_create_info.scissorCount = 1;
  viewport_state_create_info.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer_create_info = {0};
  rasterizer_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer_create_info.depthClampEnable = 0;
  rasterizer_create_info.rasterizerDiscardEnable = 0;
  rasterizer_create_info.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer_create_info.lineWidth = 1.0F;
  rasterizer_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer_create_info.depthBiasEnable = 0;
  rasterizer_create_info.depthBiasConstantFactor = 0.0F;
  rasterizer_create_info.depthBiasClamp = 0.0F;
  rasterizer_create_info.depthBiasSlopeFactor = 0.0F;

  VkPipelineMultisampleStateCreateInfo multisampling_create_info = {0};
  multisampling_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling_create_info.sampleShadingEnable = 0;
  multisampling_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling_create_info.minSampleShading = 1.0F;
  multisampling_create_info.pSampleMask = 0;
  multisampling_create_info.alphaToCoverageEnable = 0;
  multisampling_create_info.alphaToOneEnable = 0;

  VkPipelineColorBlendAttachmentState color_blend_attachment = {0};
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = 0;
  color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {0};
  depth_stencil_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil_state_create_info.depthTestEnable = 0;
  depth_stencil_state_create_info.depthWriteEnable = 0;
  depth_stencil_state_create_info.depthCompareOp = VK_COMPARE_OP_LESS;
  depth_stencil_state_create_info.depthBoundsTestEnable = 0;
  depth_stencil_state_create_info.stencilTestEnable = 0;

  VkPipelineColorBlendStateCreateInfo color_blend_create_info = {0};
  color_blend_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend_create_info.logicOpEnable = 0;
  color_blend_create_info.logicOp = VK_LOGIC_OP_COPY;
  color_blend_create_info.attachmentCount = 1;
  color_blend_create_info.pAttachments = &color_blend_attachment;
  color_blend_create_info.blendConstants[0] = 0.0F;
  color_blend_create_info.blendConstants[1] = 0.0F;
  color_blend_create_info.blendConstants[2] = 0.0F;
  color_blend_create_info.blendConstants[3] = 0.0F;

  VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {0};
  dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state_create_info.pDynamicStates = dynamic_states;
  dynamic_state_create_info.dynamicStateCount = ARRAY_COUNT(dynamic_states);

  VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {0};
  graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  graphics_pipeline_create_info.pStages = shader_stages;
  graphics_pipeline_create_info.stageCount = ARRAY_COUNT(shader_stages);
  graphics_pipeline_create_info.pVertexInputState = &vertex_input_create_info;
  graphics_pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;
  graphics_pipeline_create_info.pViewportState = &viewport_state_create_info;
  graphics_pipeline_create_info.pRasterizationState = &rasterizer_create_info;
  graphics_pipeline_create_info.pMultisampleState = &multisampling_create_info;
  graphics_pipeline_create_info.pDepthStencilState = &depth_stencil_state_create_info;
  graphics_pipeline_create_info.pColorBlendState = &color_blend_create_info;
  graphics_pipeline_create_info.pDynamicState = &dynamic_state_create_info;
  graphics_pipeline_create_info.layout = s_renderer_debug_line_pipeline_layout;
  graphics_pipeline_create_info.renderPass = g_swapchain_render_pass;
  graphics_pipeline_create_info.subpass = 0;
  graphics_pipeline_create_info.basePipelineHandle = 0;

  VULKAN_CHECK(vkCreateGraphicsPipelines(g_context_device, 0, 1, &graphics_pipeline_create_info, 0, &s_renderer_debug_line_pipeline));

  vkDestroyShaderModule(g_context_device, vertex_module, 0);
  vkDestroyShaderModule(g_context_device, fragment_module, 0);

  heap_free(vertex_shader_bytes);
  heap_free(fragment_shader_bytes);
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
static void renderer_create_cluster_buffer(void) {
  s_renderer_cluster_buffer = (VkBuffer *)heap_alloc(sizeof(VkBuffer) * s_renderer_frames_in_flight);
  s_renderer_cluster_buffer_device_memory = (VkDeviceMemory *)heap_alloc(sizeof(VkDeviceMemory) * s_renderer_frames_in_flight);
  s_renderer_cluster = (renderer_cluster_t **)heap_alloc(sizeof(renderer_cluster_t *) * s_renderer_frames_in_flight);

  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    VkDeviceSize size = sizeof(renderer_cluster_t);

    VkBufferCreateInfo buffer_create_info = {0};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = size;
    buffer_create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VULKAN_CHECK(vkCreateBuffer(g_context_device, &buffer_create_info, 0, &s_renderer_cluster_buffer[frame_index]));

    VkMemoryRequirements memory_requirements = {0};

    vkGetBufferMemoryRequirements(g_context_device, s_renderer_cluster_buffer[frame_index], &memory_requirements);

    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = context_find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);

    VULKAN_CHECK(vkAllocateMemory(g_context_device, &memory_allocate_info, 0, &s_renderer_cluster_buffer_device_memory[frame_index]));
    VULKAN_CHECK(vkBindBufferMemory(g_context_device, s_renderer_cluster_buffer[frame_index], s_renderer_cluster_buffer_device_memory[frame_index], 0));
    VULKAN_CHECK(vkMapMemory(g_context_device, s_renderer_cluster_buffer_device_memory[frame_index], 0, size, 0, &s_renderer_cluster[frame_index]));

    frame_index++;
  }
}
static void renderer_create_cluster_vertex_buffer(void) {
  s_renderer_cluster_vertex_buffer = (VkBuffer *)heap_alloc(sizeof(VkBuffer) * s_renderer_frames_in_flight);
  s_renderer_cluster_vertex_buffer_device_memory = (VkDeviceMemory *)heap_alloc(sizeof(VkDeviceMemory) * s_renderer_frames_in_flight);
  s_renderer_cluster_vertex = (renderer_cluster_vertex_t **)heap_alloc(sizeof(renderer_cluster_vertex_t *) * s_renderer_frames_in_flight);

  uint64_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    VkDeviceSize size = sizeof(renderer_cluster_vertex_t) * RENDERER_CLUSTER_VERTEX_COUNT;

    VkBufferCreateInfo buffer_create_info = {0};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = size;
    buffer_create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VULKAN_CHECK(vkCreateBuffer(g_context_device, &buffer_create_info, 0, &s_renderer_cluster_vertex_buffer[frame_index]));

    VkMemoryRequirements memory_requirements = {0};

    vkGetBufferMemoryRequirements(g_context_device, s_renderer_cluster_vertex_buffer[frame_index], &memory_requirements);

    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = context_find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VULKAN_CHECK(vkAllocateMemory(g_context_device, &memory_allocate_info, 0, &s_renderer_cluster_vertex_buffer_device_memory[frame_index]));
    VULKAN_CHECK(vkBindBufferMemory(g_context_device, s_renderer_cluster_vertex_buffer[frame_index], s_renderer_cluster_vertex_buffer_device_memory[frame_index], 0));
    VULKAN_CHECK(vkMapMemory(g_context_device, s_renderer_cluster_vertex_buffer_device_memory[frame_index], 0, size, 0, &s_renderer_cluster_vertex[frame_index]));

    frame_index++;
  }
}
static void renderer_create_cluster_index_buffer(void) {
  s_renderer_cluster_index_buffer = (VkBuffer *)heap_alloc(sizeof(VkBuffer) * s_renderer_frames_in_flight);
  s_renderer_cluster_index_buffer_device_memory = (VkDeviceMemory *)heap_alloc(sizeof(VkDeviceMemory) * s_renderer_frames_in_flight);
  s_renderer_cluster_index = (renderer_cluster_index_t **)heap_alloc(sizeof(renderer_cluster_index_t *) * s_renderer_frames_in_flight);

  uint64_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    VkDeviceSize size = sizeof(renderer_cluster_index_t) * RENDERER_CLUSTER_INDEX_COUNT;

    VkBufferCreateInfo buffer_create_info = {0};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = size;
    buffer_create_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VULKAN_CHECK(vkCreateBuffer(g_context_device, &buffer_create_info, 0, &s_renderer_cluster_index_buffer[frame_index]));

    VkMemoryRequirements memory_requirements = {0};

    vkGetBufferMemoryRequirements(g_context_device, s_renderer_cluster_index_buffer[frame_index], &memory_requirements);

    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = context_find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VULKAN_CHECK(vkAllocateMemory(g_context_device, &memory_allocate_info, 0, &s_renderer_cluster_index_buffer_device_memory[frame_index]));
    VULKAN_CHECK(vkBindBufferMemory(g_context_device, s_renderer_cluster_index_buffer[frame_index], s_renderer_cluster_index_buffer_device_memory[frame_index], 0));
    VULKAN_CHECK(vkMapMemory(g_context_device, s_renderer_cluster_index_buffer_device_memory[frame_index], 0, size, 0, &s_renderer_cluster_index[frame_index]));

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

static void renderer_create_chunk_images(void) {
  s_renderer_chunk_image = (VkImage **)heap_alloc(sizeof(VkImage *) * s_renderer_frames_in_flight);
  s_renderer_chunk_image_device_memory = (VkDeviceMemory **)heap_alloc(sizeof(VkDeviceMemory *) * s_renderer_frames_in_flight);
  s_renderer_chunk_image_view = (VkImageView ***)heap_alloc(sizeof(VkImageView **) * s_renderer_frames_in_flight);
  s_renderer_chunk_sampler = (VkSampler ***)heap_alloc(sizeof(VkSampler **) * s_renderer_frames_in_flight);

  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    s_renderer_chunk_image[frame_index] = (VkImage *)heap_alloc(sizeof(VkImage) * s_renderer_chunk_count);
    s_renderer_chunk_image_device_memory[frame_index] = (VkDeviceMemory *)heap_alloc(sizeof(VkDeviceMemory) * s_renderer_chunk_count);
    s_renderer_chunk_image_view[frame_index] = (VkImageView **)heap_alloc(sizeof(VkImageView *) * s_renderer_chunk_count);
    s_renderer_chunk_sampler[frame_index] = (VkSampler **)heap_alloc(sizeof(VkSampler *) * s_renderer_chunk_count);

    int32_t chunk_index = 0;
    while (chunk_index < s_renderer_chunk_count) {
      VkImageCreateInfo image_create_info = {0};
      image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
      image_create_info.imageType = VK_IMAGE_TYPE_3D;
      image_create_info.extent.width = g_renderer_chunk_size;
      image_create_info.extent.height = g_renderer_chunk_size;
      image_create_info.extent.depth = g_renderer_chunk_size;
      image_create_info.mipLevels = s_renderer_chunk_lod_levels;
      image_create_info.arrayLayers = 1;
      image_create_info.format = VK_FORMAT_R32_UINT;
      image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
      image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      image_create_info.usage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
      image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
      image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

      VULKAN_CHECK(vkCreateImage(g_context_device, &image_create_info, 0, &s_renderer_chunk_image[frame_index][chunk_index]));

      VkMemoryRequirements memory_requirements = {0};

      vkGetImageMemoryRequirements(g_context_device, s_renderer_chunk_image[frame_index][chunk_index], &memory_requirements);

      int32_t memory_type_index = context_find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

      VkMemoryAllocateInfo memory_allocate_info = {0};
      memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      memory_allocate_info.allocationSize = memory_requirements.size;
      memory_allocate_info.memoryTypeIndex = memory_type_index;

      VULKAN_CHECK(vkAllocateMemory(g_context_device, &memory_allocate_info, 0, &s_renderer_chunk_image_device_memory[frame_index][chunk_index]));
      VULKAN_CHECK(vkBindImageMemory(g_context_device, s_renderer_chunk_image[frame_index][chunk_index], s_renderer_chunk_image_device_memory[frame_index][chunk_index], 0));

      s_renderer_chunk_image_view[frame_index][chunk_index] = (VkImageView *)heap_alloc(sizeof(VkImageView) * s_renderer_chunk_lod_levels);
      s_renderer_chunk_sampler[frame_index][chunk_index] = (VkSampler *)heap_alloc(sizeof(VkSampler) * s_renderer_chunk_lod_levels);

      int32_t lod = 0;
      while (lod < s_renderer_chunk_lod_levels) {
        VkImageViewCreateInfo image_view_create_info = {0};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = s_renderer_chunk_image[frame_index][chunk_index];
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_3D;
        image_view_create_info.format = VK_FORMAT_R32_UINT;
        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = lod;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;

        VULKAN_CHECK(vkCreateImageView(g_context_device, &image_view_create_info, 0, &s_renderer_chunk_image_view[frame_index][chunk_index][lod]));

        VkSamplerCreateInfo sampler_create_info = {0};
        sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_create_info.magFilter = VK_FILTER_NEAREST;
        sampler_create_info.minFilter = VK_FILTER_NEAREST;
        sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler_create_info.anisotropyEnable = 1;
        sampler_create_info.maxAnisotropy = g_context_physical_device_properties.limits.maxSamplerAnisotropy;
        sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_create_info.unnormalizedCoordinates = 0;
        sampler_create_info.compareEnable = 0;
        sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
        sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        sampler_create_info.mipLodBias = 0.0F;
        sampler_create_info.minLod = 0.0F;
        sampler_create_info.maxLod = 0.0F;

        VULKAN_CHECK(vkCreateSampler(g_context_device, &sampler_create_info, 0, &s_renderer_chunk_sampler[frame_index][chunk_index][lod]));

        lod++;
      }

      VkImageMemoryBarrier image_memory_barrier = {0};
      image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
      image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      image_memory_barrier.image = s_renderer_chunk_image[frame_index][chunk_index];
      image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      image_memory_barrier.subresourceRange.baseMipLevel = 0;
      image_memory_barrier.subresourceRange.levelCount = s_renderer_chunk_lod_levels;
      image_memory_barrier.subresourceRange.baseArrayLayer = 0;
      image_memory_barrier.subresourceRange.layerCount = 1;
      image_memory_barrier.srcAccessMask = VK_ACCESS_NONE;
      image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

      VkCommandBuffer command_buffer = context_begin_command_buffer();

      vkCmdPipelineBarrier(
        command_buffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        0, 0, 0, 0, 0, 1, &image_memory_barrier);

      context_end_command_buffer(command_buffer);

      chunk_index++;
    }

    frame_index++;
  }
}

static void renderer_create_cluster_vertices(void) {
  int32_t cluster_size_x = g_renderer_chunk_size * s_renderer_chunk_count_x;
  int32_t cluster_size_y = g_renderer_chunk_size * s_renderer_chunk_count_y;
  int32_t cluster_size_z = g_renderer_chunk_size * s_renderer_chunk_count_z;

  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    s_renderer_cluster_vertex[frame_index][0].position = (ivector3_t){0, 0, 0};
    s_renderer_cluster_vertex[frame_index][1].position = (ivector3_t){0, cluster_size_y, 0};
    s_renderer_cluster_vertex[frame_index][2].position = (ivector3_t){cluster_size_x, 0, 0};
    s_renderer_cluster_vertex[frame_index][3].position = (ivector3_t){cluster_size_x, cluster_size_y, 0};
    s_renderer_cluster_vertex[frame_index][4].position = (ivector3_t){0, 0, cluster_size_z};
    s_renderer_cluster_vertex[frame_index][5].position = (ivector3_t){0, cluster_size_y, cluster_size_z};
    s_renderer_cluster_vertex[frame_index][6].position = (ivector3_t){cluster_size_x, 0, cluster_size_z};
    s_renderer_cluster_vertex[frame_index][7].position = (ivector3_t){cluster_size_x, cluster_size_y, cluster_size_z};

    frame_index++;
  }
}
static void renderer_create_cluster_indices(void) {
  uint64_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    s_renderer_cluster_index[frame_index][0] = 0;
    s_renderer_cluster_index[frame_index][1] = 2;
    s_renderer_cluster_index[frame_index][2] = 3;
    s_renderer_cluster_index[frame_index][3] = 3;
    s_renderer_cluster_index[frame_index][4] = 1;
    s_renderer_cluster_index[frame_index][5] = 0;
    s_renderer_cluster_index[frame_index][6] = 4;
    s_renderer_cluster_index[frame_index][7] = 5;
    s_renderer_cluster_index[frame_index][8] = 7;
    s_renderer_cluster_index[frame_index][9] = 7;
    s_renderer_cluster_index[frame_index][10] = 6;
    s_renderer_cluster_index[frame_index][11] = 4;
    s_renderer_cluster_index[frame_index][12] = 0;
    s_renderer_cluster_index[frame_index][13] = 1;
    s_renderer_cluster_index[frame_index][14] = 5;
    s_renderer_cluster_index[frame_index][15] = 5;
    s_renderer_cluster_index[frame_index][16] = 4;
    s_renderer_cluster_index[frame_index][17] = 0;
    s_renderer_cluster_index[frame_index][18] = 2;
    s_renderer_cluster_index[frame_index][19] = 6;
    s_renderer_cluster_index[frame_index][20] = 7;
    s_renderer_cluster_index[frame_index][21] = 7;
    s_renderer_cluster_index[frame_index][22] = 3;
    s_renderer_cluster_index[frame_index][23] = 2;
    s_renderer_cluster_index[frame_index][24] = 1;
    s_renderer_cluster_index[frame_index][25] = 3;
    s_renderer_cluster_index[frame_index][26] = 7;
    s_renderer_cluster_index[frame_index][27] = 7;
    s_renderer_cluster_index[frame_index][28] = 5;
    s_renderer_cluster_index[frame_index][29] = 1;
    s_renderer_cluster_index[frame_index][30] = 0;
    s_renderer_cluster_index[frame_index][31] = 4;
    s_renderer_cluster_index[frame_index][32] = 6;
    s_renderer_cluster_index[frame_index][33] = 6;
    s_renderer_cluster_index[frame_index][34] = 2;
    s_renderer_cluster_index[frame_index][35] = 0;

    frame_index++;
  }
}

static void renderer_update_chunk_editor_descriptor_sets(void) {
  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    VkDescriptorBufferInfo cluster_descriptor_buffer_infos[1] = {0};

    cluster_descriptor_buffer_infos[0].offset = 0;
    cluster_descriptor_buffer_infos[0].buffer = s_renderer_cluster_buffer[frame_index];
    cluster_descriptor_buffer_infos[0].range = VK_WHOLE_SIZE;

    VkDescriptorImageInfo *chunk_descriptor_image_infos = (VkDescriptorImageInfo *)heap_alloc(sizeof(VkDescriptorImageInfo) * s_renderer_chunk_count * s_renderer_chunk_lod_levels);

    for (int32_t z = 0; z < s_renderer_chunk_count_z; z++) {
      for (int32_t y = 0; y < s_renderer_chunk_count_y; y++) {
        for (int32_t x = 0; x < s_renderer_chunk_count_x; x++) {
          for (int32_t lod = 0; lod < s_renderer_chunk_lod_levels; lod++) {
            int32_t chunk_index = x + (y * s_renderer_chunk_count_x) + (z * s_renderer_chunk_count_x * s_renderer_chunk_count_y);
            int32_t descriptor_index = lod + (chunk_index * s_renderer_chunk_lod_levels);

            chunk_descriptor_image_infos[descriptor_index].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            chunk_descriptor_image_infos[descriptor_index].imageView = s_renderer_chunk_image_view[frame_index][chunk_index][lod];
            chunk_descriptor_image_infos[descriptor_index].sampler = 0;
          }
        }
      }
    }

    VkWriteDescriptorSet write_descriptor_sets[2] = {0};

    write_descriptor_sets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_sets[0].pNext = 0;
    write_descriptor_sets[0].dstSet = s_renderer_chunk_editor_descriptor_sets[frame_index];
    write_descriptor_sets[0].dstBinding = 0;
    write_descriptor_sets[0].dstArrayElement = 0;
    write_descriptor_sets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_descriptor_sets[0].descriptorCount = ARRAY_COUNT(cluster_descriptor_buffer_infos);
    write_descriptor_sets[0].pImageInfo = 0;
    write_descriptor_sets[0].pBufferInfo = cluster_descriptor_buffer_infos;
    write_descriptor_sets[0].pTexelBufferView = 0;

    write_descriptor_sets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_sets[1].pNext = 0;
    write_descriptor_sets[1].dstSet = s_renderer_chunk_editor_descriptor_sets[frame_index];
    write_descriptor_sets[1].dstBinding = 1;
    write_descriptor_sets[1].dstArrayElement = 0;
    write_descriptor_sets[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    write_descriptor_sets[1].descriptorCount = s_renderer_chunk_count * s_renderer_chunk_lod_levels;
    write_descriptor_sets[1].pImageInfo = chunk_descriptor_image_infos;
    write_descriptor_sets[1].pBufferInfo = 0;
    write_descriptor_sets[1].pTexelBufferView = 0;

    vkUpdateDescriptorSets(g_context_device, ARRAY_COUNT(write_descriptor_sets), write_descriptor_sets, 0, 0);

    heap_free(chunk_descriptor_image_infos);

    frame_index++;
  }
}
static void renderer_update_chunk_generator_descriptor_sets(void) {
  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    VkDescriptorBufferInfo cluster_descriptor_buffer_infos[1] = {0};

    cluster_descriptor_buffer_infos[0].offset = 0;
    cluster_descriptor_buffer_infos[0].buffer = s_renderer_cluster_buffer[frame_index];
    cluster_descriptor_buffer_infos[0].range = VK_WHOLE_SIZE;

    VkDescriptorImageInfo *chunk_descriptor_image_infos = (VkDescriptorImageInfo *)heap_alloc(sizeof(VkDescriptorImageInfo) * s_renderer_chunk_count * s_renderer_chunk_lod_levels);

    for (int32_t z = 0; z < s_renderer_chunk_count_z; z++) {
      for (int32_t y = 0; y < s_renderer_chunk_count_y; y++) {
        for (int32_t x = 0; x < s_renderer_chunk_count_x; x++) {
          for (int32_t lod = 0; lod < s_renderer_chunk_lod_levels; lod++) {
            int32_t chunk_index = x + (y * s_renderer_chunk_count_x) + (z * s_renderer_chunk_count_x * s_renderer_chunk_count_y);
            int32_t descriptor_index = lod + (chunk_index * s_renderer_chunk_lod_levels);

            chunk_descriptor_image_infos[descriptor_index].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            chunk_descriptor_image_infos[descriptor_index].imageView = s_renderer_chunk_image_view[frame_index][chunk_index][lod];
            chunk_descriptor_image_infos[descriptor_index].sampler = 0;
          }
        }
      }
    }

    VkWriteDescriptorSet write_descriptor_sets[2] = {0};

    write_descriptor_sets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_sets[0].pNext = 0;
    write_descriptor_sets[0].dstSet = s_renderer_chunk_generator_descriptor_sets[frame_index];
    write_descriptor_sets[0].dstBinding = 0;
    write_descriptor_sets[0].dstArrayElement = 0;
    write_descriptor_sets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_descriptor_sets[0].descriptorCount = ARRAY_COUNT(cluster_descriptor_buffer_infos);
    write_descriptor_sets[0].pImageInfo = 0;
    write_descriptor_sets[0].pBufferInfo = cluster_descriptor_buffer_infos;
    write_descriptor_sets[0].pTexelBufferView = 0;

    write_descriptor_sets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_sets[1].pNext = 0;
    write_descriptor_sets[1].dstSet = s_renderer_chunk_generator_descriptor_sets[frame_index];
    write_descriptor_sets[1].dstBinding = 1;
    write_descriptor_sets[1].dstArrayElement = 0;
    write_descriptor_sets[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    write_descriptor_sets[1].descriptorCount = s_renderer_chunk_count * s_renderer_chunk_lod_levels;
    write_descriptor_sets[1].pImageInfo = chunk_descriptor_image_infos;
    write_descriptor_sets[1].pBufferInfo = 0;
    write_descriptor_sets[1].pTexelBufferView = 0;

    vkUpdateDescriptorSets(g_context_device, ARRAY_COUNT(write_descriptor_sets), write_descriptor_sets, 0, 0);

    heap_free(chunk_descriptor_image_infos);

    frame_index++;
  }
}
static void renderer_update_chunk_mipmap_descriptor_sets(void) {
  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    VkDescriptorBufferInfo cluster_descriptor_buffer_infos[1] = {0};

    cluster_descriptor_buffer_infos[0].offset = 0;
    cluster_descriptor_buffer_infos[0].buffer = s_renderer_cluster_buffer[frame_index];
    cluster_descriptor_buffer_infos[0].range = VK_WHOLE_SIZE;

    VkDescriptorImageInfo *chunk_descriptor_image_infos = (VkDescriptorImageInfo *)heap_alloc(sizeof(VkDescriptorImageInfo) * s_renderer_chunk_count * s_renderer_chunk_lod_levels);

    for (int32_t z = 0; z < s_renderer_chunk_count_z; z++) {
      for (int32_t y = 0; y < s_renderer_chunk_count_y; y++) {
        for (int32_t x = 0; x < s_renderer_chunk_count_x; x++) {
          for (int32_t lod = 0; lod < s_renderer_chunk_lod_levels; lod++) {
            int32_t chunk_index = x + (y * s_renderer_chunk_count_x) + (z * s_renderer_chunk_count_x * s_renderer_chunk_count_y);
            int32_t descriptor_index = lod + (chunk_index * s_renderer_chunk_lod_levels);

            chunk_descriptor_image_infos[descriptor_index].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            chunk_descriptor_image_infos[descriptor_index].imageView = s_renderer_chunk_image_view[frame_index][chunk_index][lod];
            chunk_descriptor_image_infos[descriptor_index].sampler = 0;
          }
        }
      }
    }

    VkWriteDescriptorSet write_descriptor_sets[2] = {0};

    write_descriptor_sets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_sets[0].pNext = 0;
    write_descriptor_sets[0].dstSet = s_renderer_chunk_mipmap_descriptor_sets[frame_index];
    write_descriptor_sets[0].dstBinding = 0;
    write_descriptor_sets[0].dstArrayElement = 0;
    write_descriptor_sets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_descriptor_sets[0].descriptorCount = ARRAY_COUNT(cluster_descriptor_buffer_infos);
    write_descriptor_sets[0].pImageInfo = 0;
    write_descriptor_sets[0].pBufferInfo = cluster_descriptor_buffer_infos;
    write_descriptor_sets[0].pTexelBufferView = 0;

    write_descriptor_sets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_sets[1].pNext = 0;
    write_descriptor_sets[1].dstSet = s_renderer_chunk_mipmap_descriptor_sets[frame_index];
    write_descriptor_sets[1].dstBinding = 1;
    write_descriptor_sets[1].dstArrayElement = 0;
    write_descriptor_sets[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    write_descriptor_sets[1].descriptorCount = s_renderer_chunk_count * s_renderer_chunk_lod_levels;
    write_descriptor_sets[1].pImageInfo = chunk_descriptor_image_infos;
    write_descriptor_sets[1].pBufferInfo = 0;
    write_descriptor_sets[1].pTexelBufferView = 0;

    vkUpdateDescriptorSets(g_context_device, ARRAY_COUNT(write_descriptor_sets), write_descriptor_sets, 0, 0);

    heap_free(chunk_descriptor_image_infos);

    frame_index++;
  }
}
static void renderer_update_chunk_renderer_descriptor_sets(void) {
  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    VkDescriptorBufferInfo time_descriptor_buffer_infos[1] = {0};

    time_descriptor_buffer_infos[0].offset = 0;
    time_descriptor_buffer_infos[0].buffer = s_renderer_time_buffer[frame_index];
    time_descriptor_buffer_infos[0].range = VK_WHOLE_SIZE;

    VkDescriptorBufferInfo screen_descriptor_buffer_infos[1] = {0};

    screen_descriptor_buffer_infos[0].offset = 0;
    screen_descriptor_buffer_infos[0].buffer = s_renderer_screen_buffer[frame_index];
    screen_descriptor_buffer_infos[0].range = VK_WHOLE_SIZE;

    VkDescriptorBufferInfo camera_descriptor_buffer_infos[1] = {0};

    camera_descriptor_buffer_infos[0].offset = 0;
    camera_descriptor_buffer_infos[0].buffer = s_renderer_camera_buffer[frame_index];
    camera_descriptor_buffer_infos[0].range = VK_WHOLE_SIZE;

    VkDescriptorBufferInfo cluster_descriptor_buffer_infos[1] = {0};

    cluster_descriptor_buffer_infos[0].offset = 0;
    cluster_descriptor_buffer_infos[0].buffer = s_renderer_cluster_buffer[frame_index];
    cluster_descriptor_buffer_infos[0].range = VK_WHOLE_SIZE;

    VkDescriptorImageInfo *chunk_descriptor_image_infos = (VkDescriptorImageInfo *)heap_alloc(sizeof(VkDescriptorImageInfo) * s_renderer_chunk_count * s_renderer_chunk_lod_levels);

    for (int32_t z = 0; z < s_renderer_chunk_count_z; z++) {
      for (int32_t y = 0; y < s_renderer_chunk_count_y; y++) {
        for (int32_t x = 0; x < s_renderer_chunk_count_x; x++) {
          for (int32_t lod = 0; lod < s_renderer_chunk_lod_levels; lod++) {
            int32_t chunk_index = x + (y * s_renderer_chunk_count_x) + (z * s_renderer_chunk_count_x * s_renderer_chunk_count_y);
            int32_t descriptor_index = lod + (chunk_index * s_renderer_chunk_lod_levels);

            chunk_descriptor_image_infos[descriptor_index].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            chunk_descriptor_image_infos[descriptor_index].imageView = s_renderer_chunk_image_view[frame_index][chunk_index][lod];
            chunk_descriptor_image_infos[descriptor_index].sampler = s_renderer_chunk_sampler[frame_index][chunk_index][lod];
          }
        }
      }
    }

    VkWriteDescriptorSet write_descriptor_sets[5] = {0};

    write_descriptor_sets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_sets[0].pNext = 0;
    write_descriptor_sets[0].dstSet = s_renderer_chunk_renderer_descriptor_sets[frame_index];
    write_descriptor_sets[0].dstBinding = 0;
    write_descriptor_sets[0].dstArrayElement = 0;
    write_descriptor_sets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_descriptor_sets[0].descriptorCount = ARRAY_COUNT(time_descriptor_buffer_infos);
    write_descriptor_sets[0].pImageInfo = 0;
    write_descriptor_sets[0].pBufferInfo = time_descriptor_buffer_infos;
    write_descriptor_sets[0].pTexelBufferView = 0;

    write_descriptor_sets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_sets[1].pNext = 0;
    write_descriptor_sets[1].dstSet = s_renderer_chunk_renderer_descriptor_sets[frame_index];
    write_descriptor_sets[1].dstBinding = 1;
    write_descriptor_sets[1].dstArrayElement = 0;
    write_descriptor_sets[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_descriptor_sets[1].descriptorCount = ARRAY_COUNT(screen_descriptor_buffer_infos);
    write_descriptor_sets[1].pImageInfo = 0;
    write_descriptor_sets[1].pBufferInfo = screen_descriptor_buffer_infos;
    write_descriptor_sets[1].pTexelBufferView = 0;

    write_descriptor_sets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_sets[2].pNext = 0;
    write_descriptor_sets[2].dstSet = s_renderer_chunk_renderer_descriptor_sets[frame_index];
    write_descriptor_sets[2].dstBinding = 2;
    write_descriptor_sets[2].dstArrayElement = 0;
    write_descriptor_sets[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_descriptor_sets[2].descriptorCount = ARRAY_COUNT(camera_descriptor_buffer_infos);
    write_descriptor_sets[2].pImageInfo = 0;
    write_descriptor_sets[2].pBufferInfo = camera_descriptor_buffer_infos;
    write_descriptor_sets[2].pTexelBufferView = 0;

    write_descriptor_sets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_sets[3].pNext = 0;
    write_descriptor_sets[3].dstSet = s_renderer_chunk_renderer_descriptor_sets[frame_index];
    write_descriptor_sets[3].dstBinding = 3;
    write_descriptor_sets[3].dstArrayElement = 0;
    write_descriptor_sets[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_descriptor_sets[3].descriptorCount = ARRAY_COUNT(cluster_descriptor_buffer_infos);
    write_descriptor_sets[3].pImageInfo = 0;
    write_descriptor_sets[3].pBufferInfo = cluster_descriptor_buffer_infos;
    write_descriptor_sets[3].pTexelBufferView = 0;

    write_descriptor_sets[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_sets[4].pNext = 0;
    write_descriptor_sets[4].dstSet = s_renderer_chunk_renderer_descriptor_sets[frame_index];
    write_descriptor_sets[4].dstBinding = 4;
    write_descriptor_sets[4].dstArrayElement = 0;
    write_descriptor_sets[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write_descriptor_sets[4].descriptorCount = s_renderer_chunk_count * s_renderer_chunk_lod_levels;
    write_descriptor_sets[4].pImageInfo = chunk_descriptor_image_infos;
    write_descriptor_sets[4].pBufferInfo = 0;
    write_descriptor_sets[4].pTexelBufferView = 0;

    vkUpdateDescriptorSets(g_context_device, ARRAY_COUNT(write_descriptor_sets), write_descriptor_sets, 0, 0);

    heap_free(chunk_descriptor_image_infos);

    frame_index++;
  }
}
static void renderer_update_debug_line_descriptor_sets(void) {
  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    VkDescriptorBufferInfo time_descriptor_buffer_infos[1] = {0};

    time_descriptor_buffer_infos[0].offset = 0;
    time_descriptor_buffer_infos[0].buffer = s_renderer_time_buffer[frame_index];
    time_descriptor_buffer_infos[0].range = VK_WHOLE_SIZE;

    VkDescriptorBufferInfo screen_descriptor_buffer_infos[1] = {0};

    screen_descriptor_buffer_infos[0].offset = 0;
    screen_descriptor_buffer_infos[0].buffer = s_renderer_screen_buffer[frame_index];
    screen_descriptor_buffer_infos[0].range = VK_WHOLE_SIZE;

    VkDescriptorBufferInfo camera_descriptor_buffer_infos[1] = {0};

    camera_descriptor_buffer_infos[0].offset = 0;
    camera_descriptor_buffer_infos[0].buffer = s_renderer_camera_buffer[frame_index];
    camera_descriptor_buffer_infos[0].range = VK_WHOLE_SIZE;

    VkWriteDescriptorSet write_descriptor_sets[3] = {0};

    write_descriptor_sets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_sets[0].pNext = 0;
    write_descriptor_sets[0].dstSet = s_renderer_debug_line_descriptor_sets[frame_index];
    write_descriptor_sets[0].dstBinding = 0;
    write_descriptor_sets[0].dstArrayElement = 0;
    write_descriptor_sets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_descriptor_sets[0].descriptorCount = ARRAY_COUNT(time_descriptor_buffer_infos);
    write_descriptor_sets[0].pImageInfo = 0;
    write_descriptor_sets[0].pBufferInfo = time_descriptor_buffer_infos;
    write_descriptor_sets[0].pTexelBufferView = 0;

    write_descriptor_sets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_sets[1].pNext = 0;
    write_descriptor_sets[1].dstSet = s_renderer_debug_line_descriptor_sets[frame_index];
    write_descriptor_sets[1].dstBinding = 1;
    write_descriptor_sets[1].dstArrayElement = 0;
    write_descriptor_sets[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_descriptor_sets[1].descriptorCount = ARRAY_COUNT(screen_descriptor_buffer_infos);
    write_descriptor_sets[1].pImageInfo = 0;
    write_descriptor_sets[1].pBufferInfo = screen_descriptor_buffer_infos;
    write_descriptor_sets[1].pTexelBufferView = 0;

    write_descriptor_sets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_sets[2].pNext = 0;
    write_descriptor_sets[2].dstSet = s_renderer_debug_line_descriptor_sets[frame_index];
    write_descriptor_sets[2].dstBinding = 2;
    write_descriptor_sets[2].dstArrayElement = 0;
    write_descriptor_sets[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_descriptor_sets[2].descriptorCount = ARRAY_COUNT(camera_descriptor_buffer_infos);
    write_descriptor_sets[2].pImageInfo = 0;
    write_descriptor_sets[2].pBufferInfo = camera_descriptor_buffer_infos;
    write_descriptor_sets[2].pTexelBufferView = 0;

    vkUpdateDescriptorSets(g_context_device, ARRAY_COUNT(write_descriptor_sets), write_descriptor_sets, 0, 0);

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

  s_renderer_cluster[s_renderer_frame_index]->chunk_count = ivector3_xyz(s_renderer_chunk_count_x, s_renderer_chunk_count_y, s_renderer_chunk_count_z);
  s_renderer_cluster[s_renderer_frame_index]->chunk_size = g_renderer_chunk_size;
  s_renderer_cluster[s_renderer_frame_index]->max_lod_levels = s_renderer_chunk_lod_levels;
}

static void renderer_record_compute_commands(void) {
  if (s_renderer_cluster_is_dirty[s_renderer_frame_index]) {
    s_renderer_cluster_is_dirty[s_renderer_frame_index] = 0;

    {
      vkCmdBindPipeline(s_renderer_graphics_command_buffers[s_renderer_frame_index], VK_PIPELINE_BIND_POINT_COMPUTE, s_renderer_chunk_generator_pipeline);
      vkCmdBindDescriptorSets(s_renderer_graphics_command_buffers[s_renderer_frame_index], VK_PIPELINE_BIND_POINT_COMPUTE, s_renderer_chunk_generator_pipeline_layout, 0, 1, &s_renderer_chunk_generator_descriptor_sets[s_renderer_frame_index], 0, 0);

      for (int32_t z = 0; z < s_renderer_chunk_count_z; z++) {
        for (int32_t y = 0; y < s_renderer_chunk_count_y; y++) {
          for (int32_t x = 0; x < s_renderer_chunk_count_x; x++) {
            int32_t chunk_index = x + (y * s_renderer_chunk_count_x) + (z * s_renderer_chunk_count_x * s_renderer_chunk_count_y);

            renderer_chunk_generator_push_constant_t push_constant =
              {
                {x * g_renderer_chunk_size, y * g_renderer_chunk_size, z * g_renderer_chunk_size},
                0,
              };

            int32_t group_count = RENDERER_MAKE_GROUP_COUNT(g_renderer_chunk_size, 8);

            vkCmdPushConstants(s_renderer_graphics_command_buffers[s_renderer_frame_index], s_renderer_chunk_generator_pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(renderer_chunk_generator_push_constant_t), &push_constant);
            vkCmdDispatch(s_renderer_graphics_command_buffers[s_renderer_frame_index], group_count, group_count, group_count);

            VkImageMemoryBarrier image_memory_barrier = {0};
            image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
            image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
            image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            image_memory_barrier.image = s_renderer_chunk_image[s_renderer_frame_index][chunk_index];
            image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_memory_barrier.subresourceRange.baseMipLevel = 0;
            image_memory_barrier.subresourceRange.levelCount = s_renderer_chunk_lod_levels;
            image_memory_barrier.subresourceRange.baseArrayLayer = 0;
            image_memory_barrier.subresourceRange.layerCount = 1;
            image_memory_barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
            image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(
              s_renderer_graphics_command_buffers[s_renderer_frame_index],
              VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
              VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
              0, 0, 0, 0, 0, 1, &image_memory_barrier);
          }
        }
      }
    }

    {
      vkCmdBindPipeline(s_renderer_graphics_command_buffers[s_renderer_frame_index], VK_PIPELINE_BIND_POINT_COMPUTE, s_renderer_chunk_mipmap_pipeline);
      vkCmdBindDescriptorSets(s_renderer_graphics_command_buffers[s_renderer_frame_index], VK_PIPELINE_BIND_POINT_COMPUTE, s_renderer_chunk_mipmap_pipeline_layout, 0, 1, &s_renderer_chunk_mipmap_descriptor_sets[s_renderer_frame_index], 0, 0);

      for (int32_t z = 0; z < s_renderer_chunk_count_z; z++) {
        for (int32_t y = 0; y < s_renderer_chunk_count_y; y++) {
          for (int32_t x = 0; x < s_renderer_chunk_count_x; x++) {
            for (int32_t lod = 1; lod < s_renderer_chunk_lod_levels; lod++) {
              int32_t chunk_index = x + (y * s_renderer_chunk_count_x) + (z * s_renderer_chunk_count_x * s_renderer_chunk_count_y);

              renderer_chunk_mipmap_push_constant_t push_constant =
                {
                  {x * g_renderer_chunk_size, y * g_renderer_chunk_size, z * g_renderer_chunk_size},
                  lod,
                };

              int32_t group_count = RENDERER_MAKE_GROUP_COUNT(g_renderer_chunk_size / (1 << lod), 8);

              vkCmdPushConstants(s_renderer_graphics_command_buffers[s_renderer_frame_index], s_renderer_chunk_mipmap_pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(renderer_chunk_mipmap_push_constant_t), &push_constant);
              vkCmdDispatch(s_renderer_graphics_command_buffers[s_renderer_frame_index], group_count, group_count, group_count);

              VkImageMemoryBarrier image_memory_barrier = {0};
              image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
              image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
              image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
              image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
              image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
              image_memory_barrier.image = s_renderer_chunk_image[s_renderer_frame_index][chunk_index];
              image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
              image_memory_barrier.subresourceRange.baseMipLevel = 0;
              image_memory_barrier.subresourceRange.levelCount = s_renderer_chunk_lod_levels;
              image_memory_barrier.subresourceRange.baseArrayLayer = 0;
              image_memory_barrier.subresourceRange.layerCount = 1;
              image_memory_barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
              image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

              vkCmdPipelineBarrier(
                s_renderer_graphics_command_buffers[s_renderer_frame_index],
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                0, 0, 0, 0, 0, 1, &image_memory_barrier);
            }
          }
        }
      }
    }
  }

  {
    // TODO

    // int32_t group_count_x = RENDERER_MAKE_GROUP_COUNT(s_renderer_chunk_count_x, 8);
    // int32_t group_count_y = RENDERER_MAKE_GROUP_COUNT(s_renderer_chunk_count_y, 8);
    // int32_t group_count_z = RENDERER_MAKE_GROUP_COUNT(s_renderer_chunk_count_z, 8);

    // vkCmdBindPipeline(s_renderer_graphics_command_buffers[s_renderer_frame_index], VK_PIPELINE_BIND_POINT_COMPUTE, s_renderer_chunk_editor_pipeline);
    // vkCmdBindDescriptorSets(s_renderer_graphics_command_buffers[s_renderer_frame_index], VK_PIPELINE_BIND_POINT_COMPUTE, s_renderer_chunk_editor_pipeline_layout, 0, 1, &s_renderer_chunk_editor_descriptor_sets[s_renderer_frame_index], 0, 0);
    // vkCmdDispatch(s_renderer_graphics_command_buffers[s_renderer_frame_index], group_count_x, group_count_y, group_count_z);
  }
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

  vkCmdBeginRenderPass(s_renderer_graphics_command_buffers[s_renderer_frame_index], &render_pass_create_info, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport = {0};
  viewport.x = 0.0F;
  viewport.y = 0.0F;
  viewport.width = (float)g_context_surface_width;
  viewport.height = (float)g_context_surface_height;
  viewport.minDepth = 0.0F;
  viewport.maxDepth = 1.0F;

  vkCmdSetViewport(s_renderer_graphics_command_buffers[s_renderer_frame_index], 0, 1, &viewport);

  VkRect2D scissor = {0};
  scissor.offset.x = 0;
  scissor.offset.y = 0;
  scissor.extent.width = g_context_surface_width;
  scissor.extent.height = g_context_surface_height;

  vkCmdSetScissor(s_renderer_graphics_command_buffers[s_renderer_frame_index], 0, 1, &scissor);

  {
    VkBuffer vertex_buffers[] = {s_renderer_cluster_vertex_buffer[s_renderer_frame_index]};
    uint64_t vertex_offsets[] = {0, 0};

    vkCmdBindPipeline(s_renderer_graphics_command_buffers[s_renderer_frame_index], VK_PIPELINE_BIND_POINT_GRAPHICS, s_renderer_chunk_renderer_pipeline);
    vkCmdBindVertexBuffers(s_renderer_graphics_command_buffers[s_renderer_frame_index], 0, ARRAY_COUNT(vertex_buffers), vertex_buffers, vertex_offsets);
    vkCmdBindIndexBuffer(s_renderer_graphics_command_buffers[s_renderer_frame_index], s_renderer_cluster_index_buffer[s_renderer_frame_index], 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(s_renderer_graphics_command_buffers[s_renderer_frame_index], VK_PIPELINE_BIND_POINT_GRAPHICS, s_renderer_chunk_renderer_pipeline_layout, 0, 1, &s_renderer_chunk_renderer_descriptor_sets[s_renderer_frame_index], 0, 0);
    vkCmdDrawIndexed(s_renderer_graphics_command_buffers[s_renderer_frame_index], RENDERER_CLUSTER_INDEX_COUNT, 1, 0, 0, 0);
  }

  {
    if (g_renderer_enable_debug) {
      VkBuffer vertex_buffers[] = {s_renderer_debug_line_vertex_buffer[s_renderer_frame_index]};
      uint64_t vertex_offsets[] = {0};

      vkCmdBindPipeline(s_renderer_graphics_command_buffers[s_renderer_frame_index], VK_PIPELINE_BIND_POINT_GRAPHICS, s_renderer_debug_line_pipeline);
      vkCmdBindVertexBuffers(s_renderer_graphics_command_buffers[s_renderer_frame_index], 0, ARRAY_COUNT(vertex_buffers), vertex_buffers, vertex_offsets);
      vkCmdBindIndexBuffer(s_renderer_graphics_command_buffers[s_renderer_frame_index], s_renderer_debug_line_index_buffer[s_renderer_frame_index], 0, VK_INDEX_TYPE_UINT32);
      vkCmdBindDescriptorSets(s_renderer_graphics_command_buffers[s_renderer_frame_index], VK_PIPELINE_BIND_POINT_GRAPHICS, s_renderer_debug_line_pipeline_layout, 0, 1, &s_renderer_debug_line_descriptor_sets[s_renderer_frame_index], 0, 0);
      vkCmdDrawIndexed(s_renderer_graphics_command_buffers[s_renderer_frame_index], s_renderer_debug_line_index_offset[s_renderer_frame_index], 1, 0, 0, 0);

      s_renderer_debug_line_vertex_offset[s_renderer_frame_index] = 0;
      s_renderer_debug_line_index_offset[s_renderer_frame_index] = 0;
    }
  }

  vkCmdEndRenderPass(s_renderer_graphics_command_buffers[s_renderer_frame_index]);
}

static void renderer_destroy_command_buffer(void) {
  vkFreeCommandBuffers(g_context_device, g_context_command_pool, s_renderer_frames_in_flight, s_renderer_graphics_command_buffers);

  heap_free(s_renderer_graphics_command_buffers);
}
static void renderer_destroy_sync_objects(void) {
  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    vkDestroySemaphore(g_context_device, s_renderer_graphics_complete_semaphores[frame_index], 0);
    vkDestroySemaphore(g_context_device, s_renderer_present_complete_semaphores[frame_index], 0);

    vkDestroyFence(g_context_device, s_renderer_frame_fences[frame_index], 0);

    frame_index++;
  }

  heap_free(s_renderer_graphics_complete_semaphores);
  heap_free(s_renderer_present_complete_semaphores);
  heap_free(s_renderer_frame_fences);
}
static void renderer_destroy_descriptor_pools(void) {
  vkDestroyDescriptorPool(g_context_device, s_renderer_chunk_editor_descriptor_pool, 0);
  vkDestroyDescriptorPool(g_context_device, s_renderer_chunk_generator_descriptor_pool, 0);
  vkDestroyDescriptorPool(g_context_device, s_renderer_chunk_mipmap_descriptor_pool, 0);
  vkDestroyDescriptorPool(g_context_device, s_renderer_chunk_renderer_descriptor_pool, 0);
  vkDestroyDescriptorPool(g_context_device, s_renderer_debug_line_descriptor_pool, 0);
}
static void renderer_destroy_descriptor_set_layouts(void) {
  vkDestroyDescriptorSetLayout(g_context_device, s_renderer_chunk_editor_descriptor_set_layout, 0);
  vkDestroyDescriptorSetLayout(g_context_device, s_renderer_chunk_generator_descriptor_set_layout, 0);
  vkDestroyDescriptorSetLayout(g_context_device, s_renderer_chunk_mipmap_descriptor_set_layout, 0);
  vkDestroyDescriptorSetLayout(g_context_device, s_renderer_chunk_renderer_descriptor_set_layout, 0);
  vkDestroyDescriptorSetLayout(g_context_device, s_renderer_debug_line_descriptor_set_layout, 0);
}
static void renderer_destroy_descriptor_sets(void) {
  heap_free(s_renderer_chunk_editor_descriptor_sets);
  heap_free(s_renderer_chunk_generator_descriptor_sets);
  heap_free(s_renderer_chunk_mipmap_descriptor_sets);
  heap_free(s_renderer_chunk_renderer_descriptor_sets);
  heap_free(s_renderer_debug_line_descriptor_sets);
}
static void renderer_destroy_pipeline_layouts(void) {
  vkDestroyPipelineLayout(g_context_device, s_renderer_chunk_editor_pipeline_layout, 0);
  vkDestroyPipelineLayout(g_context_device, s_renderer_chunk_generator_pipeline_layout, 0);
  vkDestroyPipelineLayout(g_context_device, s_renderer_chunk_mipmap_pipeline_layout, 0);
  vkDestroyPipelineLayout(g_context_device, s_renderer_chunk_renderer_pipeline_layout, 0);
  vkDestroyPipelineLayout(g_context_device, s_renderer_debug_line_pipeline_layout, 0);
}
static void renderer_destroy_pipelines(void) {
  vkDestroyPipeline(g_context_device, s_renderer_chunk_editor_pipeline, 0);
  vkDestroyPipeline(g_context_device, s_renderer_chunk_generator_pipeline, 0);
  vkDestroyPipeline(g_context_device, s_renderer_chunk_mipmap_pipeline, 0);
  vkDestroyPipeline(g_context_device, s_renderer_chunk_renderer_pipeline, 0);
  vkDestroyPipeline(g_context_device, s_renderer_debug_line_pipeline, 0);
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
static void renderer_destroy_cluster_buffer(void) {
  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    vkUnmapMemory(g_context_device, s_renderer_cluster_buffer_device_memory[frame_index]);
    vkFreeMemory(g_context_device, s_renderer_cluster_buffer_device_memory[frame_index], 0);
    vkDestroyBuffer(g_context_device, s_renderer_cluster_buffer[frame_index], 0);

    frame_index++;
  }

  heap_free(s_renderer_cluster_buffer);
  heap_free(s_renderer_cluster_buffer_device_memory);
  heap_free(s_renderer_cluster);
}
static void renderer_destroy_cluster_vertex_buffer(void) {
  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    vkUnmapMemory(g_context_device, s_renderer_cluster_vertex_buffer_device_memory[frame_index]);
    vkFreeMemory(g_context_device, s_renderer_cluster_vertex_buffer_device_memory[frame_index], 0);
    vkDestroyBuffer(g_context_device, s_renderer_cluster_vertex_buffer[frame_index], 0);

    frame_index++;
  }

  heap_free(s_renderer_cluster_vertex_buffer);
  heap_free(s_renderer_cluster_vertex_buffer_device_memory);
  heap_free(s_renderer_cluster_vertex);
}
static void renderer_destroy_cluster_index_buffer(void) {
  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    vkUnmapMemory(g_context_device, s_renderer_cluster_index_buffer_device_memory[frame_index]);
    vkFreeMemory(g_context_device, s_renderer_cluster_index_buffer_device_memory[frame_index], 0);
    vkDestroyBuffer(g_context_device, s_renderer_cluster_index_buffer[frame_index], 0);

    frame_index++;
  }

  heap_free(s_renderer_cluster_index_buffer);
  heap_free(s_renderer_cluster_index_buffer_device_memory);
  heap_free(s_renderer_cluster_index);
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

static void renderer_destroy_chunk_images(void) {
  int32_t frame_index = 0;
  while (frame_index < s_renderer_frames_in_flight) {
    int32_t chunk_index = 0;
    while (chunk_index < s_renderer_chunk_count) {
      int32_t lod = 0;
      while (lod < s_renderer_chunk_lod_levels) {
        vkDestroySampler(g_context_device, s_renderer_chunk_sampler[frame_index][chunk_index][lod], 0);
        vkDestroyImageView(g_context_device, s_renderer_chunk_image_view[frame_index][chunk_index][lod], 0);

        lod++;
      }

      vkFreeMemory(g_context_device, s_renderer_chunk_image_device_memory[frame_index][chunk_index], 0);
      vkDestroyImage(g_context_device, s_renderer_chunk_image[frame_index][chunk_index], 0);

      heap_free(s_renderer_chunk_image_view[frame_index][chunk_index]);
      heap_free(s_renderer_chunk_sampler[frame_index][chunk_index]);

      chunk_index++;
    }

    heap_free(s_renderer_chunk_image[frame_index]);
    heap_free(s_renderer_chunk_image_device_memory[frame_index]);
    heap_free(s_renderer_chunk_image_view[frame_index]);
    heap_free(s_renderer_chunk_sampler[frame_index]);

    frame_index++;
  }

  heap_free(s_renderer_chunk_image);
  heap_free(s_renderer_chunk_image_device_memory);
  heap_free(s_renderer_chunk_image_view);
  heap_free(s_renderer_chunk_sampler);
}

static LRESULT WINAPI renderer_cluster_worker_thread(PVOID user_param) {
  VkCommandPool command_pool = 0;
  VkCommandBuffer command_buffer = 0;
  VkFence fence = 0;

  VkCommandPoolCreateInfo command_pool_create_info = {0};
  command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  command_pool_create_info.queueFamilyIndex = g_context_graphics_queue_index;

  VULKAN_CHECK(vkCreateCommandPool(g_context_device, &command_pool_create_info, 0, &command_pool));

  VkCommandBufferAllocateInfo command_buffer_allocate_info = {0};
  command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_allocate_info.commandPool = command_pool;
  command_buffer_allocate_info.commandBufferCount = 1;

  VULKAN_CHECK(vkAllocateCommandBuffers(g_context_device, &command_buffer_allocate_info, &command_buffer));

  VkFenceCreateInfo fence_create_info = {0};
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  VULKAN_CHECK(vkCreateFence(g_context_device, &fence_create_info, 0, &fence));

  while (s_renderer_cluster_worker_should_stop == 0) {
    VULKAN_CHECK(vkWaitForFences(g_context_device, 1, &fence, 1, UINT64_MAX));

    VULKAN_CHECK(vkResetFences(g_context_device, 1, &fence));
    VULKAN_CHECK(vkResetCommandBuffer(command_buffer, 0));

    VkCommandBufferBeginInfo command_buffer_begin_info = {0};
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    command_buffer_begin_info.pInheritanceInfo = 0;

    VULKAN_CHECK(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));
    VULKAN_CHECK(vkEndCommandBuffer(command_buffer));

    VkSubmitInfo graphics_submit_info = {0};
    graphics_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    graphics_submit_info.commandBufferCount = 1;
    graphics_submit_info.pCommandBuffers = &command_buffer;

    WaitForSingleObject(s_renderer_cluster_worker_mutex, INFINITE); // TODO

    VkResult graphics_submit_result = vkQueueSubmit(g_context_graphics_queue, 1, &graphics_submit_info, fence);

    if (graphics_submit_result != VK_SUCCESS) {
      break;
    }

    ReleaseMutex(s_renderer_cluster_worker_mutex); // TODO
  }

  vkDestroyFence(g_context_device, fence, 0);

  vkDestroyCommandPool(g_context_device, command_pool, 0);

  return 0;
}

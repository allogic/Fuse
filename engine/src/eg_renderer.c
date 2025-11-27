#include <engine/eg_pch.h>
#include <engine/eg_buffer.h>
#include <engine/eg_context.h>
#include <engine/eg_pipeline.h>
#include <engine/eg_renderer.h>
#include <engine/eg_swapchain.h>
#include <engine/eg_profiler.h>

#define EG_RENDERER_DEBUG_LINE_VERTEX_COUNT (1048576LL)
#define EG_RENDERER_DEBUG_LINE_INDEX_COUNT (1048576LL)

#define EG_RENDERER_MAKE_GROUP_COUNT(GLOBAL_SIZE, LOCAL_SIZE) ((int32_t)ceil((double)(GLOBAL_SIZE) / (LOCAL_SIZE)))

// TODO: implement sparse textures..
// TODO: implement viewport defragmentation..

struct eg_renderer_t {
  eg_context_t *context;
  eg_swapchain_t *swapchain;
  lb_renderer_asset_t asset;
  uint8_t is_dirty;
  uint8_t is_debug_enabled;
  uint32_t frames_in_flight;
  uint32_t frame_index;
  uint32_t image_index;
  VkCommandBuffer *command_buffer;
  lb_pipeline_type_t pipeline_type[0xFF];
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
  lb_map_t **descriptor_binding_buffers_per_frame;
  eg_debug_vertex_t **debug_line_vertices;
  uint32_t **debug_line_indices;
  eg_buffer_t **debug_line_vertex_buffers;
  eg_buffer_t **debug_line_index_buffers;
  uint32_t *debug_line_vertex_offset;
  uint32_t *debug_line_index_offset;
  VkRenderPass gbuffer_render_pass;
};

static void eg_renderer_link_pipeline(eg_renderer_t *renderer, uint32_t link_index, lb_pipeline_type_t pipeline_type, void *pipeline);

static void eg_renderer_create_global_buffers(eg_renderer_t *renderer);
static void eg_renderer_create_debug_buffers(eg_renderer_t *renderer);
static void eg_renderer_create_gbuffer_render_pass(eg_renderer_t *renderer);
static void eg_renderer_create_pipelines(eg_renderer_t *renderer);
static void eg_renderer_create_sync_objects(eg_renderer_t *renderer);
static void eg_renderer_create_command_buffer(eg_renderer_t *renderer);

static void eg_renderer_update_uniform_buffers(eg_renderer_t *renderer);

static void eg_renderer_transition_viewport_images_to_render_target(eg_renderer_t *renderer);
static void eg_renderer_transition_viewport_images_to_shader_read(eg_renderer_t *renderer);

static void eg_renderer_record_editor_commands(eg_renderer_t *renderer);
static void eg_renderer_record_compute_commands(eg_renderer_t *renderer);
static void eg_renderer_record_graphic_commands(eg_renderer_t *renderer);

static void eg_renderer_gbuffer_pass(eg_renderer_t *renderer);
static void eg_renderer_editor_pass(eg_renderer_t *renderer);

static void eg_renderer_destroy_global_buffers(eg_renderer_t *renderer);
static void eg_renderer_destroy_debug_buffers(eg_renderer_t *renderer);
static void eg_renderer_destroy_pipelines(eg_renderer_t *renderer);
static void eg_renderer_destroy_sync_objects(eg_renderer_t *renderer);
static void eg_renderer_destroy_command_buffer(eg_renderer_t *renderer);
static void eg_renderer_destroy_gbuffer_render_pass(eg_renderer_t *renderer);

void eg_renderer_create(eg_context_t *context, lb_renderer_asset_id_t renderer_asset_id) {
  eg_renderer_t *renderer = (eg_renderer_t *)lb_heap_alloc(sizeof(eg_renderer_t), 1, 0);

  eg_context_set_renderer(context, renderer);

  renderer->context = context;
  renderer->swapchain = eg_context_swapchain(renderer->context);

  uint32_t image_count = eg_swapchain_image_count(renderer->swapchain);

  renderer->asset = lb_database_load_renderer_asset_by_id(renderer_asset_id);
  renderer->is_debug_enabled = 1; // TODO
  renderer->frames_in_flight = renderer->asset.frames_in_flight;
  renderer->frames_in_flight = LB_MAX(renderer->frames_in_flight, 1);
  renderer->frames_in_flight = LB_MIN(renderer->frames_in_flight, image_count);

  strcpy(renderer->time_info_descriptor_binding_name, "time_info");
  strcpy(renderer->screen_info_descriptor_binding_name, "screen_info");
  strcpy(renderer->camera_info_descriptor_binding_name, "camera_info");

  memset(renderer->pipeline_type, 0, sizeof(renderer->pipeline_type));
  memset(renderer->pipeline_link, 0, sizeof(renderer->pipeline_link));

  eg_renderer_create_global_buffers(renderer);
  eg_renderer_create_debug_buffers(renderer);
  eg_renderer_create_gbuffer_render_pass(renderer);
  eg_renderer_create_pipelines(renderer);
  eg_renderer_create_sync_objects(renderer);
  eg_renderer_create_command_buffer(renderer);

  uint8_t is_editor_mode = eg_context_is_editor_mode(renderer->context);

  if (is_editor_mode) {
    g_context_editor_create_proc(renderer->context);
  }
}
void eg_renderer_update(eg_renderer_t *renderer) {
  lb_vector3_t right_position = {0.0F, 0.0F, 0.0F};
  lb_vector3_t right_direction = {1.0F, 0.0F, 0.0F};
  lb_vector4_t right_color = {1.0F, 0.0F, 0.0F, 1.0F};

  eg_renderer_draw_debug_line(renderer, right_position, right_direction, right_color);

  lb_vector3_t up_position = {0.0F, 0.0F, 0.0F};
  lb_vector3_t up_direction = {0.0F, 1.0F, 0.0F};
  lb_vector4_t up_color = {0.0F, 1.0F, 0.0F, 1.0F};

  eg_renderer_draw_debug_line(renderer, up_position, up_direction, up_color);

  lb_vector3_t front_position = {0.0F, 0.0F, 0.0F};
  lb_vector3_t front_direction = {0.0F, 0.0F, 1.0F};
  lb_vector4_t front_color = {0.0F, 0.0F, 1.0F, 1.0F};

  eg_renderer_draw_debug_line(renderer, front_position, front_direction, front_color);
}
void eg_renderer_draw(eg_renderer_t *renderer) {
  VkDevice device = eg_context_device(renderer->context);

  VkQueue primary_queue = eg_context_primary_queue(renderer->context);
  VkQueue present_queue = eg_context_present_queue(renderer->context);
  VkSwapchainKHR swapchain_handle = eg_swapchain_handle(renderer->swapchain);

  VkResult result = VK_SUCCESS;

  result = vkWaitForFences(device, 1, &renderer->frame_fence[renderer->frame_index], 1, UINT64_MAX);

  switch (result) {
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

  result = vkResetFences(device, 1, &renderer->frame_fence[renderer->frame_index]);

  switch (result) {
    case VK_SUCCESS: {
      break;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  result = vkResetCommandBuffer(renderer->command_buffer[renderer->frame_index], 0);

  switch (result) {
    case VK_SUCCESS: {
      break;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  result = vkAcquireNextImageKHR(device, swapchain_handle, UINT64_MAX, renderer->image_available_semaphore[renderer->frame_index], 0, &renderer->image_index);

  switch (result) {
    case VK_SUCCESS: {
      break;
    }
    case VK_NOT_READY: {
      return;
    }
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR: {

      eg_swapchain_set_dirty(renderer->swapchain, 1);

      return;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  eg_renderer_update_uniform_buffers(renderer);

  VkCommandBufferBeginInfo command_buffer_begin_info = {0};
  command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  command_buffer_begin_info.pInheritanceInfo = 0;

  EG_VULKAN_CHECK(vkBeginCommandBuffer(renderer->command_buffer[renderer->frame_index], &command_buffer_begin_info));

  eg_renderer_record_editor_commands(renderer);
  eg_renderer_record_compute_commands(renderer);
  eg_renderer_record_graphic_commands(renderer);

  EG_VULKAN_CHECK(vkEndCommandBuffer(renderer->command_buffer[renderer->frame_index]));

  // TODO: re-validate VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
  VkPipelineStageFlags graphic_wait_stages[] = {VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  VkSubmitInfo graphic_submit_info = {0};
  graphic_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  graphic_submit_info.pWaitSemaphores = &renderer->image_available_semaphore[renderer->frame_index];
  graphic_submit_info.waitSemaphoreCount = 1;
  graphic_submit_info.pSignalSemaphores = &renderer->render_finished_semaphore[renderer->image_index];
  graphic_submit_info.signalSemaphoreCount = 1;
  graphic_submit_info.pCommandBuffers = &renderer->command_buffer[renderer->frame_index];
  graphic_submit_info.commandBufferCount = 1;
  graphic_submit_info.pWaitDstStageMask = graphic_wait_stages;

  result = vkQueueSubmit(primary_queue, 1, &graphic_submit_info, renderer->frame_fence[renderer->frame_index]);

  switch (result) {
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
  present_info.pWaitSemaphores = &renderer->render_finished_semaphore[renderer->image_index];
  present_info.waitSemaphoreCount = 1;
  present_info.pSwapchains = &swapchain_handle;
  present_info.swapchainCount = 1;
  present_info.pImageIndices = &renderer->image_index;

  result = vkQueuePresentKHR(present_queue, &present_info);

  switch (result) {
    case VK_SUCCESS: {
      break;
    }
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR: {

      eg_swapchain_set_dirty(renderer->swapchain, 1);

      return;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  renderer->frame_index = (renderer->frame_index + 1) % renderer->frames_in_flight;
}
void eg_renderer_destroy(eg_renderer_t *renderer) {
  VkQueue primary_queue = eg_context_primary_queue(renderer->context);
  VkQueue present_queue = eg_context_present_queue(renderer->context);

  uint8_t is_editor_mode = eg_context_is_editor_mode(renderer->context);

  EG_VULKAN_CHECK(vkQueueWaitIdle(primary_queue));
  EG_VULKAN_CHECK(vkQueueWaitIdle(present_queue));

  if (is_editor_mode) {
    g_context_editor_destroy_proc(renderer->context);
  }

  eg_renderer_destroy_command_buffer(renderer);
  eg_renderer_destroy_sync_objects(renderer);
  eg_renderer_destroy_pipelines(renderer);
  eg_renderer_destroy_gbuffer_render_pass(renderer);
  eg_renderer_destroy_debug_buffers(renderer);
  eg_renderer_destroy_global_buffers(renderer);

  lb_database_destroy_renderer_asset(&renderer->asset);

  eg_context_set_renderer(renderer->context, 0);

  lb_heap_free(renderer);
}

void eg_renderer_draw_debug_line(eg_renderer_t *renderer, lb_vector3_t from, lb_vector3_t to, lb_vector4_t color) {
  if (renderer->is_debug_enabled) {
    eg_graphic_pipeline_t *debug_pipeline = (eg_graphic_pipeline_t *)renderer->pipeline_link[EG_RENDERER_PIPELINE_LINK_TYPE_DEBUG];

    if (debug_pipeline) {
      uint32_t vertex_offset = renderer->debug_line_vertex_offset[renderer->frame_index];
      uint32_t index_offset = renderer->debug_line_index_offset[renderer->frame_index];

      eg_debug_vertex_t *vertices = renderer->debug_line_vertices[renderer->frame_index];
      uint32_t *indices = renderer->debug_line_indices[renderer->frame_index];

      vertices[vertex_offset + 0].position = from;
      vertices[vertex_offset + 1].position = to;

      vertices[vertex_offset + 0].color = color;
      vertices[vertex_offset + 1].color = color;

      indices[index_offset + 0] = (uint32_t)(vertex_offset + 0);
      indices[index_offset + 1] = (uint32_t)(vertex_offset + 1);

      renderer->debug_line_vertex_offset[renderer->frame_index] += 2;
      renderer->debug_line_index_offset[renderer->frame_index] += 2;
    }
  }
}
void eg_renderer_draw_debug_box(eg_renderer_t *renderer, lb_vector3_t position, lb_vector3_t size, lb_vector4_t color) {
  if (renderer->is_debug_enabled) {
    eg_graphic_pipeline_t *debug_pipeline = (eg_graphic_pipeline_t *)renderer->pipeline_link[EG_RENDERER_PIPELINE_LINK_TYPE_DEBUG];

    if (debug_pipeline) {
      uint32_t vertex_offset = renderer->debug_line_vertex_offset[renderer->frame_index];
      uint32_t index_offset = renderer->debug_line_index_offset[renderer->frame_index];

      eg_debug_vertex_t *vertices = renderer->debug_line_vertices[renderer->frame_index];
      uint32_t *indices = renderer->debug_line_indices[renderer->frame_index];

      vertices[vertex_offset + 0].position = (lb_vector3_t){position.x, position.y, position.z};
      vertices[vertex_offset + 1].position = (lb_vector3_t){position.x, position.y + size.y, position.z};
      vertices[vertex_offset + 2].position = (lb_vector3_t){position.x + size.x, position.y, position.z};
      vertices[vertex_offset + 3].position = (lb_vector3_t){position.x + size.x, position.y + size.y, position.z};
      vertices[vertex_offset + 4].position = (lb_vector3_t){position.x, position.y, position.z + size.z};
      vertices[vertex_offset + 5].position = (lb_vector3_t){position.x, position.y + size.y, position.z + size.z};
      vertices[vertex_offset + 6].position = (lb_vector3_t){position.x + size.x, position.y, position.z + size.z};
      vertices[vertex_offset + 7].position = (lb_vector3_t){position.x + size.x, position.y + size.y, position.z + size.z};

      vertices[vertex_offset + 0].color = color;
      vertices[vertex_offset + 1].color = color;
      vertices[vertex_offset + 2].color = color;
      vertices[vertex_offset + 3].color = color;
      vertices[vertex_offset + 4].color = color;
      vertices[vertex_offset + 5].color = color;
      vertices[vertex_offset + 6].color = color;
      vertices[vertex_offset + 7].color = color;

      indices[index_offset + 0] = (uint32_t)(vertex_offset + 0);
      indices[index_offset + 1] = (uint32_t)(vertex_offset + 1);
      indices[index_offset + 2] = (uint32_t)(vertex_offset + 1);
      indices[index_offset + 3] = (uint32_t)(vertex_offset + 3);
      indices[index_offset + 4] = (uint32_t)(vertex_offset + 3);
      indices[index_offset + 5] = (uint32_t)(vertex_offset + 2);
      indices[index_offset + 6] = (uint32_t)(vertex_offset + 2);
      indices[index_offset + 7] = (uint32_t)(vertex_offset + 0);
      indices[index_offset + 8] = (uint32_t)(vertex_offset + 4);
      indices[index_offset + 9] = (uint32_t)(vertex_offset + 5);
      indices[index_offset + 10] = (uint32_t)(vertex_offset + 5);
      indices[index_offset + 11] = (uint32_t)(vertex_offset + 7);
      indices[index_offset + 12] = (uint32_t)(vertex_offset + 7);
      indices[index_offset + 13] = (uint32_t)(vertex_offset + 6);
      indices[index_offset + 14] = (uint32_t)(vertex_offset + 6);
      indices[index_offset + 15] = (uint32_t)(vertex_offset + 4);
      indices[index_offset + 16] = (uint32_t)(vertex_offset + 0);
      indices[index_offset + 17] = (uint32_t)(vertex_offset + 4);
      indices[index_offset + 18] = (uint32_t)(vertex_offset + 1);
      indices[index_offset + 19] = (uint32_t)(vertex_offset + 5);
      indices[index_offset + 20] = (uint32_t)(vertex_offset + 2);
      indices[index_offset + 21] = (uint32_t)(vertex_offset + 6);
      indices[index_offset + 22] = (uint32_t)(vertex_offset + 3);
      indices[index_offset + 23] = (uint32_t)(vertex_offset + 7);

      renderer->debug_line_vertex_offset[renderer->frame_index] += 8;
      renderer->debug_line_index_offset[renderer->frame_index] += 24;
    }
  }
}

uint8_t eg_renderer_is_dirty(eg_renderer_t *renderer) {
  return renderer->is_dirty;
}
uint32_t eg_renderer_frames_in_flight(eg_renderer_t *renderer) {
  return renderer->frames_in_flight;
}
uint32_t eg_renderer_frame_index(eg_renderer_t *renderer) {
  return renderer->frame_index;
}
uint32_t eg_renderer_image_index(eg_renderer_t *renderer) {
  return renderer->image_index;
}
VkRenderPass eg_renderer_gbuffer_render_pass(eg_renderer_t *renderer) {
  return renderer->gbuffer_render_pass;
}
VkCommandBuffer *eg_renderer_command_buffer(eg_renderer_t *renderer) {
  return renderer->command_buffer;
}

void eg_renderer_set_dirty(eg_renderer_t *renderer, uint8_t is_dirty) {
  renderer->is_dirty = is_dirty;
}

static void eg_renderer_link_pipeline(eg_renderer_t *renderer, uint32_t link_index, lb_pipeline_type_t pipeline_type, void *pipeline) {
  if (renderer->pipeline_link[link_index]) {

    switch (renderer->pipeline_type[link_index]) {
      case 0: {

        eg_graphic_pipeline_destroy(renderer->pipeline_link[link_index]);

        break;
      }
      case 1: {

        eg_compute_pipeline_destroy(renderer->pipeline_link[link_index]);

        break;
      }
    }
  }

  renderer->pipeline_type[link_index] = pipeline_type;
  renderer->pipeline_link[link_index] = pipeline;
}

static void eg_renderer_create_global_buffers(eg_renderer_t *renderer) {
  renderer->time_infos = (eg_time_info_t **)lb_heap_alloc(sizeof(eg_time_info_t *) * renderer->frames_in_flight, 0, 0);
  renderer->screen_infos = (eg_screen_info_t **)lb_heap_alloc(sizeof(eg_screen_info_t *) * renderer->frames_in_flight, 0, 0);
  renderer->camera_infos = (eg_camera_info_t **)lb_heap_alloc(sizeof(eg_camera_info_t *) * renderer->frames_in_flight, 0, 0);

  renderer->descriptor_binding_buffers_per_frame = (lb_map_t **)lb_heap_alloc(sizeof(lb_map_t *) * renderer->frames_in_flight, 0, 0);

  uint64_t frame_index = 0;
  uint64_t frame_count = renderer->frames_in_flight;

  while (frame_index < frame_count) {

    renderer->descriptor_binding_buffers_per_frame[frame_index] = lb_map_create();

    eg_buffer_t *time_info_descriptor_binding_buffer = eg_buffer_create_uniform_coherent(renderer->context, sizeof(eg_time_info_t));
    eg_buffer_t *screen_info_descriptor_binding_buffer = eg_buffer_create_uniform_coherent(renderer->context, sizeof(eg_screen_info_t));
    eg_buffer_t *camera_info_descriptor_binding_buffer = eg_buffer_create_uniform_coherent(renderer->context, sizeof(eg_camera_info_t));

    renderer->time_infos[frame_index] = eg_buffer_mapped_memory(time_info_descriptor_binding_buffer);
    renderer->screen_infos[frame_index] = eg_buffer_mapped_memory(screen_info_descriptor_binding_buffer);
    renderer->camera_infos[frame_index] = eg_buffer_mapped_memory(camera_info_descriptor_binding_buffer);

    lb_map_insert(renderer->descriptor_binding_buffers_per_frame[frame_index], renderer->time_info_descriptor_binding_name, strlen(renderer->time_info_descriptor_binding_name) + 1, &time_info_descriptor_binding_buffer, sizeof(eg_buffer_t *));
    lb_map_insert(renderer->descriptor_binding_buffers_per_frame[frame_index], renderer->screen_info_descriptor_binding_name, strlen(renderer->screen_info_descriptor_binding_name) + 1, &screen_info_descriptor_binding_buffer, sizeof(eg_buffer_t *));
    lb_map_insert(renderer->descriptor_binding_buffers_per_frame[frame_index], renderer->camera_info_descriptor_binding_name, strlen(renderer->camera_info_descriptor_binding_name) + 1, &camera_info_descriptor_binding_buffer, sizeof(eg_buffer_t *));

    frame_index++;
  }
}
static void eg_renderer_create_debug_buffers(eg_renderer_t *renderer) {
  renderer->debug_line_vertex_offset = (uint32_t *)lb_heap_alloc(sizeof(uint32_t) * renderer->frames_in_flight, 1, 0);
  renderer->debug_line_index_offset = (uint32_t *)lb_heap_alloc(sizeof(uint32_t) * renderer->frames_in_flight, 1, 0);

  renderer->debug_line_vertices = (eg_debug_vertex_t **)lb_heap_alloc(sizeof(eg_debug_vertex_t *) * renderer->frames_in_flight, 0, 0);
  renderer->debug_line_indices = (uint32_t **)lb_heap_alloc(sizeof(uint32_t *) * renderer->frames_in_flight, 0, 0);

  renderer->debug_line_vertex_buffers = (eg_buffer_t **)lb_heap_alloc(sizeof(eg_buffer_t *) * renderer->frames_in_flight, 0, 0);
  renderer->debug_line_index_buffers = (eg_buffer_t **)lb_heap_alloc(sizeof(eg_buffer_t *) * renderer->frames_in_flight, 0, 0);

  uint64_t frame_index = 0;
  uint64_t frame_count = renderer->frames_in_flight;

  while (frame_index < frame_count) {

    renderer->debug_line_vertex_buffers[frame_index] = eg_buffer_create_vertex_coherent(renderer->context, sizeof(eg_debug_vertex_t) * EG_RENDERER_DEBUG_LINE_VERTEX_COUNT);
    renderer->debug_line_index_buffers[frame_index] = eg_buffer_create_index_coherent(renderer->context, sizeof(uint32_t) * EG_RENDERER_DEBUG_LINE_INDEX_COUNT);

    renderer->debug_line_vertices[frame_index] = eg_buffer_mapped_memory(renderer->debug_line_vertex_buffers[frame_index]);
    renderer->debug_line_indices[frame_index] = eg_buffer_mapped_memory(renderer->debug_line_index_buffers[frame_index]);

    frame_index++;
  }
}
static void eg_renderer_create_gbuffer_render_pass(eg_renderer_t *renderer) {
  VkDevice device = eg_context_device(renderer->context);
  VkFormat surface_image_color_format = eg_context_surface_image_color_format(renderer->context);
  VkFormat surface_image_depth_format = eg_context_surface_image_depth_format(renderer->context);

  VkAttachmentDescription color_attachment_description = {0};
  color_attachment_description.format = surface_image_color_format;
  color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // TODO: change this to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR when standalone..

  VkAttachmentDescription depth_attachment_description = {0};
  depth_attachment_description.format = surface_image_depth_format;
  depth_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
  depth_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  depth_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth_attachment_description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference color_attachment_reference = {0};
  color_attachment_reference.attachment = 0;
  color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depth_attachment_reference = {0};
  depth_attachment_reference.attachment = 1;
  depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass_description = {0};
  subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass_description.colorAttachmentCount = 1;
  subpass_description.pColorAttachments = &color_attachment_reference;
  subpass_description.pDepthStencilAttachment = &depth_attachment_reference;

  VkSubpassDependency subpass_dependency = {0};
  subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  subpass_dependency.dstSubpass = 0;
  subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  subpass_dependency.srcAccessMask = VK_ACCESS_NONE;
  subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  VkAttachmentDescription attachment_descriptions[] = {color_attachment_description, depth_attachment_description};

  VkRenderPassCreateInfo render_pass_create_info = {0};
  render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_create_info.pAttachments = attachment_descriptions;
  render_pass_create_info.attachmentCount = LB_ARRAY_COUNT(attachment_descriptions);
  render_pass_create_info.pSubpasses = &subpass_description;
  render_pass_create_info.subpassCount = 1;
  render_pass_create_info.pDependencies = &subpass_dependency;
  render_pass_create_info.dependencyCount = 1;

  EG_VULKAN_CHECK(vkCreateRenderPass(device, &render_pass_create_info, 0, &renderer->gbuffer_render_pass));
}
static void eg_renderer_create_pipelines(eg_renderer_t *renderer) {
  lb_vector_t *graphic_pipeline_assets = lb_database_load_all_pipeline_assets_by_type(LB_PIPELINE_TYPE_GRAPHIC);

  uint64_t graphic_pipeline_asset_index = 0;
  uint64_t graphic_pipeline_asset_count = lb_vector_count(graphic_pipeline_assets);

  while (graphic_pipeline_asset_index < graphic_pipeline_asset_count) {

    lb_pipeline_asset_t *graphic_pipeline_asset = (lb_pipeline_asset_t *)lb_vector_at(graphic_pipeline_assets, graphic_pipeline_asset_index);

    if (graphic_pipeline_asset->auto_create_pipeline) {

      eg_graphic_pipeline_t *graphic_pipeline = eg_graphic_pipeline_create(renderer->context, graphic_pipeline_asset->id);

      uint64_t frame_index = 0;
      uint64_t frame_count = renderer->frames_in_flight;

      while (frame_index < frame_count) {

        // TODO: remove this!
        eg_graphic_pipeline_link_vertex_input_binding_buffer(graphic_pipeline, frame_index, 0, eg_buffer_handle(renderer->debug_line_vertex_buffers[frame_index]), 0);
        eg_graphic_pipeline_link_index_buffer(graphic_pipeline, frame_index, eg_buffer_handle(renderer->debug_line_index_buffers[frame_index]));

        if (graphic_pipeline_asset->auto_link_descriptor_bindings) {
          eg_graphic_pipeline_set_auto_link_descriptor_bindings(graphic_pipeline, renderer->descriptor_binding_buffers_per_frame);
        }

        frame_index++;
      }

      eg_graphic_pipeline_allocate_descriptor_sets(graphic_pipeline, 1);
      eg_graphic_pipeline_update_descriptor_sets(graphic_pipeline);

      eg_renderer_link_pipeline(renderer, graphic_pipeline_asset->link_index, graphic_pipeline_asset->type, graphic_pipeline);
    }

    graphic_pipeline_asset_index++;
  }

  lb_database_destroy_pipeline_assets(graphic_pipeline_assets);

  lb_vector_t *compute_pipeline_assets = lb_database_load_all_pipeline_assets_by_type(LB_PIPELINE_TYPE_COMPUTE);

  uint64_t compute_pipeline_asset_index = 0;
  uint64_t compute_pipeline_asset_count = lb_vector_count(compute_pipeline_assets);

  while (compute_pipeline_asset_index < compute_pipeline_asset_count) {

    lb_pipeline_asset_t *pipeline_asset = (lb_pipeline_asset_t *)lb_vector_at(compute_pipeline_assets, compute_pipeline_asset_index);

    if (pipeline_asset->auto_create_pipeline) {

      eg_compute_pipeline_t *pipeline = eg_compute_pipeline_create(renderer->context, pipeline_asset->id);

      uint64_t frame_index = 0;
      uint64_t frame_count = renderer->frames_in_flight;

      while (frame_index < frame_count) {

        if (pipeline_asset->auto_link_descriptor_bindings) {
          eg_compute_pipeline_set_auto_link_descriptor_bindings(pipeline, renderer->descriptor_binding_buffers_per_frame);
        }

        frame_index++;
      }

      eg_compute_pipeline_allocate_descriptor_sets(pipeline, 1);
      eg_compute_pipeline_update_descriptor_sets(pipeline);

      eg_renderer_link_pipeline(renderer, pipeline_asset->link_index, pipeline_asset->type, pipeline);
    }

    compute_pipeline_asset_index++;
  }

  lb_database_destroy_pipeline_assets(compute_pipeline_assets);
}
static void eg_renderer_create_sync_objects(eg_renderer_t *renderer) {
  VkDevice device = eg_context_device(renderer->context);
  uint32_t image_count = eg_swapchain_image_count(renderer->swapchain);

  renderer->render_finished_semaphore = (VkSemaphore *)lb_heap_alloc(sizeof(VkSemaphore) * image_count, 0, 0);
  renderer->image_available_semaphore = (VkSemaphore *)lb_heap_alloc(sizeof(VkSemaphore) * renderer->frames_in_flight, 0, 0);
  renderer->frame_fence = (VkFence *)lb_heap_alloc(sizeof(VkFence) * renderer->frames_in_flight, 0, 0);

  VkSemaphoreCreateInfo semaphore_create_info = {0};
  semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphore_create_info.flags = 0;

  VkFenceCreateInfo fence_create_info = {0};
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  uint64_t image_index = 0;

  while (image_index < image_count) {

    EG_VULKAN_CHECK(vkCreateSemaphore(device, &semaphore_create_info, 0, &renderer->render_finished_semaphore[image_index]));

    image_index++;
  }

  uint64_t frame_index = 0;
  uint64_t frame_count = renderer->frames_in_flight;

  while (frame_index < frame_count) {

    EG_VULKAN_CHECK(vkCreateSemaphore(device, &semaphore_create_info, 0, &renderer->image_available_semaphore[frame_index]));
    EG_VULKAN_CHECK(vkCreateFence(device, &fence_create_info, 0, &renderer->frame_fence[frame_index]));

    frame_index++;
  }
}
static void eg_renderer_create_command_buffer(eg_renderer_t *renderer) {
  VkDevice device = eg_context_device(renderer->context);
  VkCommandPool command_pool = eg_context_command_pool(renderer->context);

  renderer->command_buffer = (VkCommandBuffer *)lb_heap_alloc(sizeof(VkCommandBuffer) * renderer->frames_in_flight, 0, 0);

  VkCommandBufferAllocateInfo command_buffer_alloc_create_info = {0};
  command_buffer_alloc_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_alloc_create_info.commandPool = command_pool;
  command_buffer_alloc_create_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_alloc_create_info.commandBufferCount = (uint32_t)renderer->frames_in_flight;

  EG_VULKAN_CHECK(vkAllocateCommandBuffers(device, &command_buffer_alloc_create_info, renderer->command_buffer));
}

static void eg_renderer_update_uniform_buffers(eg_renderer_t *renderer) {
  eg_scene_t *scene = eg_context_scene(renderer->context);

  if (scene) {

    uint32_t window_width = eg_context_window_width(renderer->context);
    uint32_t window_height = eg_context_window_height(renderer->context);

    ecs_world_t *world = eg_scene_world(scene);
    ecs_entity_t player = eg_scene_player(scene);

    // TODO: find an easier way to compute world position's automatically..
    eg_transform_compute_world_position(world, player);
    eg_transform_compute_world_rotation(world, player);
    eg_transform_compute_world_scale(world, player);

    eg_transform_t *transform = ecs_get_mut(world, player, eg_transform_t);
    eg_camera_t *camera = ecs_get_mut(world, player, eg_camera_t);

    lb_vector3_t eye = transform->world_position;
    lb_vector3_t center = lb_vector3_add(transform->world_position, eg_transform_local_front(transform));
    lb_vector3_t up = lb_vector3_down();

    float fov = lb_deg_to_rad(camera->fov);
    float aspect_ratio = (float)window_width / (float)window_height;
    float near_z = camera->near_z;
    float far_z = camera->far_z;

    lb_matrix4_t view = lb_matrix4_look_at(eye, center, up);
    lb_matrix4_t projection = lb_matrix4_persp(fov, aspect_ratio, near_z, far_z);
    lb_matrix4_t view_projection = lb_matrix4_mul(view, projection);
    lb_matrix4_t view_projection_inv = lb_matrix4_inverse(view_projection);

    renderer->time_infos[renderer->frame_index]->time = eg_context_time(renderer->context);
    renderer->time_infos[renderer->frame_index]->delta_time = eg_context_delta_time(renderer->context);

    renderer->screen_infos[renderer->frame_index]->resolution = lb_vector2_xy((float)window_width, (float)window_height);

    renderer->camera_infos[renderer->frame_index]->position = transform->world_position;
    renderer->camera_infos[renderer->frame_index]->view = view;
    renderer->camera_infos[renderer->frame_index]->projection = projection;
    renderer->camera_infos[renderer->frame_index]->view_projection = view_projection;
    renderer->camera_infos[renderer->frame_index]->view_projection_inv = view_projection_inv;
  }
}

static void eg_renderer_transition_viewport_images_to_render_target(eg_renderer_t *renderer) {
  eg_viewport_t **viewports = eg_context_viewports(renderer->context);

  uint64_t viewport_index = 0;

  while (viewports[viewport_index]) {

    VkImage *color_images = eg_viewport_color_images(viewports[viewport_index]);
    VkImage *depth_images = eg_viewport_depth_images(viewports[viewport_index]);

    VkImageMemoryBarrier color_image_memory_barrier = {0};
    color_image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    color_image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    color_image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    color_image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    color_image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    color_image_memory_barrier.image = color_images[renderer->image_index];
    color_image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    color_image_memory_barrier.subresourceRange.baseMipLevel = 0;
    color_image_memory_barrier.subresourceRange.levelCount = 1;
    color_image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    color_image_memory_barrier.subresourceRange.layerCount = 1;
    color_image_memory_barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    color_image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    vkCmdPipelineBarrier(
      renderer->command_buffer[renderer->frame_index],
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      0, 0, 0, 0, 0, 1, &color_image_memory_barrier);

    VkImageMemoryBarrier depth_image_memory_barrier = {0};
    depth_image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    depth_image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    depth_image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depth_image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    depth_image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    depth_image_memory_barrier.image = depth_images[renderer->image_index];
    depth_image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    depth_image_memory_barrier.subresourceRange.baseMipLevel = 0;
    depth_image_memory_barrier.subresourceRange.levelCount = 1;
    depth_image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    depth_image_memory_barrier.subresourceRange.layerCount = 1;
    depth_image_memory_barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    depth_image_memory_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    vkCmdPipelineBarrier(
      renderer->command_buffer[renderer->frame_index],
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
      0, 0, 0, 0, 0, 1, &depth_image_memory_barrier);

    viewport_index++;
  }
}
static void eg_renderer_transition_viewport_images_to_shader_read(eg_renderer_t *renderer) {
  eg_viewport_t **viewports = eg_context_viewports(renderer->context);

  uint64_t viewport_index = 0;

  while (viewports[viewport_index]) {

    VkImage *color_images = eg_viewport_color_images(viewports[viewport_index]);
    VkImage *depth_images = eg_viewport_depth_images(viewports[viewport_index]);

    VkImageMemoryBarrier color_image_memory_barrier = {0};
    color_image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    color_image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    color_image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    color_image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    color_image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    color_image_memory_barrier.image = color_images[renderer->image_index];
    color_image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    color_image_memory_barrier.subresourceRange.baseMipLevel = 0;
    color_image_memory_barrier.subresourceRange.levelCount = 1;
    color_image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    color_image_memory_barrier.subresourceRange.layerCount = 1;
    color_image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    color_image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(
      renderer->command_buffer[renderer->frame_index],
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      0, 0, 0, 0, 0, 1, &color_image_memory_barrier);

    VkImageMemoryBarrier depth_image_memory_barrier = {0};
    depth_image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    depth_image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depth_image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    depth_image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    depth_image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    depth_image_memory_barrier.image = depth_images[renderer->image_index];
    depth_image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    depth_image_memory_barrier.subresourceRange.baseMipLevel = 0;
    depth_image_memory_barrier.subresourceRange.levelCount = 1;
    depth_image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    depth_image_memory_barrier.subresourceRange.layerCount = 1;
    depth_image_memory_barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    depth_image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(
      renderer->command_buffer[renderer->frame_index],
      VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      0, 0, 0, 0, 0, 1, &depth_image_memory_barrier);

    viewport_index++;
  }
}

static void eg_renderer_record_editor_commands(eg_renderer_t *renderer) {
  uint8_t is_editor_mode = eg_context_is_editor_mode(renderer->context);

  if (is_editor_mode) {

    g_context_editor_refresh_proc(renderer->context);
  }
}
static void eg_renderer_record_compute_commands(eg_renderer_t *renderer) {
  // TODO
  // vkCmdBindPipeline(g_renderer_graphic_command_buffers[g_renderer_frame_index], VK_PIPELINE_BIND_POINT_COMPUTE, s_renderer_chunk_editor_pipeline);
  // vkCmdBindDescriptorSets(g_renderer_graphic_command_buffers[g_renderer_frame_index], VK_PIPELINE_BIND_POINT_COMPUTE, s_renderer_chunk_editor_pipeline_layout, 0, 1, &s_renderer_chunk_editor_descriptor_sets[s_renderer_frame_index], 0, 0);
  // vkCmdDispatch(g_renderer_graphic_command_buffers[g_renderer_frame_index], group_count_x, group_count_y, group_count_z);
}
static void eg_renderer_record_graphic_commands(eg_renderer_t *renderer) {
  uint8_t is_editor_mode = eg_context_is_editor_mode(renderer->context);

  eg_renderer_gbuffer_pass(renderer);

  if (is_editor_mode) {

    eg_renderer_transition_viewport_images_to_shader_read(renderer);

    eg_renderer_editor_pass(renderer);

    eg_renderer_transition_viewport_images_to_render_target(renderer);
  }
}

static void eg_renderer_gbuffer_pass(eg_renderer_t *renderer) {
  eg_viewport_t **viewports = eg_context_viewports(renderer->context);

  VkCommandBuffer command_buffer = renderer->command_buffer[renderer->frame_index];

  uint64_t viewport_index = 0;

  while (viewports[viewport_index]) {

    VkFramebuffer *frame_buffers = eg_viewport_frame_buffers(viewports[viewport_index]);

    VkClearValue color_clear_value = {0};
    color_clear_value.color.float32[0] = 0.0F;
    color_clear_value.color.float32[1] = 0.0F;
    color_clear_value.color.float32[2] = 0.0F;
    color_clear_value.color.float32[3] = 1.0F;

    VkClearValue depth_clear_value = {0};
    depth_clear_value.depthStencil.depth = 1.0F;
    depth_clear_value.depthStencil.stencil = 0;

    VkClearValue clear_values[] = {
      color_clear_value,
      depth_clear_value,
    };

    uint32_t width = eg_viewport_width(viewports[viewport_index]);
    uint32_t height = eg_viewport_height(viewports[viewport_index]);

    VkRenderPassBeginInfo render_pass_create_info = {0};
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_create_info.renderPass = renderer->gbuffer_render_pass;
    render_pass_create_info.framebuffer = frame_buffers[renderer->image_index];
    render_pass_create_info.renderArea.offset.x = 0;
    render_pass_create_info.renderArea.offset.y = 0;
    render_pass_create_info.renderArea.extent.width = width;
    render_pass_create_info.renderArea.extent.height = height;
    render_pass_create_info.pClearValues = clear_values;
    render_pass_create_info.clearValueCount = LB_ARRAY_COUNT(clear_values);

    vkCmdBeginRenderPass(command_buffer, &render_pass_create_info, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = {0};
    viewport.x = 0.0F;
    viewport.y = 0.0F;
    viewport.width = (float)width;
    viewport.height = (float)height;
    viewport.minDepth = 0.0F;
    viewport.maxDepth = 1.0F;

    vkCmdSetViewport(command_buffer, 0, 1, &viewport);

    VkRect2D scissor = {0};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = width;
    scissor.extent.height = height;

    vkCmdSetScissor(command_buffer, 0, 1, &scissor);

    if (renderer->is_debug_enabled) {

      eg_graphic_pipeline_t *debug_pipeline = (eg_graphic_pipeline_t *)renderer->pipeline_link[EG_RENDERER_PIPELINE_LINK_TYPE_DEBUG];

      if (debug_pipeline) {

        uint32_t index_count = renderer->debug_line_index_offset[renderer->frame_index];

        eg_graphic_pipeline_execute(debug_pipeline, command_buffer, index_count);
      }
    }

    vkCmdEndRenderPass(command_buffer);

    viewport_index++;
  }

  if (renderer->is_debug_enabled) {
    renderer->debug_line_vertex_offset[renderer->frame_index] = 0;
    renderer->debug_line_index_offset[renderer->frame_index] = 0;
  }
}
static void eg_renderer_editor_pass(eg_renderer_t *renderer) {
  VkRenderPass main_render_pass = eg_swapchain_main_render_pass(renderer->swapchain);
  VkFramebuffer frame_buffer = eg_swapchain_frame_buffer(renderer->swapchain, renderer->image_index);

  VkCommandBuffer command_buffer = renderer->command_buffer[renderer->frame_index];

  VkClearValue color_clear_value = {0};
  color_clear_value.color.float32[0] = 0.0F;
  color_clear_value.color.float32[1] = 0.0F;
  color_clear_value.color.float32[2] = 0.0F;
  color_clear_value.color.float32[3] = 1.0F;

  VkClearValue depth_clear_value = {0};
  depth_clear_value.depthStencil.depth = 1.0F;
  depth_clear_value.depthStencil.stencil = 0;

  VkClearValue clear_values[] = {
    color_clear_value,
    depth_clear_value,
  };

  uint32_t window_width = eg_context_window_width(renderer->context);
  uint32_t window_height = eg_context_window_height(renderer->context);

  VkRenderPassBeginInfo render_pass_create_info = {0};
  render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_create_info.renderPass = main_render_pass;
  render_pass_create_info.framebuffer = frame_buffer;
  render_pass_create_info.renderArea.offset.x = 0;
  render_pass_create_info.renderArea.offset.y = 0;
  render_pass_create_info.renderArea.extent.width = window_width;
  render_pass_create_info.renderArea.extent.height = window_height;
  render_pass_create_info.pClearValues = clear_values;
  render_pass_create_info.clearValueCount = LB_ARRAY_COUNT(clear_values);

  vkCmdBeginRenderPass(command_buffer, &render_pass_create_info, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport = {0};
  viewport.x = 0.0F;
  viewport.y = 0.0F;
  viewport.width = (float)window_width;
  viewport.height = (float)window_height;
  viewport.minDepth = 0.0F;
  viewport.maxDepth = 1.0F;

  vkCmdSetViewport(command_buffer, 0, 1, &viewport);

  VkRect2D scissor = {0};
  scissor.offset.x = 0;
  scissor.offset.y = 0;
  scissor.extent.width = window_width;
  scissor.extent.height = window_height;

  vkCmdSetScissor(command_buffer, 0, 1, &scissor);

  g_context_editor_draw_proc(renderer->context);

  vkCmdEndRenderPass(command_buffer);
}

static void eg_renderer_destroy_global_buffers(eg_renderer_t *renderer) {
  uint64_t frame_index = 0;
  uint64_t frame_count = renderer->frames_in_flight;

  while (frame_index < frame_count) {

    lb_map_iter_t *iter = lb_map_iter_create(renderer->descriptor_binding_buffers_per_frame[frame_index]);

    while (lb_map_iter_step(iter)) {

      eg_buffer_t *buffer = *(eg_buffer_t **)lb_map_iter_value(iter);

      eg_buffer_destroy(buffer);
    }

    lb_map_iter_destroy(iter);

    lb_map_destroy(renderer->descriptor_binding_buffers_per_frame[frame_index]);

    frame_index++;
  }

  lb_heap_free(renderer->descriptor_binding_buffers_per_frame);

  lb_heap_free(renderer->time_infos);
  lb_heap_free(renderer->screen_infos);
  lb_heap_free(renderer->camera_infos);
}
static void eg_renderer_destroy_debug_buffers(eg_renderer_t *renderer) {
  uint64_t frame_index = 0;
  uint64_t frame_count = renderer->frames_in_flight;

  while (frame_index < frame_count) {

    eg_buffer_destroy(renderer->debug_line_vertex_buffers[frame_index]);
    eg_buffer_destroy(renderer->debug_line_index_buffers[frame_index]);

    frame_index++;
  }

  lb_heap_free(renderer->debug_line_vertex_buffers);
  lb_heap_free(renderer->debug_line_index_buffers);

  lb_heap_free(renderer->debug_line_vertices);
  lb_heap_free(renderer->debug_line_indices);

  lb_heap_free(renderer->debug_line_vertex_offset);
  lb_heap_free(renderer->debug_line_index_offset);
}
static void eg_renderer_destroy_pipelines(eg_renderer_t *renderer) {
  uint64_t pipeline_link_index = 0;
  uint64_t pipeline_link_count = LB_ARRAY_COUNT(renderer->pipeline_link);

  while (pipeline_link_index < pipeline_link_count) {

    if (renderer->pipeline_link[pipeline_link_index]) {

      switch (renderer->pipeline_type[pipeline_link_index]) {
        case LB_PIPELINE_TYPE_GRAPHIC: {

          eg_graphic_pipeline_destroy(renderer->pipeline_link[pipeline_link_index]);

          break;
        }
        case LB_PIPELINE_TYPE_COMPUTE: {

          eg_compute_pipeline_destroy(renderer->pipeline_link[pipeline_link_index]);

          break;
        }
      }
    }

    renderer->pipeline_type[pipeline_link_index] = 0;
    renderer->pipeline_link[pipeline_link_index] = 0;

    pipeline_link_index++;
  }
}
static void eg_renderer_destroy_sync_objects(eg_renderer_t *renderer) {
  VkDevice device = eg_context_device(renderer->context);
  uint32_t image_count = eg_swapchain_image_count(renderer->swapchain);

  uint64_t image_index = 0;

  while (image_index < image_count) {

    vkDestroySemaphore(device, renderer->render_finished_semaphore[image_index], 0);

    image_index++;
  }

  uint64_t frame_index = 0;
  uint64_t frame_count = renderer->frames_in_flight;

  while (frame_index < frame_count) {

    vkDestroySemaphore(device, renderer->image_available_semaphore[frame_index], 0);

    vkDestroyFence(device, renderer->frame_fence[frame_index], 0);

    frame_index++;
  }

  lb_heap_free(renderer->render_finished_semaphore);
  lb_heap_free(renderer->image_available_semaphore);
  lb_heap_free(renderer->frame_fence);
}
static void eg_renderer_destroy_command_buffer(eg_renderer_t *renderer) {
  VkDevice device = eg_context_device(renderer->context);
  VkCommandPool command_pool = eg_context_command_pool(renderer->context);

  vkFreeCommandBuffers(device, command_pool, (uint32_t)renderer->frames_in_flight, renderer->command_buffer);

  lb_heap_free(renderer->command_buffer);
}
static void eg_renderer_destroy_gbuffer_render_pass(eg_renderer_t *renderer) {
  VkDevice device = eg_context_device(renderer->context);

  vkDestroyRenderPass(device, renderer->gbuffer_render_pass, 0);
}

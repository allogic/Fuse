#include <stdlib.h>

#include <library/core/api.h>

#include <engine/context.h>
#include <engine/database.h>
#include <engine/macros.h>
#include <engine/pipeline.h>
#include <engine/renderer.h>
#include <engine/swapchain.h>

// TODO: implement sparse textures..

typedef enum renderer_pipeline_link_type_t {
  RENDERER_PIPELINE_LINK_TYPE_NONE = 0,
  RENDERER_PIPELINE_LINK_TYPE_DEBUG,
  RENDERER_PIPELINE_LINK_TYPE_PBR,
  RENDERER_PIPELINE_LINK_TYPE_MAX,
} renderer_pipeline_link_type_t;

typedef void *renderer_pipeline_link_t;

static void renderer_compute_local_variables(void);

static void renderer_create_pipelines(void);

static void renderer_create_command_buffer(void);
static void renderer_create_sync_objects(void);

static void renderer_update_uniform_buffers(transform_t *transform, camera_t *camera);

static void renderer_record_compute_commands(void);
static void renderer_record_graphic_commands(void);

static void renderer_destroy_pipelines(void);

static void renderer_destroy_command_buffer(void);
static void renderer_destroy_sync_objects(void);

uint8_t g_renderer_is_dirty = 0;

uint8_t g_renderer_enable_debug = 0;

int32_t g_renderer_frames_in_flight = 0;

static renderer_pipeline_link_t s_renderer_pipeline_links[RENDERER_PIPELINE_LINK_TYPE_MAX] = {0};

static VkCommandBuffer *s_renderer_graphic_command_buffers = 0;

static VkSemaphore *s_renderer_graphic_complete_semaphores = 0;
static VkSemaphore *s_renderer_present_complete_semaphores = 0;

static VkFence *s_renderer_frame_fences = 0;

static int32_t s_renderer_frames_in_flight = 0;

static int32_t s_renderer_frame_index = 0;
static int32_t s_renderer_image_index = 0;

static map_t s_renderer_graphic_pipelines = {0};
static map_t s_renderer_compute_pipelines = {0};

static void renderer_create(void) {
  s_renderer_graphic_pipelines = map_create();
  s_renderer_compute_pipelines = map_create();

  renderer_compute_local_variables();

  renderer_create_command_buffer();
  renderer_create_sync_objects();

  renderer_create_pipelines();
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

  VkResult reset_graphic_command_buffer_result = vkResetCommandBuffer(s_renderer_graphic_command_buffers[s_renderer_frame_index], 0);

  switch (reset_graphic_command_buffer_result) {
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

  VkResult begin_graphic_command_buffer_result = vkBeginCommandBuffer(s_renderer_graphic_command_buffers[s_renderer_frame_index], &command_buffer_begin_info);

  switch (begin_graphic_command_buffer_result) {
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
  renderer_record_graphic_commands();

  VkResult end_graphic_command_buffer_result = vkEndCommandBuffer(s_renderer_graphic_command_buffers[s_renderer_frame_index]);

  switch (end_graphic_command_buffer_result) {
    case VK_SUCCESS: {
      break;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  VkPipelineStageFlags graphic_wait_stages[] = {VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  VkSubmitInfo graphic_submit_info = {0};
  graphic_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  graphic_submit_info.pWaitSemaphores = &s_renderer_present_complete_semaphores[s_renderer_frame_index];
  graphic_submit_info.waitSemaphoreCount = 1;
  graphic_submit_info.pSignalSemaphores = &s_renderer_graphic_complete_semaphores[s_renderer_frame_index];
  graphic_submit_info.signalSemaphoreCount = 1;
  graphic_submit_info.pCommandBuffers = &s_renderer_graphic_command_buffers[s_renderer_frame_index];
  graphic_submit_info.commandBufferCount = 1;
  graphic_submit_info.pWaitDstStageMask = graphic_wait_stages;

  VkResult graphics_queue_submit_result = vkQueueSubmit(g_context_graphic_queue, 1, &graphic_submit_info, s_renderer_frame_fences[s_renderer_frame_index]);

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
  VULKAN_CHECK(vkQueueWaitIdle(g_context_graphic_queue));
  VULKAN_CHECK(vkQueueWaitIdle(g_context_present_queue));

  renderer_destroy_sync_objects();
  renderer_destroy_command_buffer();

  renderer_destroy_pipelines();

  map_destroy(&s_renderer_graphic_pipelines);
  map_destroy(&s_renderer_compute_pipelines);
}

void renderer_draw_debug_line(vector3_t from, vector3_t to, vector4_t color) {
  if (g_renderer_enable_debug) {
    graphic_pipeline_t *debug_pipeline = (graphic_pipeline_t *)s_renderer_pipeline_links[RENDERER_PIPELINE_LINK_TYPE_DEBUG];

    if (debug_pipeline) {
      uint64_t vertex_offset = *graphic_pipeline_vertex_offset(debug_pipeline, s_renderer_frame_index);
      uint64_t index_offset = *graphic_pipeline_index_offset(debug_pipeline, s_renderer_frame_index);

      vector3_t *world_positions = (vector3_t *)graphic_pipeline_vertex_buffer(debug_pipeline, s_renderer_frame_index, 0);
      vector4_t *colors = (vector3_t *)graphic_pipeline_vertex_buffer(debug_pipeline, s_renderer_frame_index, 1);
      uint32_t *indices = (uint32_t *)graphic_pipeline_index_buffer(debug_pipeline, s_renderer_frame_index);

      world_positions[vertex_offset + 0] = (vector3_t){from.x, from.y, from.z};
      world_positions[vertex_offset + 1] = (vector3_t){to.x, to.y, to.z};

      colors[vertex_offset + 0] = (vector4_t){color.x, color.y, color.z, color.w};
      colors[vertex_offset + 1] = (vector4_t){color.x, color.y, color.z, color.w};

      indices[index_offset + 0] = vertex_offset + 0;
      indices[index_offset + 1] = vertex_offset + 1;

      *graphic_pipeline_vertex_offset(debug_pipeline, s_renderer_frame_index) += 2;
      *graphic_pipeline_index_offset(debug_pipeline, s_renderer_frame_index) += 2;
    }
  }
}
void renderer_draw_debug_box(vector3_t position, vector3_t size, vector4_t color) {
  if (g_renderer_enable_debug) {
    graphic_pipeline_t *debug_pipeline = (graphic_pipeline_t *)s_renderer_pipeline_links[RENDERER_PIPELINE_LINK_TYPE_DEBUG];

    if (debug_pipeline) {
      uint64_t vertex_offset = *graphic_pipeline_vertex_offset(debug_pipeline, s_renderer_frame_index);
      uint64_t index_offset = *graphic_pipeline_index_offset(debug_pipeline, s_renderer_frame_index);

      vector3_t *world_positions = (vector3_t *)graphic_pipeline_vertex_buffer(debug_pipeline, s_renderer_frame_index, 0);
      vector4_t *colors = (vector3_t *)graphic_pipeline_vertex_buffer(debug_pipeline, s_renderer_frame_index, 1);
      uint32_t *indices = (uint32_t *)graphic_pipeline_index_buffer(debug_pipeline, s_renderer_frame_index);

      world_positions[vertex_offset + 0] = (vector3_t){position.x, position.y, position.z};
      world_positions[vertex_offset + 1] = (vector3_t){position.x, position.y + size.y, position.z};
      world_positions[vertex_offset + 2] = (vector3_t){position.x + size.x, position.y, position.z};
      world_positions[vertex_offset + 3] = (vector3_t){position.x + size.x, position.y + size.y, position.z};
      world_positions[vertex_offset + 4] = (vector3_t){position.x, position.y, position.z + size.z};
      world_positions[vertex_offset + 5] = (vector3_t){position.x, position.y + size.y, position.z + size.z};
      world_positions[vertex_offset + 6] = (vector3_t){position.x + size.x, position.y, position.z + size.z};
      world_positions[vertex_offset + 7] = (vector3_t){position.x + size.x, position.y + size.y, position.z + size.z};

      colors[vertex_offset + 0] = (vector4_t){color.x, color.y, color.z, color.w};
      colors[vertex_offset + 1] = (vector4_t){color.x, color.y, color.z, color.w};
      colors[vertex_offset + 2] = (vector4_t){color.x, color.y, color.z, color.w};
      colors[vertex_offset + 3] = (vector4_t){color.x, color.y, color.z, color.w};
      colors[vertex_offset + 4] = (vector4_t){color.x, color.y, color.z, color.w};
      colors[vertex_offset + 5] = (vector4_t){color.x, color.y, color.z, color.w};
      colors[vertex_offset + 6] = (vector4_t){color.x, color.y, color.z, color.w};
      colors[vertex_offset + 7] = (vector4_t){color.x, color.y, color.z, color.w};

      indices[index_offset + 0] = vertex_offset + 0;
      indices[index_offset + 1] = vertex_offset + 1;
      indices[index_offset + 2] = vertex_offset + 1;
      indices[index_offset + 3] = vertex_offset + 3;
      indices[index_offset + 4] = vertex_offset + 3;
      indices[index_offset + 5] = vertex_offset + 2;
      indices[index_offset + 6] = vertex_offset + 2;
      indices[index_offset + 7] = vertex_offset + 0;
      indices[index_offset + 8] = vertex_offset + 4;
      indices[index_offset + 9] = vertex_offset + 5;
      indices[index_offset + 10] = vertex_offset + 5;
      indices[index_offset + 11] = vertex_offset + 7;
      indices[index_offset + 12] = vertex_offset + 7;
      indices[index_offset + 13] = vertex_offset + 6;
      indices[index_offset + 14] = vertex_offset + 6;
      indices[index_offset + 15] = vertex_offset + 4;
      indices[index_offset + 16] = vertex_offset + 0;
      indices[index_offset + 17] = vertex_offset + 4;
      indices[index_offset + 18] = vertex_offset + 1;
      indices[index_offset + 19] = vertex_offset + 5;
      indices[index_offset + 20] = vertex_offset + 2;
      indices[index_offset + 21] = vertex_offset + 6;
      indices[index_offset + 22] = vertex_offset + 3;
      indices[index_offset + 23] = vertex_offset + 7;

      *graphic_pipeline_vertex_offset(debug_pipeline, s_renderer_frame_index) += 8;
      *graphic_pipeline_index_offset(debug_pipeline, s_renderer_frame_index) += 24;
    }
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

static void renderer_create_pipelines(void) {
  database_pipelines_t graphic_pipelines = database_fetch_pipelines_by_type(PIPELINE_TYPE_GRAPHIC);

  uint64_t graphic_pipeline_index = 0;
  uint64_t graphic_pipeline_count = vector_count(&graphic_pipelines.records);

  while (graphic_pipeline_index < graphic_pipeline_count) {
    database_pipeline_record_t *pipeline_record = (database_pipeline_record_t *)vector_at(&graphic_pipelines.records, graphic_pipeline_index);

    graphic_pipeline_t pipeline = graphic_pipeline_create(s_renderer_frames_in_flight, pipeline_record->name);

    graphic_pipeline_allocate_descriptor_sets(&pipeline, 1);
    graphic_pipeline_update_descriptor_sets(&pipeline);

    map_insert(&s_renderer_graphic_pipelines, pipeline_record->name, strlen(pipeline_record->name), &pipeline, sizeof(graphic_pipeline_t));

    graphic_pipeline_index++;
  }

  database_close_pipelines(&graphic_pipelines);

  database_pipelines_t compute_pipelines = database_fetch_pipelines_by_type(PIPELINE_TYPE_COMPUTE);

  uint64_t compute_pipeline_index = 0;
  uint64_t compute_pipeline_count = vector_count(&compute_pipelines.records);

  while (compute_pipeline_index < compute_pipeline_count) {
    database_pipeline_record_t *pipeline_record = (database_pipeline_record_t *)vector_at(&compute_pipelines.records, compute_pipeline_index);

    compute_pipeline_t pipeline = compute_pipeline_create(s_renderer_frames_in_flight, pipeline_record->name);

    compute_pipeline_allocate_descriptor_sets(&pipeline, 1);
    compute_pipeline_update_descriptor_sets(&pipeline);

    map_insert(&s_renderer_compute_pipelines, pipeline_record->name, strlen(pipeline_record->name), &pipeline, sizeof(compute_pipeline_t));

    compute_pipeline_index++;
  }

  database_close_pipelines(&compute_pipelines);
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

static void renderer_update_uniform_buffers(transform_t *transform, camera_t *camera) {
  graphic_pipeline_t *debug_pipeline = (graphic_pipeline_t *)s_renderer_pipeline_links[RENDERER_PIPELINE_LINK_TYPE_DEBUG];
  graphic_pipeline_t *pbr_pipeline = (graphic_pipeline_t *)s_renderer_pipeline_links[RENDERER_PIPELINE_LINK_TYPE_PBR];

  frame->time_info.time = (float)g_context_time;
  frame->time_info.delta_time = (float)g_context_delta_time;

  frame->screen_info.resolution = vector2_xy((float)g_context_surface_width, (float)g_context_surface_height);

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

  frame->camera_info.world_position = transform->world_position;
  frame->camera_info.view = view;
  frame->camera_info.projection = projection;
  frame->camera_info.view_projection = view_projection;
  frame->camera_info.view_projection_inv = view_projection_inv;
  frame->camera_info.max_ray_steps = camera->ray_steps;
}

static void renderer_record_compute_commands(void) {
  // TODO
  // vkCmdBindPipeline(s_renderer_graphic_command_buffers[s_renderer_frame_index], VK_PIPELINE_BIND_POINT_COMPUTE, s_renderer_chunk_editor_pipeline);
  // vkCmdBindDescriptorSets(s_renderer_graphic_command_buffers[s_renderer_frame_index], VK_PIPELINE_BIND_POINT_COMPUTE, s_renderer_chunk_editor_pipeline_layout, 0, 1, &s_renderer_chunk_editor_descriptor_sets[s_renderer_frame_index], 0, 0);
  // vkCmdDispatch(s_renderer_graphic_command_buffers[s_renderer_frame_index], group_count_x, group_count_y, group_count_z);
}
static void renderer_record_graphic_commands(void) {
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
    // vkCmdBindPipeline(s_renderer_graphic_command_buffers[s_renderer_frame_index], VK_PIPELINE_BIND_POINT_GRAPHICS, s_renderer_chunk_renderer_pipeline);
    // vkCmdBindVertexBuffers(s_renderer_graphic_command_buffers[s_renderer_frame_index], 0, ARRAY_COUNT(vertex_buffers), vertex_buffers, vertex_offsets);
    // vkCmdBindIndexBuffer(s_renderer_graphic_command_buffers[s_renderer_frame_index], s_renderer_cluster_index_buffer[s_renderer_frame_index], 0, VK_INDEX_TYPE_UINT32);
    // vkCmdBindDescriptorSets(s_renderer_graphic_command_buffers[s_renderer_frame_index], VK_PIPELINE_BIND_POINT_GRAPHICS, s_renderer_chunk_renderer_pipeline_layout, 0, 1, &s_renderer_chunk_renderer_descriptor_sets[s_renderer_frame_index], 0, 0);
    // vkCmdDrawIndexed(s_renderer_graphic_command_buffers[s_renderer_frame_index], RENDERER_CLUSTER_INDEX_COUNT, 1, 0, 0, 0);
  }

  {
    if (g_renderer_enable_debug) {
      graphic_pipeline_t *debug_pipeline = (graphic_pipeline_t *)s_renderer_pipeline_links[RENDERER_PIPELINE_LINK_TYPE_DEBUG];

      if (debug_pipeline) {
        graphic_pipeline_execute(debug_pipeline);
      }
    }
  }

  vkCmdEndRenderPass(s_renderer_graphic_command_buffers[s_renderer_frame_index]);
}

static void renderer_destroy_pipelines(void) {
  map_iter_t graphic_pipeline_iter = map_iter_create_from(&s_renderer_graphic_pipelines);

  while (map_iter_valid(&graphic_pipeline_iter)) {
    graphic_pipeline_t *pipeline = (graphic_pipeline_t *)map_iter_value(&graphic_pipeline_iter);

    graphic_pipeline_destroy(pipeline);

    map_iter_advance(&graphic_pipeline_iter);
  }

  map_iter_t compute_pipeline_iter = map_iter_create_from(&s_renderer_compute_pipelines);

  while (map_iter_valid(&compute_pipeline_iter)) {
    compute_pipeline_t *pipeline = (compute_pipeline_t *)map_iter_value(&compute_pipeline_iter);

    compute_pipeline_destroy(pipeline);

    map_iter_advance(&compute_pipeline_iter);
  }
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

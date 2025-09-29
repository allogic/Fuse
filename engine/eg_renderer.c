#include <engine/eg_pch.h>
#include <engine/eg_context.h>
#include <engine/eg_pipeline.h>
#include <engine/eg_renderer.h>
#include <engine/eg_swapchain.h>

#include <ui/ui_itself.h>

// TODO: implement sparse textures..

typedef enum renderer_pipeline_link_type_t {
  RENDERER_PIPELINE_LINK_TYPE_DEBUG,
  RENDERER_PIPELINE_LINK_TYPE_PBR,
} renderer_pipeline_link_type_t;

static void renderer_link_pipeline(uint32_t link_index, uint32_t pipeline_type, void *pipeline);

static void renderer_create_pipeline_links(void);
static void renderer_create_pipelines(void);
static void renderer_create_command_buffer(void);
static void renderer_create_sync_objects(void);

static void renderer_update_uniform_buffers(transform_t *transform, camera_t *camera);

static void renderer_record_compute_commands(void);
static void renderer_record_graphic_commands(void);

static void renderer_destroy_pipeline_links(void);
static void renderer_destroy_pipelines(void);
static void renderer_destroy_command_buffer(void);
static void renderer_destroy_sync_objects(void);

static uint32_t s_renderer_pipeline_types[0xFF] = {0};
static void *s_renderer_pipeline_links[0xFF] = {0};

static VkSemaphore *s_renderer_render_finished_semaphores = 0;
static VkSemaphore *s_renderer_image_available_semaphores = 0;

static VkFence *s_renderer_frame_fences = 0;

void renderer_create(void) {
  renderer_asset_t renderer_asset = database_load_renderer_default_asset();

  g_globals.renderer_frames_in_flight = renderer_asset.frames_in_flight;

  g_globals.renderer_frames_in_flight = MAX(g_globals.renderer_frames_in_flight, 1);
  g_globals.renderer_frames_in_flight = MIN(g_globals.renderer_frames_in_flight, g_globals.swapchain_image_count);

  g_globals.renderer_frame_index = 0;
  g_globals.renderer_image_index = 0;

  renderer_create_sync_objects();
  renderer_create_command_buffer();
  renderer_create_pipeline_links();
  renderer_create_pipelines();

  UI_CREATE();

  database_destroy_renderer_asset(&renderer_asset);
}
void renderer_update(void) {
  if (g_globals.renderer_enable_debug) {
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
  VkResult result = VK_SUCCESS;

  result = vkWaitForFences(g_globals.context_device, 1, &s_renderer_frame_fences[g_globals.renderer_frame_index], 1, UINT64_MAX);

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

  result = vkResetFences(g_globals.context_device, 1, &s_renderer_frame_fences[g_globals.renderer_frame_index]);

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

  result = vkResetCommandBuffer(g_globals.renderer_graphic_command_buffers[g_globals.renderer_frame_index], 0);

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

  result = vkAcquireNextImageKHR(g_globals.context_device, g_globals.swapchain, UINT64_MAX, s_renderer_image_available_semaphores[g_globals.renderer_frame_index], 0, &g_globals.renderer_image_index);

  switch (result) {
    case VK_SUCCESS: {
      break;
    }
    case VK_NOT_READY: {
      return;
    }
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR: {
      g_globals.swapchain_is_dirty = 1;

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

  VULKAN_CHECK(vkBeginCommandBuffer(g_globals.renderer_graphic_command_buffers[g_globals.renderer_frame_index], &command_buffer_begin_info));

  renderer_record_compute_commands();
  renderer_record_graphic_commands();

  VULKAN_CHECK(vkEndCommandBuffer(g_globals.renderer_graphic_command_buffers[g_globals.renderer_frame_index]));

  // TODO: re-validate VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
  VkPipelineStageFlags graphic_wait_stages[] = {VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  VkSubmitInfo graphic_submit_info = {0};
  graphic_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  graphic_submit_info.pWaitSemaphores = &s_renderer_image_available_semaphores[g_globals.renderer_frame_index];
  graphic_submit_info.waitSemaphoreCount = 1;
  graphic_submit_info.pSignalSemaphores = &s_renderer_render_finished_semaphores[g_globals.renderer_image_index];
  graphic_submit_info.signalSemaphoreCount = 1;
  graphic_submit_info.pCommandBuffers = &g_globals.renderer_graphic_command_buffers[g_globals.renderer_frame_index];
  graphic_submit_info.commandBufferCount = 1;
  graphic_submit_info.pWaitDstStageMask = graphic_wait_stages;

  result = vkQueueSubmit(g_globals.context_graphic_queue, 1, &graphic_submit_info, s_renderer_frame_fences[g_globals.renderer_frame_index]);

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
  present_info.pWaitSemaphores = &s_renderer_render_finished_semaphores[g_globals.renderer_image_index];
  present_info.waitSemaphoreCount = 1;
  present_info.pSwapchains = &g_globals.swapchain;
  present_info.swapchainCount = 1;
  present_info.pImageIndices = &g_globals.renderer_image_index;

  result = vkQueuePresentKHR(g_globals.context_present_queue, &present_info);

  switch (result) {
    case VK_SUCCESS: {
      break;
    }
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR: {
      g_globals.swapchain_is_dirty = 1;

      return;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  g_globals.renderer_frame_index = (g_globals.renderer_frame_index + 1) % g_globals.renderer_frames_in_flight;
}
void renderer_destroy(void) {
  VULKAN_CHECK(vkQueueWaitIdle(g_globals.context_graphic_queue));
  VULKAN_CHECK(vkQueueWaitIdle(g_globals.context_present_queue));

  UI_DESTROY();

  renderer_destroy_sync_objects();
  renderer_destroy_command_buffer();
  renderer_destroy_pipeline_links();
  renderer_destroy_pipelines();
}

void renderer_draw_debug_line(vector3_t from, vector3_t to, vector4_t color) {
  if (g_globals.renderer_enable_debug) {
    graphic_pipeline_t *debug_pipeline = (graphic_pipeline_t *)s_renderer_pipeline_links[RENDERER_PIPELINE_LINK_TYPE_DEBUG];

    if (debug_pipeline) {
      /*
            uint64_t vertex_offset = *graphic_pipeline_vertex_offset(debug_pipeline, g_globals.renderer_frame_index);
            uint64_t index_offset = *graphic_pipeline_index_offset(debug_pipeline, g_globals.renderer_frame_index);

            vector3_t *world_positions = (vector3_t *)graphic_pipeline_vertex_buffer(debug_pipeline, g_globals.renderer_frame_index, 0);
            vector4_t *colors = (vector4_t *)graphic_pipeline_vertex_buffer(debug_pipeline, g_globals.renderer_frame_index, 1);
            uint32_t *indices = (uint32_t *)graphic_pipeline_index_buffer(debug_pipeline, g_globals.renderer_frame_index);

            world_positions[vertex_offset + 0] = (vector3_t){from.x, from.y, from.z};
            world_positions[vertex_offset + 1] = (vector3_t){to.x, to.y, to.z};

            colors[vertex_offset + 0] = (vector4_t){color.x, color.y, color.z, color.w};
            colors[vertex_offset + 1] = (vector4_t){color.x, color.y, color.z, color.w};

            indices[index_offset + 0] = (uint32_t)(vertex_offset + 0);
            indices[index_offset + 1] = (uint32_t)(vertex_offset + 1);

            *graphic_pipeline_vertex_offset(debug_pipeline, g_globals.renderer_frame_index) += 2;
            *graphic_pipeline_index_offset(debug_pipeline, g_globals.renderer_frame_index) += 2;
      */
    }
  }
}
void renderer_draw_debug_box(vector3_t position, vector3_t size, vector4_t color) {
  if (g_globals.renderer_enable_debug) {
    graphic_pipeline_t *debug_pipeline = (graphic_pipeline_t *)s_renderer_pipeline_links[RENDERER_PIPELINE_LINK_TYPE_DEBUG];

    if (debug_pipeline) {
      /*
            uint64_t vertex_offset = *graphic_pipeline_vertex_offset(debug_pipeline, g_globals.renderer_frame_index);
            uint64_t index_offset = *graphic_pipeline_index_offset(debug_pipeline, g_globals.renderer_frame_index);

            vector3_t *world_positions = (vector3_t *)graphic_pipeline_vertex_buffer(debug_pipeline, g_globals.renderer_frame_index, 0);
            vector4_t *colors = (vector4_t *)graphic_pipeline_vertex_buffer(debug_pipeline, g_globals.renderer_frame_index, 1);
            uint32_t *indices = (uint32_t *)graphic_pipeline_index_buffer(debug_pipeline, g_globals.renderer_frame_index);

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

            *graphic_pipeline_vertex_offset(debug_pipeline, g_globals.renderer_frame_index) += 8;
            *graphic_pipeline_index_offset(debug_pipeline, g_globals.renderer_frame_index) += 24;
      */
    }
  }
}

static void renderer_link_pipeline(uint32_t link_index, uint32_t pipeline_type, void *pipeline) {
  if (s_renderer_pipeline_links[link_index]) {

    switch (s_renderer_pipeline_types[link_index]) {
      case 0: {
        graphic_pipeline_destroy(s_renderer_pipeline_links[link_index]);
        break;
      }
      case 1: {
        compute_pipeline_destroy(s_renderer_pipeline_links[link_index]);
        break;
      }
    }
  }

  s_renderer_pipeline_types[link_index] = pipeline_type;
  s_renderer_pipeline_links[link_index] = pipeline;
}

static void renderer_create_pipeline_links(void) {
  memset(s_renderer_pipeline_types, 0, sizeof(s_renderer_pipeline_types));
  memset(s_renderer_pipeline_links, 0, sizeof(s_renderer_pipeline_links));
}
static void renderer_create_pipelines(void) {
  vector_t graphic_pipeline_assets = database_load_pipeline_assets_by_type(0);

  uint64_t graphic_pipeline_asset_index = 0;
  uint64_t graphic_pipeline_asset_count = vector_count(&graphic_pipeline_assets);

  while (graphic_pipeline_asset_index < graphic_pipeline_asset_count) {
    pipeline_asset_t *pipeline_asset = (pipeline_asset_t *)vector_at(&graphic_pipeline_assets, graphic_pipeline_asset_index);

    if (pipeline_asset->auto_create) {
      graphic_pipeline_t *pipeline = graphic_pipeline_create(g_globals.renderer_frames_in_flight, pipeline_asset->id);

      // TODO
      // graphic_pipeline_allocate_descriptor_sets(&pipeline, 1);
      // graphic_pipeline_update_descriptor_sets(&pipeline);

      renderer_link_pipeline(pipeline_asset->link_index, pipeline_asset->type, pipeline);
    }

    graphic_pipeline_asset_index++;
  }

  database_destroy_pipeline_assets(&graphic_pipeline_assets);

  vector_t compute_pipeline_assets = database_load_pipeline_assets_by_type(1);

  uint64_t compute_pipeline_asset_index = 0;
  uint64_t compute_pipeline_asset_count = vector_count(&compute_pipeline_assets);

  while (compute_pipeline_asset_index < compute_pipeline_asset_count) {
    pipeline_asset_t *pipeline_asset = (pipeline_asset_t *)vector_at(&compute_pipeline_assets, compute_pipeline_asset_index);

    if (pipeline_asset->auto_create) {
      compute_pipeline_t *pipeline = compute_pipeline_create(g_globals.renderer_frames_in_flight, pipeline_asset->id);

      // TODO
      // compute_pipeline_allocate_descriptor_sets(&pipeline, 1);
      // compute_pipeline_update_descriptor_sets(&pipeline);

      renderer_link_pipeline(pipeline_asset->link_index, pipeline_asset->type, pipeline);
    }

    compute_pipeline_asset_index++;
  }

  database_destroy_pipeline_assets(&compute_pipeline_assets);
}
static void renderer_create_command_buffer(void) {
  g_globals.renderer_graphic_command_buffers = (VkCommandBuffer *)heap_alloc(sizeof(VkCommandBuffer) * g_globals.renderer_frames_in_flight);

  VkCommandBufferAllocateInfo command_buffer_alloc_create_info = {0};
  command_buffer_alloc_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_alloc_create_info.commandPool = g_globals.context_command_pool;
  command_buffer_alloc_create_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_alloc_create_info.commandBufferCount = g_globals.renderer_frames_in_flight;

  VULKAN_CHECK(vkAllocateCommandBuffers(g_globals.context_device, &command_buffer_alloc_create_info, g_globals.renderer_graphic_command_buffers));
}
static void renderer_create_sync_objects(void) {
  s_renderer_render_finished_semaphores = (VkSemaphore *)heap_alloc(sizeof(VkSemaphore) * g_globals.swapchain_image_count);
  s_renderer_image_available_semaphores = (VkSemaphore *)heap_alloc(sizeof(VkSemaphore) * g_globals.renderer_frames_in_flight);
  s_renderer_frame_fences = (VkFence *)heap_alloc(sizeof(VkFence) * g_globals.renderer_frames_in_flight);

  VkSemaphoreCreateInfo semaphore_create_info = {0};
  semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphore_create_info.flags = 0;

  VkFenceCreateInfo fence_create_info = {0};
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  uint32_t image_index = 0;
  while (image_index < g_globals.swapchain_image_count) {
    VULKAN_CHECK(vkCreateSemaphore(g_globals.context_device, &semaphore_create_info, 0, &s_renderer_render_finished_semaphores[image_index]));

    image_index++;
  }

  uint32_t frame_index = 0;
  while (frame_index < g_globals.renderer_frames_in_flight) {
    VULKAN_CHECK(vkCreateSemaphore(g_globals.context_device, &semaphore_create_info, 0, &s_renderer_image_available_semaphores[frame_index]));

    VULKAN_CHECK(vkCreateFence(g_globals.context_device, &fence_create_info, 0, &s_renderer_frame_fences[frame_index]));

    frame_index++;
  }
}

static void renderer_update_uniform_buffers(transform_t *transform, camera_t *camera) {
  graphic_pipeline_t *debug_pipeline = (graphic_pipeline_t *)s_renderer_pipeline_links[RENDERER_PIPELINE_LINK_TYPE_DEBUG];
  graphic_pipeline_t *pbr_pipeline = (graphic_pipeline_t *)s_renderer_pipeline_links[RENDERER_PIPELINE_LINK_TYPE_PBR];

  // frame->time_info.time = (float)g_context_time;
  // frame->time_info.delta_time = (float)g_context_delta_time;

  // frame->screen_info.resolution = vector2_xy((float)g_context_surface_width, (float)g_context_surface_height);

  vector3_t eye = transform->world_position;
  vector3_t center = vector3_add(transform->world_position, transform->local_front);
  vector3_t up = vector3_down();

  float fov = math_deg_to_rad(camera->fov);
  float aspect_ratio = (float)g_globals.context_surface_width / (float)g_globals.context_surface_height;
  float near_z = camera->near_z;
  float far_z = camera->far_z;

  matrix4_t view = matrix4_look_at(eye, center, up);
  matrix4_t projection = matrix4_persp(fov, aspect_ratio, near_z, far_z);
  matrix4_t view_projection = matrix4_mul(view, projection);
  matrix4_t view_projection_inv = matrix4_inverse(view_projection);

  // frame->camera_info.world_position = transform->world_position;
  // frame->camera_info.view = view;
  // frame->camera_info.projection = projection;
  // frame->camera_info.view_projection = view_projection;
  // frame->camera_info.view_projection_inv = view_projection_inv;
  // frame->camera_info.max_ray_steps = camera->ray_steps;
}

static void renderer_record_compute_commands(void) {
  // TODO
  // vkCmdBindPipeline(g_globals.renderer_graphic_command_buffers[g_globals.renderer_frame_index], VK_PIPELINE_BIND_POINT_COMPUTE, s_renderer_chunk_editor_pipeline);
  // vkCmdBindDescriptorSets(g_globals.renderer_graphic_command_buffers[g_globals.renderer_frame_index], VK_PIPELINE_BIND_POINT_COMPUTE, s_renderer_chunk_editor_pipeline_layout, 0, 1, &s_renderer_chunk_editor_descriptor_sets[s_renderer_frame_index], 0, 0);
  // vkCmdDispatch(g_globals.renderer_graphic_command_buffers[g_globals.renderer_frame_index], group_count_x, group_count_y, group_count_z);
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
  render_pass_create_info.renderPass = g_globals.swapchain_render_pass;
  render_pass_create_info.framebuffer = g_globals.swapchain_frame_buffers[g_globals.renderer_image_index];
  render_pass_create_info.renderArea.offset.x = 0;
  render_pass_create_info.renderArea.offset.y = 0;
  render_pass_create_info.renderArea.extent.width = g_globals.context_surface_width;
  render_pass_create_info.renderArea.extent.height = g_globals.context_surface_height;
  render_pass_create_info.pClearValues = clear_values;
  render_pass_create_info.clearValueCount = ARRAY_COUNT(clear_values);

  vkCmdBeginRenderPass(g_globals.renderer_graphic_command_buffers[g_globals.renderer_frame_index], &render_pass_create_info, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport = {0};
  viewport.x = 0.0F;
  viewport.y = 0.0F;
  viewport.width = (float)g_globals.context_surface_width;
  viewport.height = (float)g_globals.context_surface_height;
  viewport.minDepth = 0.0F;
  viewport.maxDepth = 1.0F;

  vkCmdSetViewport(g_globals.renderer_graphic_command_buffers[g_globals.renderer_frame_index], 0, 1, &viewport);

  VkRect2D scissor = {0};
  scissor.offset.x = 0;
  scissor.offset.y = 0;
  scissor.extent.width = g_globals.context_surface_width;
  scissor.extent.height = g_globals.context_surface_height;

  vkCmdSetScissor(g_globals.renderer_graphic_command_buffers[g_globals.renderer_frame_index], 0, 1, &scissor);

  {
    // VkBuffer vertex_buffers[] = {s_renderer_cluster_vertex_buffer[g_globals.renderer_frame_index]};
    // uint64_t vertex_offsets[] = {0, 0};
    //
    // vkCmdBindPipeline(g_globals.renderer_graphic_command_buffers[g_globals.renderer_frame_index], VK_PIPELINE_BIND_POINT_GRAPHICS, s_renderer_chunk_renderer_pipeline);
    // vkCmdBindVertexBuffers(g_globals.renderer_graphic_command_buffers[g_globals.renderer_frame_index], 0, ARRAY_COUNT(vertex_buffers), vertex_buffers, vertex_offsets);
    // vkCmdBindIndexBuffer(g_globals.renderer_graphic_command_buffers[g_globals.renderer_frame_index], s_renderer_cluster_index_buffer[g_globals.renderer_frame_index], 0, VK_INDEX_TYPE_UINT32);
    // vkCmdBindDescriptorSets(g_globals.renderer_graphic_command_buffers[g_globals.renderer_frame_index], VK_PIPELINE_BIND_POINT_GRAPHICS, s_renderer_chunk_renderer_pipeline_layout, 0, 1, &s_renderer_chunk_renderer_descriptor_sets[g_globals.renderer_frame_index], 0, 0);
    // vkCmdDrawIndexed(g_globals.renderer_graphic_command_buffers[g_globals.renderer_frame_index], RENDERER_CLUSTER_INDEX_COUNT, 1, 0, 0, 0);
  }

  {
    if (g_globals.renderer_enable_debug) {
      graphic_pipeline_t *debug_pipeline = (graphic_pipeline_t *)s_renderer_pipeline_links[RENDERER_PIPELINE_LINK_TYPE_DEBUG];

      if (debug_pipeline) {
        graphic_pipeline_execute(debug_pipeline);
      }
    }
  }

  UI_DRAW();

  vkCmdEndRenderPass(g_globals.renderer_graphic_command_buffers[g_globals.renderer_frame_index]);
}

static void renderer_destroy_pipeline_links(void) {
  uint64_t pipeline_link_index = 0;
  uint64_t pipeline_link_count = ARRAY_COUNT(s_renderer_pipeline_links);

  while (pipeline_link_index < pipeline_link_count) {
    if (s_renderer_pipeline_links[pipeline_link_index]) {

      switch (s_renderer_pipeline_types[pipeline_link_index]) {
        case 0: {
          graphic_pipeline_destroy(s_renderer_pipeline_links[pipeline_link_index]);
          break;
        }
        case 1: {
          compute_pipeline_destroy(s_renderer_pipeline_links[pipeline_link_index]);
          break;
        }
      }
    }

    s_renderer_pipeline_types[pipeline_link_index] = 0;
    s_renderer_pipeline_links[pipeline_link_index] = 0;

    pipeline_link_index++;
  }
}
static void renderer_destroy_pipelines(void) {
  // TODO
  // graphic_pipeline_destroy(s_renderer_graphic_pipelines);
  // compute_pipeline_destroy(s_renderer_compute_pipelines);
}
static void renderer_destroy_command_buffer(void) {
  vkFreeCommandBuffers(g_globals.context_device, g_globals.context_command_pool, g_globals.renderer_frames_in_flight, g_globals.renderer_graphic_command_buffers);

  heap_free(g_globals.renderer_graphic_command_buffers);
}
static void renderer_destroy_sync_objects(void) {
  uint32_t image_index = 0;
  while (image_index < g_globals.swapchain_image_count) {
    vkDestroySemaphore(g_globals.context_device, s_renderer_render_finished_semaphores[image_index], 0);

    image_index++;
  }

  uint32_t frame_index = 0;
  while (frame_index < g_globals.renderer_frames_in_flight) {
    vkDestroySemaphore(g_globals.context_device, s_renderer_image_available_semaphores[frame_index], 0);

    vkDestroyFence(g_globals.context_device, s_renderer_frame_fences[frame_index], 0);

    frame_index++;
  }

  heap_free(s_renderer_render_finished_semaphores);
  heap_free(s_renderer_image_available_semaphores);
  heap_free(s_renderer_frame_fences);
}

#include <engine/eg_pch.h>

#define EG_RENDERER_DEBUG_LINE_VERTEX_COUNT (1048576LL)
#define EG_RENDERER_DEBUG_LINE_INDEX_COUNT (1048576LL)

#define EG_RENDERER_MAKE_GROUP_COUNT(GLOBAL_SIZE, LOCAL_SIZE) ((int32_t)ceil((double)(GLOBAL_SIZE) / (LOCAL_SIZE)))

// TODO: implement sparse textures..
// TODO: implement viewport defragmentation..

struct eg_renderer_t {
  eg_renderer_asset_t asset;
  uint8_t is_dirty;
  uint8_t is_debug_enabled;
  uint32_t frames_in_flight;
  uint32_t frame_index;
  uint32_t image_index;
  VkCommandBuffer *command_buffer;
  eg_pipeline_type_t pipeline_type[0xFF];
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
  eg_map_t **descriptor_binding_buffers_per_frame;
  eg_debug_vertex_t **debug_line_vertices;
  uint32_t **debug_line_indices;
  eg_buffer_t **debug_line_vertex_buffers;
  eg_buffer_t **debug_line_index_buffers;
  uint32_t *debug_line_vertex_offset;
  uint32_t *debug_line_index_offset;
};

static void eg_renderer_link_pipeline(uint32_t link_index, eg_pipeline_type_t pipeline_type, void *pipeline);

static void eg_renderer_create_global_buffers(void);
static void eg_renderer_create_debug_buffers(void);
static void eg_renderer_create_pipelines(void);
static void eg_renderer_create_sync_objects(void);
static void eg_renderer_create_command_buffer(void);

static void eg_renderer_update_uniform_buffers(void);

static void eg_renderer_transition_viewport_images_to_render_target(void);
static void eg_renderer_transition_viewport_images_to_shader_read(void);

static void eg_renderer_record_editor_commands(void);
static void eg_renderer_record_compute_commands(void);
static void eg_renderer_record_graphic_commands(void);

static void eg_renderer_gbuffer_pass(void);
static void eg_renderer_main_pass(void);

static void eg_renderer_destroy_global_buffers(void);
static void eg_renderer_destroy_debug_buffers(void);
static void eg_renderer_destroy_pipelines(void);
static void eg_renderer_destroy_sync_objects(void);
static void eg_renderer_destroy_command_buffer(void);

static eg_renderer_t *s_renderer_current = 0;

eg_renderer_t *eg_renderer_current(void) {
  return s_renderer_current;
}

void eg_renderer_create(eg_renderer_asset_id_t renderer_asset_id) {
  s_renderer_current = (eg_renderer_t *)eg_heap_alloc(sizeof(eg_renderer_t), 1, 0);

  uint32_t image_count = eg_swapchain_image_count();

  s_renderer_current->asset = eg_database_load_renderer_asset_by_id(renderer_asset_id);
  s_renderer_current->is_debug_enabled = 1; // TODO
  s_renderer_current->frames_in_flight = s_renderer_current->asset.frames_in_flight;
  s_renderer_current->frames_in_flight = EG_MAX(s_renderer_current->frames_in_flight, 1);
  s_renderer_current->frames_in_flight = EG_MIN(s_renderer_current->frames_in_flight, image_count);

  strcpy(s_renderer_current->time_info_descriptor_binding_name, "time_info");
  strcpy(s_renderer_current->screen_info_descriptor_binding_name, "screen_info");
  strcpy(s_renderer_current->camera_info_descriptor_binding_name, "camera_info");

  memset(s_renderer_current->pipeline_type, 0, sizeof(s_renderer_current->pipeline_type));
  memset(s_renderer_current->pipeline_link, 0, sizeof(s_renderer_current->pipeline_link));

  eg_renderer_create_global_buffers();
  eg_renderer_create_debug_buffers();
  eg_renderer_create_pipelines();
  eg_renderer_create_sync_objects();
  eg_renderer_create_command_buffer();

  if (eg_context_is_editor_mode()) {
    g_context_editor_create_proc();
  }
}
void eg_renderer_update(void) {
  eg_scene_t *scene = eg_context_main_scene();

  if (scene) {
    eg_scene_update(scene);
  }

  eg_vector3_t right_position = {0.0F, 0.0F, 0.0F};
  eg_vector3_t right_direction = {1.0F, 0.0F, 0.0F};
  eg_vector4_t right_color = {1.0F, 0.0F, 0.0F, 1.0F};

  eg_renderer_draw_debug_line(right_position, right_direction, right_color);

  eg_vector3_t up_position = {0.0F, 0.0F, 0.0F};
  eg_vector3_t up_direction = {0.0F, 1.0F, 0.0F};
  eg_vector4_t up_color = {0.0F, 1.0F, 0.0F, 1.0F};

  eg_renderer_draw_debug_line(up_position, up_direction, up_color);

  eg_vector3_t front_position = {0.0F, 0.0F, 0.0F};
  eg_vector3_t front_direction = {0.0F, 0.0F, 1.0F};
  eg_vector4_t front_color = {0.0F, 0.0F, 1.0F, 1.0F};

  eg_renderer_draw_debug_line(front_position, front_direction, front_color);
}
void eg_renderer_draw(void) {
  VkSwapchainKHR swapchain_handle = eg_swapchain_handle();

  VkResult result = VK_SUCCESS;

  result = vkWaitForFences(eg_context_device(), 1, &s_renderer_current->frame_fence[s_renderer_current->frame_index], 1, UINT64_MAX);

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

  result = vkResetFences(eg_context_device(), 1, &s_renderer_current->frame_fence[s_renderer_current->frame_index]);

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

  result = vkResetCommandBuffer(s_renderer_current->command_buffer[s_renderer_current->frame_index], 0);

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

  result = vkAcquireNextImageKHR(eg_context_device(), swapchain_handle, UINT64_MAX, s_renderer_current->image_available_semaphore[s_renderer_current->frame_index], 0, &s_renderer_current->image_index);

  switch (result) {
    case VK_SUCCESS: {
      break;
    }
    case VK_NOT_READY: {
      return;
    }
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR: {

      eg_swapchain_set_dirty(1);

      return;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  eg_renderer_update_uniform_buffers();

  VkCommandBufferBeginInfo command_buffer_begin_info = {0};
  command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  command_buffer_begin_info.pInheritanceInfo = 0;

  EG_VULKAN_CHECK(vkBeginCommandBuffer(s_renderer_current->command_buffer[s_renderer_current->frame_index], &command_buffer_begin_info));

  eg_renderer_record_editor_commands();
  eg_renderer_record_compute_commands();
  eg_renderer_record_graphic_commands();

  EG_VULKAN_CHECK(vkEndCommandBuffer(s_renderer_current->command_buffer[s_renderer_current->frame_index]));

  // TODO: re-validate VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
  VkPipelineStageFlags graphic_wait_stages[] = {
    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
  };

  VkSubmitInfo graphic_submit_info = {0};
  graphic_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  graphic_submit_info.pWaitSemaphores = &s_renderer_current->image_available_semaphore[s_renderer_current->frame_index];
  graphic_submit_info.waitSemaphoreCount = 1;
  graphic_submit_info.pSignalSemaphores = &s_renderer_current->render_finished_semaphore[s_renderer_current->image_index];
  graphic_submit_info.signalSemaphoreCount = 1;
  graphic_submit_info.pCommandBuffers = &s_renderer_current->command_buffer[s_renderer_current->frame_index];
  graphic_submit_info.commandBufferCount = 1;
  graphic_submit_info.pWaitDstStageMask = graphic_wait_stages;

  result = vkQueueSubmit(eg_context_primary_queue(), 1, &graphic_submit_info, s_renderer_current->frame_fence[s_renderer_current->frame_index]);

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
  present_info.pWaitSemaphores = &s_renderer_current->render_finished_semaphore[s_renderer_current->image_index];
  present_info.waitSemaphoreCount = 1;
  present_info.pSwapchains = &swapchain_handle;
  present_info.swapchainCount = 1;
  present_info.pImageIndices = &s_renderer_current->image_index;

  result = vkQueuePresentKHR(eg_context_present_queue(), &present_info);

  switch (result) {
    case VK_SUCCESS: {
      break;
    }
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR: {

      eg_swapchain_set_dirty(1);

      return;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  s_renderer_current->frame_index = (s_renderer_current->frame_index + 1) % s_renderer_current->frames_in_flight;
}
void eg_renderer_destroy(void) {
  if (eg_context_is_editor_mode()) {
    g_context_editor_destroy_proc();
  }

  eg_renderer_destroy_command_buffer();
  eg_renderer_destroy_sync_objects();
  eg_renderer_destroy_pipelines();
  eg_renderer_destroy_debug_buffers();
  eg_renderer_destroy_global_buffers();

  eg_database_destroy_renderer_asset(&s_renderer_current->asset);

  eg_heap_free(s_renderer_current);
}

void eg_renderer_draw_debug_line(eg_vector3_t from, eg_vector3_t to, eg_vector4_t color) {
  if (s_renderer_current->is_debug_enabled) {
    eg_graphic_pipeline_t *debug_pipeline = (eg_graphic_pipeline_t *)s_renderer_current->pipeline_link[EG_RENDERER_PIPELINE_LINK_TYPE_DEBUG];

    if (debug_pipeline) {
      uint32_t vertex_offset = s_renderer_current->debug_line_vertex_offset[s_renderer_current->frame_index];
      uint32_t index_offset = s_renderer_current->debug_line_index_offset[s_renderer_current->frame_index];

      eg_debug_vertex_t *vertices = s_renderer_current->debug_line_vertices[s_renderer_current->frame_index];
      uint32_t *indices = s_renderer_current->debug_line_indices[s_renderer_current->frame_index];

      vertices[vertex_offset + 0].position = from;
      vertices[vertex_offset + 1].position = to;

      vertices[vertex_offset + 0].color = color;
      vertices[vertex_offset + 1].color = color;

      indices[index_offset + 0] = (uint32_t)(vertex_offset + 0);
      indices[index_offset + 1] = (uint32_t)(vertex_offset + 1);

      s_renderer_current->debug_line_vertex_offset[s_renderer_current->frame_index] += 2;
      s_renderer_current->debug_line_index_offset[s_renderer_current->frame_index] += 2;
    }
  }
}
void eg_renderer_draw_debug_box(eg_vector3_t position, eg_vector3_t size, eg_vector4_t color) {
  if (s_renderer_current->is_debug_enabled) {
    eg_graphic_pipeline_t *debug_pipeline = (eg_graphic_pipeline_t *)s_renderer_current->pipeline_link[EG_RENDERER_PIPELINE_LINK_TYPE_DEBUG];

    if (debug_pipeline) {
      uint32_t vertex_offset = s_renderer_current->debug_line_vertex_offset[s_renderer_current->frame_index];
      uint32_t index_offset = s_renderer_current->debug_line_index_offset[s_renderer_current->frame_index];

      eg_debug_vertex_t *vertices = s_renderer_current->debug_line_vertices[s_renderer_current->frame_index];
      uint32_t *indices = s_renderer_current->debug_line_indices[s_renderer_current->frame_index];

      vertices[vertex_offset + 0].position = (eg_vector3_t){position.x, position.y, position.z};
      vertices[vertex_offset + 1].position = (eg_vector3_t){position.x, position.y + size.y, position.z};
      vertices[vertex_offset + 2].position = (eg_vector3_t){position.x + size.x, position.y, position.z};
      vertices[vertex_offset + 3].position = (eg_vector3_t){position.x + size.x, position.y + size.y, position.z};
      vertices[vertex_offset + 4].position = (eg_vector3_t){position.x, position.y, position.z + size.z};
      vertices[vertex_offset + 5].position = (eg_vector3_t){position.x, position.y + size.y, position.z + size.z};
      vertices[vertex_offset + 6].position = (eg_vector3_t){position.x + size.x, position.y, position.z + size.z};
      vertices[vertex_offset + 7].position = (eg_vector3_t){position.x + size.x, position.y + size.y, position.z + size.z};

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

      s_renderer_current->debug_line_vertex_offset[s_renderer_current->frame_index] += 8;
      s_renderer_current->debug_line_index_offset[s_renderer_current->frame_index] += 24;
    }
  }
}

uint8_t eg_renderer_is_dirty(void) {
  return s_renderer_current->is_dirty;
}
uint32_t eg_renderer_frames_in_flight(void) {
  return s_renderer_current->frames_in_flight;
}
uint32_t eg_renderer_frame_index(void) {
  return s_renderer_current->frame_index;
}
uint32_t eg_renderer_image_index(void) {
  return s_renderer_current->image_index;
}
VkCommandBuffer *eg_renderer_command_buffer(void) {
  return s_renderer_current->command_buffer;
}

void eg_renderer_set_dirty(uint8_t is_dirty) {
  s_renderer_current->is_dirty = is_dirty;
}

static void eg_renderer_link_pipeline(uint32_t link_index, eg_pipeline_type_t pipeline_type, void *pipeline) {
  if (s_renderer_current->pipeline_link[link_index]) {

    switch (s_renderer_current->pipeline_type[link_index]) {
      case 0: {

        eg_graphic_pipeline_destroy(s_renderer_current->pipeline_link[link_index]);

        break;
      }
      case 1: {

        eg_compute_pipeline_destroy(s_renderer_current->pipeline_link[link_index]);

        break;
      }
    }
  }

  s_renderer_current->pipeline_type[link_index] = pipeline_type;
  s_renderer_current->pipeline_link[link_index] = pipeline;
}

static void eg_renderer_create_global_buffers(void) {
  s_renderer_current->time_infos = (eg_time_info_t **)eg_heap_alloc(sizeof(eg_time_info_t *) * s_renderer_current->frames_in_flight, 0, 0);
  s_renderer_current->screen_infos = (eg_screen_info_t **)eg_heap_alloc(sizeof(eg_screen_info_t *) * s_renderer_current->frames_in_flight, 0, 0);
  s_renderer_current->camera_infos = (eg_camera_info_t **)eg_heap_alloc(sizeof(eg_camera_info_t *) * s_renderer_current->frames_in_flight, 0, 0);

  s_renderer_current->descriptor_binding_buffers_per_frame = (eg_map_t **)eg_heap_alloc(sizeof(eg_map_t *) * s_renderer_current->frames_in_flight, 0, 0);

  uint32_t frame_index = 0;
  uint32_t frame_count = s_renderer_current->frames_in_flight;

  while (frame_index < frame_count) {

    s_renderer_current->descriptor_binding_buffers_per_frame[frame_index] = eg_map_create();

    eg_buffer_t *time_info_descriptor_binding_buffer = eg_buffer_create_uniform_coherent(sizeof(eg_time_info_t));
    eg_buffer_t *screen_info_descriptor_binding_buffer = eg_buffer_create_uniform_coherent(sizeof(eg_screen_info_t));
    eg_buffer_t *camera_info_descriptor_binding_buffer = eg_buffer_create_uniform_coherent(sizeof(eg_camera_info_t));

    s_renderer_current->time_infos[frame_index] = eg_buffer_mapped_memory(time_info_descriptor_binding_buffer);
    s_renderer_current->screen_infos[frame_index] = eg_buffer_mapped_memory(screen_info_descriptor_binding_buffer);
    s_renderer_current->camera_infos[frame_index] = eg_buffer_mapped_memory(camera_info_descriptor_binding_buffer);

    eg_map_insert(s_renderer_current->descriptor_binding_buffers_per_frame[frame_index], s_renderer_current->time_info_descriptor_binding_name, strlen(s_renderer_current->time_info_descriptor_binding_name) + 1, &time_info_descriptor_binding_buffer, sizeof(eg_buffer_t *));
    eg_map_insert(s_renderer_current->descriptor_binding_buffers_per_frame[frame_index], s_renderer_current->screen_info_descriptor_binding_name, strlen(s_renderer_current->screen_info_descriptor_binding_name) + 1, &screen_info_descriptor_binding_buffer, sizeof(eg_buffer_t *));
    eg_map_insert(s_renderer_current->descriptor_binding_buffers_per_frame[frame_index], s_renderer_current->camera_info_descriptor_binding_name, strlen(s_renderer_current->camera_info_descriptor_binding_name) + 1, &camera_info_descriptor_binding_buffer, sizeof(eg_buffer_t *));

    frame_index++;
  }
}
static void eg_renderer_create_debug_buffers(void) {
  s_renderer_current->debug_line_vertex_offset = (uint32_t *)eg_heap_alloc(sizeof(uint32_t) * s_renderer_current->frames_in_flight, 1, 0);
  s_renderer_current->debug_line_index_offset = (uint32_t *)eg_heap_alloc(sizeof(uint32_t) * s_renderer_current->frames_in_flight, 1, 0);

  s_renderer_current->debug_line_vertices = (eg_debug_vertex_t **)eg_heap_alloc(sizeof(eg_debug_vertex_t *) * s_renderer_current->frames_in_flight, 0, 0);
  s_renderer_current->debug_line_indices = (uint32_t **)eg_heap_alloc(sizeof(uint32_t *) * s_renderer_current->frames_in_flight, 0, 0);

  s_renderer_current->debug_line_vertex_buffers = (eg_buffer_t **)eg_heap_alloc(sizeof(eg_buffer_t *) * s_renderer_current->frames_in_flight, 0, 0);
  s_renderer_current->debug_line_index_buffers = (eg_buffer_t **)eg_heap_alloc(sizeof(eg_buffer_t *) * s_renderer_current->frames_in_flight, 0, 0);

  uint32_t frame_index = 0;
  uint32_t frame_count = s_renderer_current->frames_in_flight;

  while (frame_index < frame_count) {

    s_renderer_current->debug_line_vertex_buffers[frame_index] = eg_buffer_create_vertex_coherent(sizeof(eg_debug_vertex_t) * EG_RENDERER_DEBUG_LINE_VERTEX_COUNT);
    s_renderer_current->debug_line_index_buffers[frame_index] = eg_buffer_create_index_coherent(sizeof(uint32_t) * EG_RENDERER_DEBUG_LINE_INDEX_COUNT);

    s_renderer_current->debug_line_vertices[frame_index] = eg_buffer_mapped_memory(s_renderer_current->debug_line_vertex_buffers[frame_index]);
    s_renderer_current->debug_line_indices[frame_index] = eg_buffer_mapped_memory(s_renderer_current->debug_line_index_buffers[frame_index]);

    frame_index++;
  }
}
static void eg_renderer_create_pipelines(void) {
  eg_vector_t *graphic_pipeline_assets = eg_database_load_all_pipeline_assets_by_type(EG_PIPELINE_TYPE_GRAPHIC);

  uint64_t graphic_pipeline_asset_index = 0;
  uint64_t graphic_pipeline_asset_count = eg_vector_count(graphic_pipeline_assets);

  while (graphic_pipeline_asset_index < graphic_pipeline_asset_count) {

    eg_pipeline_asset_t *graphic_pipeline_asset = (eg_pipeline_asset_t *)eg_vector_at(graphic_pipeline_assets, graphic_pipeline_asset_index);

    if (graphic_pipeline_asset->auto_create_pipeline) {

      eg_graphic_pipeline_t *graphic_pipeline = eg_graphic_pipeline_create(graphic_pipeline_asset->id);

      uint32_t frame_index = 0;
      uint32_t frame_count = s_renderer_current->frames_in_flight;

      while (frame_index < frame_count) {

        // TODO: remove this!
        // eg_graphic_pipeline_link_vertex_input_binding_buffer(graphic_pipeline, frame_index, 0, eg_buffer_handle(s_renderer_current->debug_line_vertex_buffers[frame_index]), 0);
        // eg_graphic_pipeline_link_index_buffer(graphic_pipeline, frame_index, eg_buffer_handle(s_renderer_current->debug_line_index_buffers[frame_index]));

        // if (graphic_pipeline_asset->auto_link_descriptor_binding) {
        //   eg_graphic_pipeline_set_auto_link_descriptor_bindings(graphic_pipeline, s_renderer_current->descriptor_binding_buffers_per_frame);
        // }

        frame_index++;
      }

      // eg_graphic_pipeline_allocate_descriptor_sets(graphic_pipeline, 1);
      // eg_graphic_pipeline_update_descriptor_sets(graphic_pipeline);

      eg_renderer_link_pipeline(graphic_pipeline_asset->link_index, graphic_pipeline_asset->type, graphic_pipeline);
    }

    graphic_pipeline_asset_index++;
  }

  eg_database_destroy_pipeline_assets(graphic_pipeline_assets);

  eg_vector_t *compute_pipeline_assets = eg_database_load_all_pipeline_assets_by_type(EG_PIPELINE_TYPE_COMPUTE);

  uint64_t compute_pipeline_asset_index = 0;
  uint64_t compute_pipeline_asset_count = eg_vector_count(compute_pipeline_assets);

  while (compute_pipeline_asset_index < compute_pipeline_asset_count) {

    eg_pipeline_asset_t *pipeline_asset = (eg_pipeline_asset_t *)eg_vector_at(compute_pipeline_assets, compute_pipeline_asset_index);

    if (pipeline_asset->auto_create_pipeline) {

      eg_compute_pipeline_t *pipeline = eg_compute_pipeline_create(pipeline_asset->id);

      uint32_t frame_index = 0;
      uint32_t frame_count = s_renderer_current->frames_in_flight;

      while (frame_index < frame_count) {

        // if (pipeline_asset->auto_link_descriptor_binding) {
        //   eg_compute_pipeline_set_auto_link_descriptor_bindings(pipeline, s_renderer_current->descriptor_binding_buffers_per_frame);
        // }

        frame_index++;
      }

      // eg_compute_pipeline_allocate_descriptor_sets(pipeline, 1);
      // eg_compute_pipeline_update_descriptor_sets(pipeline);

      eg_renderer_link_pipeline(pipeline_asset->link_index, pipeline_asset->type, pipeline);
    }

    compute_pipeline_asset_index++;
  }

  eg_database_destroy_pipeline_assets(compute_pipeline_assets);
}
static void eg_renderer_create_sync_objects(void) {
  s_renderer_current->render_finished_semaphore = (VkSemaphore *)eg_heap_alloc(sizeof(VkSemaphore) * eg_swapchain_image_count(), 0, 0);
  s_renderer_current->image_available_semaphore = (VkSemaphore *)eg_heap_alloc(sizeof(VkSemaphore) * s_renderer_current->frames_in_flight, 0, 0);
  s_renderer_current->frame_fence = (VkFence *)eg_heap_alloc(sizeof(VkFence) * s_renderer_current->frames_in_flight, 0, 0);

  VkSemaphoreCreateInfo semaphore_create_info = {0};
  semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphore_create_info.flags = 0;

  VkFenceCreateInfo fence_create_info = {0};
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  uint32_t image_index = 0;
  uint32_t image_count = eg_swapchain_image_count();

  while (image_index < image_count) {

    EG_VULKAN_CHECK(vkCreateSemaphore(eg_context_device(), &semaphore_create_info, 0, &s_renderer_current->render_finished_semaphore[image_index]));

    image_index++;
  }

  uint32_t frame_index = 0;
  uint32_t frame_count = s_renderer_current->frames_in_flight;

  while (frame_index < frame_count) {

    EG_VULKAN_CHECK(vkCreateSemaphore(eg_context_device(), &semaphore_create_info, 0, &s_renderer_current->image_available_semaphore[frame_index]));
    EG_VULKAN_CHECK(vkCreateFence(eg_context_device(), &fence_create_info, 0, &s_renderer_current->frame_fence[frame_index]));

    frame_index++;
  }
}
static void eg_renderer_create_command_buffer(void) {
  s_renderer_current->command_buffer = (VkCommandBuffer *)eg_heap_alloc(sizeof(VkCommandBuffer) * s_renderer_current->frames_in_flight, 0, 0);

  VkCommandBufferAllocateInfo command_buffer_alloc_create_info = {0};
  command_buffer_alloc_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_alloc_create_info.commandPool = eg_context_command_pool();
  command_buffer_alloc_create_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_alloc_create_info.commandBufferCount = (uint32_t)s_renderer_current->frames_in_flight;

  EG_VULKAN_CHECK(vkAllocateCommandBuffers(eg_context_device(), &command_buffer_alloc_create_info, s_renderer_current->command_buffer));
}

static void eg_renderer_update_uniform_buffers(void) {
  eg_scene_t *scene = eg_context_main_scene();

  if (scene) {

    uint32_t window_width = eg_context_window_width();
    uint32_t window_height = eg_context_window_height();

    ecs_world_t *world = eg_scene_world(scene);
    ecs_entity_t player = eg_scene_player(scene);

    // TODO: find an easier way to compute world position's automatically..
    eg_transform_compute_world_position(world, player);
    eg_transform_compute_world_rotation(world, player);
    eg_transform_compute_world_scale(world, player);

    eg_transform_t *transform = ecs_get_mut(world, player, eg_transform_t);
    eg_camera_t *camera = ecs_get_mut(world, player, eg_camera_t);

    eg_vector3_t eye = transform->world_position;
    eg_vector3_t center = eg_vector3_add(transform->world_position, eg_transform_local_front(transform));
    eg_vector3_t up = eg_vector3_down();

    float fov = eg_deg_to_rad(camera->fov);
    float aspect_ratio = (float)window_width / (float)window_height;
    float near_z = camera->near_z;
    float far_z = camera->far_z;

    eg_matrix4_t view = eg_matrix4_look_at(eye, center, up);
    eg_matrix4_t projection = eg_matrix4_persp(fov, aspect_ratio, near_z, far_z);
    eg_matrix4_t view_projection = eg_matrix4_mul(view, projection);
    eg_matrix4_t view_projection_inv = eg_matrix4_inverse(view_projection);

    s_renderer_current->time_infos[s_renderer_current->frame_index]->time = eg_context_time();
    s_renderer_current->time_infos[s_renderer_current->frame_index]->delta_time = eg_context_delta_time();

    s_renderer_current->screen_infos[s_renderer_current->frame_index]->resolution = eg_vector2_xy((float)window_width, (float)window_height);

    s_renderer_current->camera_infos[s_renderer_current->frame_index]->position = transform->world_position;
    s_renderer_current->camera_infos[s_renderer_current->frame_index]->view = view;
    s_renderer_current->camera_infos[s_renderer_current->frame_index]->projection = projection;
    s_renderer_current->camera_infos[s_renderer_current->frame_index]->view_projection = view_projection;
    s_renderer_current->camera_infos[s_renderer_current->frame_index]->view_projection_inv = view_projection_inv;
  }
}

static void eg_renderer_transition_viewport_images_to_render_target(void) {
  eg_viewport_t **viewports = eg_context_viewports();

  uint64_t viewport_index = 0;

  while (viewports[viewport_index]) {

    VkImageMemoryBarrier color_image_memory_barrier = {0};
    color_image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    color_image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    color_image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    color_image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    color_image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    color_image_memory_barrier.image = eg_viewport_color_image(viewports[viewport_index], s_renderer_current->image_index);
    color_image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    color_image_memory_barrier.subresourceRange.baseMipLevel = 0;
    color_image_memory_barrier.subresourceRange.levelCount = 1;
    color_image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    color_image_memory_barrier.subresourceRange.layerCount = 1;
    color_image_memory_barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    color_image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    vkCmdPipelineBarrier(
      s_renderer_current->command_buffer[s_renderer_current->frame_index],
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      0, 0, 0, 0, 0, 1, &color_image_memory_barrier);

    VkImageMemoryBarrier depth_image_memory_barrier = {0};
    depth_image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    depth_image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    depth_image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depth_image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    depth_image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    depth_image_memory_barrier.image = eg_viewport_depth_image(viewports[viewport_index], s_renderer_current->image_index);
    depth_image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    depth_image_memory_barrier.subresourceRange.baseMipLevel = 0;
    depth_image_memory_barrier.subresourceRange.levelCount = 1;
    depth_image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    depth_image_memory_barrier.subresourceRange.layerCount = 1;
    depth_image_memory_barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    depth_image_memory_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    vkCmdPipelineBarrier(
      s_renderer_current->command_buffer[s_renderer_current->frame_index],
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
      0, 0, 0, 0, 0, 1, &depth_image_memory_barrier);

    viewport_index++;
  }
}
static void eg_renderer_transition_viewport_images_to_shader_read(void) {
  eg_viewport_t **viewports = eg_context_viewports();

  uint64_t viewport_index = 0;

  while (viewports[viewport_index]) {

    VkImageMemoryBarrier color_image_memory_barrier = {0};
    color_image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    color_image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    color_image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    color_image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    color_image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    color_image_memory_barrier.image = eg_viewport_color_image(viewports[viewport_index], s_renderer_current->image_index);
    color_image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    color_image_memory_barrier.subresourceRange.baseMipLevel = 0;
    color_image_memory_barrier.subresourceRange.levelCount = 1;
    color_image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    color_image_memory_barrier.subresourceRange.layerCount = 1;
    color_image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    color_image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(
      s_renderer_current->command_buffer[s_renderer_current->frame_index],
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      0, 0, 0, 0, 0, 1, &color_image_memory_barrier);

    VkImageMemoryBarrier depth_image_memory_barrier = {0};
    depth_image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    depth_image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depth_image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    depth_image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    depth_image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    depth_image_memory_barrier.image = eg_viewport_depth_image(viewports[viewport_index], s_renderer_current->image_index);
    depth_image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    depth_image_memory_barrier.subresourceRange.baseMipLevel = 0;
    depth_image_memory_barrier.subresourceRange.levelCount = 1;
    depth_image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    depth_image_memory_barrier.subresourceRange.layerCount = 1;
    depth_image_memory_barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    depth_image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(
      s_renderer_current->command_buffer[s_renderer_current->frame_index],
      VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      0, 0, 0, 0, 0, 1, &depth_image_memory_barrier);

    viewport_index++;
  }
}

static void eg_renderer_record_editor_commands(void) {
  if (eg_context_is_editor_mode()) {
    g_context_editor_refresh_proc();
  }
}
static void eg_renderer_record_compute_commands(void) {
  // TODO
  // vkCmdBindPipeline(g_renderer_graphic_command_buffers[g_renderer_frame_index], VK_PIPELINE_BIND_POINT_COMPUTE, s_renderer_chunk_editor_pipeline);
  // vkCmdBindDescriptorSets(g_renderer_graphic_command_buffers[g_renderer_frame_index], VK_PIPELINE_BIND_POINT_COMPUTE, s_renderer_chunk_editor_pipeline_layout, 0, 1, &s_renderer_chunk_editor_descriptor_sets[s_renderer_frame_index], 0, 0);
  // vkCmdDispatch(g_renderer_graphic_command_buffers[g_renderer_frame_index], group_count_x, group_count_y, group_count_z);
}
static void eg_renderer_record_graphic_commands(void) {
  eg_renderer_gbuffer_pass();

  if (eg_context_is_editor_mode()) {
    eg_renderer_transition_viewport_images_to_shader_read();
  }

  eg_renderer_main_pass();

  if (eg_context_is_editor_mode()) {
    eg_renderer_transition_viewport_images_to_render_target();
  }
}

static void eg_renderer_gbuffer_pass(void) {
  eg_viewport_t **viewports = eg_context_viewports();

  VkCommandBuffer command_buffer = s_renderer_current->command_buffer[s_renderer_current->frame_index];

  uint64_t viewport_index = 0;

  while (viewports[viewport_index]) {

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
    render_pass_create_info.renderPass = eg_renderpass_gbuffer();
    render_pass_create_info.framebuffer = eg_viewport_frame_buffer(viewports[viewport_index], s_renderer_current->image_index);
    render_pass_create_info.renderArea.offset.x = 0;
    render_pass_create_info.renderArea.offset.y = 0;
    render_pass_create_info.renderArea.extent.width = width;
    render_pass_create_info.renderArea.extent.height = height;
    render_pass_create_info.pClearValues = clear_values;
    render_pass_create_info.clearValueCount = EG_ARRAY_COUNT(clear_values);

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

    if (s_renderer_current->is_debug_enabled) {

      eg_graphic_pipeline_t *debug_pipeline = (eg_graphic_pipeline_t *)s_renderer_current->pipeline_link[EG_RENDERER_PIPELINE_LINK_TYPE_DEBUG];

      if (debug_pipeline) {

        uint32_t index_count = s_renderer_current->debug_line_index_offset[s_renderer_current->frame_index];

        // eg_graphic_pipeline_execute(debug_pipeline, command_buffer, index_count);
      }
    }

    vkCmdEndRenderPass(command_buffer);

    viewport_index++;
  }

  if (s_renderer_current->is_debug_enabled) {
    s_renderer_current->debug_line_vertex_offset[s_renderer_current->frame_index] = 0;
    s_renderer_current->debug_line_index_offset[s_renderer_current->frame_index] = 0;
  }
}
static void eg_renderer_main_pass(void) {
  uint8_t is_editor_mode = eg_context_is_editor_mode();

  VkCommandBuffer command_buffer = s_renderer_current->command_buffer[s_renderer_current->frame_index];

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

  uint32_t window_width = 0;
  uint32_t window_height = 0;

  if (is_editor_mode) {

    window_width = eg_context_window_width();
    window_height = eg_context_window_height();

  } else {

    eg_viewport_t *viewport = eg_context_main_viewport();

    window_width = eg_viewport_width(viewport);
    window_height = eg_viewport_height(viewport);
  }

  VkRenderPassBeginInfo render_pass_create_info = {0};
  render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_create_info.renderPass = eg_renderpass_main();
  render_pass_create_info.framebuffer = eg_swapchain_frame_buffer(s_renderer_current->image_index);
  render_pass_create_info.renderArea.offset.x = 0;
  render_pass_create_info.renderArea.offset.y = 0;
  render_pass_create_info.renderArea.extent.width = window_width;
  render_pass_create_info.renderArea.extent.height = window_height;
  render_pass_create_info.pClearValues = clear_values;
  render_pass_create_info.clearValueCount = EG_ARRAY_COUNT(clear_values);

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

  if (is_editor_mode) {

    g_context_editor_draw_proc();

  } else {
    // TODO
  }

  vkCmdEndRenderPass(command_buffer);
}

static void eg_renderer_destroy_global_buffers(void) {
  uint32_t frame_index = 0;
  uint32_t frame_count = s_renderer_current->frames_in_flight;

  while (frame_index < frame_count) {

    eg_map_iter_t *iter = eg_map_iter_create(s_renderer_current->descriptor_binding_buffers_per_frame[frame_index]);

    while (eg_map_iter_step(iter)) {

      eg_buffer_t *buffer = *(eg_buffer_t **)eg_map_iter_value(iter);

      eg_buffer_destroy(buffer);
    }

    eg_map_iter_destroy(iter);

    eg_map_destroy(s_renderer_current->descriptor_binding_buffers_per_frame[frame_index]);

    frame_index++;
  }

  eg_heap_free(s_renderer_current->descriptor_binding_buffers_per_frame);

  eg_heap_free(s_renderer_current->time_infos);
  eg_heap_free(s_renderer_current->screen_infos);
  eg_heap_free(s_renderer_current->camera_infos);
}
static void eg_renderer_destroy_debug_buffers(void) {
  uint32_t frame_index = 0;
  uint32_t frame_count = s_renderer_current->frames_in_flight;

  while (frame_index < frame_count) {

    eg_buffer_destroy(s_renderer_current->debug_line_vertex_buffers[frame_index]);
    eg_buffer_destroy(s_renderer_current->debug_line_index_buffers[frame_index]);

    frame_index++;
  }

  eg_heap_free(s_renderer_current->debug_line_vertex_buffers);
  eg_heap_free(s_renderer_current->debug_line_index_buffers);

  eg_heap_free(s_renderer_current->debug_line_vertices);
  eg_heap_free(s_renderer_current->debug_line_indices);

  eg_heap_free(s_renderer_current->debug_line_vertex_offset);
  eg_heap_free(s_renderer_current->debug_line_index_offset);
}
static void eg_renderer_destroy_pipelines(void) {
  uint64_t pipeline_link_index = 0;
  uint64_t pipeline_link_count = EG_ARRAY_COUNT(s_renderer_current->pipeline_link);

  while (pipeline_link_index < pipeline_link_count) {

    if (s_renderer_current->pipeline_link[pipeline_link_index]) {

      switch (s_renderer_current->pipeline_type[pipeline_link_index]) {
        case EG_PIPELINE_TYPE_GRAPHIC: {

          eg_graphic_pipeline_destroy(s_renderer_current->pipeline_link[pipeline_link_index]);

          break;
        }
        case EG_PIPELINE_TYPE_COMPUTE: {

          eg_compute_pipeline_destroy(s_renderer_current->pipeline_link[pipeline_link_index]);

          break;
        }
      }
    }

    s_renderer_current->pipeline_type[pipeline_link_index] = 0;
    s_renderer_current->pipeline_link[pipeline_link_index] = 0;

    pipeline_link_index++;
  }
}
static void eg_renderer_destroy_sync_objects(void) {
  uint32_t image_index = 0;
  uint32_t image_count = eg_swapchain_image_count();

  while (image_index < image_count) {

    vkDestroySemaphore(eg_context_device(), s_renderer_current->render_finished_semaphore[image_index], 0);

    image_index++;
  }

  uint32_t frame_index = 0;
  uint32_t frame_count = s_renderer_current->frames_in_flight;

  while (frame_index < frame_count) {

    vkDestroySemaphore(eg_context_device(), s_renderer_current->image_available_semaphore[frame_index], 0);

    vkDestroyFence(eg_context_device(), s_renderer_current->frame_fence[frame_index], 0);

    frame_index++;
  }

  eg_heap_free(s_renderer_current->render_finished_semaphore);
  eg_heap_free(s_renderer_current->image_available_semaphore);
  eg_heap_free(s_renderer_current->frame_fence);
}
static void eg_renderer_destroy_command_buffer(void) {
  vkFreeCommandBuffers(eg_context_device(), eg_context_command_pool(), (uint32_t)s_renderer_current->frames_in_flight, s_renderer_current->command_buffer);

  eg_heap_free(s_renderer_current->command_buffer);
}

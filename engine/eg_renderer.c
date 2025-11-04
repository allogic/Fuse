#include <engine/eg_pch.h>
#include <engine/eg_buffer.h>
#include <engine/eg_context.h>
#include <engine/eg_pipeline.h>
#include <engine/eg_renderer.h>
#include <engine/eg_swapchain.h>

#define RENDERER_DEBUG_LINE_VERTEX_COUNT (1048576LL)
#define RENDERER_DEBUG_LINE_INDEX_COUNT (1048576LL)

#define RENDERER_MAKE_GROUP_COUNT(GLOBAL_SIZE, LOCAL_SIZE) ((int32_t)ceil((double)(GLOBAL_SIZE) / (LOCAL_SIZE)))

// TODO: implement sparse textures..

typedef enum renderer_pipeline_link_type_t {
  RENDERER_PIPELINE_LINK_TYPE_DEBUG,
  RENDERER_PIPELINE_LINK_TYPE_PBR,
} renderer_pipeline_link_type_t;

static void renderer_link_pipeline(renderer_t *renderer, uint32_t link_index, uint32_t pipeline_type, void *pipeline);

static void renderer_create_global_buffers(renderer_t *renderer);
static void renderer_create_debug_buffers(renderer_t *renderer);
static void renderer_create_pipelines(renderer_t *renderer);
static void renderer_create_sync_objects(renderer_t *renderer);
static void renderer_create_command_buffer(renderer_t *renderer);

static void renderer_update_uniform_buffers(renderer_t *renderer);

static void renderer_record_compute_commands(renderer_t *renderer);
static void renderer_record_graphic_commands(renderer_t *renderer);

static void renderer_gbuffer_pass(renderer_t *renderer);
static void renderer_imgui_pass(renderer_t *renderer);

static void renderer_destroy_global_buffers(renderer_t *renderer);
static void renderer_destroy_debug_buffers(renderer_t *renderer);
static void renderer_destroy_pipelines(renderer_t *renderer);
static void renderer_destroy_sync_objects(renderer_t *renderer);
static void renderer_destroy_command_buffer(renderer_t *renderer);

void renderer_create(context_t *context) {
  renderer_t *renderer = (renderer_t *)heap_alloc(sizeof(renderer_t), 1, 0);

  renderer->context = context;
  renderer->context->renderer = renderer;
  renderer->is_debug_enabled = 1; // TODO

  strcpy(renderer->time_info_descriptor_binding_name, "time_info");
  strcpy(renderer->screen_info_descriptor_binding_name, "screen_info");
  strcpy(renderer->camera_info_descriptor_binding_name, "camera_info");

  renderer_asset_t renderer_asset = database_load_renderer_default_asset();

  renderer->frames_in_flight = renderer_asset.frames_in_flight;

  renderer->frames_in_flight = MAX(renderer->frames_in_flight, 1);
  renderer->frames_in_flight = MIN(renderer->frames_in_flight, renderer->context->swapchain->image_count);

  renderer->frame_index = 0;
  renderer->image_index = 0;

  memset(renderer->pipeline_types, 0, sizeof(renderer->pipeline_types));
  memset(renderer->pipeline_links, 0, sizeof(renderer->pipeline_links));

  renderer_create_global_buffers(renderer);
  renderer_create_debug_buffers(renderer);
  renderer_create_pipelines(renderer);
  renderer_create_sync_objects(renderer);
  renderer_create_command_buffer(renderer);

  if (g_context_imgui_create_proc) {
    g_context_imgui_create_proc(renderer->context);
  }

  database_destroy_renderer_asset(&renderer_asset);
}
void renderer_update(renderer_t *renderer) {
  vector3_t right_position = {0.0F, 0.0F, 0.0F};
  vector3_t right_direction = {1.0F, 0.0F, 0.0F};
  vector4_t right_color = {1.0F, 0.0F, 0.0F, 1.0F};

  renderer_draw_debug_line(renderer, right_position, right_direction, right_color);

  vector3_t up_position = {0.0F, 0.0F, 0.0F};
  vector3_t up_direction = {0.0F, 1.0F, 0.0F};
  vector4_t up_color = {0.0F, 1.0F, 0.0F, 1.0F};

  renderer_draw_debug_line(renderer, up_position, up_direction, up_color);

  vector3_t front_position = {0.0F, 0.0F, 0.0F};
  vector3_t front_direction = {0.0F, 0.0F, 1.0F};
  vector4_t front_color = {0.0F, 0.0F, 1.0F, 1.0F};

  renderer_draw_debug_line(renderer, front_position, front_direction, front_color);
}
void renderer_draw(renderer_t *renderer) {
  VkResult result = VK_SUCCESS;

  result = vkWaitForFences(renderer->context->device, 1, &renderer->frame_fences[renderer->frame_index], 1, UINT64_MAX);

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

  result = vkResetFences(renderer->context->device, 1, &renderer->frame_fences[renderer->frame_index]);

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

  result = vkResetCommandBuffer(renderer->graphic_command_buffers[renderer->frame_index], 0);

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

  result = vkAcquireNextImageKHR(renderer->context->device, renderer->context->swapchain->handle, UINT64_MAX, renderer->image_available_semaphores[renderer->frame_index], 0, &renderer->image_index);

  switch (result) {
    case VK_SUCCESS: {
      break;
    }
    case VK_NOT_READY: {
      return;
    }
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR: {
      renderer->context->swapchain->is_dirty = 1;

      return;
    }
#ifdef BUILD_DEBUG
    default: {
      __debugbreak();
    }
#endif // BUILD_DEBUG
  }

  renderer_update_uniform_buffers(renderer);

  VkCommandBufferBeginInfo command_buffer_begin_info = {0};
  command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  command_buffer_begin_info.pInheritanceInfo = 0;

  VULKAN_CHECK(vkBeginCommandBuffer(renderer->graphic_command_buffers[renderer->frame_index], &command_buffer_begin_info));

  renderer_record_compute_commands(renderer);
  renderer_record_graphic_commands(renderer);

  VULKAN_CHECK(vkEndCommandBuffer(renderer->graphic_command_buffers[renderer->frame_index]));

  // TODO: re-validate VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
  VkPipelineStageFlags graphic_wait_stages[] = {VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  VkSubmitInfo graphic_submit_info = {0};
  graphic_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  graphic_submit_info.pWaitSemaphores = &renderer->image_available_semaphores[renderer->frame_index];
  graphic_submit_info.waitSemaphoreCount = 1;
  graphic_submit_info.pSignalSemaphores = &renderer->render_finished_semaphores[renderer->image_index];
  graphic_submit_info.signalSemaphoreCount = 1;
  graphic_submit_info.pCommandBuffers = &renderer->graphic_command_buffers[renderer->frame_index];
  graphic_submit_info.commandBufferCount = 1;
  graphic_submit_info.pWaitDstStageMask = graphic_wait_stages;

  result = vkQueueSubmit(renderer->context->graphic_queue, 1, &graphic_submit_info, renderer->frame_fences[renderer->frame_index]);

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
  present_info.pWaitSemaphores = &renderer->render_finished_semaphores[renderer->image_index];
  present_info.waitSemaphoreCount = 1;
  present_info.pSwapchains = &renderer->context->swapchain->handle;
  present_info.swapchainCount = 1;
  present_info.pImageIndices = &renderer->image_index;

  result = vkQueuePresentKHR(renderer->context->present_queue, &present_info);

  switch (result) {
    case VK_SUCCESS: {
      break;
    }
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR: {
      renderer->context->swapchain->is_dirty = 1;

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
void renderer_destroy(renderer_t *renderer) {
  VULKAN_CHECK(vkQueueWaitIdle(renderer->context->graphic_queue));
  VULKAN_CHECK(vkQueueWaitIdle(renderer->context->present_queue));

  if (g_context_imgui_destroy_proc) {
    g_context_imgui_destroy_proc(renderer->context);
  }

  renderer_destroy_global_buffers(renderer);
  renderer_destroy_debug_buffers(renderer);
  renderer_destroy_pipelines(renderer);
  renderer_destroy_sync_objects(renderer);
  renderer_destroy_command_buffer(renderer);

  renderer->context->renderer = 0;

  heap_free(renderer);
}

void renderer_draw_debug_line(renderer_t *renderer, vector3_t from, vector3_t to, vector4_t color) {
  if (renderer->is_debug_enabled) {
    graphic_pipeline_t *debug_pipeline = (graphic_pipeline_t *)renderer->pipeline_links[RENDERER_PIPELINE_LINK_TYPE_DEBUG];

    if (debug_pipeline) {
      uint32_t vertex_offset = renderer->debug_line_vertex_offsets[renderer->frame_index];
      uint32_t index_offset = renderer->debug_line_index_offsets[renderer->frame_index];

      debug_vertex_t *vertices = renderer->debug_line_vertices[renderer->frame_index];
      uint32_t *indices = renderer->debug_line_indices[renderer->frame_index];

      vertices[vertex_offset + 0].position = from;
      vertices[vertex_offset + 1].position = to;

      vertices[vertex_offset + 0].color = color;
      vertices[vertex_offset + 1].color = color;

      indices[index_offset + 0] = (uint32_t)(vertex_offset + 0);
      indices[index_offset + 1] = (uint32_t)(vertex_offset + 1);

      renderer->debug_line_vertex_offsets[renderer->frame_index] += 2;
      renderer->debug_line_index_offsets[renderer->frame_index] += 2;
    }
  }
}
void renderer_draw_debug_box(renderer_t *renderer, vector3_t position, vector3_t size, vector4_t color) {
  if (renderer->is_debug_enabled) {
    graphic_pipeline_t *debug_pipeline = (graphic_pipeline_t *)renderer->pipeline_links[RENDERER_PIPELINE_LINK_TYPE_DEBUG];

    if (debug_pipeline) {
      uint32_t vertex_offset = renderer->debug_line_vertex_offsets[renderer->frame_index];
      uint32_t index_offset = renderer->debug_line_index_offsets[renderer->frame_index];

      debug_vertex_t *vertices = renderer->debug_line_vertices[renderer->frame_index];
      uint32_t *indices = renderer->debug_line_indices[renderer->frame_index];

      vertices[vertex_offset + 0].position = (vector3_t){position.x, position.y, position.z};
      vertices[vertex_offset + 1].position = (vector3_t){position.x, position.y + size.y, position.z};
      vertices[vertex_offset + 2].position = (vector3_t){position.x + size.x, position.y, position.z};
      vertices[vertex_offset + 3].position = (vector3_t){position.x + size.x, position.y + size.y, position.z};
      vertices[vertex_offset + 4].position = (vector3_t){position.x, position.y, position.z + size.z};
      vertices[vertex_offset + 5].position = (vector3_t){position.x, position.y + size.y, position.z + size.z};
      vertices[vertex_offset + 6].position = (vector3_t){position.x + size.x, position.y, position.z + size.z};
      vertices[vertex_offset + 7].position = (vector3_t){position.x + size.x, position.y + size.y, position.z + size.z};

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

      renderer->debug_line_vertex_offsets[renderer->frame_index] += 8;
      renderer->debug_line_index_offsets[renderer->frame_index] += 24;
    }
  }
}

static void renderer_link_pipeline(renderer_t *renderer, uint32_t link_index, uint32_t pipeline_type, void *pipeline) {
  if (renderer->pipeline_links[link_index]) {

    switch (renderer->pipeline_types[link_index]) {
      case 0: {
        graphic_pipeline_destroy(renderer->pipeline_links[link_index]);

        break;
      }
      case 1: {
        compute_pipeline_destroy(renderer->pipeline_links[link_index]);

        break;
      }
    }
  }

  renderer->pipeline_types[link_index] = pipeline_type;
  renderer->pipeline_links[link_index] = pipeline;
}

static void renderer_create_global_buffers(renderer_t *renderer) {
  renderer->time_infos = (time_info_t **)heap_alloc(sizeof(time_info_t *) * renderer->frames_in_flight, 0, 0);
  renderer->screen_infos = (screen_info_t **)heap_alloc(sizeof(screen_info_t *) * renderer->frames_in_flight, 0, 0);
  renderer->camera_infos = (camera_info_t **)heap_alloc(sizeof(camera_info_t *) * renderer->frames_in_flight, 0, 0);

  renderer->descriptor_binding_buffers_per_frame = (map_t *)heap_alloc(sizeof(map_t) * renderer->frames_in_flight, 0, 0);

  uint64_t frame_index = 0;
  uint64_t frame_count = renderer->frames_in_flight;

  while (frame_index < frame_count) {

    renderer->descriptor_binding_buffers_per_frame[frame_index] = map_create();

    buffer_t *time_info_descriptor_binding_buffer = buffer_create_uniform_coherent(renderer->context, sizeof(time_info_t));
    buffer_t *screen_info_descriptor_binding_buffer = buffer_create_uniform_coherent(renderer->context, sizeof(screen_info_t));
    buffer_t *camera_info_descriptor_binding_buffer = buffer_create_uniform_coherent(renderer->context, sizeof(camera_info_t));

    renderer->time_infos[frame_index] = time_info_descriptor_binding_buffer->mapped_memory;
    renderer->screen_infos[frame_index] = screen_info_descriptor_binding_buffer->mapped_memory;
    renderer->camera_infos[frame_index] = camera_info_descriptor_binding_buffer->mapped_memory;

    map_insert(&renderer->descriptor_binding_buffers_per_frame[frame_index], renderer->time_info_descriptor_binding_name, strlen(renderer->time_info_descriptor_binding_name) + 1, &time_info_descriptor_binding_buffer, sizeof(buffer_t *));
    map_insert(&renderer->descriptor_binding_buffers_per_frame[frame_index], renderer->screen_info_descriptor_binding_name, strlen(renderer->screen_info_descriptor_binding_name) + 1, &screen_info_descriptor_binding_buffer, sizeof(buffer_t *));
    map_insert(&renderer->descriptor_binding_buffers_per_frame[frame_index], renderer->camera_info_descriptor_binding_name, strlen(renderer->camera_info_descriptor_binding_name) + 1, &camera_info_descriptor_binding_buffer, sizeof(buffer_t *));

    frame_index++;
  }
}
static void renderer_create_debug_buffers(renderer_t *renderer) {
  renderer->debug_line_vertex_offsets = (uint32_t *)heap_alloc(sizeof(uint32_t) * renderer->frames_in_flight, 1, 0);
  renderer->debug_line_index_offsets = (uint32_t *)heap_alloc(sizeof(uint32_t) * renderer->frames_in_flight, 1, 0);

  renderer->debug_line_vertices = (debug_vertex_t **)heap_alloc(sizeof(debug_vertex_t *) * renderer->frames_in_flight, 0, 0);
  renderer->debug_line_indices = (uint32_t **)heap_alloc(sizeof(uint32_t *) * renderer->frames_in_flight, 0, 0);

  renderer->debug_line_vertex_buffers = (buffer_t **)heap_alloc(sizeof(buffer_t *) * renderer->frames_in_flight, 0, 0);
  renderer->debug_line_index_buffers = (buffer_t **)heap_alloc(sizeof(buffer_t *) * renderer->frames_in_flight, 0, 0);

  uint64_t frame_index = 0;
  uint64_t frame_count = renderer->frames_in_flight;

  while (frame_index < frame_count) {

    renderer->debug_line_vertex_buffers[frame_index] = buffer_create_vertex_coherent(renderer->context, sizeof(debug_vertex_t) * RENDERER_DEBUG_LINE_VERTEX_COUNT);
    renderer->debug_line_index_buffers[frame_index] = buffer_create_index_coherent(renderer->context, sizeof(uint32_t) * RENDERER_DEBUG_LINE_INDEX_COUNT);

    renderer->debug_line_vertices[frame_index] = renderer->debug_line_vertex_buffers[frame_index]->mapped_memory;
    renderer->debug_line_indices[frame_index] = renderer->debug_line_index_buffers[frame_index]->mapped_memory;

    frame_index++;
  }
}
static void renderer_create_pipelines(renderer_t *renderer) {
  vector_t graphic_pipeline_assets = database_load_pipeline_assets_by_type(0);

  uint64_t graphic_pipeline_asset_index = 0;
  uint64_t graphic_pipeline_asset_count = vector_count(&graphic_pipeline_assets);

  while (graphic_pipeline_asset_index < graphic_pipeline_asset_count) {

    pipeline_asset_t *pipeline_asset = (pipeline_asset_t *)vector_at(&graphic_pipeline_assets, graphic_pipeline_asset_index);

    if (pipeline_asset->auto_create_pipeline) {

      graphic_pipeline_t *pipeline = graphic_pipeline_create(renderer->context, pipeline_asset);

      uint64_t frame_index = 0;
      uint64_t frame_count = renderer->frames_in_flight;

      while (frame_index < frame_count) {

        // TODO: remove this!
        graphic_pipeline_link_vertex_input_binding_buffer(pipeline, frame_index, 0, renderer->debug_line_vertex_buffers[frame_index]->handle, 0);
        graphic_pipeline_link_index_buffer(pipeline, frame_index, renderer->debug_line_index_buffers[frame_index]->handle);

        if (pipeline_asset->auto_link_descriptor_bindings) {
          graphic_pipeline_set_auto_link_descriptor_bindings(pipeline, renderer->descriptor_binding_buffers_per_frame);
        }

        frame_index++;
      }

      graphic_pipeline_allocate_descriptor_sets(pipeline, 1);
      graphic_pipeline_update_descriptor_sets(pipeline);

      renderer_link_pipeline(renderer, pipeline_asset->link_index, pipeline_asset->type, pipeline);
    }

    graphic_pipeline_asset_index++;
  }

  database_destroy_pipeline_assets(&graphic_pipeline_assets);

  vector_t compute_pipeline_assets = database_load_pipeline_assets_by_type(1);

  uint64_t compute_pipeline_asset_index = 0;
  uint64_t compute_pipeline_asset_count = vector_count(&compute_pipeline_assets);

  while (compute_pipeline_asset_index < compute_pipeline_asset_count) {

    pipeline_asset_t *pipeline_asset = (pipeline_asset_t *)vector_at(&compute_pipeline_assets, compute_pipeline_asset_index);

    if (pipeline_asset->auto_create_pipeline) {

      compute_pipeline_t *pipeline = compute_pipeline_create(renderer->context, pipeline_asset);

      uint64_t frame_index = 0;
      uint64_t frame_count = renderer->frames_in_flight;

      while (frame_index < frame_count) {

        if (pipeline_asset->auto_link_descriptor_bindings) {
          compute_pipeline_set_auto_link_descriptor_bindings(pipeline, renderer->descriptor_binding_buffers_per_frame);
        }

        frame_index++;
      }

      compute_pipeline_allocate_descriptor_sets(pipeline, 1);
      compute_pipeline_update_descriptor_sets(pipeline);

      renderer_link_pipeline(renderer, pipeline_asset->link_index, pipeline_asset->type, pipeline);
    }

    compute_pipeline_asset_index++;
  }

  database_destroy_pipeline_assets(&compute_pipeline_assets);
}
static void renderer_create_sync_objects(renderer_t *renderer) {
  renderer->render_finished_semaphores = (VkSemaphore *)heap_alloc(sizeof(VkSemaphore) * renderer->context->swapchain->image_count, 0, 0);
  renderer->image_available_semaphores = (VkSemaphore *)heap_alloc(sizeof(VkSemaphore) * renderer->frames_in_flight, 0, 0);
  renderer->frame_fences = (VkFence *)heap_alloc(sizeof(VkFence) * renderer->frames_in_flight, 0, 0);

  VkSemaphoreCreateInfo semaphore_create_info = {0};
  semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphore_create_info.flags = 0;

  VkFenceCreateInfo fence_create_info = {0};
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  uint64_t image_index = 0;
  uint64_t image_count = renderer->context->swapchain->image_count;

  while (image_index < image_count) {

    VULKAN_CHECK(vkCreateSemaphore(renderer->context->device, &semaphore_create_info, 0, &renderer->render_finished_semaphores[image_index]));

    image_index++;
  }

  uint64_t frame_index = 0;
  uint64_t frame_count = renderer->frames_in_flight;

  while (frame_index < frame_count) {

    VULKAN_CHECK(vkCreateSemaphore(renderer->context->device, &semaphore_create_info, 0, &renderer->image_available_semaphores[frame_index]));
    VULKAN_CHECK(vkCreateFence(renderer->context->device, &fence_create_info, 0, &renderer->frame_fences[frame_index]));

    frame_index++;
  }
}
static void renderer_create_command_buffer(renderer_t *renderer) {
  renderer->graphic_command_buffers = (VkCommandBuffer *)heap_alloc(sizeof(VkCommandBuffer) * renderer->frames_in_flight, 0, 0);

  VkCommandBufferAllocateInfo command_buffer_alloc_create_info = {0};
  command_buffer_alloc_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_alloc_create_info.commandPool = renderer->context->command_pool;
  command_buffer_alloc_create_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_alloc_create_info.commandBufferCount = (uint32_t)renderer->frames_in_flight;

  VULKAN_CHECK(vkAllocateCommandBuffers(renderer->context->device, &command_buffer_alloc_create_info, renderer->graphic_command_buffers));
}

static void renderer_update_uniform_buffers(renderer_t *renderer) {
  scene_t *scene = renderer->context->scene;

  if (scene) {

    // TODO: find an easier way to compute world position's automatically..
    transform_compute_world_position(scene->world, scene->player);
    transform_compute_world_rotation(scene->world, scene->player);
    transform_compute_world_scale(scene->world, scene->player);

    transform_t *transform = ecs_get_mut(scene->world, scene->player, transform_t);
    camera_t *camera = ecs_get_mut(scene->world, scene->player, camera_t);

    renderer->time_infos[renderer->frame_index]->time = (float)renderer->context->time;
    renderer->time_infos[renderer->frame_index]->delta_time = (float)renderer->context->delta_time;

    vector2_t resolution = vector2_xy((float)renderer->context->window_width, (float)renderer->context->window_height);

    renderer->screen_infos[renderer->frame_index]->resolution = resolution;

    vector3_t eye = transform->world_position;
    vector3_t center = vector3_add(transform->world_position, transform_local_front(transform));
    vector3_t up = vector3_down();

    float fov = deg_to_rad(camera->fov);
    float aspect_ratio = (float)renderer->context->window_width / (float)renderer->context->window_height;
    float near_z = camera->near_z;
    float far_z = camera->far_z;

    matrix4_t view = matrix4_look_at(eye, center, up);
    matrix4_t projection = matrix4_persp(fov, aspect_ratio, near_z, far_z);
    matrix4_t view_projection = matrix4_mul(view, projection);
    matrix4_t view_projection_inv = matrix4_inverse(view_projection);

    renderer->camera_infos[renderer->frame_index]->position = transform->world_position;
    renderer->camera_infos[renderer->frame_index]->view = view;
    renderer->camera_infos[renderer->frame_index]->projection = projection;
    renderer->camera_infos[renderer->frame_index]->view_projection = view_projection;
    renderer->camera_infos[renderer->frame_index]->view_projection_inv = view_projection_inv;
  }
}

static void renderer_record_compute_commands(renderer_t *renderer) {
  // TODO
  // vkCmdBindPipeline(g_renderer_graphic_command_buffers[g_renderer_frame_index], VK_PIPELINE_BIND_POINT_COMPUTE, s_renderer_chunk_editor_pipeline);
  // vkCmdBindDescriptorSets(g_renderer_graphic_command_buffers[g_renderer_frame_index], VK_PIPELINE_BIND_POINT_COMPUTE, s_renderer_chunk_editor_pipeline_layout, 0, 1, &s_renderer_chunk_editor_descriptor_sets[s_renderer_frame_index], 0, 0);
  // vkCmdDispatch(g_renderer_graphic_command_buffers[g_renderer_frame_index], group_count_x, group_count_y, group_count_z);
}
static void renderer_record_graphic_commands(renderer_t *renderer) {
  renderer_gbuffer_pass(renderer);

  if (renderer->context->is_editor_mode) {

    g_context_imgui_pre_draw_proc(renderer->context);

    renderer_imgui_pass(renderer);

    g_context_imgui_post_draw_proc(renderer->context);
  }
}

static void renderer_gbuffer_pass(renderer_t *renderer) {
  VkCommandBuffer command_buffer = renderer->graphic_command_buffers[renderer->frame_index];

  if (renderer->context->is_editor_mode) {

    uint64_t viewport_index = 0;
    uint64_t viewport_count = g_context_imgui_viewport_count_proc(renderer->context);

    while (viewport_index < viewport_count) {

      VkClearValue color_clear_value = {0};
      color_clear_value.color.float32[0] = 0.0F;
      color_clear_value.color.float32[1] = 0.0F;
      color_clear_value.color.float32[2] = 0.0F;
      color_clear_value.color.float32[3] = 1.0F;

      VkClearValue depth_clear_value = {0};
      depth_clear_value.depthStencil.depth = 1.0F;
      depth_clear_value.depthStencil.stencil = 0;

      VkClearValue clear_values[] = {color_clear_value, depth_clear_value};

      uint32_t width = g_context_imgui_viewport_width_proc(renderer->context, viewport_index);
      uint32_t height = g_context_imgui_viewport_height_proc(renderer->context, viewport_index);

      VkRenderPassBeginInfo render_pass_create_info = {0};
      render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      render_pass_create_info.renderPass = renderer->context->swapchain->gbuffer_render_pass;
      render_pass_create_info.framebuffer = renderer->context->swapchain->gbuffer_frame_buffers[viewport_index][renderer->image_index];
      render_pass_create_info.renderArea.offset.x = 0;
      render_pass_create_info.renderArea.offset.y = 0;
      render_pass_create_info.renderArea.extent.width = width;
      render_pass_create_info.renderArea.extent.height = height;
      render_pass_create_info.pClearValues = clear_values;
      render_pass_create_info.clearValueCount = ARRAY_COUNT(clear_values);

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

        graphic_pipeline_t *debug_pipeline = (graphic_pipeline_t *)renderer->pipeline_links[RENDERER_PIPELINE_LINK_TYPE_DEBUG];

        if (debug_pipeline) {

          uint32_t index_count = renderer->debug_line_index_offsets[renderer->frame_index];

          graphic_pipeline_execute(debug_pipeline, command_buffer, index_count);
        }

        renderer->debug_line_vertex_offsets[renderer->frame_index] = 0;
        renderer->debug_line_index_offsets[renderer->frame_index] = 0;
      }

      vkCmdEndRenderPass(command_buffer);

      viewport_index++;
    }

  } else {

    VkClearValue color_clear_value = {0};
    color_clear_value.color.float32[0] = 0.0F;
    color_clear_value.color.float32[1] = 0.0F;
    color_clear_value.color.float32[2] = 0.0F;
    color_clear_value.color.float32[3] = 1.0F;

    VkClearValue depth_clear_value = {0};
    depth_clear_value.depthStencil.depth = 1.0F;
    depth_clear_value.depthStencil.stencil = 0;

    VkClearValue clear_values[] = {color_clear_value, depth_clear_value};

    uint32_t width = renderer->context->window_width;
    uint32_t height = renderer->context->window_height;

    VkRenderPassBeginInfo render_pass_create_info = {0};
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_create_info.renderPass = renderer->context->swapchain->gbuffer_render_pass;
    render_pass_create_info.framebuffer = renderer->context->swapchain->gbuffer_frame_buffers[0][renderer->image_index];
    render_pass_create_info.renderArea.offset.x = 0;
    render_pass_create_info.renderArea.offset.y = 0;
    render_pass_create_info.renderArea.extent.width = width;
    render_pass_create_info.renderArea.extent.height = height;
    render_pass_create_info.pClearValues = clear_values;
    render_pass_create_info.clearValueCount = ARRAY_COUNT(clear_values);

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

      graphic_pipeline_t *debug_pipeline = (graphic_pipeline_t *)renderer->pipeline_links[RENDERER_PIPELINE_LINK_TYPE_DEBUG];

      if (debug_pipeline) {

        uint32_t index_count = renderer->debug_line_index_offsets[renderer->frame_index];

        graphic_pipeline_execute(debug_pipeline, command_buffer, index_count);
      }

      renderer->debug_line_vertex_offsets[renderer->frame_index] = 0;
      renderer->debug_line_index_offsets[renderer->frame_index] = 0;
    }

    vkCmdEndRenderPass(command_buffer);
  }
}
static void renderer_imgui_pass(renderer_t *renderer) {
  VkCommandBuffer command_buffer = renderer->graphic_command_buffers[renderer->frame_index];

  VkClearValue color_clear_value = {0};
  color_clear_value.color.float32[0] = 0.0F;
  color_clear_value.color.float32[1] = 0.0F;
  color_clear_value.color.float32[2] = 0.0F;
  color_clear_value.color.float32[3] = 1.0F;

  VkClearValue depth_clear_value = {0};
  depth_clear_value.depthStencil.depth = 1.0F;
  depth_clear_value.depthStencil.stencil = 0;

  VkClearValue clear_values[] = {color_clear_value, depth_clear_value};

  uint32_t width = renderer->context->window_width;
  uint32_t height = renderer->context->window_height;

  VkRenderPassBeginInfo render_pass_create_info = {0};
  render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_create_info.renderPass = renderer->context->swapchain->imgui_render_pass;
  render_pass_create_info.framebuffer = renderer->context->swapchain->imgui_frame_buffers[renderer->image_index];
  render_pass_create_info.renderArea.offset.x = 0;
  render_pass_create_info.renderArea.offset.y = 0;
  render_pass_create_info.renderArea.extent.width = width;
  render_pass_create_info.renderArea.extent.height = height;
  render_pass_create_info.pClearValues = clear_values;
  render_pass_create_info.clearValueCount = ARRAY_COUNT(clear_values);

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

  g_context_imgui_draw_proc(renderer->context);

  vkCmdEndRenderPass(command_buffer);
}

static void renderer_destroy_global_buffers(renderer_t *renderer) {
  uint64_t frame_index = 0;
  uint64_t frame_count = renderer->frames_in_flight;

  while (frame_index < frame_count) {

    map_iter_t iter = map_iter(&renderer->descriptor_binding_buffers_per_frame[frame_index]);

    while (map_iter_step(&iter)) {

      buffer_t *buffer = *(buffer_t **)map_iter_value(&iter);

      buffer_destroy(buffer);
    }

    map_destroy(&renderer->descriptor_binding_buffers_per_frame[frame_index]);

    frame_index++;
  }

  heap_free(renderer->descriptor_binding_buffers_per_frame);

  heap_free(renderer->time_infos);
  heap_free(renderer->screen_infos);
  heap_free(renderer->camera_infos);
}
static void renderer_destroy_debug_buffers(renderer_t *renderer) {
  uint64_t frame_index = 0;
  uint64_t frame_count = renderer->frames_in_flight;

  while (frame_index < frame_count) {

    buffer_destroy(renderer->debug_line_vertex_buffers[frame_index]);
    buffer_destroy(renderer->debug_line_index_buffers[frame_index]);

    frame_index++;
  }

  heap_free(renderer->debug_line_vertex_buffers);
  heap_free(renderer->debug_line_index_buffers);

  heap_free(renderer->debug_line_vertices);
  heap_free(renderer->debug_line_indices);

  heap_free(renderer->debug_line_vertex_offsets);
  heap_free(renderer->debug_line_index_offsets);
}
static void renderer_destroy_pipelines(renderer_t *renderer) {
  uint64_t pipeline_link_index = 0;
  uint64_t pipeline_link_count = ARRAY_COUNT(renderer->pipeline_links);

  while (pipeline_link_index < pipeline_link_count) {

    if (renderer->pipeline_links[pipeline_link_index]) {

      switch (renderer->pipeline_types[pipeline_link_index]) {
        case 0: {
          graphic_pipeline_destroy(renderer->pipeline_links[pipeline_link_index]);
          break;
        }
        case 1: {
          compute_pipeline_destroy(renderer->pipeline_links[pipeline_link_index]);
          break;
        }
      }
    }

    renderer->pipeline_types[pipeline_link_index] = 0;
    renderer->pipeline_links[pipeline_link_index] = 0;

    pipeline_link_index++;
  }
}
static void renderer_destroy_sync_objects(renderer_t *renderer) {
  uint64_t image_index = 0;
  uint64_t image_count = renderer->context->swapchain->image_count;

  while (image_index < image_count) {

    vkDestroySemaphore(renderer->context->device, renderer->render_finished_semaphores[image_index], 0);

    image_index++;
  }

  uint64_t frame_index = 0;
  uint64_t frame_count = renderer->frames_in_flight;

  while (frame_index < frame_count) {

    vkDestroySemaphore(renderer->context->device, renderer->image_available_semaphores[frame_index], 0);

    vkDestroyFence(renderer->context->device, renderer->frame_fences[frame_index], 0);

    frame_index++;
  }

  heap_free(renderer->render_finished_semaphores);
  heap_free(renderer->image_available_semaphores);
  heap_free(renderer->frame_fences);
}
static void renderer_destroy_command_buffer(renderer_t *renderer) {
  vkFreeCommandBuffers(renderer->context->device, renderer->context->command_pool, (uint32_t)renderer->frames_in_flight, renderer->graphic_command_buffers);

  heap_free(renderer->graphic_command_buffers);
}

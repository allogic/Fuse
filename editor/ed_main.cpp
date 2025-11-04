#include <editor/ed_pch.h>
#include <editor/ed_main.h>
#include <editor/ed_catalog.h>
#include <editor/ed_hierarchy.h>
#include <editor/ed_detail.h>
#include <editor/ed_dockspace.h>
#include <editor/ed_inspector.h>
#include <editor/ed_titlebar.h>
#include <editor/ed_statusbar.h>
#include <editor/ed_viewport.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

static void imgui_create(context_t *context);
static void imgui_pre_draw(context_t *context);
static void imgui_draw(context_t *context);
static void imgui_post_draw(context_t *context);
static void imgui_destroy(context_t *context);

static uint64_t imgui_viewport_count(context_t *context);
static uint32_t imgui_viewport_width(context_t *context, uint64_t index);
static uint32_t imgui_viewport_height(context_t *context, uint64_t index);

static int32_t ImGui_CreateSurfaceDummy(ImGuiViewport *viewport, ImU64 instance, const void *allocators, ImU64 *out_surface);

ImFont *g_editor_commit_mono_h1 = 0;
ImFont *g_editor_commit_mono_h2 = 0;
ImFont *g_editor_commit_mono_h3 = 0;
ImFont *g_editor_commit_mono_h4 = 0;
ImFont *g_editor_commit_mono_h5 = 0;
ImFont *g_editor_commit_mono_h6 = 0;
ImFont *g_editor_commit_mono = 0;

ImFont *g_editor_material_symbols_h1 = 0;
ImFont *g_editor_material_symbols_h2 = 0;
ImFont *g_editor_material_symbols_h3 = 0;
ImFont *g_editor_material_symbols_h4 = 0;
ImFont *g_editor_material_symbols_h5 = 0;
ImFont *g_editor_material_symbols_h6 = 0;
ImFont *g_editor_material_symbols = 0;

viewport_t *g_editor_viewports[4] = {};

static VkDescriptorPool s_editor_descriptor_pool = 0;

int32_t main(int32_t argc, char **argv) {
  g_context_imgui_create_proc = imgui_create;
  g_context_imgui_pre_draw_proc = imgui_pre_draw;
  g_context_imgui_draw_proc = imgui_draw;
  g_context_imgui_post_draw_proc = imgui_post_draw;
  g_context_imgui_destroy_proc = imgui_destroy;
  g_context_imgui_viewport_count_proc = imgui_viewport_count;
  g_context_imgui_viewport_width_proc = imgui_viewport_width;
  g_context_imgui_viewport_height_proc = imgui_viewport_height;
  g_context_imgui_message_proc = ImGui_ImplWin32_WndProcHandler;

  context_t *context = context_create(1920, 1080, 1);

  // TODO: move scene stuff somewhere else
  context->scene = scene_create(context);

  while (context_is_running(context)) {

    context_begin_frame(context);

    scene_update(context->scene);

    // TODO: remove these render calls, inline them into the context itself..
    renderer_update(context->renderer);
    renderer_draw(context->renderer);

    context_end_frame(context);
  }

  scene_destroy(context->scene);

  uint64_t viewport_index = 0;
  uint64_t viewport_count = 4;

  while (viewport_index < viewport_count) {

    if (g_editor_viewports[viewport_index]) {
      viewport_destroy(g_editor_viewports[viewport_index]);
    }

    viewport_index++;
  }

  context_destroy(context);

  heap_reset();

  return 0;
}

static void imgui_create(context_t *context) {
  // TODO: double check descriptor pool's..

  VkDescriptorPoolSize s_editor_descriptor_pool_sizes[] = {
    {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
  };

  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
  descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  descriptor_pool_create_info.pPoolSizes = s_editor_descriptor_pool_sizes;
  descriptor_pool_create_info.poolSizeCount = (uint32_t)ARRAY_COUNT(s_editor_descriptor_pool_sizes);
  descriptor_pool_create_info.maxSets = 1000 * (uint32_t)context->renderer->frames_in_flight;

  vkCreateDescriptorPool(context->device, &descriptor_pool_create_info, 0, &s_editor_descriptor_pool);

  IMGUI_CHECKVERSION();

  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  ImGuiPlatformIO &platform_io = ImGui::GetPlatformIO();
  ImGuiStyle &style = ImGui::GetStyle();

  static ImWchar icon_glyph_ranges[] = {
    (wchar_t)ICON_MIN_MS,
    (wchar_t)ICON_MAX_MS,
    0,
  };

  // TODO: integrate fonts into database..

  g_editor_commit_mono = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 14.0F);
  g_editor_commit_mono_h6 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 16.0F);
  g_editor_commit_mono_h5 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 18.0F);
  g_editor_commit_mono_h4 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 20.0F);
  g_editor_commit_mono_h3 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 22.0F);
  g_editor_commit_mono_h2 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 24.0F);
  g_editor_commit_mono_h1 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 26.0F);

  g_editor_material_symbols = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-outlined.ttf", 14.0F, 0, icon_glyph_ranges);
  g_editor_material_symbols_h6 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-outlined.ttf", 18.0F, 0, icon_glyph_ranges);
  g_editor_material_symbols_h5 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-outlined.ttf", 22.0F, 0, icon_glyph_ranges);
  g_editor_material_symbols_h4 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-outlined.ttf", 26.0F, 0, icon_glyph_ranges);
  g_editor_material_symbols_h3 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-outlined.ttf", 30.0F, 0, icon_glyph_ranges);
  g_editor_material_symbols_h2 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-outlined.ttf", 34.0F, 0, icon_glyph_ranges);
  g_editor_material_symbols_h1 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-outlined.ttf", 38.0F, 0, icon_glyph_ranges);

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigWindowsMoveFromTitleBarOnly = 1;

  platform_io.Platform_CreateVkSurface = ImGui_CreateSurfaceDummy;

  style.Alpha = 1.0F;
  style.DisabledAlpha = 1.0F;
  style.WindowPadding = ImVec2{0.0F, 0.0F};
  style.WindowRounding = 0.0F;
  style.WindowBorderSize = 0.0F;
  style.WindowTitleAlign = ImVec2{0.0F, 0.5F};
  style.WindowMenuButtonPosition = ImGuiDir_None;
  style.ChildRounding = 5.0F;
  style.ChildBorderSize = 0.0F;
  style.PopupRounding = 0.0F;
  style.FrameRounding = 0.0F;
  style.ItemSpacing = ImVec2{8.0F, 4.0F};
  style.ScrollbarSize = 15.0F;
  style.ScrollbarRounding = 5.0F;
  style.GrabRounding = 0.0F;
  style.TabRounding = 5.0F;
  style.TabBorderSize = 2.0F;
  style.TabCloseButtonMinWidthSelected = -1.0F;
  style.TabCloseButtonMinWidthUnselected = -1.0F;
  style.TabBarBorderSize = 0.0F;
  style.TabBarOverlineSize = 0.0F;
  style.DockingSeparatorSize = 10.0F;

  ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(50, 50, 50, 255));
  ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_TitleBg, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_TitleBgActive, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_ResizeGrip, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_DockingPreview, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_Tab, IM_COL32(50, 50, 50, 255));
  ImGui::PushStyleColor(ImGuiCol_TabHovered, IM_COL32(50, 50, 50, 255));
  ImGui::PushStyleColor(ImGuiCol_TabActive, IM_COL32(50, 50, 50, 255));
  ImGui::PushStyleColor(ImGuiCol_TabUnfocused, IM_COL32(50, 50, 50, 255));
  ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, IM_COL32(50, 50, 50, 255));
  ImGui::PushStyleColor(ImGuiCol_Separator, IM_COL32(70, 70, 70, 255));
  ImGui::PushStyleColor(ImGuiCol_SeparatorActive, IM_COL32(50, 50, 50, 255));
  ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, IM_COL32(50, 50, 50, 255));
  ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, IM_COL32(50, 50, 50, 255));
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, IM_COL32(30, 30, 30, 255));

  ImGui_ImplWin32_Init(context->window_handle);

  ImGui_ImplVulkan_InitInfo imgui_vulkan_init_info = {};
  imgui_vulkan_init_info.Instance = context->instance;
  imgui_vulkan_init_info.PhysicalDevice = context->physical_device;
  imgui_vulkan_init_info.Device = context->device;
  imgui_vulkan_init_info.QueueFamily = context->graphic_queue_index;
  imgui_vulkan_init_info.Queue = context->graphic_queue;
  imgui_vulkan_init_info.PipelineCache = 0;
  imgui_vulkan_init_info.DescriptorPool = s_editor_descriptor_pool;
  imgui_vulkan_init_info.PipelineInfoMain.RenderPass = context->swapchain->imgui_render_pass;
  imgui_vulkan_init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  imgui_vulkan_init_info.MinImageCount = context->surface_capabilities.minImageCount;
  imgui_vulkan_init_info.ImageCount = (uint32_t)context->swapchain->image_count;
  imgui_vulkan_init_info.Allocator = 0;
  imgui_vulkan_init_info.CheckVkResultFn = 0;

  ImGui_ImplVulkan_Init(&imgui_vulkan_init_info);

  titlebar_create(context);
  catalog_create(context);
  detail_create(context);
  hierarchy_create(context);
  inspector_create(context);
  dockspace_create(context);
  statusbar_create(context);
}
static void imgui_pre_draw(context_t *context) {
  VkCommandBuffer command_buffer = context->renderer->graphic_command_buffers[context->renderer->frame_index];

  uint64_t viewport_index = 0;
  uint64_t viewport_count = g_context_imgui_viewport_count_proc(context);

  while (viewport_index < viewport_count) {

    VkImage gbuffer_color_image = context->swapchain->gbuffer_color_image[viewport_index][context->renderer->image_index];
    VkImage gbuffer_depth_image = context->swapchain->gbuffer_depth_image[viewport_index][context->renderer->image_index];

    image_layout_transition(context, gbuffer_color_image, command_buffer, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
    image_layout_transition(context, gbuffer_depth_image, command_buffer, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT);

    viewport_index++;
  }
}
static void imgui_draw(context_t *context) {
  VkCommandBuffer command_buffer = context->renderer->graphic_command_buffers[context->renderer->frame_index];

  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplWin32_NewFrame();

  ImGui::NewFrame();

  titlebar_draw(context);
  dockspace_draw(context);
  statusbar_draw(context);

  ImGui::Render();

  ImDrawData *draw_data = ImGui::GetDrawData();

  ImGui_ImplVulkan_RenderDrawData(draw_data, command_buffer);
}
static void imgui_post_draw(context_t *context) {
  VkCommandBuffer command_buffer = context->renderer->graphic_command_buffers[context->renderer->frame_index];

  uint64_t viewport_index = 0;
  uint64_t viewport_count = g_context_imgui_viewport_count_proc(context);

  while (viewport_index < viewport_count) {

    VkImage gbuffer_color_image = context->swapchain->gbuffer_color_image[viewport_index][context->renderer->image_index];
    VkImage gbuffer_depth_image = context->swapchain->gbuffer_depth_image[viewport_index][context->renderer->image_index];

    image_layout_transition(context, gbuffer_color_image, command_buffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_ASPECT_COLOR_BIT);
    image_layout_transition(context, gbuffer_depth_image, command_buffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT);

    viewport_index++;
  }
}
static uint64_t imgui_viewport_count(context_t *context) {
  return 4; // TODO
}
static uint32_t imgui_viewport_width(context_t *context, uint64_t index) {
  return g_editor_viewports[index]->width;
}
static uint32_t imgui_viewport_height(context_t *context, uint64_t index) {
  return g_editor_viewports[index]->height;
}
static void imgui_destroy(context_t *context) {
  titlebar_destroy(context);
  catalog_destroy(context);
  detail_destroy(context);
  hierarchy_destroy(context);
  inspector_destroy(context);
  dockspace_destroy(context);
  statusbar_destroy(context);

  ImGui::PopStyleColor(24);

  ImGui_ImplVulkan_Shutdown();

  ImGui_ImplWin32_Shutdown();

  ImGui::DestroyContext();

  vkDestroyDescriptorPool(context->device, s_editor_descriptor_pool, 0);
}

static int32_t ImGui_CreateSurfaceDummy(ImGuiViewport *viewport, ImU64 instance, const void *allocators, ImU64 *out_surface) {
  return 0;
}

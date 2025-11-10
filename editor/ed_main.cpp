#include <editor/ed_pch.h>
#include <editor/ed_main.h>
#include <editor/ed_catalog.h>
#include <editor/ed_hierarchy.h>
#include <editor/ed_detail.h>
#include <editor/ed_dockspace.h>
#include <editor/ed_inspector.h>
#include <editor/ed_titlebar.h>
#include <editor/ed_statusbar.h>
#include <editor/ed_sceneview.h>
#include <editor/ed_modelview.h>
#include <editor/ed_profiler.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

static void editor_create(context_t *context);
static void editor_refresh(context_t *context);
static void editor_draw(context_t *context);
static void editor_destroy(context_t *context);

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

vector_t g_editor_scenes = {0};

sceneview_t *g_editor_sceneviews[0xFF] = {0};
modelview_t *g_editor_modelviews[0xFF] = {0};

int64_t g_editor_selected_scene_asset = 0;

static VkDescriptorPool s_editor_descriptor_pool = 0;

int32_t main(int32_t argc, char **argv) {
  g_context_editor_create_proc = editor_create;
  g_context_editor_refresh_proc = editor_refresh;
  g_context_editor_draw_proc = editor_draw;
  g_context_editor_destroy_proc = editor_destroy;
  g_context_editor_message_proc = ImGui_ImplWin32_WndProcHandler;

  context_t *context = context_create(1920, 1080, 1);

  // TODO: move scene stuff somewhere else
  context->scene = scene_create(context);

  context_run(context);

  scene_destroy(context->scene);

  context_destroy(context);

  heap_reset();

  return 0;
}

static void editor_create(context_t *context) {
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

  ImGuiContext *imgui_context = ImGui::CreateContext();
  ImPlotContext *implot_context = ImPlot::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  ImGuiPlatformIO &platform_io = ImGui::GetPlatformIO();
  ImGuiStyle &style = ImGui::GetStyle();

  static ImWchar icon_glyph_ranges[] = {
    (wchar_t)ICON_MIN_MS,
    (wchar_t)ICON_MAX_MS,
    0,
  };

  // TODO: integrate fonts into database..

  g_editor_commit_mono = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 15.0F);
  g_editor_commit_mono_h6 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 16.0F);
  g_editor_commit_mono_h5 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 18.0F);
  g_editor_commit_mono_h4 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 20.0F);
  g_editor_commit_mono_h3 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 22.0F);
  g_editor_commit_mono_h2 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 24.0F);
  g_editor_commit_mono_h1 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 26.0F);

  g_editor_material_symbols = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-rounded-fill.ttf", 15.0F, 0, icon_glyph_ranges);
  g_editor_material_symbols_h6 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-rounded-fill.ttf", 18.0F, 0, icon_glyph_ranges);
  g_editor_material_symbols_h5 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-rounded-fill.ttf", 22.0F, 0, icon_glyph_ranges);
  g_editor_material_symbols_h4 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-rounded-fill.ttf", 26.0F, 0, icon_glyph_ranges);
  g_editor_material_symbols_h3 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-rounded-fill.ttf", 30.0F, 0, icon_glyph_ranges);
  g_editor_material_symbols_h2 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-rounded-fill.ttf", 34.0F, 0, icon_glyph_ranges);
  g_editor_material_symbols_h1 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-rounded-fill.ttf", 38.0F, 0, icon_glyph_ranges);

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigWindowsMoveFromTitleBarOnly = 1;

  platform_io.Platform_CreateVkSurface = ImGui_CreateSurfaceDummy;

  style.Alpha = 1.0F;
  style.DisabledAlpha = 1.0F;
  style.WindowPadding = ImVec2(0.0F, 0.0F);
  style.WindowRounding = 0.0F;
  style.WindowBorderSize = 0.0F;
  style.WindowTitleAlign = ImVec2(0.0F, 0.5F);
  style.WindowMenuButtonPosition = ImGuiDir_None;
  style.ChildRounding = 1.0F;
  style.ChildBorderSize = 0.0F;
  style.PopupRounding = 0.0F;
  style.FrameRounding = 0.0F;
  style.ItemSpacing = ImVec2(8.0F, 4.0F);
  style.ScrollbarSize = 12.0F;
  style.ScrollbarRounding = 2.5F;
  style.GrabRounding = 0.0F;
  style.TabRounding = 2.5F;
  style.TabBorderSize = 2.0F;
  style.TabCloseButtonMinWidthSelected = -1.0F;
  style.TabCloseButtonMinWidthUnselected = -1.0F;
  style.TabBarBorderSize = 0.0F;
  style.TabBarOverlineSize = 0.0F;
  style.DockingSeparatorSize = 5.0F;

  ImGui::PushStyleColor(ImGuiCol_WindowBg, EDITOR_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_Border, EDITOR_DOCKING_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ChildBg, EDITOR_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, EDITOR_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_TitleBg, EDITOR_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, EDITOR_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_TitleBgActive, EDITOR_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ResizeGrip, EDITOR_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, EDITOR_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, EDITOR_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, EDITOR_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_DockingPreview, EDITOR_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_Tab, EDITOR_DOCKING_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_TabHovered, EDITOR_DOCKING_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_TabActive, EDITOR_DOCKING_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_TabUnfocused, EDITOR_DOCKING_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, EDITOR_DOCKING_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_Separator, EDITOR_HIGHLIGHT_COLOR);
  ImGui::PushStyleColor(ImGuiCol_SeparatorActive, EDITOR_DOCKING_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, EDITOR_DOCKING_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, EDITOR_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, EDITOR_DOCKING_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, EDITOR_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, EDITOR_BACKGROUND_COLOR);

  ImGui_ImplWin32_Init(context->window_handle);

  ImGui_ImplVulkan_InitInfo imgui_vulkan_init_info = {0};
  imgui_vulkan_init_info.Instance = context->instance;
  imgui_vulkan_init_info.PhysicalDevice = context->physical_device;
  imgui_vulkan_init_info.Device = context->device;
  imgui_vulkan_init_info.QueueFamily = context->graphic_queue_index;
  imgui_vulkan_init_info.Queue = context->graphic_queue;
  imgui_vulkan_init_info.PipelineCache = 0;
  imgui_vulkan_init_info.DescriptorPool = s_editor_descriptor_pool;
  imgui_vulkan_init_info.PipelineInfoMain.RenderPass = context->swapchain->main_render_pass;
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
  profiler_create(context);
  sceneview_create(context, "Scene");
  sceneview_create(context, "Game");
}
static void editor_refresh(context_t *context) {
  uint64_t sceneview_index = 0;

  while (g_editor_sceneviews[sceneview_index]) {

    sceneview_refresh(g_editor_sceneviews[sceneview_index]);

    sceneview_index++;
  }

  uint64_t modelview_index = 0;

  while (g_editor_modelviews[modelview_index]) {

    modelview_refresh(g_editor_modelviews[modelview_index]);

    modelview_index++;
  }
}
static void editor_draw(context_t *context) {
  VkCommandBuffer command_buffer = context->renderer->command_buffer[context->renderer->frame_index];

  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplWin32_NewFrame();

  ImGui::NewFrame();

  titlebar_draw(context);
  dockspace_draw(context);
  statusbar_draw(context);

  // ImPlot::ShowDemoWindow(); // TODO

  ImGui::Render();

  ImDrawData *draw_data = ImGui::GetDrawData();

  ImGui_ImplVulkan_RenderDrawData(draw_data, command_buffer);
}
static void editor_destroy(context_t *context) {
  uint64_t sceneview_index = 0;

  while (g_editor_sceneviews[sceneview_index]) {

    sceneview_destroy(g_editor_sceneviews[sceneview_index]);

    sceneview_index++;
  }

  uint64_t modelview_index = 0;

  while (g_editor_modelviews[modelview_index]) {

    modelview_destroy(g_editor_modelviews[modelview_index]);

    modelview_index++;
  }

  titlebar_destroy(context);
  catalog_destroy(context);
  detail_destroy(context);
  hierarchy_destroy(context);
  inspector_destroy(context);
  profiler_destroy(context);
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

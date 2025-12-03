#include <editor/ed_pch.h>
#include <editor/ed_main.h>
#include <editor/ed_titlebar.h>
#include <editor/ed_statusbar.h>
#include <editor/ed_dockspace.h>

#include <editor/view/ed_canvas.h>
#include <editor/view/ed_catalog.h>
#include <editor/view/ed_hierarchy.h>
#include <editor/view/ed_geometry.h>
#include <editor/view/ed_inspector.h>
#include <editor/view/ed_profiler.h>
#include <editor/view/ed_viewport.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

static void editor_create(void);
static void editor_refresh(void);
static void editor_draw(void);
static void editor_destroy(void);

static int32_t ImGui_CreateSurfaceDummy(ImGuiViewport *viewport, ImU64 instance, const void *allocators, ImU64 *out_surface);

VkDescriptorPool g_editor_descriptor_pool = 0;

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

ed_viewport_t *g_editor_viewport = 0;

ed_statusbar_t *g_editor_statusbar = 0;
ed_dockspace_t *g_editor_dockspace = 0;
ed_titlebar_t *g_editor_titlebar = 0;

int32_t main(int32_t argc, char **argv) {
  g_context_editor_create_proc = editor_create;
  g_context_editor_refresh_proc = editor_refresh;
  g_context_editor_draw_proc = editor_draw;
  g_context_editor_destroy_proc = editor_destroy;
  g_context_editor_message_proc = ImGui_ImplWin32_WndProcHandler;

  eg_context_create(1920, 1080, 1);
  eg_context_run();
  eg_context_destroy();

  eg_heap_reset();

  return 0;
}

static void editor_create(void) {
  uint32_t frames_in_flight = eg_renderer_frames_in_flight();

  // TODO: double check descriptor pool's..

  VkDescriptorPoolSize s_editor_descriptor_pool_sizes[] = {
    {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
  };

  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
  descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  descriptor_pool_create_info.pPoolSizes = s_editor_descriptor_pool_sizes;
  descriptor_pool_create_info.poolSizeCount = (uint32_t)EG_ARRAY_COUNT(s_editor_descriptor_pool_sizes);
  descriptor_pool_create_info.maxSets = 1000 * frames_in_flight; // TODO: adjust this value..

  vkCreateDescriptorPool(eg_context_device(), &descriptor_pool_create_info, 0, &g_editor_descriptor_pool);

  IMGUI_CHECKVERSION();

  ImGuiContext *imgui_context = ImGui::CreateContext();
  ImPlotContext *implot_context = ImPlot::CreateContext();
  ImNodesContext *imnodes_context = ImNodes::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  ImGuiPlatformIO &platform_io = ImGui::GetPlatformIO();
  ImGuiStyle &style = ImGui::GetStyle();
  ImNodesStyle &nodes_style = ImNodes::GetStyle();

  static ImWchar icon_glyph_ranges[] = {
    (wchar_t)ICON_MIN_MS,
    (wchar_t)ICON_MAX_MS,
    0,
  };

  // TODO: integrate fonts into database..

  g_editor_commit_mono = io.Fonts->AddFontFromFileTTF(EG_ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 15.0F);
  g_editor_commit_mono_h6 = io.Fonts->AddFontFromFileTTF(EG_ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 16.0F);
  g_editor_commit_mono_h5 = io.Fonts->AddFontFromFileTTF(EG_ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 18.0F);
  g_editor_commit_mono_h4 = io.Fonts->AddFontFromFileTTF(EG_ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 20.0F);
  g_editor_commit_mono_h3 = io.Fonts->AddFontFromFileTTF(EG_ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 22.0F);
  g_editor_commit_mono_h2 = io.Fonts->AddFontFromFileTTF(EG_ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 24.0F);
  g_editor_commit_mono_h1 = io.Fonts->AddFontFromFileTTF(EG_ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 26.0F);

  g_editor_material_symbols = io.Fonts->AddFontFromFileTTF(EG_ASSET_ROOT_DIR "\\font\\material-symbols-rounded-fill.ttf", 15.0F, 0, icon_glyph_ranges);
  g_editor_material_symbols_h6 = io.Fonts->AddFontFromFileTTF(EG_ASSET_ROOT_DIR "\\font\\material-symbols-rounded-fill.ttf", 18.0F, 0, icon_glyph_ranges);
  g_editor_material_symbols_h5 = io.Fonts->AddFontFromFileTTF(EG_ASSET_ROOT_DIR "\\font\\material-symbols-rounded-fill.ttf", 22.0F, 0, icon_glyph_ranges);
  g_editor_material_symbols_h4 = io.Fonts->AddFontFromFileTTF(EG_ASSET_ROOT_DIR "\\font\\material-symbols-rounded-fill.ttf", 25.0F, 0, icon_glyph_ranges);
  g_editor_material_symbols_h3 = io.Fonts->AddFontFromFileTTF(EG_ASSET_ROOT_DIR "\\font\\material-symbols-rounded-fill.ttf", 30.0F, 0, icon_glyph_ranges);
  g_editor_material_symbols_h2 = io.Fonts->AddFontFromFileTTF(EG_ASSET_ROOT_DIR "\\font\\material-symbols-rounded-fill.ttf", 34.0F, 0, icon_glyph_ranges);
  g_editor_material_symbols_h1 = io.Fonts->AddFontFromFileTTF(EG_ASSET_ROOT_DIR "\\font\\material-symbols-rounded-fill.ttf", 38.0F, 0, icon_glyph_ranges);

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

  nodes_style.GridSpacing = 10.0F;
  nodes_style.NodeCornerRounding = 1.0F;
  // nodes_style.NodePadding = ImVec2(0.0F, 0.0F);
  nodes_style.NodeBorderThickness = 0.0F;
  // nodes_style.LinkThickness = 0.0F;
  nodes_style.LinkLineSegmentsPerLength = 0.0F;
  // nodes_style.LinkHoverDistance = 0.0F;
  // nodes_style.PinCircleRadius = 0.0F;
  // nodes_style.PinQuadSideLength = 0.0F;
  // nodes_style.PinTriangleSideLength = 0.0F;
  // nodes_style.PinLineThickness = 0.0F;
  // nodes_style.PinHoverRadius = 0.0F;
  // nodes_style.PinOffset = 0.0F;
  // nodes_style.MiniMapPadding = ImVec2(0.0F, 0.0F);
  // nodes_style.MiniMapOffset = ImVec2(0.0F, 0.0F);
  nodes_style.Flags = ImNodesStyleFlags_GridSnapping;

  ImGui::PushStyleColor(ImGuiCol_WindowBg, ED_DARK_GREY);
  ImGui::PushStyleColor(ImGuiCol_Border, ED_SHALLOW_GRAY_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ChildBg, ED_DARK_GREY);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, ED_DARK_GREY);
  ImGui::PushStyleColor(ImGuiCol_TitleBg, ED_DARK_GREY);
  ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ED_DARK_GREY);
  ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ED_DARK_GREY);
  ImGui::PushStyleColor(ImGuiCol_ResizeGrip, ED_DARK_GREY);
  ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, ED_DARK_GREY);
  ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, ED_DARK_GREY);
  ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ED_DARK_GREY);
  ImGui::PushStyleColor(ImGuiCol_DockingPreview, ED_DARK_GREY);
  ImGui::PushStyleColor(ImGuiCol_Tab, ED_SHALLOW_GRAY_COLOR);
  ImGui::PushStyleColor(ImGuiCol_TabHovered, ED_SHALLOW_GRAY_COLOR);
  ImGui::PushStyleColor(ImGuiCol_TabActive, ED_SHALLOW_GRAY_COLOR);
  ImGui::PushStyleColor(ImGuiCol_TabUnfocused, ED_SHALLOW_GRAY_COLOR);
  ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, ED_SHALLOW_GRAY_COLOR);
  ImGui::PushStyleColor(ImGuiCol_Separator, ED_LIGHT_GRAY_COLOR);
  ImGui::PushStyleColor(ImGuiCol_SeparatorActive, ED_SHALLOW_GRAY_COLOR);
  ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, ED_SHALLOW_GRAY_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, ED_DARK_GREY);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, ED_SHALLOW_GRAY_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, ED_DARK_GREY);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, ED_DARK_GREY);

  ImNodes::PushColorStyle(ImNodesCol_NodeBackground, ED_SHALLOW_GRAY_COLOR);
  ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundHovered, ED_SHALLOW_GRAY_COLOR);
  // ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundSelected);
  // ImNodes::PushColorStyle(ImNodesCol_NodeOutline);
  // ImNodes::PushColorStyle(ImNodesCol_TitleBar);
  // ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered);
  // ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected);
  // ImNodes::PushColorStyle(ImNodesCol_Link);
  // ImNodes::PushColorStyle(ImNodesCol_LinkHovered);
  // ImNodes::PushColorStyle(ImNodesCol_LinkSelected);
  // ImNodes::PushColorStyle(ImNodesCol_Pin);
  // ImNodes::PushColorStyle(ImNodesCol_PinHovered);
  // ImNodes::PushColorStyle(ImNodesCol_BoxSelector);
  // ImNodes::PushColorStyle(ImNodesCol_BoxSelectorOutline);
  ImNodes::PushColorStyle(ImNodesCol_GridBackground, ED_SPARSE_GRAY_COLOR);
  ImNodes::PushColorStyle(ImNodesCol_GridLine, ED_SPARSE_GRAY_COLOR);
  ImNodes::PushColorStyle(ImNodesCol_GridLinePrimary, ED_SPARSE_GRAY_COLOR);

  ImGui_ImplWin32_Init(eg_context_window_handle());

  ImGui_ImplVulkan_InitInfo imgui_vulkan_init_info = {0};
  imgui_vulkan_init_info.Instance = eg_context_instance();
  imgui_vulkan_init_info.PhysicalDevice = eg_context_physical_device();
  imgui_vulkan_init_info.Device = eg_context_device();
  imgui_vulkan_init_info.QueueFamily = eg_context_primary_queue_index();
  imgui_vulkan_init_info.Queue = eg_context_primary_queue();
  imgui_vulkan_init_info.PipelineCache = 0;
  imgui_vulkan_init_info.DescriptorPool = g_editor_descriptor_pool;
  imgui_vulkan_init_info.PipelineInfoMain.RenderPass = eg_renderpass_main();
  imgui_vulkan_init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  imgui_vulkan_init_info.MinImageCount = eg_context_surface_min_image_count();
  imgui_vulkan_init_info.ImageCount = eg_swapchain_image_count();
  imgui_vulkan_init_info.Allocator = 0;
  imgui_vulkan_init_info.CheckVkResultFn = 0;

  ImGui_ImplVulkan_Init(&imgui_vulkan_init_info);

  g_editor_viewport = ed_heap_alloc<ed_viewport_t>("Scene", 1);

  g_editor_statusbar = ed_heap_alloc<ed_statusbar_t>();
  g_editor_dockspace = ed_heap_alloc<ed_dockspace_t>(g_editor_viewport);
  g_editor_titlebar = ed_heap_alloc<ed_titlebar_t>();
}
static void editor_refresh(void) {
  g_editor_statusbar->refresh();
  g_editor_dockspace->refresh();
  g_editor_titlebar->refresh();
}
static void editor_draw(void) {
  uint32_t frame_index = eg_renderer_frame_index();

  VkCommandBuffer *command_buffer = eg_renderer_command_buffer();

  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplWin32_NewFrame();

  ImGui::NewFrame();

  g_editor_titlebar->draw();
  g_editor_dockspace->draw();
  g_editor_statusbar->draw();

  // ImGui::ShowDemoWindow(); // TODO
  // ImPlot::ShowDemoWindow(); // TODO

  ImGui::Render();

  ImDrawData *draw_data = ImGui::GetDrawData();

  ImGui_ImplVulkan_RenderDrawData(draw_data, command_buffer[frame_index]);
}
static void editor_destroy(void) {
  ed_heap_free<ed_statusbar_t>(g_editor_statusbar);
  ed_heap_free<ed_dockspace_t>(g_editor_dockspace);
  ed_heap_free<ed_titlebar_t>(g_editor_titlebar);

  ed_heap_free<ed_viewport_t>(g_editor_viewport);

  ImNodes::PopColorStyle();
  ImNodes::PopColorStyle();
  ImNodes::PopColorStyle();
  ImNodes::PopColorStyle();
  ImNodes::PopColorStyle();

  ImGui::PopStyleColor(24);

  ImGui_ImplVulkan_Shutdown();

  ImGui_ImplWin32_Shutdown();

  ImNodes::DestroyContext();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();

  vkDestroyDescriptorPool(eg_context_device(), g_editor_descriptor_pool, 0);
}

static int32_t ImGui_CreateSurfaceDummy(ImGuiViewport *viewport, ImU64 instance, const void *allocators, ImU64 *out_surface) {
  return 0;
}

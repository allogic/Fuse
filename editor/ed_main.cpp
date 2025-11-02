#include <editor/ed_pch.h>
#include <editor/ed_main.h>
#include <editor/ed_catalog.h>
#include <editor/ed_hierarchy.h>
#include <editor/ed_detail.h>
#include <editor/ed_dockspace.h>
#include <editor/ed_inspector.h>
#include <editor/ed_titlebar.h>
#include <editor/ed_statusbar.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

static void imgui_create(context_t *context);
static void imgui_draw(context_t *context);
static void imgui_destroy(context_t *context);

static int32_t ImGui_CreateSurfaceDummy(ImGuiViewport *viewport, ImU64 instance, const void *allocators, ImU64 *out_surface);

static VkDescriptorPoolSize const s_editor_descriptor_pool_sizes[] = {
  {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE},
};

static VkDescriptorPool s_editor_descriptor_pool = 0;

ImFont *g_commit_mono_h1 = 0;
ImFont *g_commit_mono_h2 = 0;
ImFont *g_commit_mono_h3 = 0;
ImFont *g_commit_mono_h4 = 0;
ImFont *g_commit_mono_h5 = 0;
ImFont *g_commit_mono_h6 = 0;
ImFont *g_commit_mono = 0;

ImFont *g_material_symbols_h1 = 0;
ImFont *g_material_symbols_h2 = 0;
ImFont *g_material_symbols_h3 = 0;
ImFont *g_material_symbols_h4 = 0;
ImFont *g_material_symbols_h5 = 0;
ImFont *g_material_symbols_h6 = 0;
ImFont *g_material_symbols = 0;

int32_t main(int32_t argc, char **argv) {
  g_context_imgui_create_proc = imgui_create;
  g_context_imgui_draw_proc = imgui_draw;
  g_context_imgui_destroy_proc = imgui_destroy;
  g_context_imgui_message_proc = ImGui_ImplWin32_WndProcHandler;

  context_t *context = context_create(1920, 1080);

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

  context_destroy(context);

  heap_reset();

  return 0;
}

static void imgui_create(context_t *context) {
  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
  descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

  for (VkDescriptorPoolSize const &descriptor_pool_size : s_editor_descriptor_pool_sizes) {
    descriptor_pool_create_info.maxSets += descriptor_pool_size.descriptorCount;
  }

  descriptor_pool_create_info.pPoolSizes = s_editor_descriptor_pool_sizes;
  descriptor_pool_create_info.poolSizeCount = ARRAY_COUNT(s_editor_descriptor_pool_sizes);

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

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigWindowsMoveFromTitleBarOnly = 1;

  // TODO: integrate fonts into database..

  g_commit_mono = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 14.0F);
  g_commit_mono_h6 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 16.0F);
  g_commit_mono_h5 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 18.0F);
  g_commit_mono_h4 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 20.0F);
  g_commit_mono_h3 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 22.0F);
  g_commit_mono_h2 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 24.0F);
  g_commit_mono_h1 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\commit-mono-latin-400-normal.ttf", 26.0F);

  g_material_symbols = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-outlined.ttf", 14.0F, 0, icon_glyph_ranges);
  g_material_symbols_h6 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-outlined.ttf", 18.0F, 0, icon_glyph_ranges);
  g_material_symbols_h5 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-outlined.ttf", 22.0F, 0, icon_glyph_ranges);
  g_material_symbols_h4 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-outlined.ttf", 26.0F, 0, icon_glyph_ranges);
  g_material_symbols_h3 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-outlined.ttf", 30.0F, 0, icon_glyph_ranges);
  g_material_symbols_h2 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-outlined.ttf", 34.0F, 0, icon_glyph_ranges);
  g_material_symbols_h1 = io.Fonts->AddFontFromFileTTF(ASSET_ROOT_DIR "\\font\\material-symbols-outlined.ttf", 38.0F, 0, icon_glyph_ranges);

  platform_io.Platform_CreateVkSurface = ImGui_CreateSurfaceDummy;

  style.Alpha = 1.0F;
  style.DisabledAlpha = 1.0F;
  style.WindowPadding = ImVec2{0.0F, 0.0F};
  style.WindowRounding = 0.0F;
  style.WindowBorderSize = 0.0F;
  style.WindowMinSize = ImVec2{32.0F, 32.0F};
  style.WindowTitleAlign = ImVec2{0.0F, 0.5F};
  style.WindowMenuButtonPosition = ImGuiDir_None;
  style.ChildRounding = 5.0F;
  style.ChildBorderSize = 0.0F;
  style.PopupRounding = 0.0F;
  // style.PopupBorderSize = 1.0F;
  // style.FramePadding = ImVec2{7.5F, 7.5F};
  style.FrameRounding = 0.0F;
  // style.FrameBorderSize = 1.0F;
  style.ItemSpacing = ImVec2{8.0F, 4.0F};
  // style.ItemInnerSpacing = ImVec2{4.0F, 4.0F};
  // style.CellPadding = ImVec2{4.0F, 2.0F};
  // style.IndentSpacing = 21.0F;
  // style.ColumnsMinSpacing = 6.0F;
  style.ScrollbarSize = 15.0F;
  style.ScrollbarRounding = 5.0F;
  // style.GrabMinSize = 25.0F;
  style.GrabRounding = 0.0F;
  style.TabRounding = 5.0F;
  style.TabBorderSize = 2.0F;
  // style.TabMinWidthBase = 200.0F;
  // style.TabMinWidthShrink = 200.0F;
  style.TabCloseButtonMinWidthSelected = -1.0F;
  style.TabCloseButtonMinWidthUnselected = -1.0F;
  style.TabBarBorderSize = 0.0F;
  style.TabBarOverlineSize = 0.0F;

  style.DockingSeparatorSize = 5.0F;

  // style.ColorButtonPosition = ImGuiDir_Right;
  // style.ButtonTextAlign = ImVec2{0.5F, 0.5F};
  // style.SelectableTextAlign = ImVec2{0.0F, 0.0F};

  // style.Colors[ImGuiCol_Text] = ImVec4{1.0F, 1.0F, 1.0F, 1.0F};
  // style.Colors[ImGuiCol_TextDisabled] = ImVec4{0.4980392158031464F, 0.4980392158031464F, 0.4980392158031464F, 1.0F};
  // style.Colors[ImGuiCol_WindowBg] = ImVec4{0.1764705926179886F, 0.1764705926179886F, 0.1764705926179886F, 1.0F};
  // style.Colors[ImGuiCol_ChildBg] = ImVec4{0.2784313857555389F, 0.2784313857555389F, 0.2784313857555389F, 0.0F};
  // style.Colors[ImGuiCol_PopupBg] = ImVec4{0.3098039329051971F, 0.3098039329051971F, 0.3098039329051971F, 1.0F};
  // style.Colors[ImGuiCol_Border] = ImVec4{0.2627451121807098F, 0.2627451121807098F, 0.2627451121807098F, 1.0F};
  // style.Colors[ImGuiCol_BorderShadow] = ImVec4{0.0F, 0.0F, 0.0F, 0.0F};
  // style.Colors[ImGuiCol_FrameBg] = ImVec4{0.1568627506494522F, 0.1568627506494522F, 0.1568627506494522F, 1.0F};
  // style.Colors[ImGuiCol_FrameBgHovered] = ImVec4{0.2000000029802322F, 0.2000000029802322F, 0.2000000029802322F, 1.0F};
  // style.Colors[ImGuiCol_FrameBgActive] = ImVec4{0.2784313857555389F, 0.2784313857555389F, 0.2784313857555389F, 1.0F};
  // style.Colors[ImGuiCol_TitleBg] = ImVec4{0.1176470588235294F, 0.1176470588235294F, 0.1176470588235294F, 1.0F};
  // style.Colors[ImGuiCol_TitleBgActive] = ImVec4{0.1176470588235294F, 0.1176470588235294F, 0.1176470588235294F, 1.0F};
  // style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.1450980454683304F, 0.1450980454683304F, 0.1450980454683304F, 1.0F};
  // style.Colors[ImGuiCol_MenuBarBg] = ImVec4{0.1921568661928177F, 0.1921568661928177F, 0.1921568661928177F, 1.0F};
  // style.Colors[ImGuiCol_ScrollbarBg] = ImVec4{0.1568627506494522F, 0.1568627506494522F, 0.1568627506494522F, 1.0F};
  // style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4{0.2745098173618317F, 0.2745098173618317F, 0.2745098173618317F, 1.0F};
  // style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{0.2980392277240753F, 0.2980392277240753F, 0.2980392277240753F, 1.0F};
  // style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{1.0F, 0.3882353007793427F, 0.0F, 1.0F};
  // style.Colors[ImGuiCol_CheckMark] = ImVec4{1.0F, 1.0F, 1.0F, 1.0F};
  // style.Colors[ImGuiCol_SliderGrab] = ImVec4{0.3882353007793427F, 0.3882353007793427F, 0.3882353007793427F, 1.0F};
  // style.Colors[ImGuiCol_SliderGrabActive] = ImVec4{1.0F, 0.3882353007793427F, 0.0F, 1.0F};
  // style.Colors[ImGuiCol_Button] = ImVec4{1.0F, 1.0F, 1.0F, 0.0F};
  // style.Colors[ImGuiCol_ButtonHovered] = ImVec4{1.0F, 1.0F, 1.0F, 0.1560000032186508F};
  // style.Colors[ImGuiCol_ButtonActive] = ImVec4{1.0F, 1.0F, 1.0F, 0.3910000026226044F};
  // style.Colors[ImGuiCol_Header] = ImVec4{0.3098039329051971F, 0.3098039329051971F, 0.3098039329051971F, 1.0F};
  // style.Colors[ImGuiCol_HeaderHovered] = ImVec4{0.4666666686534882F, 0.4666666686534882F, 0.4666666686534882F, 1.0F};
  // style.Colors[ImGuiCol_HeaderActive] = ImVec4{0.4666666686534882F, 0.4666666686534882F, 0.4666666686534882F, 1.0F};
  // style.Colors[ImGuiCol_Separator] = ImVec4{0.2627451121807098F, 0.2627451121807098F, 0.2627451121807098F, 1.0F};
  // style.Colors[ImGuiCol_SeparatorHovered] = ImVec4{0.3882353007793427F, 0.3882353007793427F, 0.3882353007793427F, 1.0F};
  // style.Colors[ImGuiCol_SeparatorActive] = ImVec4{1.0F, 0.3882353007793427F, 0.0F, 1.0F};
  // style.Colors[ImGuiCol_ResizeGrip] = ImVec4{0.0F, 0.0F, 0.0F, 0.0F};
  // style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4{0.0F, 0.0F, 0.0F, 0.0F};
  // style.Colors[ImGuiCol_ResizeGripActive] = ImVec4{0.0F, 0.0F, 0.0F, 0.0F};
  // style.Colors[ImGuiCol_Tab] = ImVec4{0.09411764889955521F, 0.09411764889955521F, 0.09411764889955521F, 1.0F};
  // style.Colors[ImGuiCol_TabHovered] = ImVec4{0.3490196168422699F, 0.3490196168422699F, 0.3490196168422699F, 1.0F};
  // style.Colors[ImGuiCol_TabActive] = ImVec4{0.1921568661928177F, 0.1921568661928177F, 0.1921568661928177F, 1.0F};
  // style.Colors[ImGuiCol_TabUnfocused] = ImVec4{0.09411764889955521F, 0.09411764889955521F, 0.09411764889955521F, 1.0F};
  // style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.1921568661928177F, 0.1921568661928177F, 0.1921568661928177F, 1.0F};
  // style.Colors[ImGuiCol_PlotLines] = ImVec4{0.4666666686534882F, 0.4666666686534882F, 0.4666666686534882F, 1.0F};
  // style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4{1.0F, 0.3882353007793427F, 0.0F, 1.0F};
  // style.Colors[ImGuiCol_PlotHistogram] = ImVec4{0.5843137502670288F, 0.5843137502670288F, 0.5843137502670288F, 1.0F};
  // style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4{1.0F, 0.3882353007793427F, 0.0F, 1.0F};
  // style.Colors[ImGuiCol_TableHeaderBg] = ImVec4{0.1882352977991104F, 0.1882352977991104F, 0.2000000029802322F, 1.0F};
  // style.Colors[ImGuiCol_TableBorderStrong] = ImVec4{0.3098039329051971F, 0.3098039329051971F, 0.3490196168422699F, 1.0F};
  // style.Colors[ImGuiCol_TableBorderLight] = ImVec4{0.2274509817361832F, 0.2274509817361832F, 0.2470588237047195F, 1.0F};
  // style.Colors[ImGuiCol_TableRowBg] = ImVec4{0.0F, 0.0F, 0.0F, 0.0F};
  // style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4{1.0F, 1.0F, 1.0F, 0.05999999865889549F};
  // style.Colors[ImGuiCol_TextSelectedBg] = ImVec4{1.0F, 1.0F, 1.0F, 0.1560000032186508F};
  // style.Colors[ImGuiCol_DragDropTarget] = ImVec4{1.0F, 0.3882353007793427F, 0.0F, 1.0F};
  // style.Colors[ImGuiCol_NavHighlight] = ImVec4{1.0F, 0.3882353007793427F, 0.0F, 1.0F};
  // style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4{1.0F, 0.3882353007793427F, 0.0F, 1.0F};
  // style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4{0.0F, 0.0F, 0.0F, 0.5860000252723694F};
  // style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4{0.0F, 0.0F, 0.0F, 0.5860000252723694F};
  // style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4{0.1176470588235294F, 0.1176470588235294F, 0.1176470588235294F, 1.0F};
  // style.Colors[ImGuiCol_DockingPreview] = ImVec4{0.1176470588235294F, 0.1176470588235294F, 0.1176470588235294F, 1.0F};

  ImGui_ImplWin32_Init(context->window_handle);

  ImGui_ImplVulkan_InitInfo imgui_vulkan_init_info = {};
  imgui_vulkan_init_info.Instance = context->instance;
  imgui_vulkan_init_info.PhysicalDevice = context->physical_device;
  imgui_vulkan_init_info.Device = context->device;
  imgui_vulkan_init_info.QueueFamily = context->graphic_queue_index;
  imgui_vulkan_init_info.Queue = context->graphic_queue;
  imgui_vulkan_init_info.PipelineCache = 0;
  imgui_vulkan_init_info.DescriptorPool = s_editor_descriptor_pool;
  imgui_vulkan_init_info.PipelineInfoMain.RenderPass = context->swapchain->render_pass;
  imgui_vulkan_init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  imgui_vulkan_init_info.MinImageCount = context->surface_capabilities.minImageCount;
  imgui_vulkan_init_info.ImageCount = (uint32_t)context->swapchain->image_count;
  imgui_vulkan_init_info.Allocator = 0;
  imgui_vulkan_init_info.CheckVkResultFn = 0;

  ImGui_ImplVulkan_Init(&imgui_vulkan_init_info);

  titlebar_create();
  catalog_create();
  detail_create();
  hierarchy_create();
  inspector_create();
  dockspace_create();
  statusbar_create();
}
static void imgui_draw(context_t *context) {
  ImGui_ImplVulkan_NewFrame();

  ImGui_ImplWin32_NewFrame();

  ImGui::NewFrame();

  titlebar_draw(context);
  dockspace_draw(context);
  statusbar_draw(context);

  ImGui::Render();

  ImDrawData *draw_data = ImGui::GetDrawData();

  ImGui_ImplVulkan_RenderDrawData(draw_data, context->renderer->graphic_command_buffers[context->renderer->frame_index]);
}
static void imgui_destroy(context_t *context) {
  titlebar_destroy();
  catalog_destroy();
  detail_destroy();
  hierarchy_destroy();
  inspector_destroy();
  dockspace_destroy();
  statusbar_destroy();

  ImGui_ImplVulkan_Shutdown();

  ImGui_ImplWin32_Shutdown();

  ImGui::DestroyContext();

  vkDestroyDescriptorPool(context->device, s_editor_descriptor_pool, 0);
}

static int32_t ImGui_CreateSurfaceDummy(ImGuiViewport *viewport, ImU64 instance, const void *allocators, ImU64 *out_surface) {
  return 0;
}

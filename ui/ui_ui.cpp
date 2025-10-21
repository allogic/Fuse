#include <ui/ui_pch.h>
#include <ui/ui_db.h>
#include <ui/ui_ui.h>
#include <ui/ui_renderer.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_vulkan.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

static VkDescriptorPoolSize const s_ui_descriptor_pool_sizes[] = {
  {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE},
};

static VkDescriptorPool s_ui_descriptor_pool = 0;

void ui_create(void) {
  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
  descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

  for (VkDescriptorPoolSize const &descriptor_pool_size : s_ui_descriptor_pool_sizes) {
    descriptor_pool_create_info.maxSets += descriptor_pool_size.descriptorCount;
  }

  descriptor_pool_create_info.pPoolSizes = s_ui_descriptor_pool_sizes;
  descriptor_pool_create_info.poolSizeCount = ARRAY_COUNT(s_ui_descriptor_pool_sizes);

  vkCreateDescriptorPool(g_context_device, &descriptor_pool_create_info, 0, &s_ui_descriptor_pool);

  IMGUI_CHECKVERSION();

  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigWindowsMoveFromTitleBarOnly = 1;

  ImGuiStyle &style = ImGui::GetStyle();
  style.WindowMinSize = ImVec2(160, 20);
  style.FramePadding = ImVec2(4, 2);
  style.ItemSpacing = ImVec2(6, 2);
  style.ItemInnerSpacing = ImVec2(6, 4);
  style.Alpha = 0.95f;
  style.WindowRounding = 0.0f;
  style.FrameRounding = 0.0f;
  style.IndentSpacing = 6.0f;
  style.ItemInnerSpacing = ImVec2(2, 4);
  style.ColumnsMinSpacing = 50.0f;
  style.GrabMinSize = 14.0f;
  style.GrabRounding = 0.0f;
  style.ScrollbarSize = 12.0f;
  style.ScrollbarRounding = 0.0f;

  style.Colors[ImGuiCol_Text] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.92f, 0.18f, 0.29f, 0.76f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.9f);

  ImGui_ImplWin32_Init(g_context_window_handle);

  ImGui_ImplVulkan_InitInfo imgui_vulkan_init_info = {};
  imgui_vulkan_init_info.Instance = g_context_instance;
  imgui_vulkan_init_info.PhysicalDevice = g_context_physical_device;
  imgui_vulkan_init_info.Device = g_context_device;
  imgui_vulkan_init_info.QueueFamily = g_context_graphic_queue_index;
  imgui_vulkan_init_info.Queue = g_context_graphic_queue;
  imgui_vulkan_init_info.PipelineCache = 0;
  imgui_vulkan_init_info.DescriptorPool = s_ui_descriptor_pool;
  imgui_vulkan_init_info.RenderPass = g_swapchain_render_pass;
  imgui_vulkan_init_info.Subpass = 0;
  imgui_vulkan_init_info.MinImageCount = g_swapchain_image_count; // TODO
  imgui_vulkan_init_info.ImageCount = g_swapchain_image_count;    // TODO
  imgui_vulkan_init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  imgui_vulkan_init_info.Allocator = 0;
  imgui_vulkan_init_info.CheckVkResultFn = 0;

  ImGui_ImplVulkan_Init(&imgui_vulkan_init_info);

  ui_db_create();
  ui_renderer_create();
}
void ui_draw() {
  ImGui_ImplVulkan_NewFrame();

  ImGui_ImplWin32_NewFrame();

  ImGui::NewFrame();

  ui_db_draw();
  ui_renderer_draw();

  ImGui::Render();

  ImDrawData *draw_data = ImGui::GetDrawData();

  ImGui_ImplVulkan_RenderDrawData(draw_data, g_renderer_graphic_command_buffers[g_renderer_frame_index]);
}
void ui_destroy(void) {
  ui_db_destroy();
  ui_renderer_destroy();

  ImGui_ImplVulkan_Shutdown();

  ImGui_ImplWin32_Shutdown();

  ImGui::DestroyContext();

  vkDestroyDescriptorPool(g_context_device, s_ui_descriptor_pool, 0);
}

void ui_forward_message(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param) {
  ImGui_ImplWin32_WndProcHandler(window_handle, message, w_param, l_param);
}

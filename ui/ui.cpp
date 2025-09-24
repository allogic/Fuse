#include <ui/ui.h>

#include <library/core/api.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_vulkan.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

static VkDescriptorPoolSize const s_ui_descriptor_pool_sizes[] = {
  {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE},
};

static VkDescriptorPool s_ui_descriptor_pool = 0;

void ui_create(HWND window_handle, VkInstance instance, VkPhysicalDevice physical_device, VkDevice device, uint32_t queue_index, VkQueue queue, VkRenderPass render_pass, uint32_t min_image_count) {
  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
  descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

  for (VkDescriptorPoolSize const &descriptor_pool_size : s_ui_descriptor_pool_sizes) {
    descriptor_pool_create_info.maxSets += descriptor_pool_size.descriptorCount;
  }

  descriptor_pool_create_info.pPoolSizes = s_ui_descriptor_pool_sizes;
  descriptor_pool_create_info.poolSizeCount = ARRAY_COUNT(s_ui_descriptor_pool_sizes);

  vkCreateDescriptorPool(device, &descriptor_pool_create_info, 0, &s_ui_descriptor_pool);

  IMGUI_CHECKVERSION();

  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigWindowsMoveFromTitleBarOnly = 1;

  ImGui::StyleColorsDark();

  ImGui_ImplWin32_Init(window_handle);

  ImGui_ImplVulkan_InitInfo imgui_vulkan_init_info = {};
  imgui_vulkan_init_info.Instance = instance;
  imgui_vulkan_init_info.PhysicalDevice = physical_device;
  imgui_vulkan_init_info.Device = device;
  imgui_vulkan_init_info.QueueFamily = queue_index;
  imgui_vulkan_init_info.Queue = queue;
  imgui_vulkan_init_info.PipelineCache = 0;
  imgui_vulkan_init_info.DescriptorPool = s_ui_descriptor_pool;
  imgui_vulkan_init_info.RenderPass = render_pass;
  imgui_vulkan_init_info.Subpass = 0;
  imgui_vulkan_init_info.MinImageCount = min_image_count; // TODO
  imgui_vulkan_init_info.ImageCount = min_image_count;    // TODO
  imgui_vulkan_init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  imgui_vulkan_init_info.Allocator = 0;
  imgui_vulkan_init_info.CheckVkResultFn = 0;

  ImGui_ImplVulkan_Init(&imgui_vulkan_init_info);
}
void ui_draw(VkCommandBuffer command_buffer) {
  ImGui_ImplVulkan_NewFrame();

  ImGui_ImplWin32_NewFrame();

  ImGui::NewFrame();

  ImGui::Begin("Hello, World!");
  ImGui::End();

  ImGui::Render();

  ImDrawData *draw_data = ImGui::GetDrawData();

  ImGui_ImplVulkan_RenderDrawData(draw_data, command_buffer);
}
void ui_destroy(VkDevice device) {
  ImGui_ImplVulkan_Shutdown();

  ImGui_ImplWin32_Shutdown();

  ImGui::DestroyContext();

  vkDestroyDescriptorPool(device, s_ui_descriptor_pool, 0);
}

void ui_forward_message(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param) {
  ImGui_ImplWin32_WndProcHandler(window_handle, message, w_param, l_param);
}

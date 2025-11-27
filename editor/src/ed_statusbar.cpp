#include <editor/ed_pch.h>
#include <editor/ed_main.h>
#include <editor/ed_statusbar.h>

void ed_statusbar_draw(eg_context_t *context) {
  uint32_t window_width = eg_context_window_width(context);
  uint32_t window_height = eg_context_window_height(context);
  uint32_t window_statusbar_height = eg_context_statusbar_height(context);

  ImGui::SetNextWindowPos(ImVec2(0.0F, (float)window_height - (float)window_statusbar_height));
  ImGui::SetNextWindowSize(ImVec2((float)window_width, (float)window_statusbar_height));

  ImGui::PushStyleColor(ImGuiCol_Button, ED_DARK_GREY);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ED_LIGHT_GRAY_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ED_ACTIVE_GREY_COLOR);

  ImGuiWindowFlags titlebar_flags =
    ImGuiWindowFlags_NoDocking |
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoScrollWithMouse |
    ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoBringToFrontOnFocus;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0F, 5.0F));
  ImGui::Begin("Statusbar", 0, titlebar_flags);
  ImGui::PopStyleVar(1);

  ImGui::End();
  ImGui::PopStyleColor(3);
}

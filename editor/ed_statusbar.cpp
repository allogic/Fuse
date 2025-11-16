#include <editor/ed_pch.h>
#include <editor/ed_main.h>
#include <editor/ed_statusbar.h>

void ed_statusbar_draw(eg_context_t *context) {
  ImGui::SetNextWindowPos(ImVec2(0.0F, (float)context->window_height - (float)context->window_statusbar_height));
  ImGui::SetNextWindowSize(ImVec2((float)context->window_width, (float)context->window_statusbar_height));

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

#include <editor/ed_pch.h>
#include <editor/ed_statusbar.h>
#include <editor/ed_main.h>

void statusbar_create() {
}
void statusbar_refresh() {
}
void statusbar_draw(context_t *context) {
  ImGui::SetNextWindowPos(ImVec2(0.0F, (float)context->window_height - (float)context->window_statusbar_height));
  ImGui::SetNextWindowSize(ImVec2((float)context->window_width, (float)context->window_statusbar_height));

  ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(70, 70, 70, 255));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(90, 90, 90, 255));

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

  ImGui::PushFont(g_material_symbols_h5);
  ImGui::Text(ICON_MS_TOOLTIP_2);
  ImGui::PopFont();

  ImGui::End();
  ImGui::PopStyleVar(1);
  ImGui::PopStyleColor(4);
}
void statusbar_destroy() {
}

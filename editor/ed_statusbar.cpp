#include <editor/ed_pch.h>
#include <editor/ed_statusbar.h>
#include <editor/ed_main.h>

void statusbar_create(context_t *context) {
}
void statusbar_refresh(context_t *context) {
}
void statusbar_draw(context_t *context) {
  ImGui::SetNextWindowPos(ImVec2(0.0F, (float)context->window_height - (float)context->window_statusbar_height));
  ImGui::SetNextWindowSize(ImVec2((float)context->window_width, (float)context->window_statusbar_height));

  ImGui::PushStyleColor(ImGuiCol_Button, EDITOR_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EDITOR_HIGHLIGHT_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, EDITOR_ACTIVE_COLOR);

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
void statusbar_destroy(context_t *context) {
}

#include <editor/ed_pch.h>
#include <editor/ed_viewport.h>
#include <editor/ed_titlebar.h>

void viewport_create() {
}
void viewport_refresh() {
}
void viewport_draw(context_t *context) {
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar;

  ImGui::Begin("Viewport", &g_titlebar_hierarchy_open, window_flags);

  ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50, 255));
  ImGui::BeginChild("ViewportFirstChild");

  ImVec2 second_window_size = ImGui::GetWindowSize();
  ImVec2 second_cursor_position = ImVec2(5.0F, 5.0F);

  second_window_size.x -= 10.0F;
  second_window_size.y -= 30.0F;

  ImGui::SetCursorPos(second_cursor_position);
  ImGui::BeginChild("ViewportSecondChild", second_window_size);

  // ImGui::Image(0, second_window_size);

  ImGui::EndChild();

  ImGui::EndChild();
  ImGui::PopStyleColor();

  ImGui::End();
}
void viewport_destroy() {
}

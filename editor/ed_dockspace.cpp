#include <editor/ed_pch.h>
#include <editor/ed_catalog.h>
#include <editor/ed_detail.h>
#include <editor/ed_hierarchy.h>
#include <editor/ed_dockspace.h>
#include <editor/ed_main.h>
#include <editor/ed_inspector.h>
#include <editor/ed_viewport.h>

void dockspace_create(context_t *context) {
}
void dockspace_refresh(context_t *context) {
}
void dockspace_draw(context_t *context) {
  ImGui::SetNextWindowPos(ImVec2(0.0F, (float)context->window_titlebar_height));
  ImGui::SetNextWindowSize(ImVec2((float)context->window_width, (float)context->window_height - (float)context->window_titlebar_height - context->window_statusbar_height));

  ImGuiWindowFlags docking_flags =
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoScrollWithMouse |
    ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoBringToFrontOnFocus;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0F, 5.0F));
  ImGui::Begin("Root", 0, docking_flags);

  ImGuiDockNodeFlags dock_node_flags =
    ImGuiDockNodeFlags_NoCloseButton;

  ImGui::DockSpace(ImGui::GetID("Dockspace"), ImVec2(0.0F, 0.0F), dock_node_flags);
  ImGui::PopStyleVar(1);

  catalog_draw(context);
  detail_draw(context);
  hierarchy_draw(context);
  inspector_draw(context);

  uint64_t viewport_index = 0;
  uint64_t viewport_count = 4;

  while (viewport_index < viewport_count) {

    if (g_editor_viewports[viewport_index]) {
      viewport_draw(g_editor_viewports[viewport_index]);
    }

    viewport_index++;
  }

  ImGui::End();
}
void dockspace_destroy(context_t *context) {
}

bool dockspace_begin_child(char const *name, bool *is_open, bool *is_docked) {
  if (*is_open) {
    ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoScrollbar |
      ImGuiWindowFlags_NoCollapse;

    ImGui::Begin(name, is_open, window_flags);

    *is_docked = ImGui::IsWindowDocked();

    if (*is_docked) {
      ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50, 255));
      ImGui::BeginChild("FirstChild");

      ImVec2 second_window_size = ImGui::GetWindowSize();
      ImVec2 second_cursor_position = ImVec2(5.0F, 5.0F);

      second_window_size.x -= 10.0F;
      second_window_size.y -= 10.0F;

      ImGui::SetCursorPos(second_cursor_position);
      ImGui::BeginChild("SecondChild", second_window_size);
    }

    return true;
  }

  return false;
}
void dockspace_end_child(bool is_docked) {
  if (is_docked) {
    ImGui::EndChild();
    ImGui::EndChild();
    ImGui::PopStyleColor(1);
  }

  ImGui::End();
}

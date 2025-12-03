#include <editor/ed_pch.h>
#include <editor/ed_dockspace.h>

ed_dockspace_t ::~ed_dockspace_t() {
}

void ed_dockspace_t::refresh() {
}

void ed_dockspace_t::draw() {
  static ImGuiWindowFlags window_flags =
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoScrollWithMouse |
    ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoBringToFrontOnFocus;

  static ImGuiDockNodeFlags dock_node_flags =
    ImGuiDockNodeFlags_NoCloseButton;

  uint32_t window_width = eg_context_window_width();
  uint32_t window_height = eg_context_window_height();
  uint32_t window_titlebar_height = eg_context_titlebar_height();
  uint32_t window_statusbar_height = eg_context_statusbar_height();

  ImGui::SetNextWindowPos(ImVec2(0.0F, (float)window_titlebar_height));
  ImGui::SetNextWindowSize(ImVec2((float)window_width, (float)window_height - (float)window_titlebar_height - window_statusbar_height));

  ImGuiID dockspace_id = ImGui::GetID(ED_DOCKSPACE_MAIN_ID);

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0F, 5.0F));
  ImGui::Begin(ED_DOCKSPACE_WINDOW_ID, 0, window_flags);
  ImGui::PopStyleVar();
  ImGui::DockSpace(dockspace_id, ImVec2(0.0F, 0.0F), dock_node_flags);

  if (m_is_dirty) {

    m_is_dirty = 0;

    ImVec2 dockspace_size = ImGui::GetWindowSize();

    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, dockspace_size);

    ImGuiID viewport_id = dockspace_id;
    ImGuiID inspector_id = 0;
    ImGuiID asset_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.15F, 0, &dockspace_id);
    ImGuiID right_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25F, 0, &dockspace_id);
    ImGuiID hierarchy_id = ImGui::DockBuilderSplitNode(right_id, ImGuiDir_Up, 0.25F, 0, &inspector_id);

    ImGui::DockBuilderDockWindow(ED_VIEWPORT_WINDOW_ID, viewport_id);
    ImGui::DockBuilderDockWindow(ED_ASSET_WINDOW_ID, asset_id);
    ImGui::DockBuilderDockWindow(ED_HIERARCHY_WINDOW_ID, hierarchy_id);
    ImGui::DockBuilderDockWindow(ED_INSPECTOR_WINDOW_ID, inspector_id);

    ImGui::DockBuilderFinish(dockspace_id);
  }

  ImGui::End();

  for (ed_view_t *view : m_views) {

    begin_child(view, 5.0F);

    view->draw();

    end_child(view);
  }
}

void ed_dockspace_t::begin_child(ed_view_t *view, float border_size) {
  if (view->is_open()) {

    static ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoScrollbar |
      ImGuiWindowFlags_NoCollapse;

    bool is_open = view->is_open();

    ImGui::Begin(view->name(), &is_open, window_flags);

    bool is_docked = ImGui::IsWindowDocked();

    view->set_open(is_open);
    view->set_docked(is_docked);

    if (view->is_docked()) {

      ImGui::PushStyleColor(ImGuiCol_ChildBg, ED_SHALLOW_GRAY_COLOR);
      ImGui::BeginChild("first_child");

      ImVec2 second_window_size = ImGui::GetWindowSize();
      ImVec2 second_cursor_position = ImVec2(border_size, border_size);

      second_window_size.x -= border_size * 2.0F;
      second_window_size.y -= border_size * 2.0F;

      ImGui::SetCursorPos(second_cursor_position);

      ImGui::PushStyleColor(ImGuiCol_ChildBg, ED_DARK_GREY);
      ImGui::BeginChild("second_child", second_window_size);
    }
  }
}

void ed_dockspace_t::end_child(ed_view_t *view) {
  if (view->is_docked()) {

    ImGui::EndChild();
    ImGui::PopStyleColor(1);

    ImGui::EndChild();
    ImGui::PopStyleColor(1);
  }

  ImGui::End();
}

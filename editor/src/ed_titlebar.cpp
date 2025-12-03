#include <editor/ed_pch.h>
#include <editor/ed_titlebar.h>
#include <editor/ed_main.h>

#include <editor/view/ed_hierarchy.h>
#include <editor/view/ed_inspector.h>
#include <editor/view/ed_profiler.h>

ed_titlebar_t::ed_titlebar_t() {
}
ed_titlebar_t::~ed_titlebar_t() {
}

void ed_titlebar_t::refresh() {
}
void ed_titlebar_t::draw() {
  uint32_t window_width = eg_context_window_width();
  uint32_t window_titlebar_height = eg_context_titlebar_height();

  ImGui::SetNextWindowPos(ImVec2(0.0F, 0.0F));
  ImGui::SetNextWindowSize(ImVec2((float)window_width, (float)window_titlebar_height));

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
  ImGui::Begin(ED_TITLEBAR_WINDOW_ID, 0, titlebar_flags);
  ImGui::PopStyleVar(1);

  draw_icon();
  draw_main_menu();
  draw_window_controls();
  draw_scene_controls();

  reset_drag_state();

  ImGui::End();
  ImGui::PopStyleColor(3);
}

void ed_titlebar_t::reset_drag_state() {
  if (ImGui::IsWindowHovered() &&
      !ImGui::IsAnyItemHovered() &&
      ImGui::IsMouseDown(ImGuiMouseButton_Left)) {

    ReleaseCapture();

    HWND window_handle = eg_context_window_handle();

    SendMessage(window_handle, WM_NCLBUTTONDOWN, HTCAPTION, 0);
  }

  ImGui::GetIO().MouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
}

void ed_titlebar_t::draw_icon() {
  ImGui::SetCursorPos(ImVec2(10.0F, 10.0F));
  ImGui::PushFont(g_editor_material_symbols_h1);
  ImGui::Text(ICON_MS_TOKEN);
  ImGui::PopFont();
}
void ed_titlebar_t::draw_main_menu() {
  ImGui::SetCursorPos(ImVec2(50.0F, 0.0F));

  ImGui::SameLine();
  if (ImGui::Button("Hierarchy")) {
    // g_hierarchy_scene->base.is_open = !g_hierarchy_scene->base.is_open;
  }

  ImGui::SameLine();
  if (ImGui::Button("Inspector")) {
    // g_inspector_scene->base.is_open = !g_inspector_scene->base.is_open;
  }

  ImGui::SameLine();
  if (ImGui::Button("Profiler")) {
    // g_profiler_scene->base.is_open = !g_profiler_scene->base.is_open;
  }
}
void ed_titlebar_t::draw_window_controls() {
  HWND window_handle = eg_context_window_handle();

  ImVec2 window_size = ImGui::GetWindowSize();

  ImGui::SetCursorPos(ImVec2(window_size.x - 90.0F, 5.0F));

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);
  ImGui::PushFont(g_editor_material_symbols_h6);

  if (ImGui::Button(ICON_MS_REMOVE)) {
    ShowWindow(window_handle, SW_MINIMIZE);
  }

  ImGui::SameLine();
  if (ImGui::Button(ICON_MS_RECTANGLE)) {
    static uint8_t is_maximized = 0;

    if (is_maximized) {
      ShowWindow(window_handle, SW_RESTORE);
    } else {
      ShowWindow(window_handle, SW_MAXIMIZE);
    }

    is_maximized = !is_maximized;
  }

  ImGui::SameLine();

  if (ImGui::Button(ICON_MS_CLOSE)) {
    eg_context_set_running(0);
  }

  ImGui::PopFont();
  ImGui::PopStyleVar();
}
void ed_titlebar_t::draw_scene_controls() {
  ImVec2 window_size = ImGui::GetWindowSize();

  static ImGuiComboFlags combo_flags =
    ImGuiComboFlags_None;

  ImGui::SetCursorPos(ImVec2(50.0F, 37.0F));
  ImGui::SetNextItemWidth(100.0F);

  /*
  if (ImGui::BeginCombo("##DockspaceType", g_dockspace_type_names[g_dockspace_selected_type], combo_flags)) {

    uint64_t type_index = 0;
    uint64_t type_count = ED_DOCKSPACE_TYPE_COUNT;

    while (type_index < type_count) {

      uint8_t is_selected = (g_dockspace_selected_type == type_index);

      if (ImGui::Selectable(g_dockspace_type_names[type_index], is_selected)) {
        g_dockspace_selected_type = (ed_dockspace_type_t)type_index;
        g_dockspace_is_dirty = 1;
      }

      if (is_selected) {
        ImGui::SetItemDefaultFocus();
      }

      type_index++;
    }

    ImGui::EndCombo();
  }
  */

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);
  ImGui::PushFont(g_editor_material_symbols_h6);

  ImGui::SetCursorPos(ImVec2(170.0F, 35.0F));

  if (ImGui::Button(ICON_MS_SAVE)) {
  }

  ImGui::SetCursorPos(ImVec2(220.0F, 35.0F));

  if (ImGui::Button(ICON_MS_PLAY_ARROW)) {
  }

  ImGui::SameLine();

  if (ImGui::Button(ICON_MS_PAUSE)) {
  }

  ImGui::SameLine();

  if (ImGui::Button(ICON_MS_STOP)) {
  }

  ImGui::PopFont();
  ImGui::PopStyleVar();
}
